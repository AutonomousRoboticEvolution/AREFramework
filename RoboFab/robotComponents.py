## This file defines a class for the robots (the evolved individuals to be made by the RoboFab, not the RoboFab itself)
## It also contains classes for the sub-components: the Organ and Cable


import math
from helperFunctions import makeTransform, makeTransformInputFormatted, debugPrint, findAngleBetweenTransforms, \
    makeTransformMillimetersDegrees, findDisplacementBetweenTransforms
from typing import List
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

def makeOrganFromBlueprintData(blueprintRow, dictionaryOfAllOrganTypes, i2cAddress=0x00 , gripper_TCP=makeTransform()):
    skeletonID = int(blueprintRow[0]) #the skeleton part ID to which this organ is attached
    organType=blueprintRow[1]
    blueprintX=blueprintRow[2]
    blueprintY=blueprintRow[3]
    blueprintZ=blueprintRow[4]
    blueprintRotX=blueprintRow[5]
    blueprintRotY=blueprintRow[6]
    blueprintRotZ=blueprintRow[7]
    # note! the blueprint angles are ordered x,y,z, so we must be careful in the order we create the Transform:
    position = makeTransform(blueprintX,blueprintY,blueprintZ) * makeTransform(rotX=blueprintRotX) * makeTransform(rotY=blueprintRotY) * makeTransform(rotZ = blueprintRotZ)
    return Organ(organType=organType , positionTransform=position, dictionaryOfAllOrganTypes=dictionaryOfAllOrganTypes, i2cAddress=i2cAddress , gripper_TCP=gripper_TCP , skeletonID=skeletonID)

