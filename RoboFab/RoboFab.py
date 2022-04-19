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
        self.myRobot = None
        self.robotID = None

        self.logDirectory = configurationData["logDirectory"]


    ## just makes the "robot" object within the code
    def setupRobotObject(self, robotID:str, printer:Printer):

        self.robotID = robotID
        self.myRobot = Robot ( origin=printer.origin * printer.skeletonPositionOnPrintbed, ID=robotID )

        # open blueprint and parse basic organ data
        debugPrint ( "Loading the blueprint file: blueprint_" + robotID , messageVerbosity=0 )
        blueprintList = []
        # with open ( './blueprints/blueprint'+robotID+'.csv', newline='' ) as blueprintFile:
        with open ( '{}/waiting_to_be_built/blueprint_'.format(self.logDirectory)+robotID+'.csv', newline='' ) as blueprintFile:
            blueprintReader = csv.reader ( blueprintFile, delimiter=' ', quotechar='|' )
            for rowString in blueprintReader:
                rowAsListOfStrings = rowString [ 0 ].split ( ',' )
                #Reads the information about organs and converts to correct format
                #i = 0,1 type & parent ID,  i = 2-4 position in m, i = 5-7 rotation in radians
                blueprintRowToAppend: List[float] = [ int(float(i)) for i in rowAsListOfStrings[0:2] ] + \
                                       [float(i) for i in rowAsListOfStrings[2:5]] + \
                                       [float(i) for i in rowAsListOfStrings[5:8]]
                
                blueprintList.append(blueprintRowToAppend) # converted to meters & radians
        debugPrint( 'organ locations are: ' + str( blueprintList ) ,messageVerbosity=1 )


        # define all the required organs to the robot object:
        debugPrint( "Defining the organs" ,messageVerbosity=1)
        for organ_raw_data in blueprintList: # n.b. the first row must be the core organ
            debugPrint ( "adding an organ of type {} ({})".format(str( organ_raw_data[1] ) , self.dictionaryOfOrganTypes[str(organ_raw_data[1])]["friendlyName"]) ,messageVerbosity=2)
            self.myRobot.addOrgan (
                makeOrganFromBlueprintData ( blueprintRow=organ_raw_data,dictionaryOfAllOrganTypes=self.dictionaryOfOrganTypes , gripper_TCP=self.gripperTCP_A)
            )

        # information on where the cables need to go
        # each organ that has a cable to connect (so organ.transformOrganOriginToMaleCableSocket is not None) needs to have a point defined where this will be put, called organ.cableDestination
        self.myRobot.createLimbList()
        self.myRobot.determineCableDestinations()
        self.myRobot.drawRobot(self.logDirectory)

    # returns true if the bank has enough organs to provide all those required by the loaded robot
    def checkBankHasEnoughOrgans(self):
        # count the organs we need:
        organs_in_robot = {}
        for organ in self.myRobot.organsList:
            if organ.organType in organs_in_robot:
                organs_in_robot[organ.organType] += 1
            else:
                organs_in_robot[organ.organType] = 1

        # count the organs we have in the bank:
        organs_in_bank = self.organBank.countOrgansAvailable()

        # determine if there is enough of every type
        enoughOrgansAvailable=True
        for organType in organs_in_robot:
            if organType not in organs_in_bank: # this type of organ doesn't exist in the bank
                enoughOrgansAvailable=False
                debugPrint("Bank does not contain any organs of type {} (robot needs {})".format(organType,organs_in_robot[organType]))
            else:
                debugPrint("Organs type: {}, bank has: {}, robot needs {}".format(organType,organs_in_bank[organType],organs_in_robot[organType]))
                if organs_in_robot[organType] > organs_in_bank[organType]: # this type of organ exists in the bank, but more are needed than are in the bank
                    enoughOrgansAvailable=False
                    debugPrint("Bank does not contain enough organs of type {} (bank has {}, robot needs {})".format(organType,organs_in_bank[organType],organs_in_robot[organType]))
        return enoughOrgansAvailable


    ## physically construct the robot
    def buildRobot( self, printer:Printer ):
        timer=Timer()
        timer.start()
        self.UR5.moveBetweenStations("home")
        self.AF.homeStepperMotor ()  # need to re-home after a period being disabled, in case it moved. Will automatically enable if not already enabled.

        # attach the FIRST organ only, i.e. the head organ (which MUST be the first row of the blueprint file).
        if DO_CORE_ORGAN_INSERT:
            debugPrint( "Core organ insert..." )
            self.UR5.insertHeadOrgan ( bank=self.organBank, printer=printer, robot=self.myRobot, gripperTCP=self.gripperTCP_A, assemblyFixture=self.AF )
        else:
            self.AF.turnElectromagnetsOn() # grip the robot
        self.myRobot.origin = self.AF.currentPosition # update the origin position of the robot now that it is on the assembly fixture
        timer.add("Finished Head Insert")


        # attach each organ in turn
        if DO_ORGAN_INSERTIONS:
            debugPrint( "Doing organ insertions" )
            while not all ( [x.hasBeenInserted for x in self.myRobot.organsByLimbList[0]] ) :
                nextOrganFromRobot = self.myRobot.getNextOrganToInsert()
                if nextOrganFromRobot.transformOrganOriginToMaleCableSocket is None: # no cable to insert
                    thisOrgan = self.UR5.insertOrganWithoutCable(bank=self.organBank, organInRobot=nextOrganFromRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_A)
                else: # has a cable that needs connecting to Head
                    thisOrgan = self.UR5.insertOrganWithCable ( bank = self.organBank, organInRobot=nextOrganFromRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_A )
        else:
            debugPrint( "Organ insertions skipped" )
        timer.add("Finished Organs Insert")


        if DO_GO_HOME_AT_FINISH:
            self.UR5.moveBetweenStations("home")
        timer.finish()

        if DO_EXPORT_ORGANS_LIST:
            self.save_log_files()
            self.myRobot.drawRobot(self.logDirectory) # re-draw now that the organs have their i2c addresses assigned

        self.AF.disableStepperMotor ()  # prevent stepper wasting energy and getting hot while waiting, e.g. for the next print

    def save_log_files( self ):
        # write list_of_organs file
        os.makedirs("{}/waiting_to_be_evaluated".format ( self.logDirectory ), exist_ok=True) # create the folder if it doesn't already exists
        file = open ( "{}/waiting_to_be_evaluated/list_of_organs_{}.csv".format ( self.logDirectory, self.robotID ) , "w" )
        for organ in self.myRobot.organsList:
            file.write ( "{},{}\n".format(organ.organType, organ.I2CAddress) )
            # NOTE! the string of the address value saved into list_of_organs file is in decimal, e.g. 0x63 = "99"
        file.close ()

        # move the blueprint and mesh files to "archive" folder
        os.makedirs("{}/blueprint_archive".format ( self.logDirectory ), exist_ok=True) # create the folder if it doesn't already exists
        blueprint_old_path = os.path.join(self.logDirectory,"waiting_to_be_built","blueprint_{}.csv".format(self.robotID))
        mesh_old_path = os.path.join(self.logDirectory,"waiting_to_be_built","mesh_{}.stl".format(self.robotID))
        blueprint_new_path = os.path.join(self.logDirectory,"blueprint_archive","blueprint_{}.csv".format(self.robotID))
        mesh_new_path = os.path.join(self.logDirectory,"blueprint_archive","mesh_{}.stl".format(self.robotID))
        if os.path.exists(blueprint_new_path): os.remove(blueprint_new_path) # delete it if it already exists to avoid errors
        if os.path.exists(mesh_new_path): os.remove(mesh_new_path) # delete it if it already exists to avoid errors
        shutil.move(blueprint_old_path, blueprint_new_path)
        shutil.move(mesh_old_path, mesh_new_path)

        # move the morphology genome from waiting_to_be_built to waiting_to_be_evaluated:
        genome_old_path = os.path.join(self.logDirectory,"waiting_to_be_built","morph_genome_{}".format(self.robotID))
        genome_new_path = os.path.join(self.logDirectory,"waiting_to_be_evaluated","morph_genome_{}".format(self.robotID))
        if os.path.exists(genome_new_path): os.remove(genome_new_path)
        shutil.move ( genome_old_path, genome_new_path)

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

    if DO_CORE_ORGAN_INSERT:
        printer_number=0
        printer=Printer( configurationData["network"]["PRINTER{}_IP_ADDRESS".format(printer_number)], configurationData, printer_number=0 )
    else:
        printer=Printer(None, configurationData, printer_number=0)

    # startup
    RoboFab = RoboFab_host (configurationData)

    # open blueprint file
    RoboFab.setupRobotObject ( robotID= "14_9" , printer=printer)


    # make robot:
    RoboFab.buildRobot(printer)

    # disconnect gracefully:
    RoboFab.UR5.disableServoOnFinish=True # as we have reached the end of the code, should be safe to release the gripper
    RoboFab.disconnectAll()

    timestamp_finish = time.time()
    print("total time: {}\n=======".format(timestamp_finish-timestamp_start))
    debugPrint("RoboFab completed")
