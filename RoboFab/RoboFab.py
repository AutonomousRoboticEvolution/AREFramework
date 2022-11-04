#! /usr/bin/env python
# -*- coding: utf-8 -*-

## Software to talk to all the various parts of the RoboFab
## goes from a single blueprint file to a single physical robot
import os
import shutil # for file copying
import time
from typing import List
import math
import json # for reading the configuration file
import numpy as np

from makeConfigurationFile import makeFile as makeConfigurationFile
from helperFunctions import debugPrint, makeTransformInputFormatted, makeTransformMillimetersDegrees, Timer
from RoboFabComponents import AssemblyFixture, Bank
from robotComponents import Robot, Organ, Cable, makeOrganFromBlueprintData
from UR5_host import UR5Robot
from robotConnection import RobotConnection
from printer import Printer
from video_recorder import VideoRecorder

# debugging flags, human switchable to turn parts of the process on/off
DO_RECORD_VIDEO = 1
DO_CORE_ORGAN_INSERT = 1 #finishes with head and skeleton on assembly fixture
DO_ORGAN_INSERTIONS = 1
DO_GO_HOME_AT_FINISH = 1
DO_TURN_MAGNETS_OFF = 1


## top-level class. Call RoboFab.setupRobotObject(blueprint_file_name), then RoboFab.buildRobot()
class RoboFab_host:
    def __init__(self , configurationData):
        debugPrint("Creating a RoboFab_host object")


        # Defines properties of each organ type, set in the configuration file
        self.dictionaryOfOrganTypes = configurationData["dictionaryOfOrganTypes"]

        # Define the gripper TCP(s). This is a transform from the tool flange to the tip of the gripper
        self.gripperTCP_A = np.matrix( ( configurationData [ "gripper" ] [ "TCP_A" ] ) )
        self.gripperTCP_FOR_CABLES = np.matrix( ( configurationData [ "gripper" ] [ "TCP_FOR_CABLES" ] ) )

        # define the organ bank
        self.organBank = Bank ( configurationData [ "ORGAN_BANK_1" ] , configurationData["dictionaryOfOrganTypes"] )

        # connect to assembly fixture
        self.AF = AssemblyFixture ( configurationData ["ASSEMBLY_FIXTURE"] )

        # connect to UR5 (which includes the gripper)
        debugPrint("Connecting to UR5...")
        self.UR5 = UR5Robot( configurationData )
        debugPrint("Connected to UR5")

        # connect to the local webcam which will record the assembly process:
        if DO_RECORD_VIDEO: self.webcam = VideoRecorder( configurationData ["RoboFabWebcamPipe"] )

        # initialise a robot object (will be filled in setupRobotObject)
        self.myRobot = None
        self.robotID = None

        self.logDirectory = configurationData["logDirectory"]
        self.timer=Timer()


    ## just makes the "robot" object within the code
    def setupRobotObject(self, robotID:str, printer:Printer):

        self.robotID = robotID
        self.myRobot = Robot ( origin=printer.origin * printer.skeletonPositionOnPrintbed, ID=robotID )

        self.myRobot.createOrgansFromBlueprint('{}/waiting_to_be_built/blueprint_'.format(self.logDirectory)+robotID+'.csv', self.dictionaryOfOrganTypes, self.gripperTCP_A)

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

        if DO_RECORD_VIDEO: self.webcam.start_recording()

        self.timer.start()
        self.UR5.moveBetweenStations("home")
        self.AF.homeStepperMotor ()  # need to re-home after a period being disabled, in case it moved. Will automatically enable if not already enabled.

        # attach the FIRST organ only, i.e. the head organ (which MUST be the first row of the blueprint file).
        if DO_CORE_ORGAN_INSERT:
            debugPrint( "Core organ insert..." )
            self.UR5.insertHeadOrgan ( bank=self.organBank, printer=printer, robot=self.myRobot, gripperTCP=self.gripperTCP_A, assemblyFixture=self.AF )
        else:
            self.AF.turnElectromagnetsOn() # grip the robot
        self.myRobot.origin = self.AF.currentPosition # update the origin position of the robot now that it is on the assembly fixture
        self.timer.add("Finished Head Insert")
        self.save_log_files()  # calling this after every organ ensures that if the program crashes, we at least have the log for the progress so far


        # attach each organ in turn
        if DO_ORGAN_INSERTIONS:
            debugPrint( "Doing organ insertions" )
            while not all ( [x.hasBeenInserted for x in self.myRobot.organsByLimbList[0]] ) :
                nextOrganFromRobot = self.myRobot.getNextOrganToInsert()
                #if nextOrganFromRobot.transformOrganOriginToMaleCableSocket is None: # no cable to insert
                if nextOrganFromRobot.transformOrganOriginToGripper is None:
                    # use male cable to insert the organ
                    thisOrgan = self.UR5.insertOrganWithCable ( bank = self.organBank, organInRobot=nextOrganFromRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_FOR_CABLES )
                else: # use the defined gripper location to pick up the organ
                    thisOrgan = self.UR5.insertOrganUsingGripperFeature(bank=self.organBank, organInRobot=nextOrganFromRobot, assemblyFixture=self.AF, gripperTCP=self.gripperTCP_A)
                self.timer.add("finished organ of type {}, address {}".format(thisOrgan.friendlyName, thisOrgan.I2CAddress))
                self.save_log_files() # calling this after every organ ensures that if the program crashes, we at least have the log for the progress so far

        else:
            debugPrint( "Organ insertions skipped" )
        self.timer.add("Finished all organs")


        if DO_GO_HOME_AT_FINISH:
            self.UR5.moveBetweenStations("home")
        self.timer.finish()


        self.save_log_files()
        self.move_files()

        if DO_RECORD_VIDEO: self.webcam.save_recording( "{}/assembly_videos/assembly_{}".format( self.logDirectory, self.robotID ) )

        self.AF.disableStepperMotor ()  # prevent stepper wasting energy and getting hot while waiting, e.g. for the next print
        self.UR5.gripper.disableServos()

    def save_log_files( self ):

        self.myRobot.drawRobot(self.logDirectory)  # re-draw with all known organ i2c addresses included
        self.timer.save(self.logDirectory, self.robotID)

        # write list_of_organs file
        os.makedirs("{}/waiting_to_be_evaluated".format ( self.logDirectory ), exist_ok=True) # create the folder if it doesn't already exists
        file = open ( "{}/waiting_to_be_evaluated/list_of_organs_{}.csv".format ( self.logDirectory, self.robotID ) , "w" )
        for organ in self.myRobot.organsList:
            file.write ( "{},{}\n".format(organ.organType, organ.I2CAddress) )
            # NOTE! the string of the address value saved into list_of_organs file is in decimal, e.g. 0x63 = "99"
        file.close ()

    def move_files(self):
        os.makedirs("{}/blueprint_archive".format ( self.logDirectory ), exist_ok=True) # create the archive folder if it doesn't already exists

        # move the mesh file to "archive" folder
        mesh_old_path = os.path.join(self.logDirectory,"waiting_to_be_built","mesh_{}.stl".format(self.robotID))
        if os.path.exists(mesh_old_path):
            mesh_new_path = os.path.join(self.logDirectory,"blueprint_archive","mesh_{}.stl".format(self.robotID))
            if os.path.exists(mesh_new_path): os.remove(mesh_new_path) # delete it if it already exists to avoid errors
            shutil.move(mesh_old_path, mesh_new_path)
        else:
            debugPrint("WARNING: {} does not exist".format(mesh_old_path),messageVerbosity=0)

        # move the blueprint file to "archive" folder
        blueprint_old_path = os.path.join(self.logDirectory,"waiting_to_be_built","blueprint_{}.csv".format(self.robotID))
        if os.path.exists(blueprint_old_path):
            blueprint_new_path = os.path.join(self.logDirectory,"blueprint_archive","blueprint_{}.csv".format(self.robotID))
            if os.path.exists(blueprint_new_path): os.remove(blueprint_new_path) # delete it if it already exists to avoid errors
            shutil.move(blueprint_old_path, blueprint_new_path)
        else:
            debugPrint("WARNING: {} does not exist".format(blueprint_old_path),messageVerbosity=0)

        # move the morphology genome from waiting_to_be_built to waiting_to_be_evaluated:
        genome_old_path = os.path.join(self.logDirectory,"waiting_to_be_built","morph_genome_{}".format(self.robotID))
        if os.path.exists(genome_old_path):
            genome_new_path = os.path.join(self.logDirectory,"waiting_to_be_evaluated","morph_genome_{}".format(self.robotID))
            if os.path.exists(genome_new_path): os.remove(genome_new_path)
            shutil.move(genome_old_path, genome_new_path)
        else:
            debugPrint("WARNING: {} does not exist".format(genome_old_path),messageVerbosity=0)

    def disconnectAll( self ):
        self.UR5.stopArm()
        self.AF.disableStepperMotor()
        if DO_RECORD_VIDEO: self.webcam.disconnect()


