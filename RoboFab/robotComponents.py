## This file defines a class for the robots (the evolved individuals to be made by the RoboFab, not the RoboFab itself)
## It also contains classes for the sub-components: the Organ and Cable


import math
from helperFunctions import makeTransform, makeTransformInputFormatted, debugPrint, findAngleBetweenTransforms, makeTransformMillimetersDegrees
from typing import List
import numpy as np

def makeOrganFromBlueprintData(blueprintRow, dictionaryOfAllOrganTypes, i2cAddress=0x00 , gripper_TCP=makeTransform()):
    # blueprintRow[0] is the skeleton part ID, todo: implement using this
    organType=blueprintRow[1]
    blueprintX=blueprintRow[2]
    blueprintY=blueprintRow[3]
    blueprintZ=blueprintRow[4]
    blueprintRotX=blueprintRow[5]
    blueprintRotY=blueprintRow[6]
    blueprintRotZ=blueprintRow[7]
    # note! the blueprint angles are ordered x,y,z, so we must be careful in the order we create the Transform:
    position = makeTransform(blueprintX,blueprintY,blueprintZ) * makeTransform(rotX=blueprintRotX) * makeTransform(rotY=blueprintRotY) * makeTransform(rotZ = blueprintRotZ)
    return Organ(organType=organType , positionTransform=position, dictionaryOfAllOrganTypes=dictionaryOfAllOrganTypes, i2cAddress=i2cAddress , gripper_TCP=gripper_TCP)

## class to describe each individual organ
# organType should be a number, 0=Head, 1=wheel, 2=sensor, 3=castor, 4=joint
# positionTransform refers to its position relative to its parent's origin, i.e. when in bank to Bank.origin and when in a robot to Robot.origin
class Organ:
    def __init__ ( self, organType, positionTransform, dictionaryOfAllOrganTypes, i2cAddress="0" , gripper_TCP=makeTransform() , isInBankFlag=False):

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

        self.organType =dictionaryOfThisOrganType["organType"]
        self.isInBank = isInBankFlag
        self.friendlyName = dictionaryOfThisOrganType["friendlyName"]
        self.forceModeTravelDistance = dictionaryOfThisOrganType["forceModeTravelDistance"]
        self.postInsertExtraPushDistance = dictionaryOfThisOrganType["postInsertExtraPushDistance"]
        self.pickupExtraPushDistance = dictionaryOfThisOrganType["pickupExtraPushDistance"]
        # self.gripPosition = np.matrix( dictionaryOfThisOrganType["gripPosition"] )
        self.transformOrganOriginToGripper = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToGripper"] )
        self.transformOrganOriginToClipCentre = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToClipCentre"] )
        if dictionaryOfThisOrganType["transformOrganOriginToCableSocket"] is None:
            self.transformOrganOriginToCableSocket = None
        else:
            self.transformOrganOriginToCableSocket = [np.matrix(x) for x in dictionaryOfThisOrganType["transformOrganOriginToCableSocket"] ]
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

    def __init__ (self, origin=makeTransformInputFormatted ()):
        self.organsList = [ ]
        self.cablesList = [ ]
        self.origin = origin
        self.organInsertionTrackingList = [ ] # Will be a list with one entry per organ. Start as False. Once an organ is physically inserted, its value in this list is turned True.
        self.hasOrgansNeedingInsertion = False # if any of the organs in the previous list are still False (ie not yet inserted), this will be True (ie "we need to keep going!")
        self.cableInsertionTrackingList = [ ]  # Will be a list with one entry per cable. Start as False. Once a cable is physically inserted, its value in this list is turned True.
        self.hasCablesNeedingInsertion = False  # if any of the cables in the previous list are still False (ie not yet inserted), this will be True (ie "we need to keep going!")

    def addCable ( self, cableToAdd ):
        self.cablesList.append ( cableToAdd )
        self.cableInsertionTrackingList.append ( False )
        self.hasCablesNeedingInsertion = True

    def addOrgan ( self, organToAdd ):
        self.organsList.append ( organToAdd )
        self.organInsertionTrackingList.append ( False )
        self.hasOrgansNeedingInsertion = True

    ## find the next organ not yet inserted, return it and mark it as inserted in the list
    def getNextOrganToInsert ( self ):
        index = self.organInsertionTrackingList.index(False) # find the first item that is "False" in the tracking list
        self.organInsertionTrackingList [ index ] = True
        if all ( self.organInsertionTrackingList ):  # check if all are now complete
            self.hasOrgansNeedingInsertion = False
        debugPrint("Returning organ {} of {}".format(index,len(self.organInsertionTrackingList)),messageVerbosity=0)
        return self.organsList [ index ]

    ## find the next cable not yet inserted, return it and mark it as inserted in the list
    def getNextCableToInsert ( self ):
        index = next ( x for x in range ( len ( self.cableInsertionTrackingList ) ) if
                       self.cableInsertionTrackingList [ x ] == False )
        self.cableInsertionTrackingList [ index ] = True
        if all ( self.cableInsertionTrackingList ):  # check if all are now complete
            self.hasCablesNeedingInsertion = False
        return self.cablesList [ index ]

    def countOrgansOfType(self, organType):
        count = 0
        for organ in self.organsList:
            if organ.organType == organType:
                count += 1
        return count



if __name__ == "__main__":
    import json
    configurationData = json.load(open('configuration_BRL.json'))
    org=Organ(1, makeTransformMillimetersDegrees(x=25,y=50,z=20,rotY=30,rotZ=0), configurationData["dictionaryOfOrganTypes"])
    print(org.requiredAssemblyFixtureRotationRadians)
