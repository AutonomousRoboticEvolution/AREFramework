#! /usr/bin/env python
# -*- coding: utf-8 -*-

## Software to talk to all the various parts of the RoboFab
## goes from a single blueprint file to a single physical robot
import os
import shutil # for file copying
import time
from typing import List
import math
import csv # for loading blueprints
import json # for reading the configuration file
import numpy as np

from makeConfigurationFile import makeFile as makeConfigurationFile
from helperFunctions import debugPrint, makeTransformInputFormatted, makeTransformMillimetersDegrees, Timer
from RoboFabComponents import AssemblyFixture, Bank
from robotComponents import Robot, Organ, Cable, makeOrganFromBlueprintData
from UR5_host import UR5Robot
from robotConnection import RobotConnection
from printer import Printer

# debugging flags, human switchable to turn parts of the process on/off
DO_PRINT_SKELETON = 0
DO_CORE_ORGAN_INSERT = 1
DO_ORGAN_INSERTIONS = 1
DO_GO_HOME_AT_FINISH = 1
DO_TURN_MAGNETS_OFF = 1
DO_EXPORT_ORGANS_LIST=1


## top-level class. Call RoboFab.setupRobotObject(blueprint_file_name), then RoboFab.buildRobot()
class RoboFab_host:
    def __init__(self , configurationData):
        debugPrint("Creating a RoboFab_host object")


        # Defines properties of each organ type, set in the configuration file
        self.dictionaryOfOrganTypes = configurationData["dictionaryOfOrganTypes"]

        # set up / connect to printer(s), and an object for doing slicing
        if DO_PRINT_SKELETON or DO_CORE_ORGAN_INSERT:
            self.printer1 = Printer(IPAddress = configurationData [ "network" ] [ "PRINTER1_IP_ADDRESS" ] , config= configurationData [ "PRINTER_1" ])
        else:
            self.printer1 = Printer(IPAddress = None , config= configurationData [ "PRINTER_1" ])


        # Define the two gripper TCPs. This is a transform from the tool flange to the tip of each gripper
        self.gripperTCP_A = np.matrix( ( configurationData [ "gripper" ] [ "TCP_A" ] ) )
        self.gripperTCP_B = np.matrix( ( configurationData [ "gripper" ] [ "TCP_B" ] ) )

        # define the organ bank
        self.organBank = Bank ( configurationData [ "ORGAN_BANK_1" ] , configurationData["dictionaryOfOrganTypes"] )

        # connect to assembly fixture
        self.AF = AssemblyFixture ( configurationData ["ASSEMBLY_FIXTURE"] )

        # connect to UR5 (which includes the gripper)
        debugPrint("Connecting to UR5...")
        self.UR5 = UR5Robot( configurationData )
        debugPrint("Connected to UR5")

        # initialise a robot object (will be filled in setupRobotObject)
        self.myRobot = Robot( origin=self.printer1.origin * self.printer1.skeletonPositionOnPrintbed )
        self.robotID = None

        self.logDirectory = configurationData["logDirectory"]


    ## just makes the "robot" object within the code
    def setupRobotObject(self, robotID:str):

        self.robotID = robotID

        # open blueprint and parse basic organ data
        debugPrint ( "making blueprintList list from the blueprint file: blueprint_" + robotID , messageVerbosity=1 )
        blueprintList = []
        # with open ( './blueprints/blueprint'+robotID+'.csv', newline='' ) as blueprintFile:
        with open ( '/home/robofab/are-logs/test_are_generate/waiting_to_be_built/blueprint_'+robotID+'.csv', newline='' ) as blueprintFile:
            blueprintReader = csv.reader ( blueprintFile, delimiter=' ', quotechar='|' )
            for rowString in blueprintReader:
                rowAsListOfStrings = rowString [ 0 ].split ( ',' )
                print(rowAsListOfStrings[5:8])
                #Reads the information about organs and converts to correct format
                #i = 0,1 type & parent ID,  i = 2-4 position in m, i = 5-7 rotation in radians
                blueprintRowToAppend: List[float] = [ int(float(i)) for i in rowAsListOfStrings[0:2] ] + \
                                       [float(i) for i in rowAsListOfStrings[2:5]] + \
                                       [float(i) for i in rowAsListOfStrings[5:8]]
                
                blueprintList.append(blueprintRowToAppend) # converted to meters & radians
        debugPrint( 'organ locations are: ' + str( blueprintList ) ,messageVerbosity=1 )


        # define all the required organs and cables to the robot object:
        debugPrint( "Defining the organs" ,messageVerbosity=1)
        for organ_raw_data in blueprintList: # n.b. the first row must be the core organ
            debugPrint ( "adding an organ of type " + str( organ_raw_data[1 ] ) ,messageVerbosity=2)
            self.myRobot.addOrgan (
                makeOrganFromBlueprintData ( blueprintRow=organ_raw_data,dictionaryOfAllOrganTypes=self.dictionaryOfOrganTypes , gripper_TCP=self.gripperTCP_A)
            )

        # information on where the cables need to go - currently manually defined.
        # each organ that has a cable to connect (so organ.transformOrganOriginToMaleCableSocket is not None) needs to have a point defined where this will be put, called organ.cableDestination
        # todo: need to automatically compute which Head slot each organ should be attached to
        if DO_ORGAN_INSERTIONS:
            debugPrint ( "manually defining some cables" ,messageVerbosity=1 )
            i=-1
            for organ in self.myRobot.organsList:
                if organ.transformOrganOriginToMaleCableSocket is not None:
                    i+=1
                    organ.cableDestination = self.myRobot.organsList[0].positionTransformWithinBankOrRobot \
                                             * self.myRobot.organsList[0].transformOrganOriginToCableSocket[i] # socket slot in Head, relative to robot origin

    ## physically construct the robot
    def buildRobot( self ):
        timer=Timer()
        timer.start()
        self.UR5.moveBetweenStations("home")

        # do the slicing and printing
        if DO_PRINT_SKELETON:
            self.AF.disableStepperMotor() # prevent stepper wasting/energy getting hot during the printing
            self.printer1.printARobot(self.robotID, FAKE_SLICE_ONLY=False)
            self.AF.enableStepperMotor()
            self.AF.homeStepperMotor() # need to re-home after a period being disabled, in case it moved
        timer.add("Finished printing")

        # attach the FIRST organ only, i.e. the head organ (which MUST be the first row of the blueprint file).
        if DO_CORE_ORGAN_INSERT:
            debugPrint( "Core organ insert..." )
            self.UR5.insertHeadOrgan ( bank=self.organBank, printer=self.printer1, robot=self.myRobot, gripperTCP=self.gripperTCP_A , assemblyFixture=self.AF)
        else:
            self.myRobot.organInsertionTrackingList[0]=True # pretend we have done the core organ insert
            self.AF.turnElectromagnetsOn() # grip the robot
        self.myRobot.origin = self.AF.currentPosition # update the origin position of the robot now that it is on the assembly fixture
        timer.add("Finished Head Insert")


        # attach each organ in turn
        if DO_ORGAN_INSERTIONS:
            debugPrint( "Doing organ insertions" )
            while self.myRobot.hasOrgansNeedingInsertion:
                nextOrganFromRobot = self.myRobot.getNextOrganToInsert()
                if nextOrganFromRobot.transformOrganOriginToMaleCableSocket is None:
                    thisOrgan = self.UR5.insertOrganWithoutCable(bank=self.organBank, organInRobot=nextOrganFromRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_A)
                else:
                    thisOrgan = self.UR5.insertOrganWithCable ( bank = self.organBank, organInRobot=nextOrganFromRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_A )
        else:
            debugPrint( "Organ insertions skipped" )
        timer.add("Finished Organs Insert")


        if DO_GO_HOME_AT_FINISH:
            self.UR5.moveBetweenStations("home")
        timer.finish()

        if DO_EXPORT_ORGANS_LIST:
            self.save_log_files()

    def save_log_files( self ):
        file = open ( "robot_build_file.txt", "w" )
        # file.write ( "ID:{}\n".format(self.robotID) )
        for organ in self.myRobot.organsList:
            file.write ( "{},{}\n".format(organ.organType, organ.I2CAddress) )
        file.close ()

        # move build log specified log folder, and now that it is build delete the blueprint :
        print("{}/waiting_to_be_evaluated".format ( self.logDirectory ))
        print("{}/blueprint_archive".format(self.logDirectory))
        os.makedirs("{}/waiting_to_be_evaluated".format ( self.logDirectory ), exist_ok=True) # create the folder if it doesn't already exists
        shutil.move ( "robot_build_file.txt","{}/waiting_to_be_evaluated/list_of_organs_addresses_{}.csv".format ( self.logDirectory, self.robotID ) )
        print("Saved list of organs as: {}/waiting_to_be_evaluated/list_of_organs_addresses_{}.csv".format ( self.logDirectory, self.robotID ) )

        ## move the blueprint to the archive folder
        # os.makedirs("{}/blueprint_archive".format(self.logDirectory), exist_ok=True) # create the folder if it doesn't already exists
        # shutil.move ( "{}/waiting_to_be_built/blueprint{}.csv".format(self.logDirectory,self.robotID), "{}/blueprint_archive/blueprint{}.csv".format(self.logDirectory,self.robotID))

    def temp_limb_demo(self):
        ## temporary demo for limb assembly; these would need to be computed automatically based on robot morphology
        skeleton_pickup_point = makeTransformInputFormatted([0.4753, -0.0326, 0.086, math.pi, 0, 0])
        female_limb_assembly_fixture_position = makeTransformInputFormatted([-0.5122, -0.3891, -0.0610, math.pi, 0, 0])
        male_limb_assembly_fixture_position = makeTransformInputFormatted([-0.3133, -0.3741, -0.0304 , math.pi, 0, 0])
        joint_skeleton_dropoff_point = makeTransformInputFormatted([-0.3106, -0.4883, -0.0489 , math.pi, 0, 0])
        joint_skeleton_extra_push_distance = 4/1000 # positive means when clipping the skeleton to the joint in the fixture, it will move a bit further to make sure the clip attaches
        sensor_pickup_point = self.organBank.origin * makeTransformInputFormatted([0.1564, 0.1071, 0.0272]) * makeTransformInputFormatted([-0.006, 0, -0.0096, math.pi, 0, math.pi / 2])
        sensor_dropoff_point = makeTransformInputFormatted([-0.4926, -0.4481, -0.0560, math.pi, 0, 0])
        joint_pickup_point = self.organBank.origin * makeTransformInputFormatted([0.30006, 0.07508, 0.0335, 0, 0, math.radians(90)]) * makeTransformInputFormatted([-0.0415, 0, -0.0033 , math.pi, 0, math.pi / 2])

        # put joint onto limb fixture male
        self.UR5.moveBetweenStations("organ_bank")
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * joint_pickup_point)  # above joint
        self.UR5.moveArm(joint_pickup_point)
        self.UR5.setGripperPosition(0.5)  # close gripper
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * joint_pickup_point)  # picked up joint
        self.UR5.moveBetweenStations("limb_assembly_fixture")
        self.UR5.moveArm(male_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0, -0.03]))  # above the attachment point
        self.UR5.setMoveSpeed(self.UR5.speedValueReallySlow)
        self.UR5.moveCompliant(male_limb_assembly_fixture_position)  # do the attachment
        self.UR5.setMoveSpeed(self.UR5.speedValueNormal)
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(male_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0, -0.2]))  # above the attachment point

        ## limb skeleton pickup
        self.UR5.setGripperPosition(0.0)
        self.UR5.moveBetweenStations("printer")
        self.UR5.moveArm(skeleton_pickup_point * makeTransformInputFormatted([0, 0, -0.1]))
        self.UR5.moveArm(skeleton_pickup_point)
        self.UR5.setGripperPosition(0.5)  # close gripper
        self.UR5.moveArm(skeleton_pickup_point * makeTransformInputFormatted([0, 0, -0.1]))

        ## attach skeleton to female limb assembly fixture
        self.UR5.moveBetweenStations("limb_assembly_fixture")
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0.03, -0.2]))  # out and above
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0.03, 0.03]))  # out and below
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0, 0.03]))  # below
        self.UR5.setMoveSpeed(self.UR5.speedValueReallySlow)
        self.UR5.moveCompliant(female_limb_assembly_fixture_position)  # attached
        self.UR5.setMoveSpeed(self.UR5.speedValueNormal)
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0, -0.2]))  # up and out of the way

        # put sensor onto end of skeleton
        self.UR5.moveBetweenStations("organ_bank")
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * sensor_pickup_point)  # above sensor
        self.UR5.moveArm(sensor_pickup_point)
        self.UR5.setGripperPosition(0.5)  # close gripper
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * sensor_pickup_point)  # picked up sensor
        self.UR5.moveBetweenStations("limb_assembly_fixture")
        self.UR5.moveArm(sensor_dropoff_point * makeTransformInputFormatted([0, 0, -0.1]))  # above the attachment point
        self.UR5.moveArm(sensor_dropoff_point * makeTransformInputFormatted([0, 0, -0.03]))  # just above the attachment point
        self.UR5.setMoveSpeed(self.UR5.speedValueReallySlow)
        self.UR5.moveCompliant(sensor_dropoff_point)  # do the attachment
        self.UR5.setMoveSpeed(self.UR5.speedValueNormal)
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(sensor_dropoff_point * makeTransformInputFormatted([0, 0, -0.2]))  # above the attachment point

        # remove skeleton from female limb assembly fixture
        # self.UR5.moveBetweenStations("limb_assembly_fixture")
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0, -0.2]))  # above
        self.UR5.moveArm(female_limb_assembly_fixture_position)  # attached
        self.UR5.setGripperPosition(0.5)  # grip onto skeleton
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0, 0.03]))  # below
        self.UR5.moveArm(female_limb_assembly_fixture_position * makeTransformInputFormatted([0, 0.10, 0.03]))  # out and below

        # put skeleton (with other organ(s) attached) onto the joint (which in the male limb assembly fixutre)
        self.UR5.moveArm(joint_skeleton_dropoff_point * makeTransformInputFormatted([0, 0.03, 0.03]))  # out and below
        self.UR5.moveArm(joint_skeleton_dropoff_point * makeTransformInputFormatted([0, 0, 0.03]))  # below
        # self.UR5.setMoveSpeed(self.UR5.speedValueReallySlow)
        self.UR5.moveArm(joint_skeleton_dropoff_point * makeTransformInputFormatted([0, 0, -joint_skeleton_extra_push_distance]))  # do the attachment
        # self.UR5.setMoveSpeed(self.UR5.speedValueNormal)
        self.UR5.setGripperPosition(0.0)  # open gripper
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * joint_skeleton_dropoff_point)  # move up

        # remove completed limb!
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * male_limb_assembly_fixture_position)  # above pickup point
        self.UR5.moveArm(male_limb_assembly_fixture_position )  # pickup point
        self.UR5.setGripperPosition(0.5)  # grip onto joint
        self.UR5.moveArm(makeTransformInputFormatted([0, 0, 0.2]) * male_limb_assembly_fixture_position)  # above pickup point


    def disconnectAll( self ):
        self.UR5.stopArm()
        self.AF.disableStepperMotor()


## Run an example
if __name__ == "__main__":

    timestamp_start = time.time()

    debugPrint("Running a demonstration of RoboFab",messageVerbosity=0)

    # Make the settings file then extract the settings from it
    makeConfigurationFile(location="BRL") # <--- change this depending on if you're in York or BRL
    configurationData = json.load(open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

    # startup
    RoboFab = RoboFab_host (configurationData)

    # open blueprint file
    RoboFab.setupRobotObject ( robotID= "14_9" )


    # make robot:
    RoboFab.buildRobot()

    # disconnect gracefully:
    RoboFab.UR5.disableServoOnFinish=True # as we have reached the end of the code, should be safe to release the gripper
    RoboFab.disconnectAll()

    timestamp_finish = time.time()
    print("total time: {}\n=======".format(timestamp_finish-timestamp_start))
    debugPrint("RoboFab completed")