## class to describe each individual organ
# organType should be a number, 0=Head, 1=wheel, 2=sensor, 3=castor, 4=joint
# positionTransform refers to its position relative to its parent's origin, i.e. when in bank to Bank.origin and when in a robot to Robot.origin
class Organ:
    def __init__ ( self, organType, positionTransform, dictionaryOfAllOrganTypes, i2cAddress="0" , gripper_TCP=makeTransform() , isInBankFlag=False, skeletonID=0):

        def findRequiredAssemblyFixtureRotationRadians():

            # this is the angle between the x axis of the clip and straight down, i.e. the magnitude of the angle from which the gripper approaches relative to global vertical. This is small when the gripper comes in "from above":
            # angleFromVertical = findAngleBetweenTransforms(
            #     A = self.positionTransformWithinBankOrRobot,
            #     B = makeTransformMillimetersDegrees(rotY=90),
            #     axis="x"
            # )
            # debugPrint("Angle of organ attachment from vertical is {}degrees".format(math.degrees(angleFromVertical)) , messageVerbosity=3)

            # print("----")
            # print("type: "+str(self.organType))
            # print(self.positionTransformWithinBankOrRobot)
            # print(self.transformOrganOriginToClipCentre)
            # print(self.transformOrganOriginToMaleCableSocket)
            # print(gripper_TCP)

            # rotate the AF such that the wrist of the UR5 is off to the side
            wrist_position = self.positionTransformWithinBankOrRobot * np.linalg.inv(self.transformOrganOriginToClipCentre) * self.transformOrganOriginToMaleCableSocket * np.linalg.inv(gripper_TCP) *makeTransformMillimetersDegrees(z=-300)
            x = wrist_position[0, 3]
            y = wrist_position[1, 3]
            return math.atan2(x, y) + math.radians(-15)

            # if angleFromVertical < math.radians(30):
                # # coming in from above, rotate so the position of organ is off to the side
                # x = self.positionTransformWithinBankOrRobot[0, 3]
                # y = self.positionTransformWithinBankOrRobot[1, 3]
                # coming in from above, rotate so the position of UR5 wrist is off to the side
                # return math.atan2(x, y) + math.radians(0)
            # else: # arm is coming in from the side, rotate so that approach direction is from the (+y) side
            #     debugPrint("so coming in from side",3)
            #     tempTransform = self.positionTransformWithinBankOrRobot.copy()
            #     tempTransform[0:3,3]=0 # set translation to zero, so left with just rotation
            #     tempTransform *= makeTransform(x=-1) # apply rotation to unit vector in negative x direction. The result is a unit vector which describes the direction from which the gripper will approach
            #     x = tempTransform[0, 3]
            #     y = tempTransform[1, 3]
            #     return math.atan2(x, y) + math.radians(-15) # the angle to make the gripper approach from the "left" (viewing the RoboFab from the front)

        dictionaryOfThisOrganType = dictionaryOfAllOrganTypes [ str( organType ) ]

        self.skeletonID=skeletonID
        self.organType =dictionaryOfThisOrganType["organType"]
        self.isInBank = isInBankFlag
        self.hasBeenInserted = False # converted to True once the RoboFab has inserted this organ, i.e. put it on the robot
        self.friendlyName = dictionaryOfThisOrganType["friendlyName"]
        self.forceModeTravelDistance = dictionaryOfThisOrganType["forceModeTravelDistance"]
        self.postInsertExtraPushDistance = dictionaryOfThisOrganType["postInsertExtraPushDistance"]
        self.pickupExtraPushDistance = dictionaryOfThisOrganType["pickupExtraPushDistance"]
        # self.gripPosition = np.matrix( dictionaryOfThisOrganType["gripPosition"] )
        self.transformOrganOriginToGripper = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToGripper"] )
        self.transformOrganOriginToClipCentre = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToClipCentre"] )
        if dictionaryOfThisOrganType["transformOrganOriginToFemaleCableSocket"] is None: # this is a list of the female TRRS sockets (e.g. the 8 on the Head)
            self.transformOrganOriginToFemaleCableSocket = None
            self.femaleTRRSSocketsUsedList = None
        else:
            self.transformOrganOriginToFemaleCableSocket = [np.matrix(x) for x in dictionaryOfThisOrganType["transformOrganOriginToFemaleCableSocket"]]
            self.femaleTRRSSocketsUsedList = [False]*len(self.transformOrganOriginToFemaleCableSocket)
        self.useForceMode :bool = dictionaryOfThisOrganType["USE_FORCE_MODE"]
        self.I2CAddress = i2cAddress
        self.positionTransformWithinBankOrRobot = positionTransform
        self.flipGripperOrientation = False
        self.gripperOpenPosition = dictionaryOfThisOrganType["gripperOpeningFraction"]
        self.gripperClosedPosition = dictionaryOfThisOrganType["gripperClosedFraction"]
        self.AssemblyFixtureRotationOffsetFudgeAngle = 0
        self.actualDropoffPosition = None # optionally used to store the actual position the arm moves to when inserting the organ, which may be different due to use of force mode. This might be useful when we come back to the organ later.

        if dictionaryOfThisOrganType["transformOrganOriginToMaleCableSocket"] is None:
            self.transformOrganOriginToMaleCableSocket = None
        else:
            self.transformOrganOriginToMaleCableSocket = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToMaleCableSocket"] )
        self.cableDestination = makeTransform() # used when the cable is contained in the organ

        # compute assembly fixture position to be used (zero if Head organ, otherwise call helper function defined above)
        if isInBankFlag or self.organType == 0:
            self.requiredAssemblyFixtureRotationRadians = 0
        else:
            self.requiredAssemblyFixtureRotationRadians = findRequiredAssemblyFixtureRotationRadians()



## DEPRECATED! cables are now included with organs
## describes an individual cable.
## transformEnd1 and transformEnd2 are position transforms for pickup point for each end of the cable relative to parent origin (i.e. when in the bank, relative to Bank.origin, and when in a robot relative to Robot.origin)
class Cable:
    def __init__ ( self, transforms , gripPoint ):
        self.transformEnd1 = np.matrix(transforms[0])  # point (in frame of bank or robot which cable is in) where the end is currently
        self.transformEnd2 = np.matrix(transforms[1])
        self.gripPoint = gripPoint
        self.forceModeTravelDistance = 0.012  # 12mm