## Run an example
if __name__ == "__main__":

    timestamp_start = time.time()

    debugPrint("Running a demonstration of RoboFab",messageVerbosity=0)

    # Make the settings file then extract the settings from it
    with open('location.txt') as f:
        location = f.read().replace("\n", "")
        print("location: {}".format(location))
    makeConfigurationFile(location=location) # <--- change this depending on if you're in York or BRL
    configurationData = json.load(open('configuration_{}.json'.format(location)))  # <--- change this depending on if you're in York or BRL

    printer_number=0 #set which printer to use

    if DO_CORE_ORGAN_INSERT:
        printer=Printer( configurationData["network"]["PRINTER{}_IP_ADDRESS".format(printer_number)], configurationData, printer_number=printer_number )
    else:
        printer=Printer(None, configurationData, printer_number=printer_number)

    # startup
    RoboFab = RoboFab_host (configurationData)

    # open blueprint file
    RoboFab.setupRobotObject ( robotID= "test0" , printer=printer)

    if not RoboFab.checkBankHasEnoughOrgans():
        raise RuntimeError("Bank does not have enough organs")

    # make robot:
    RoboFab.buildRobot(printer)

    # disconnect gracefully:
    RoboFab.UR5.disableServoOnFinish=True # as we have reached the end of the code, should be safe to release the gripper
    RoboFab.disconnectAll()

    timestamp_finish = time.time()
    print("total time: {}\n=======".format(timestamp_finish-timestamp_start))
    debugPrint("RoboFab completed")
