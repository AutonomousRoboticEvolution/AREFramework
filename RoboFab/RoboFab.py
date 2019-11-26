#! /usr/bin/env python
# -*- coding: utf-8 -*-

## Software to talk to all the various parts of the RoboFab
## goes from a single blueprint file to a single physical robot

from typing import List
import math
import csv # for loading blueprints
import json # for reading the configuration file
import numpy as np

from makeConfigurationFile import makeFile as makeConfigurationFile
from helperFunctions import debugPrint, makeTransform
from RoboFabComponents import AssemblyFixture, Bank
from robotComponents import Robot, Organ, Cable
from UR5_host import UR5Robot
from robotConnection import RobotConnection

# debugging flags, human switchable to turn parts of the process on/off
DO_CORE_ORGAN_INSERT = 0
DO_MOVE_FROM_PRINTER_TO_ASSEMBLY_FIXTURE = 0 # requires DO_CORE_ORGAN_INSERT
DO_ORGAN_INSERTIONS = 0
DO_CABLE_INSERTIONS = 0
DO_SEND_CONTROLLER_TO_ROBOT = 0


## top-level class. Call RoboFab.setupRobotObject(blueprint_file_name), then RoboFab.buildRobot()
class RoboFab_host:
    def __init__(self , configurationData):
        debugPrint("Creating a RoboFab_host object")


        # Defines properties of each organ type, set in the configuration file
        self.dictionaryOfOrganTypes = configurationData["dictionaryOfOrganTypes"]

        # set up / connect to printer(s)
        self.printerLocation =  np.matrix ( configurationData [ "PRINTER_1" ] [ "ORIGIN" ] ) #todo: class for talking to printer
        self.skeletonPositionOnPrintbed =  np.matrix ( configurationData [ "PRINTER_1" ] [ "skeletonPositionOnPrintbed" ] ) #todo: class for talking to printer

        # Define the two gripper TCPs. This is a transform from the tool flange to the tip of each gripper
        self.gripperTCP_A = np.matrix( ( configurationData [ "gripper" ] [ "TCP_A" ] ) )
        self.gripperTCP_B = np.matrix( ( configurationData [ "gripper" ] [ "TCP_B" ] ) )

        # define the organ bank
        self.organBank = Bank ( configurationData [ "ORGAN_BANK_1" ] , configurationData["dictionaryOfOrganTypes"] )
        print(self.organBank.organsList)

        # define cable bank
        self.cableBank = Bank ( configurationData [ "CABLE_BANK_1" ] )


        # connect to assembly fixture
        self.AF = AssemblyFixture ( configurationData ["ASSEMBLY_FIXTURE"] )

        # connect to UR5 (which includes the gripper)
        debugPrint("Connecting to UR5...")
        self.UR5 = UR5Robot( configurationData )
        debugPrint("Connected to UR5")

        # initialise a robot object (will be filled in setupRobotObject)
        self.myRobot = Robot( origin=self.printerLocation * self.skeletonPositionOnPrintbed )
        self.robotID = None

    def setupRobotObject(self, robotID:str):

        self.robotID = robotID

        # open blueprint and parse basic organ data
        debugPrint ( "making blueprintList list from the blueprint file: BP" + robotID , messageVerbosity=1 )
        blueprintList = []
        with open ( './blueprints/BP'+robotID+'.csv', newline='' ) as blueprintFile:
            blueprintReader = csv.reader ( blueprintFile, delimiter=' ', quotechar='|' )
            for rowString in blueprintReader:
                rowAsListOfStrings = rowString [ 0 ].split ( ',' )
                #Does read data on organs and converts to correct format
                #i = 0,1 type & parent ID,  i = 2-4 position in m, i = 5-7 rotation in radians
                blueprintRowToAppend: List[float] = [ int(i) for i in rowAsListOfStrings[0:2] ] + \
                                       [float(i) for i in rowAsListOfStrings[2:5]] + \
                                       [float(i) for i in rowAsListOfStrings[5:]]
                
                blueprintList.append(blueprintRowToAppend) # converted to meters & radians
        debugPrint( 'organ locations are: ' + str( blueprintList ) ,messageVerbosity=1 )


        # define all the required organs and cables to the robot object:
        debugPrint( "Defining the organs" ,messageVerbosity=1)
        for organ_raw_data in blueprintList: # n.b. the first row must be the core organ
            debugPrint ( "adding an organ of type " + str( organ_raw_data[1 ] ) ,messageVerbosity=2)
            self.myRobot.addOrgan (Organ ( organ_raw_data , self.dictionaryOfOrganTypes) )
                
            if not organ_raw_data[1] ==0: # not a core organ, so add a pull point
                self.myRobot.addPrintbedPullPoint(makeTransform(organ_raw_data[1:7]))

        # currently manually define the fixture rotation desired for each motor organ
        # todo: compute these properly
        # self.myRobot.organsList[1].requiredAssemblyFixtureRotationRadians = math.radians(30)
        # self.myRobot.organsList[1].flipGripperOrientation = True
        # self.myRobot.organsList[1].AssemblyFixtureRotationOffsetFudgeAngle = math.radians(1.5)
        # self.myRobot.organsList[2].requiredAssemblyFixtureRotationRadians = math.radians(30)
        # self.myRobot.organsList[2].flipGripperOrientation = True
        # self.myRobot.organsList[2].AssemblyFixtureRotationOffsetFudgeAngle = math.radians(-2)
        #
        # self.myRobot.organsList[3].requiredAssemblyFixtureRotationRadians = math.radians(120)
        # self.myRobot.organsList[3].flipGripperOrientation = False
        # self.myRobot.organsList[3].AssemblyFixtureRotationOffsetFudgeAngle = math.radians(-2)
        # self.myRobot.organsList[4].requiredAssemblyFixtureRotationRadians = math.radians(-60)
        # self.myRobot.organsList[4].flipGripperOrientation = False
        # self.myRobot.organsList[4].AssemblyFixtureRotationOffsetFudgeAngle = math.radians(0)


        #information on where the cables need to go - currently manually defined.
        # todo: create a method for self.myRobot.addCableBetween( Organ1, Organ2). This will need the organ object to include information on where the cable connection sites are, and keep track of which have been used already
        if DO_CABLE_INSERTIONS:
            debugPrint ( "manually defining some cables" ,messageVerbosity=1 )
            self.myRobot.addCable ( # sensor 2
                Cable (
                    transforms = [self.myRobot.organsList[0].positionTransformWithinBankOrRobot * self.myRobot.organsList[0].transformOrganOriginToCableSocket[0], # in core
                                  self.myRobot.organsList[4].positionTransformWithinBankOrRobot * np.linalg.inv(self.myRobot.organsList[4].transformOrganOriginToClipCentre) * self.myRobot.organsList[4].transformOrganOriginToCableSocket[0]],
                      gripPoint= configurationData["CABLE_BANK_1"]["CABLE_GRIP_POINT"]
                )
            )
            self.myRobot.addCable( # motor 1
                Cable(transforms = [self.myRobot.organsList[0].positionTransformWithinBankOrRobot * self.myRobot.organsList[0].transformOrganOriginToCableSocket[1], # in core
                                    self.myRobot.organsList[1].positionTransformWithinBankOrRobot * np.linalg.inv(self.myRobot.organsList[1].transformOrganOriginToClipCentre) * self.myRobot.organsList[1].transformOrganOriginToCableSocket[0]],
                      gripPoint= configurationData["CABLE_BANK_1"]["CABLE_GRIP_POINT"]
                      )
            )
            self.myRobot.addCable ( # motor 2
                Cable (
                    transforms = [self.myRobot.organsList[0].positionTransformWithinBankOrRobot * self.myRobot.organsList[0].transformOrganOriginToCableSocket[2], # in core
                                  self.myRobot.organsList[2].positionTransformWithinBankOrRobot * np.linalg.inv(self.myRobot.organsList[2].transformOrganOriginToClipCentre) * self.myRobot.organsList[2].transformOrganOriginToCableSocket[0]],
                      gripPoint= configurationData["CABLE_BANK_1"]["CABLE_GRIP_POINT"]
                )
            )
            self.myRobot.addCable ( # sensor 1
                Cable (
                    transforms = [self.myRobot.organsList[0].positionTransformWithinBankOrRobot * self.myRobot.organsList[0].transformOrganOriginToCableSocket[3], # in core
                                  self.myRobot.organsList[3].positionTransformWithinBankOrRobot * np.linalg.inv(self.myRobot.organsList[3].transformOrganOriginToClipCentre) * self.myRobot.organsList[3].transformOrganOriginToCableSocket[0]],
                      gripPoint= configurationData["CABLE_BANK_1"]["CABLE_GRIP_POINT"]
                )
            )

        

    def buildRobot( self ):

        # do the printing
        #todo: implement talking to the printer. For now, printer should be run manually in advance

        # attach the FIRST organ only, i.e. the core organ (which MUST be the first row of the blueprint file).
        # n.b. assemblyFixture=None because the robot is on printer at this point
        if DO_CORE_ORGAN_INSERT:
            debugPrint( "Core organ insert..." )
            actualCoreInsertedPosition = self.UR5.insertOrgan ( bank=self.organBank, robot=self.myRobot, assemblyFixture=None, gripperTCP=self.gripperTCP_A)
        else:
            self.myRobot.organInsertionTrackingList[0]=True # pretend we have done the core organ insert

        # remove from printbed, move to assembly fixture
        if DO_MOVE_FROM_PRINTER_TO_ASSEMBLY_FIXTURE:
            debugPrint( "Remove from printbed..." )
            self.UR5.removeRobotFromPrinter(self.myRobot,assemblyFixture=self.AF , gripperTCP=self.gripperTCP_A , actualDropoffPosition=actualCoreInsertedPosition)
        else:
            self.AF.turnElectromagnetsOn()
        self.myRobot.origin = self.AF.currentPosition

        # attach each organ in turn
        if DO_ORGAN_INSERTIONS:
            debugPrint( "Doing organ insertions" )
            while self.myRobot.hasOrgansNeedingInsertion:
                self.UR5.insertOrgan ( bank = self.organBank, robot=self.myRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_A )
        else:
            debugPrint( "Organ insertions skipped" )


        if DO_CABLE_INSERTIONS:
            if not DO_ORGAN_INSERTIONS:
                import time
                time.sleep(3)
            self.AF.setPosition(0)
            self.UR5.setGripperPosition(self.cableBank.GripperPowerForOpenPosition,"A")
            self.UR5.setGripperPosition(self.cableBank.GripperPowerForOpenPosition,"B")
            while self.myRobot.hasCablesNeedingInsertion:
                debugPrint( "Doing a cable insertion..." )
                self.UR5.insertCable( self.cableBank, self.myRobot, self.AF, self.gripperTCP_A, self.gripperTCP_B , postInsertExtraPushDistanceA = 1.0/1000 , postInsertExtraPushDistanceB = 4.0/1000 )
        else:
            debugPrint( "Cable insertions skipped" )

        self.UR5.moveBetweenStations("home")

        if DO_SEND_CONTROLLER_TO_ROBOT:
            debugPrint("Trying to connect")
            connection= RobotConnection("192.168.20.101")
            print(" ---------------- Robot ready  ---------------- ")
            input("Press any key to start controller")
            connection.robotStart('genome220')


            print(" ---------------- Robot running  ---------------- ")
            input("Press any key to stop controller")
            # input()
            import time
            # time.sleep(30)
            connection.closePort()
            print("done")

        # self.UR5.moveBetweenStations("organ_bank")
        # self.UR5.setTCP(self.gripperTCP_A)
        # self.UR5.moveArm( makeTransform([0,0,0.2]) * self.organBank.origin * makeTransform([0,0,0 , math.pi, 0 ,0]) )
        # self.UR5.moveBetweenStations("printer")
        # self.UR5.setTCP(self.gripperTCP_A)
        # self.UR5.moveArm( makeTransform([0,0,0.2]) * self.printerLocation * makeTransform([0,0,0 , math.pi, 0 ,0]) )
        # self.UR5.moveBetweenStations("AF")
        # self.UR5.setTCP(self.gripperTCP_A)
        # self.UR5.moveArm( makeTransform([0,0,-0.2]) * self.AF.originNoRotation * makeTransform([0,0,0 , 0, math.pi ,0]) * makeTransform([0,0,0,0,0,math.pi]) )

        return 0

    def disconnectAll( self ):
        self.UR5.stopArm()
        self.AF.disableStepperMotor()


## Run an example
if __name__ == "__main__":
    debugPrint("Running a demonstration of RoboFab",messageVerbosity=0)

    # Make the settings file then extract the settings from it
    makeConfigurationFile(location="BRL")
    configurationData = json.load(open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

    # startup
    RoboFab = RoboFab_host (configurationData)


    # open blueprint file
    # RoboFab.setupRobotObject ( robotID= "14104" )
    RoboFab.setupRobotObject ( robotID= "temp" )

    # make robot:
    RoboFab.buildRobot()


    debugPrint("RoboFab completed")

    # disconnect gracefully:
    RoboFab.UR5.disableServoOnFinish=True # as we have reached the end of the code, should be safe to release the gripper
    RoboFab.disconnectAll()