## class describing an individual evolved robot ie the one to be constructed.
#
# Will contain organsList, a list of the organs that should go in the robot (each a member of the Organ class). This list exists even before the organs are actually added.
class Robot:
    organsList: List [ Organ ]
    cablesList: List [ Cable ]

    def __init__ (self, origin=makeTransformInputFormatted (), ID=""):
        self.organsByLimbList = [ ] # list of lists of organ objects, by which skeleton part they are attached to. Does not include the Head.
        self.jointsList = [] # list of organ objects of just the joints
        self.organsList = [ ]
        self.cablesList = [ ]
        self.origin = origin
        self.ID = ID
        #self.organInsertionTrackingList = [ ] # Will be a list with one entry per organ. Start as False. Once an organ is physically inserted, its value in this list is turned True.
        #self.hasOrgansNeedingInsertion = False # if any of the organs in the previous list are still False (ie not yet inserted), this will be True (ie "we need to keep going!")

    def addOrgan ( self, organToAdd ):
        self.organsList.append ( organToAdd )
        self.hasOrgansNeedingInsertion = True

    def createLimbList ( self ):
        # the Robot.organsByLimbList object is a list of lists.
        # Each sub-list represents a skeleton part (the 0th is the main skeleton, and each subsequent one a limb bone)
        # and is a list of the organ(s) attached to that skeleton part
        # first create the right number of lists, empty for now:
        for _ in range(max( [ o.skeletonID for o in self.organsList ] )+1): self.organsByLimbList.append([])
        for organ in self.organsList [ 1: ]:  # don't include Head (0th item in list)
            self.organsByLimbList [0].append(organ)
            if organ.friendlyName.upper().startswith("JOINT"):
                self.jointsList.append(organ)


    ## find the next organ not yet inserted, return it and mark it as inserted in the list
    def getNextOrganToInsert ( self ):
        index = [x.hasBeenInserted for x in self.organsByLimbList[0]].index(False) # find the first organ attached to main skeleton that hasn't yet been inserted
        self.organsByLimbList[0][ index ].hasBeenInserted = True
        #if all ( [x.hasBeenInserted for x in self.organsByLimbList[0]] ):  # check if all are now complete
        debugPrint("Inserting organ {} (a {}) out of {} attached to main skeleton".format(index, self.organsByLimbList[0][index].friendlyName ,len(self.organsByLimbList[0])),messageVerbosity=0)
        return self.organsByLimbList[0][ index ]

    def countOrgansOfType(self, organType):
        count = 0
        for organ in self.organsList:
            if organ.organType == organType:
                count += 1
        return count

    def determineCableDestinations(self):
        debugPrint("Allocating sockets for the organs with cables...",0)
        headOrigin = self.organsList[0].positionTransformWithinBankOrRobot

        indexesOfJoints = [ i for i,o in enumerate(self.organsList) if o.friendlyName.upper().startswith("JOINT")] # get a list of index values of self.myRobot.organsList that refer to joints
        # sort this list in order of the absolute x coordinate of each organ's male TRRS socket:
        indexesOfJoints = [indexesOfJoints[i] for i in np.argsort( [
            math.atan( abs((o.positionTransformWithinBankOrRobot * np.linalg.inv(o.transformOrganOriginToClipCentre) * o.transformOrganOriginToMaleCableSocket)[1,3]) /
                              abs((o.positionTransformWithinBankOrRobot * np.linalg.inv(o.transformOrganOriginToClipCentre) * o.transformOrganOriginToMaleCableSocket)[0, 3])
                   ) # <== this is the function by which the organ allocation order will be sorted
            for o in [self.organsList[j] for j in indexesOfJoints] ]) ]

        # now do the equivalent to find the order to consider all the other organs which have cables that need routing
        indexesOfOtherOrgans = [i for i,o in enumerate(self.organsList) if  o.transformOrganOriginToMaleCableSocket is not None and i not in indexesOfJoints]
        indexesOfOtherOrgans = [indexesOfOtherOrgans[i] for i in np.argsort([
            math.atan( abs( (o.positionTransformWithinBankOrRobot * np.linalg.inv(o.transformOrganOriginToClipCentre) * o.transformOrganOriginToMaleCableSocket)[1,3]) /
                              abs((o.positionTransformWithinBankOrRobot * np.linalg.inv(o.transformOrganOriginToClipCentre) * o.transformOrganOriginToMaleCableSocket)[0, 3])
                   ) # <== this is the function by which the organ allocation order will be sorted
            for i, o in enumerate([self.organsList[j] for j in indexesOfOtherOrgans])])]

        debugPrint("Indexes of Joints: {}\nIndexes of others: {}".format(indexesOfJoints,indexesOfOtherOrgans),1)
        assert (len(indexesOfJoints) <= 4)  # we cannot have more than 4 joints, because we only have 4 pairs of sockets (and are enforcing each joint is on a different pair)

        # do the connections in the order determined above:
        for index in indexesOfJoints:
            self.connectCableToClosestSocket( self.organsList[index ] )
        for index in indexesOfOtherOrgans:
            self.connectCableToClosestSocket( self.organsList[index ] )

    def connectCableToClosestSocket( self, organ ):

        # The TRRS sockets on the Head are (electrically) in pairs.
        # The list "transformOrganOriginToFemaleCableSocket" is defined in order so that each pair are listed next to each other, e.g. 0&1 are a pair, 2&3 are a pair etc.
        # Given the index of a socket, this function will return the index of the paired socket
        def returnPartnerIndex ( index ):
            if index % 2 == 0:  # is even
                return index + 1
            else:  # is odd
                return index - 1

        headOrigin = self.organsList[0].positionTransformWithinBankOrRobot
        if organ.transformOrganOriginToMaleCableSocket is not None:  # if this organ has a cable to consider (if not ignore it)
            if np.allclose(organ.cableDestination, makeTransform()):  # and if we haven't yet sorted this organ's cable, which we can tell because cableDestination hasn't been modified from its starting value
                # let's find the closest socket to this organ, and set the cable destination to be that socket:
                minDistance = 999
                closestSocketFound = -1  # the index in the sockets list self.myRobot.organsList[0].transformOrganOriginToFemaleCableSocket
                for socketNumber, candidateSocketLocation in enumerate(self.organsList[0].transformOrganOriginToFemaleCableSocket):
                    if self.organsList[0].femaleTRRSSocketsUsedList[socketNumber] == False:  # only check this socket if it hasn't already been used
                        # print("partner used: {}".format(self.myRobot.organsList[0].femaleTRRSSocketsUsedList[returnPartnerIndex(socketNumber)] ))

                        #if this is a joint, only check this socket if its partner hasn't been used yet
                        if (not organ.friendlyName.upper().startswith("JOINT")) or self.organsList[0].femaleTRRSSocketsUsedList[returnPartnerIndex(socketNumber)] == False:
                            distance = findDisplacementBetweenTransforms(organ.positionTransformWithinBankOrRobot * np.linalg.inv(organ.transformOrganOriginToClipCentre) * organ.transformOrganOriginToMaleCableSocket,  # where the cable comes from in this organ
                                                                         headOrigin * candidateSocketLocation)["magnitude"]
                            if distance < minDistance:
                                minDistance = distance
                                closestSocketFound = socketNumber
                assert (closestSocketFound != -1)  # check we have found a socket for this cable
                # print("Closest socket found was {} at a distance of {}".format(closestSocketFound,minDistance))
                organ.cableDestination = headOrigin * self.organsList[0].transformOrganOriginToFemaleCableSocket[closestSocketFound]
                self.organsList[0].femaleTRRSSocketsUsedList[closestSocketFound] = True  # mark that socket (in the Head) as having been used already

    # remove all the cable allocations, and reset the list of used sockets in the Head so none have been used
    def clearCables(self):
        for organ in self.organsList:
            if organ.transformOrganOriginToMaleCableSocket is not None:
                organ.cableDestination = makeTransform()
        self.organsList[0].femaleTRRSSocketsUsedList=[False]*len(self.organsList[0].transformOrganOriginToFemaleCableSocket)

    # creates a pdf that shows what organs the robot has; their layout; their i2c addresses
    def drawRobot(self,saveDirectory):
        for i,organ in enumerate(self.organsList):
            x= ( organ.positionTransformWithinBankOrRobot * np.linalg.inv( organ.transformOrganOriginToClipCentre ) )[0,3]
            y= ( organ.positionTransformWithinBankOrRobot * np.linalg.inv( organ.transformOrganOriginToClipCentre ) )[1,3]
            # print("{}: x: {}, y: {}".format( organ.friendlyName,x,y ))

            if not organ.friendlyName.upper().startswith("HEAD"):
                plt.plot(x, y, "bo")
                plt.plot(organ.positionTransformWithinBankOrRobot[0,3] , organ.positionTransformWithinBankOrRobot[1,3],"go") # clip
                plt.plot([x,organ.positionTransformWithinBankOrRobot[0,3]] , [y,organ.positionTransformWithinBankOrRobot[1,3]],"g--") # clip
                plt.text(x, y + 0.005, "{}:{}".format(i,organ.friendlyName), color="b", ha='center')
                plt.text(x, y - 0.005, "{}".format(organ.I2CAddress), color="b", ha='center')

                if organ.transformOrganOriginToMaleCableSocket is not None: # has a cable to draw
                    plt.plot( # line for cable
                        [ organ.cableDestination[0,3] , (organ.positionTransformWithinBankOrRobot * np.linalg.inv( organ.transformOrganOriginToClipCentre ) * organ.transformOrganOriginToMaleCableSocket)[0,3] ],
                        [ organ.cableDestination[1,3] , (organ.positionTransformWithinBankOrRobot * np.linalg.inv( organ.transformOrganOriginToClipCentre ) * organ.transformOrganOriginToMaleCableSocket)[1,3] ],
                        "k-")
                    plt.plot(organ.cableDestination[0,3] , organ.cableDestination[1,3],"ko") # dot for cable socket in Head

        # square around Head:
        head_size = 0.085 / 2
        plt.plot([-head_size, head_size, head_size, -head_size, -head_size], [-head_size, -head_size, head_size, head_size, -head_size], "r-")
        # empty sockets:
        for i,socket in enumerate(self.organsList[0].transformOrganOriginToFemaleCableSocket):
            if self.organsList[0].femaleTRRSSocketsUsedList[i] == False:
                plt.plot(socket[0,3], socket[1,3], "kx")

        plt.gcf().set_size_inches(5, 5)
        plt.xlim([-0.25, 0.25])
        plt.ylim([-0.25, 0.25])
        plt.xlabel("x")
        plt.ylabel("y")
        plt.title("Robot ID: {}".format(self.ID))

        # save as pdf
        pdf = PdfPages ( "{}/waiting_to_be_evaluated/plan_{}".format(saveDirectory,self.ID) )
        pdf.savefig ( plt.gcf() )
        pdf.close ()

if __name__ == "__main__":
    import json
    configurationData = json.load(open('configuration_BRL.json'))
    org=Organ(1, makeTransformMillimetersDegrees(x=25,y=50,z=20,rotY=30,rotZ=0), configurationData["dictionaryOfOrganTypes"])
    print(org.requiredAssemblyFixtureRotationRadians)
