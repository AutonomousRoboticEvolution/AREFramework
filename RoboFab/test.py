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

makeConfigurationFile(location="BRL")  # <--- change this depending on if you're in York or BRL
configurationData = json.load(open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

printer_number=0
# printer=Printer( configurationData["network"]["PRINTER{}_IP_ADDRESS".format(printer_number)], configurationData, printer_number=0 )
printer=Printer( None, configurationData, printer_number=0 )

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

    def determineCableDestinations(self):
        i = -1
        for organ in self.myRobot.organsList:
            if organ.transformOrganOriginToMaleCableSocket is not None:
                i += 1
                organ.cableDestination = self.myRobot.organsList[0].positionTransformWithinBankOrRobot \
                                         * self.myRobot.organsList[0].transformOrganOriginToCableSocket[i]  # socket slot in Head, relative to robot origin

    def drawCables(self):
        for organ in self.myRobot.organsList:
            x= ( organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) )[0,3]
            y= ( organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) )[1,3]
            print("{}: x: {}, y: {}".format( organ.friendlyName,x,y ))

            plt.plot(x, y, "bo")
            plt.plot(organ.positionTransformWithinBankOrRobot[0,3] , organ.positionTransformWithinBankOrRobot[1,3],"go") # clip
            plt.plot([x,organ.positionTransformWithinBankOrRobot[0,3]] , [y,organ.positionTransformWithinBankOrRobot[1,3]],"g--") # clip
            plt.text(x, y + 0.005, organ.friendlyName, color="b", ha='center')

            if organ.transformOrganOriginToMaleCableSocket is not None:
                plt.plot( # line for cable
                    [ organ.cableDestination[0,3] , (organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) * organ.transformOrganOriginToMaleCableSocket)[0,3] ],
                    [ organ.cableDestination[1,3] , (organ.positionTransformWithinBankOrRobot * inv( organ.transformOrganOriginToClipCentre ) * organ.transformOrganOriginToMaleCableSocket)[1,3] ],
                    "k-")
                plt.plot(organ.cableDestination[0,3] , organ.cableDestination[1,3],"ko") # dot for cable socket in Head

        # square around Head:
        head_size = 0.085 / 2
        plt.plot([-head_size, head_size, head_size, -head_size, -head_size], [-head_size, -head_size, head_size, head_size, -head_size], "r-")

        plt.gcf().set_size_inches(8, 8)
        plt.xlim([-0.2, 0.2])
        plt.ylim([-0.2, 0.2])
        plt.xlabel("x")
        plt.ylabel("y")
        plt.draw()


filename="test1"

tester=Tester(configurationData, robotID=filename)
tester.determineCableDestinations()
tester.drawCables()

# printer.createSTL(filename)

plt.show()