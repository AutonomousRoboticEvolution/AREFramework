## This file defines a class for the robots (the evolved individuals to be made by the RoboFab, not the RoboFab itself)
## It also contains classes for the sub-components: the Organ and Cable


import math
from helperFunctions import makeTransform
from typing import List
import numpy as np

## class to describe each individual organ
#
#  positionTransform refers to its position relative to its parent's origin, i.e. when in bank to Bank.origin and when in a robot to Robot.origin
class Organ:
    def __init__ ( self, blueprintRow, dictionaryOfAllOrganTypes, i2cAddress=0x00 ):
        dictionaryOfThisOrganType = dictionaryOfAllOrganTypes [ str(blueprintRow [ 1 ]) ]

        self.organType =dictionaryOfThisOrganType["organType"]
        self.friendlyName = dictionaryOfThisOrganType["organType"]
        self.forceModeTravelDistance = dictionaryOfThisOrganType["forceModeTravelDistance"]
        self.postInsertExtraPushDistance = dictionaryOfThisOrganType["postInsertExtraPushDistance"]
        self.pickupExtraPushDistance = dictionaryOfThisOrganType["pickupExtraPushDistance"]
        # self.gripPosition = np.matrix( dictionaryOfThisOrganType["gripPosition"] )
        self.transformOrganOriginToGripper = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToGripper"] )
        self.transformOrganOriginToClipCentre = np.matrix( dictionaryOfThisOrganType["transformOrganOriginToClipCentre"] )
        self.transformOrganOriginToCableSocket = [np.matrix(x) for x in dictionaryOfThisOrganType["transformOrganOriginToCableSocket"] ]
        self.useForceMode :bool = dictionaryOfThisOrganType["USE_FORCE_MODE"]
        self.I2CAddress = i2cAddress
        self.positionTransformWithinBankOrRobot = makeTransform (blueprintRow [2:9])
        self.requiredAssemblyFixtureRotationRadians = 0.0  # todo: find some way to choose what this should be
        self.flipGripperOrientation = False
        self.gripperOpenPosition = dictionaryOfThisOrganType["gripperOpeningFraction"]
        self.gripperClosedPosition = dictionaryOfThisOrganType["gripperClosedFraction"]
        self.AssemblyFixtureRotationOffsetFudgeAngle = 0



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

    def __init__ ( self, origin=makeTransform () ):
        self.organsList = [ ]
        self.cablesList = [ ]
        self.printbedPullPoints = [ ]
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

    def addPrintbedPullPoint ( self, location ):
        self.printbedPullPoints.append ( location )

    ## find the next organ not yet inserted, return it and mark it as inserted in the list
    def getNextOrganToInsert ( self ):
        index = next ( x for x in range ( len ( self.organInsertionTrackingList ) ) if
                       self.organInsertionTrackingList [ x ] == False )
        self.organInsertionTrackingList [ index ] = True
        if all ( self.organInsertionTrackingList ):  # check if all are now complete
            self.hasOrgansNeedingInsertion = False
        return self.organsList [ index ]

    ## find the next cable not yet inserted, return it and mark it as inserted in the list
    def getNextCableToInsert ( self ):
        index = next ( x for x in range ( len ( self.cableInsertionTrackingList ) ) if
                       self.cableInsertionTrackingList [ x ] == False )
        self.cableInsertionTrackingList [ index ] = True
        if all ( self.cableInsertionTrackingList ):  # check if all are now complete
            self.hasCablesNeedingInsertion = False
        return self.cablesList [ index ]