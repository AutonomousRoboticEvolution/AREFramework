from typing import List
from RoboFab import RoboFab_host
from helperFunctions import *
from makeConfigurationFile import makeFile as makeConfigurationFile
import json
from printer import Printer
from robotComponents import Robot, makeOrganFromBlueprintData
import csv
from numpy.linalg import inv

import matplotlib.pyplot as plt
import random

makeConfigurationFile(location="BRL")  # <--- change this depending on if you're in York or BRL
configurationData = json.load(open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

printer_number=0
# printer=Printer( configurationData["network"]["PRINTER{}_IP_ADDRESS".format(printer_number)], configurationData, printer_number=0 )
printer=Printer( None, configurationData, printer_number=0 )

def makeRandomBlueprint(filename):
    with open("/home/robofab/are-logs/test_are_generate/waiting_to_be_built/blueprint_{}.csv".format(filename), 'w') as f:
        f.write("0,0,0,0,0.0054,0,0,0,") # Head line

        n_organs= random.randrange(1,9)
        for i in range(n_organs):
            type=random.choice([1,2,4])
            keep_this_set=False
            while not keep_this_set:
                x_pos = -0.2 + 0.4*random.random()
                y_pos = -0.2 + 0.4*random.random()
                if abs(x_pos)>0.07 or abs(y_pos)>0.07: keep_this_set=True
            print("{}, {}".format(x_pos,y_pos))
            f.write("\n0,{},{},{},0.0198007,0,-1.5708,3.1415,".format(type,x_pos,y_pos))

class Tester:
    def __init__(self,configurationData, robotID):
        self.dictionaryOfOrganTypes = configurationData["dictionaryOfOrganTypes"]
        self.myRobot = Robot()
        self.gripperTCP_A = np.matrix( ( configurationData [ "gripper" ] [ "TCP_A" ] ) )

        debugPrint("Loading the blueprint file: blueprint_" + robotID, messageVerbosity=0)
        blueprintList = []
        # with open ( './blueprints/blueprint'+robotID+'.csv', newline='' ) as blueprintFile:
        with open('/home/robofab/are-logs/test_are_generate/waiting_to_be_built/blueprint_' + robotID + '.csv', newline='') as blueprintFile:
            blueprintReader = csv.reader(blueprintFile, delimiter=' ', quotechar='|')
            for rowString in blueprintReader:
                rowAsListOfStrings = rowString[0].split(',')
                # Reads the information about organs and converts to correct format
                # i = 0,1 type & parent ID,  i = 2-4 position in m, i = 5-7 rotation in radians
                blueprintRowToAppend: List[float] = [int(float(i)) for i in rowAsListOfStrings[0:2]] + \
                                                    [float(i) for i in rowAsListOfStrings[2:5]] + \
                                                    [float(i) for i in rowAsListOfStrings[5:8]]

                blueprintList.append(blueprintRowToAppend)  # converted to meters & radians
        debugPrint('organ locations are: ' + str(blueprintList), messageVerbosity=1)

        for organ_raw_data in blueprintList: # n.b. the first row must be the core organ
            debugPrint ( "adding an organ of type {} ({})".format(str( organ_raw_data[1] ) , self.dictionaryOfOrganTypes[str(organ_raw_data[1])]["friendlyName"]) ,messageVerbosity=2)
            self.myRobot.addOrgan (
                makeOrganFromBlueprintData ( blueprintRow=organ_raw_data,dictionaryOfAllOrganTypes=self.dictionaryOfOrganTypes , gripper_TCP=self.gripperTCP_A)
            )

    def determineCableDestinations_original(self):
        i = -1
        for organ in self.myRobot.organsList:
            if organ.transformOrganOriginToMaleCableSocket is not None:
                i += 1
                organ.cableDestination = self.myRobot.organsList[0].positionTransformWithinBankOrRobot \
                                         * self.myRobot.organsList[0].transformOrganOriginToFemaleCableSocket[i]  # socket slot in Head, relative to robot origin

    def determineCableDestinations_closest(self):
        headOrigin = self.myRobot.organsList[0].positionTransformWithinBankOrRobot

        allFinished=False
        while not allFinished:
            for organ in self.myRobot.organsList:
                allFinished=True # will be re-set to False if any organ gets modified this time through
                if organ.transformOrganOriginToMaleCableSocket is not None: # if this organ has a cable to consider (if not ignore it)
                    if np.allclose(organ.cableDestination, makeTransform()): # and if we haven't yet sorted this organ's cable, which we can tell because cableDestination hasn't been modified from its starting value
                        allFinished=False # since there is at least one organ needing assignment, we are not done yet

                        # let's find the closest socket to this organ, and set the cable destination to be that socket:
                        minDistance=999
                        closestSocketFound=-1 # the index in the sockets list self.myRobot.organsList[0].transformOrganOriginToFemaleCableSocket
                        for socketNumber,candidateSocketLocation in enumerate(self.myRobot.organsList[0].transformOrganOriginToFemaleCableSocket):
                            if self.myRobot.organsList[0].femaleTRRSSocketsUsedList[socketNumber] == False: # only check this socket if it hasn't already been used
                                distance = findDisplacementBetweenTransforms(organ.positionTransformWithinBankOrRobot * organ.transformOrganOriginToMaleCableSocket, # where the cable comes from in this organ
                                                                  headOrigin * candidateSocketLocation)["magnitude"]
                                if distance<minDistance:
                                    minDistance=distance
                                    closestSocketFound=socketNumber

                        assert (closestSocketFound != -1) # check we have found a socket for this cable

                        print("Closest socket found was {} at a distance of {}".format(closestSocketFound,minDistance))
                        organ.cableDestination = headOrigin * self.myRobot.organsList[0].transformOrganOriginToFemaleCableSocket[closestSocketFound]
                        self.myRobot.organsList[0].femaleTRRSSocketsUsedList[closestSocketFound] = True # mark that socket (in the Head) as having been used already



    def clearCables(self):
        for organ in self.myRobot.organsList:
            if organ.transformOrganOriginToMaleCableSocket is not None:
                organ.cableDestination = makeTransform()

    def drawCables(self,axesHandles,i_plot,title=""):
        # set actuve subplot:
        plt.sca(axesHandles[i_plot])

        for i,organ in enumerate(self.myRobot.organsList):
            x= ( organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) )[0,3]
            y= ( organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) )[1,3]
            # print("{}: x: {}, y: {}".format( organ.friendlyName,x,y ))

            if not organ.friendlyName.startswith("Head"):
                plt.plot(x, y, "bo")
                plt.plot(organ.positionTransformWithinBankOrRobot[0,3] , organ.positionTransformWithinBankOrRobot[1,3],"go") # clip
                plt.plot([x,organ.positionTransformWithinBankOrRobot[0,3]] , [y,organ.positionTransformWithinBankOrRobot[1,3]],"g--") # clip
                plt.text(x, y + 0.005, "{}:{}".format(i,organ.friendlyName), color="b", ha='center')

            if organ.transformOrganOriginToMaleCableSocket is not None:
                plt.plot( # line for cable
                    [ organ.cableDestination[0,3] , (organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) * organ.transformOrganOriginToMaleCableSocket)[0,3] ],
                    [ organ.cableDestination[1,3] , (organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) * organ.transformOrganOriginToMaleCableSocket)[1,3] ],
                    "k-")
                plt.plot(organ.cableDestination[0,3] , organ.cableDestination[1,3],"ko") # dot for cable socket in Head

        # square around Head:
        head_size = 0.085 / 2
        plt.plot([-head_size, head_size, head_size, -head_size, -head_size], [-head_size, -head_size, head_size, head_size, -head_size], "r-")
        # empty sockets:
        for i,socket in enumerate(self.myRobot.organsList[0].transformOrganOriginToFemaleCableSocket):
            if self.myRobot.organsList[0].femaleTRRSSocketsUsedList[i] == False:
                plt.plot(socket[0,3], socket[1,3], "kx")

        plt.gcf().set_size_inches(16, 8)
        plt.xlim([-0.25, 0.25])
        plt.ylim([-0.25, 0.25])
        plt.xlabel("x")
        plt.ylabel("y")
        plt.title(title)


filename="randomTest"

makeRandomBlueprint(filename)

fig, axesHandles = plt.subplots(1, 2) # n_rows, n_columns

tester=Tester(configurationData, robotID=filename)

tester.determineCableDestinations_original()
tester.drawCables(axesHandles,0,"in blueprint order")

tester.clearCables()
tester.determineCableDestinations_closest()
tester.drawCables(axesHandles,1,"closest, assigned in blueprint order")


# printer.createSTL(filename)

plt.show()