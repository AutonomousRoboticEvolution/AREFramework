##

#! /usr/bin/env python
# -*- coding: utf-8 -*-

import socket # for talking to UR5 over ethernet
import serial # for talking to arduino to connect to RF gripper
import time
import threading
import math
import numpy as np
from helperFunctions import debugPrint, makeTransform, connect2USB, convertPoseToTransform, convertTransformToPose, findAngleBetweenTransforms
from toolSerialCommunication import GripperHandler

# temporary, experimental settings - may be incorporated into config file later
USE_FORCE_FOR_ASSEMBLY_FIXTURE_ATTACHMENT = False
GRIPPER_OPEN_POSITION_FOR_CLIPS = 0.5 # 1=fully closed, 0=fully open
DO_PULL_ON_EACH_CLIP = False

## Class to handle the communication with the UR5
#
#  Talks to the UR5. Able to do things like request the current position and execute movements to given positions.
#  When a new command is issued, will wait for the arm to report itself as ready for a new command before sending it. The functions will only return once this has happened.
#
# "TCP" is the "tool centre point". This defines the location of the nominal "tip" of the gripper from the
# flange of the UR5. This will be changed on-the-fly to switch between grippers. It will also be changed to
# represent the clip of the organ/cable when the gripper is holding one.
class UR5Robot:
    def __init__ ( self, configurationData ): # here configuration Data should be the top level, to include the "gripper" field as well as "UR5"

        # extract settings from configurationData
        self.HOME_POSITIONS = configurationData ["UR5"] ["HOME_POSITIONS"]
        self.disableServoOnFinish = configurationData ["UR5"] ["disableServoOnFinish"]

        # connect to the remote controlled gripper via arduino:
        self.gripper = GripperHandler ( configurationData)

        # Set up ethernet connection to the UR5 control box:
        laptopAddress = configurationData ["network"] ["COMPUTER_ADDRESS"]
        Port = configurationData ["network"] ["PORT_FOR_UR5"]
        self.UR5Socket = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
        self.UR5Socket.setsockopt ( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
        self.UR5Socket.bind ( (laptopAddress, Port) )
        self.UR5Socket.listen ( 5 )
        self.c, self.robotAddress = self.UR5Socket.accept ()
        debugPrint ( "UR5 with address " + str ( self.robotAddress [ 0 ] ) + " has connected" )
        self.isRunning = True



        self.poseDataReceived = [ 0.0 ] * 6
        self.homePosition = makeTransform()
        self.isReadyForNextCommand = False  # flag to make sure previous command is finished
        # flags for managing the receipt of pose data:
        self.__isExpectingHomePosition = False
        self.__isExpectingRequestedPose = False
        self.requestedPoseIsReady = False  # tells the outside world when we have received a pose
        self.requestedPose = [ 0.0 ] * 6
        self.currentStation='home'

        self.speedValueNormal = configurationData["UR5"]["speedValueNormal"]
        self.speedValueSlow = configurationData["UR5"]["speedValueSlow"]
        self.speedValueReallySlow = configurationData["UR5"]["speedValueReallySlow"]

        threading.Thread ( target=self.__threadedListener ).start () # starting in tread allows messages to be received in the background

        # wait for the UR to be set-up (this happens in the other thread we just started):
        while not self.UR5isConnected:
            time.sleep ( 0.1 )
        debugPrint("UR5 is setup and ready")
        self.setMoveSpeed ( self.speedValueNormal )
        debugPrint("UR5 set up, going to home")
        self.moveArmToJointAngles( self.HOME_POSITIONS[ "home" ] )
        debugPrint("Home")

    ## function to make the large movements between different areas ("stations"), e.g. printer, assembly fixture, bank etc.
    # This will do a joni-space move between the pre-defined home positions for each station (defined as joint values in the config file)
    # The input should be a string, the name of the station to move to, as defined in the config file
    def moveBetweenStations( self , stationToMoveTo="home" ):
        debugPrint("moving from station "+str(self.currentStation)+" to station "+str(stationToMoveTo))

        currentStationCrouched =  [self.HOME_POSITIONS[self.currentStation][0]] + self.HOME_POSITIONS["home"][1:5] + [self.HOME_POSITIONS[self.currentStation][5]]
        destinationStationCrouched =  [self.HOME_POSITIONS[stationToMoveTo][0]] + self.HOME_POSITIONS["home"][1:5] + [self.HOME_POSITIONS[stationToMoveTo][5]]

        self.moveArmToJointAngles( self.HOME_POSITIONS[self.currentStation ] )
        if stationToMoveTo != self.currentStation:
            self.moveArmToJointAngles( currentStationCrouched )
            self.moveArmToJointAngles( destinationStationCrouched )
            self.moveArmToJointAngles( self.HOME_POSITIONS[stationToMoveTo ] )
            self.currentStation = stationToMoveTo

    ## Low-level function to send a text string to UR5 controller
    def sendString ( self, stringToSend ):
        debugPrint("Sending string to UR5: "+stringToSend , messageVerbosity=3)
        self.c.send ( stringToSend.encode ( 'ASCII' ) )
        time.sleep(0.01)

    ## Tells arm to stop and closes socket
    def stopArm ( self ):
        if self.isRunning:
            self.sendString ( "stop" )  # command for arm to stop
            self.isRunning=False
        self.UR5Socket.close ()
        if self.disableServoOnFinish:
            self.gripper.disableServos()

    # Will return only once the UR5 reports it is ready for the next command
    #  this should be called at the start of any movement functions etc. to make sure that any previous command finishes before the new one is sent.
    def waitForArmToBeReady ( self ):
        while not self.isReadyForNextCommand:  # wait for ready
            time.sleep ( 0.1 )
            # debugPrint( "Waiting for arm to be ready..." )
        self.isReadyForNextCommand = False


    ## Linear movement
    #  Executes the "movel" URScript command, to create a linear motion from the current position to positionTransform (in UR5 base frame)
    def moveArm ( self, positionTransform ):
        poseToSend = convertTransformToPose ( positionTransform )  # convert to "pose" format as expected by UR5
        self.waitForArmToBeReady ()
        self.sendString ( "movel" )  # command for linear move
        time.sleep ( 0.01 )  # prevents the two packages getting merged
        self.sendString ( "(" + str ( poseToSend ) [ 1:-1 ] + ")" )

    ## Movement where the position is specified in cartesian (transformation matrix), but movement will be linear in the joint space.
    #  Executes the "movej" URScript command.
    def moveArmLinearInJointSpace ( self, transformToSend ):
        poseToSend = convertTransformToPose ( transformToSend )  # convert to "pose" format as expected by UR5
        self.waitForArmToBeReady ()
        self.sendString ( "move_j" )  # command for linear move
        time.sleep ( 0.01 )  # prevents the two packages getting merged
        self.sendString ( "(" + str ( poseToSend ) [ 1:-1 ] + ")" )    ## Joint space movement

    ## Movement where the position is specified as a set of target joint positions (in radians)
    def moveArmToJointAngles ( self, listOfJointAngles ):
        self.waitForArmToBeReady ()
        self.sendString ( "move_j_joints" )  # command for linear move
        time.sleep ( 0.01 )  # prevents the two packages getting merged
        self.sendString ( "(" + str ( listOfJointAngles ) [ 1:-1 ] + ")" )

    def setHome( self , home ):
        self.homePosition=home

    # TCP = tool center point = transform so the arm uses it inverse kinematics to move the end of the gripper to the right place
    def setTCP( self, transformToSend ):
        poseToSend = convertTransformToPose ( transformToSend )  # convert to "pose" format as expected by UR5
        self.waitForArmToBeReady ()
        time.sleep ( 0.1 )
        self.sendString ( "set_tcp" )
        time.sleep ( 0.01 )  # prevents the two packages getting merged
        self.sendString ( "(" + str ( poseToSend ) [ 1:-1 ] + ")" )


    ## Force mode movement
    #  Executes a "movel" URScript command in force mode with zero force demand, such that the end effector is free to move in the x, y and rz directions, in the end effector frame.
    def moveCompliant ( self, transformToSend ):
        poseToSend = convertTransformToPose ( transformToSend )  # convert to "pose" format as expected by UR5
        self.waitForArmToBeReady ()
        self.sendString ( "move_insert" )  # command for "force mode" move
        time.sleep ( 0.1 )  # prevents the two packages getting merged
        self.sendString ( "(" + str ( poseToSend ) [ 1:-1 ] + ")" )

    ## Force mode movement designed for bed removal
    #  Executes a "movel" URScript command in force mode with a force in z and torque about x and/or y axes
    # The forces/torques to be applied need to be set in the polyscope program
    def moveWithForceForBedRemoval ( self, transformToSend ):
        poseToSend = convertTransformToPose ( transformToSend )  # convert to "pose" format as expected by UR5
        self.waitForArmToBeReady ()
        self.sendString ( "move_bed_removal" )  # command for "force mode" move
        time.sleep ( 0.1 )  # prevents the two packages getting merged
        self.sendString ( "(" + str ( poseToSend ) [ 1:-1 ] + ")" )



    ## Set the move speed for subsequent move commands
    #
    #  Saves a variable on the UR5 which is used for the speed in all subsequent move commands (including moveArm and moveCompliant)
    def setMoveSpeed ( self, speed ):
        self.waitForArmToBeReady ()
        self.sendString ( "set_move_speed" )
        time.sleep ( 0.1 )
        self.sendString ( "(" + str ( speed ) + ")" )
        debugPrint( "Move speed set to: " + str( speed ) ,messageVerbosity=2)

    # This is a dummy message to send to the arm.
    # By doing this (for example, before activating the gripper), it should ensure the UR5 has actually reached the point previously commanded.
    # Otherwise, it may still be mid-motion, causing the gripper to activate too soon
    def sendNothingToArm(self):
        self.waitForArmToBeReady()
        # time.sleep(0.1)
        self.sendString('nothing')  # to reset waitForArmToBeReady

    def setGripperPosition (self, newGripperPower:float, AorB:str = 'A'):
        # set newGripperPower: 0.0 = fully open, 1.0 = fully closed
        # this function just passes the values down to the gripper, after ensuring the UR5 is in position using the sendNothingToArm() function
        self.sendNothingToArm()
        self.gripper.setGripperPosition (newGripperPower,AorB)

    ## Get the current end effector position as a transform matrix (from UR base frame)
    def getCurrentPosition ( self ):
        #  find out where the arm is at the moment
        self.waitForArmToBeReady ()
        self.requestedPoseIsReady = False  # ensure flag starts as false
        self.sendString ( "request_pose" )  # command for asking for the current pose
        while not self.requestedPoseIsReady:
            time.sleep ( 0.1 )
        self.requestedPoseIsReady = False  # reset for next time
        return self.requestedPose

    ##  Handles receipt of messages from the UR5.
    #  Should be started as a thread after the socket has been set up.
    def __threadedListener ( self ):
        self.UR5isConnected = False # start not connected
        while True:
            msg = self.c.recv ( 1024 )
            debugPrint ( "Received message: " + str ( msg ) ,messageVerbosity=2)
            if msg:
                if msg == b'UR_ready':
                    # print("UR is ready")
                    self.isReadyForNextCommand = True
                    if not self.UR5isConnected:
                        self.UR5isConnected = True
                elif msg == b'starting_point':
                    self.__isExpectingHomePosition = True
                    debugPrint("__isExpectingHomePosition set true",messageVerbosity=3)
                elif msg == b'sending_pose':
                    self.__isExpectingRequestedPose = True
                elif msg.decode ( 'utf-8' ) [ 0:2 ] == 'p[':
                    # is a pose
                    pose_data = [ float ( x ) for x in msg [ 2:-1 ].decode ( 'utf-8' ).split ( "," ) ]
                    debugPrint ( "Received pose data is: " + str ( pose_data ) ,messageVerbosity=2 )
                    if self.__isExpectingHomePosition:
                        self.__isExpectingHomePosition = False
                        self.homePosition = convertPoseToTransform ( pose_data )
                        self.UR5isConnected = True # this is a flag to the main UR5_host that we have connected and can
                        # start doing stuff. It works because the UR5 polyscope program sends it's starting location as
                        # the first thing it does.
                        debugPrint ( "Home pos set" ,messageVerbosity=2)
                    elif self.__isExpectingRequestedPose:
                        self.__isExpectingRequestedPose = False
                        self.requestedPose = convertPoseToTransform ( pose_data )
                        self.requestedPoseIsReady = True
                        debugPrint ( "received a pose from UR5" ,messageVerbosity=2)
                    else:
                        debugPrint ( "WARNING Pose data received when not expected" )
                else:
                    debugPrint ( "WARNING Unexpected message: " + msg.decode ( 'utf-8' ) )

            else: # the socket has disappeared - this happens if the UR code crashes, or when the "stop" message is sent to the UR5
                debugPrint ( 'WARNING Disconnected from UR arm' )
                self.UR5isConnected = False
                self.isRunning=False
                self.stopArm()
                break

    # core organ should already be inserted when this is called
    def removeRobotFromPrinter( self , robot, assemblyFixture ,gripperTCP ,actualDropoffPosition ):
        coreOrgan = robot.organsList[0]
        verticalClearanceBeforePickingUp = 0.120
        verticalClearanceForPostPickup = 0.025
        bedRemovalPullUpAngle = math.radians( 30 )
        postInsertExtraPushDistance = assemblyFixture.CORE_ORGAN_ATTACHMENT_Z_OFFSET
        heightAboveClipApproach = 20.0/1000
        clipPullGripTransform = makeTransform([0,0,0 , math.radians(90),0,0]) # todo: work this out!
        useForceModeForBedRemoval = False
        pushBackIntoBedAfterPickup = True
        pushBackIntoBedPosition = robot.origin * coreOrgan.positionTransformWithinBankOrRobot * makeTransform([0,0,-coreOrgan.postInsertExtraPushDistance])
        distanceForCompliantMove = 10/1000 if USE_FORCE_FOR_ASSEMBLY_FIXTURE_ATTACHMENT else 0

        assemblyFixture.setPosition(0)
        assemblyFixtureLocation = assemblyFixture.currentPosition

        # self.moveBetweenStations("printer")

        ## todo: pull on each pull location?
        if DO_PULL_ON_EACH_CLIP:
            self.setTCP ( clipPullGripTransform )
            self.setGripperPosition (GRIPPER_OPEN_POSITION_FOR_CLIPS)
            for i in range(len(robot.printbedPullPoints)):
                clipPosition = robot.origin*robot.printbedPullPoints[i]
                self.moveArm(makeTransform([0,0,verticalClearanceBeforePickingUp]) * clipPosition * makeTransform([0,0,-heightAboveClipApproach]) ) #back and above
                self.moveArm( clipPosition * makeTransform([0,0,-heightAboveClipApproach]) ) #back
                self.moveArm( clipPosition ) #clip position
                self.setGripperPosition(1.0)
                self.moveCompliant( makeTransform([0,0,heightAboveClipApproach]) * clipPosition ) #straght up
                self.moveArm( clipPosition ) # back to starting (theoretical) clip position
                self.setGripperPosition(GRIPPER_OPEN_POSITION_FOR_CLIPS)
                self.moveArm( clipPosition * makeTransform([0,0,-heightAboveClipApproach]) ) #back
                self.moveArm(makeTransform([0,0,verticalClearanceBeforePickingUp]) * clipPosition * makeTransform([0,0,-heightAboveClipApproach]) ) #back and above

        ## pick up by the core organ and take to assembly fixture:
        gripPosition = np.linalg.inv(coreOrgan.transformOrganOriginToGripper) * coreOrgan.transformOrganOriginToClipCentre
        if gripPosition is None:
            raise Exception("gripPosition has not been set")
        # self.setGripperPosition (coreOrgan.gripperOpenPosition)

        # compute all relevant locations. The "prePickup" and "preDropoff" are so the end effector approaches from the right direction.
        # note these are all transform matrices (not poses) and the desired position of the organ origin at these points
        pickupPoint = actualDropoffPosition
        prePickupPoint = makeTransform (
            [ 0, 0, verticalClearanceBeforePickingUp ] ) * pickupPoint  # point above pickup point by verticalClearanceBeforePickingUp
        # postPickupPoint = makeTransform(
        #     [0, -verticalClearanceBeforePickingUp*math.tan(bedRemovalPullUpAngle), verticalClearanceBeforePickingUp]) * pickupPoint  # point move to just after grabbing - defined to pull up at an angle
        postPickupPoint = pickupPoint * makeTransform([verticalClearanceForPostPickup*math.tan(bedRemovalPullUpAngle) , -verticalClearanceForPostPickup*math.tan(bedRemovalPullUpAngle) , verticalClearanceForPostPickup])# point move to just after grabbing - designed to pull up at an angle
        dropoffPoint = assemblyFixtureLocation * coreOrgan.positionTransformWithinBankOrRobot * makeTransform (
            [ 0, 0, -postInsertExtraPushDistance ] )
        preDropoffPoint = dropoffPoint * makeTransform (
            [ 0, 0, verticalClearanceBeforePickingUp ] )  # point back from (in organ frame) final position
        postDropoffPoint = makeTransform ([ 0, 0, -verticalClearanceBeforePickingUp ] )  * dropoffPoint # linear move back toward the "safe" position
        preForceModePoint = dropoffPoint * makeTransform (
            [ 0, 0,
              distanceForCompliantMove + postInsertExtraPushDistance ] )  # point back from final position from which to start force mode

        # pickup:
        # self.setTCP ( gripperTCP * np.linalg.inv ( gripPosition ) )
        # self.moveArm ( prePickupPoint )  # move to new TCP
        # self.setMoveSpeed ( self.speedValueSlow )  # slow
        # self.moveArm ( pickupPoint )  # pickup point
        # self.setGripperPosition(coreOrgan.gripperClosedPosition) # close
        self.setMoveSpeed ( self.speedValueReallySlow )  # really slow for peeling off bed
        if useForceModeForBedRemoval:
            self.moveWithForceForBedRemoval ( postPickupPoint )
        self.moveArm ( postPickupPoint )
        if pushBackIntoBedAfterPickup:
            self.moveArm(pickupPoint*makeTransform([0,0,verticalClearanceForPostPickup]))
            self.moveArm(pushBackIntoBedPosition)
        self.setTCP ( gripperTCP * gripPosition )
        self.moveArm(prePickupPoint)

        self.setMoveSpeed ( self.speedValueNormal )  # normal

        # go to to AF
        self.moveBetweenStations("AF_core")
        # now on the Assembly fixture, turn on the magnets
        self.sendNothingToArm()
        assemblyFixture.turnElectromagnetsOn()

        # dropoff:
        self.moveArmLinearInJointSpace ( preDropoffPoint )  # above (or below, in real world...) dropoff
        self.setMoveSpeed ( self.speedValueSlow )  # slow

        # force mode:
        if USE_FORCE_FOR_ASSEMBLY_FIXTURE_ATTACHMENT:
            self.moveArm ( preForceModePoint )
            self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
            self.moveCompliant (dropoffPoint )  # "force mode" compliant move for last section to dropoff point

        else:
            self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
            self.moveArm ( dropoffPoint )


        self.setMoveSpeed ( self.speedValueNormal )
        actualDropoffPosition = self.getCurrentPosition ()  # for seeing how far off the expected position we were
        self.setGripperPosition (coreOrgan.gripperOpenPosition)
        self.moveArm ( postDropoffPoint)  # back off

        # go to "home"
        self.moveBetweenStations("home")
        self.setGripperPosition(0.0) # open

        misalignment = [ a_i - b_i for a_i, b_i in zip ( convertTransformToPose ( actualDropoffPosition ),
                                                         convertTransformToPose ( dropoffPoint ) ) ]
        return misalignment


    ## Pickup an organ and insert it
    #
    #  A high-level function to perform an organ insertion.
    #  Will find a suitable organ in the bank, pick it up, insert it into the right location and then send UR home.
    #  Takes inputs:
    #  bank: where to search for the organ
    #  robot: the robot class being inserted into
    def insertOrgan ( self, bank, robot, assemblyFixture, gripperTCP ):
        # assemblyFixture is None when inserting into the printer, ie the core organ
        vertical_clearance = 0.10  if assemblyFixture is None else -0.20 # how high vertically above the pickup and preDropoff to for pre-pickup and abovePreDropoff

        organInRobot = robot.getNextOrganToInsert ()  ## get the next organ which needs to be inserted
        # if organInRobot.organType in ORGANS_TO_ACTUALLY_DO:

        if organInRobot.flipGripperOrientation:
            gripperTCP = gripperTCP * makeTransform([0,0,0,0,0,math.radians(180)])

        preDropoffDistanceUp = 0.02 # how far back in the end effector frame to go just before pushing in the organ

        debugPrint ( "Inserting organ of type: " + str (organInRobot.friendlyName) ,messageVerbosity=2)
        useForceMode=organInRobot.useForceMode

        distanceForCompliantMove = organInRobot.forceModeTravelDistance  # distance at end of insertion for which "force mode" is used on UR5
        postInsertExtraPushDistance = organInRobot.postInsertExtraPushDistance
        debugPrint("postInsertExtraPushDistance: "+str(postInsertExtraPushDistance),messageVerbosity=3)
        # gripPosition = organInRobot.gripPosition  # transform of organ origin to required point of gripper tip
        # if gripPosition is None:
        #     RuntimeError("no grip Position set")

        self.moveBetweenStations("organ_bank")
        self.setGripperPosition (organInRobot.gripperOpenPosition)

        # rotate assembly fixure
        if assemblyFixture is None:
            originPosition = robot.origin # no assembly fixture, ie. robot still on printer bed, use the saved origin position
        else:
            debugPrint("In insert organ, trying to turn AF to: "+str(organInRobot.requiredAssemblyFixtureRotationRadians),messageVerbosity=2)
            assemblyFixture.setPosition(organInRobot.requiredAssemblyFixtureRotationRadians)
            originPosition = assemblyFixture.currentPosition * makeTransform([0,0,0,0,0, organInRobot.AssemblyFixtureRotationOffsetFudgeAngle ])

        # first we need to find a suitable organ in the bank.
        organFromBank = bank.findAnOrgan ( organInRobot.organType )
        organInRobot.I2CAddress = organFromBank.I2CAddress   # transfer over the i2c address

        # compute all relevant locations. The "prePickup" and "preDropoff" are so the end effector approaches from the right direction.
        # note these are all transform matrices (not poses) and the desired position of the organ origin at these points
        pickupPoint = bank.origin * organFromBank.positionTransformWithinBankOrRobot * makeTransform([0,0,organFromBank.pickupExtraPushDistance])
        prePickupPoint = makeTransform ([ 0, 0, abs(vertical_clearance) ] ) * pickupPoint  # point above pickup point by vertical_clearance
        dropoffPoint =  originPosition * organInRobot.positionTransformWithinBankOrRobot
        dropoffPointWithExtraPush =  originPosition * organInRobot.positionTransformWithinBankOrRobot * makeTransform( [ 0, 0, -postInsertExtraPushDistance ] )
        preDropoffPoint = dropoffPoint * makeTransform ( [ 0, 0, preDropoffDistanceUp ] )  # point back from (in organ clip frame) final position so we approach from right direction
        preForceModePoint = dropoffPoint * makeTransform (
            [ 0, 0, distanceForCompliantMove ] )  # point back from final position from which to start force mode
        abovePreDropoffPoint = makeTransform ( [ 0, 0,vertical_clearance ] ) * preDropoffPoint  # point above (in world frame) predropoff point by vertical_clearance

        # pickup:
        self.setTCP(gripperTCP * np.linalg.inv(organInRobot.transformOrganOriginToGripper))  # TCP for pickup is organ origin
        debugPrint("move to prePickupPoint",messageVerbosity=2)
        self.moveArm ( prePickupPoint )  # move to new TCP
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        debugPrint("move to pickupPoint",messageVerbosity=2)
        self.moveArm ( pickupPoint )  # pickup point
        debugPrint("close gripper",messageVerbosity=2)
        self.setGripperPosition (organInRobot.gripperClosedPosition)
        debugPrint("move to prePickupPoint",messageVerbosity=2)
        self.moveArm ( prePickupPoint )
        self.setMoveSpeed ( self.speedValueNormal )  # normal

        if assemblyFixture is None: # robot is still on printer
            self.moveBetweenStations("printer")
            dropoffPointWithExtraPush = dropoffPointWithExtraPush * makeTransform([0, 0, 0, 0, math.radians(1), 0])
        else:
            self.moveBetweenStations("AF")
        # dropoff:
        self.setTCP(gripperTCP * np.linalg.inv(organInRobot.transformOrganOriginToGripper) * organInRobot.transformOrganOriginToClipCentre)  # TCP for pickup is centre of clip
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        debugPrint("move to abovePreDropoffPoint",messageVerbosity=2)
        self.moveArmLinearInJointSpace ( abovePreDropoffPoint )  # above dropoff
        # self.moveArm ( abovePreDropoffPoint )  # above dropoff
        debugPrint("move to preDropoffPoint",messageVerbosity=2)
        self.moveArm ( preDropoffPoint )

        # force mode:
        debugPrint("insertion")
        if useForceMode:
            self.moveArm ( preForceModePoint )
            self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
            debugPrint("move to dropoffPointWithExtraPush (force mode)",messageVerbosity=2)
            self.moveCompliant (
                dropoffPointWithExtraPush )  # "force mode" compliant move for last section to dropoff point, and a bit beyond to get a good "seat"
        else:
            self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
            debugPrint("move to dropoffPointWithExtraPush",messageVerbosity=2)
            self.moveArm(dropoffPointWithExtraPush )

        debugPrint("move to dropoffPoint",messageVerbosity=2)
        self.moveCompliant ( dropoffPoint )
        actualDropoffPosition = self.getCurrentPosition ()  # for seeing how far off the expected position we were
        if assemblyFixture is None: # core
            # self.moveArm(actualDropoffPosition)
            pass
        else:
            self.setGripperPosition (organInRobot.gripperOpenPosition)
            self.setMoveSpeed ( self.speedValueSlow )  # slow
            debugPrint("move to preDropoffPoint",messageVerbosity=2)
            self.moveArm ( preDropoffPoint )  # back off
            self.moveArm ( abovePreDropoffPoint  )  # up to clear height
            self.setMoveSpeed ( self.speedValueNormal )

            debugPrint("Open gripper",messageVerbosity=2)
            self.setGripperPosition(0.0) # open

        # insertionMeasuredMisalignment = convertTransformToPose(actualDropoffPosition) - convertTransformToPose(dropoffPoint) # how far from the expected dropoffpoint we ended up - a measure of how misaligned things were
        # misalignment = [ a_i - b_i for a_i, b_i in zip ( convertTransformToPose ( actualDropoffPosition ),
        #                                                  convertTransformToPose ( dropoffPoint ) ) ]
        return actualDropoffPosition

        # else:
        #     debugPrint("Organ insert skipped",messageVerbosity=1)

    ## equivalent of insertOrgan for cables
    def insertCable( self, bank, robot, assemblyFixture, gripperTCP_A, gripperTCP_B, postInsertExtraPushDistanceA =0 , postInsertExtraPushDistanceB =0 ):


        gripperOpenPosition = bank.GripperPowerForOpenPosition  # 0 = fully open, 1= fully closed
        cableFromBank = bank.findACable ()
        cableIntoRobot = robot.getNextCableToInsert ()
        gripPosition =cableIntoRobot.gripPoint


        # pickup a cable end from the bank
        # will approach from the side, so first goes to a point sideways-and-bank, then to sideways, then the pickupPoint itself, then back from pickup point
        def pickup(pickupPoint,gripper_letter):
            # some settings:
            sidewaysClearanceForPrePickup = 5/1000
            backwardsClearance =  60/1000
            self.setMoveSpeed ( self.speedValueSlow )  # slow

            # compute the positions:
            prePickupPoint = pickupPoint * makeTransform([0, sidewaysClearanceForPrePickup, 0])  # sideways from pickup point
            backFromPrePickupPoint = prePickupPoint * makeTransform([0,0,backwardsClearance])
            postPickupPoint = pickupPoint * makeTransform([0,0,backwardsClearance])

            # do the movements:
            self.moveArm(backFromPrePickupPoint)
            self.moveArm(prePickupPoint)
            self.moveArm(pickupPoint)
            self.setGripperPosition(1.02,gripper_letter)
            self.moveArm(postPickupPoint)

        def computeAngleToRotate(dropoffTransform):
            # first, project the global coordinates into the frame of the dropoff point:
            globalCoordinatesInLocalFrame = np.linalg.inv(dropoffTransform)

            # extract the global x-axis, projected onto the xy plane of the local frame
            xAxisInLocalXYPLane = np.row_stack ( (globalCoordinatesInLocalFrame[0:2,0] ,np.zeros([1,1]))  )
            xAxisInLocalXYPLane = xAxisInLocalXYPLane / np.linalg.norm(xAxisInLocalXYPLane) # make unit vector
            localXAxis = np.matrix( [1,0,0] )

            # find the angle between this and the local x-axis
            dotProduct = np.dot(localXAxis, xAxisInLocalXYPLane)
            magnitudeOfAngle = float(np.arccos(np.clip(dotProduct, -1.0, 1.0)))
            if xAxisInLocalXYPLane[1] > 0:
                return magnitudeOfAngle
            else:
                return -magnitudeOfAngle

        def dropoff(dropoffPoint,preDropoffPointPoint,gripper_letter,gripperOpenPower ,postInsertExtraPushDistance,distanceForCompliantMove):
            ## Find the angle which we should rotate, around the z axis, to bring the x axis as close to parallel as possible with the global x axis.
            ## This should make the gripper position more consistent


            # some settings:
            useForceMode=True

            # compute the positions:
            # preDropoffPointPoint = dropoffPoint * makeTransform([0, 0, preDropoffDistance])
            postDropoffPointPoint = makeTransform([0,0,-0.02])*preDropoffPointPoint

            # do the movements:
            self.moveArm(preDropoffPointPoint)
            if useForceMode:
                self.moveArm(dropoffPoint * makeTransform([0, 0, distanceForCompliantMove])) # point back from final position from which to start force mode
                self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow for compliant move
                self.moveCompliant(dropoffPoint * makeTransform([0, 0,-postInsertExtraPushDistance]))  # "force mode" compliant move for last section to dropoff point
                self.moveCompliant(dropoffPoint )  # "force mode" compliant move for last section to dropoff point
                self.setMoveSpeed ( self.speedValueSlow )  # slow
                # self.moveArm(dropoffPoint)
            else:  # NO FORCE MODE
                self.moveArm(dropoffPoint * makeTransform([0, 0, -postInsertExtraPushDistance]))
                self.moveArm(dropoffPoint)
            self.setGripperPosition(gripperOpenPower,gripper_letter)
            self.moveArm(postDropoffPointPoint)

        ## prepare to pickup cable
        debugPrint("preparing to pickup cable",messageVerbosity=2)
        self.moveBetweenStations("cable_bank")
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        # self.setGripperPosition (gripperOpenPosition, "A")
        # self.setGripperPosition (gripperOpenPosition, "B")


        ## pickup end2, gripperB
        debugPrint("Pickup end 2, using Gripper B",messageVerbosity=2)
        self.setTCP(gripperTCP_B * gripPosition * makeTransform([0,0,0,0,0,math.radians(180)])) # the second grip position is 180deg from the first - this prevent the whole gripper having to flip over between picking up the first and second ends
        pickup( bank.origin * cableFromBank.transformEnd2 *makeTransform([0,0,0,0,0,math.radians(180)]),"B")

        ## pickup end1, gripperA
        debugPrint("Pickup end 1, using Gripper A",messageVerbosity=2)
        self.setTCP(gripperTCP_A * gripPosition)
        pickup( bank.origin * cableFromBank.transformEnd1 *makeTransform([0,0,0,0,0,math.radians(180)]) ,"A")

        ## insertion settings
        preDropoffDistance = 20.0 / 1000
        belowPreDropoffDistance = 80.0/1000
        belowPostDropoffDistance = 60.0/1000
        horizontalOffsetForPreparation = -350.0 / 1000
        horizontalOffsetForRotationBetweenAandB = -30.0 / 1000
        belowPostSecondDropoffDistance = 0.2

        ## prepare to insert into robot
        debugPrint("preparing to insert cable into robot",messageVerbosity=2)
        self.setMoveSpeed ( self.speedValueNormal )  # normal
        self.moveBetweenStations("cable_preparation")
        self.setMoveSpeed ( self.speedValueSlow )  # slow



        ## dropoff end1, gripperA
        dropoffPoint = assemblyFixture.currentPosition * cableIntoRobot.transformEnd1 * makeTransform([0,0,0,0,0,math.radians(-90)])
        # dropoffPoint = dropoffPoint * makeTransform([0, 0, 0, 0, 0, computeAngleToRotate(dropoffPoint)]) * makeTransform([0,0,0,0,0,math.radians(90)]) # rotate to align with global frame, so that the gripper is always in the same orientation
        preDropoffPoint = dropoffPoint * makeTransform([0,0,preDropoffDistance])
        belowPreDropoffPoint = makeTransform([0,0,-belowPreDropoffDistance]) * preDropoffPoint
        postDropoffSidewaysAndDown = makeTransform([horizontalOffsetForRotationBetweenAandB,0,-belowPostDropoffDistance]) * preDropoffPoint
        belowPreDropoffPointRotated = dropoffPoint * makeTransform([0, 0, 0, 0, 0, computeAngleToRotate(dropoffPoint)])
        preparationPosition = makeTransform([horizontalOffsetForPreparation,0,0]) * belowPreDropoffPoint
        self.setTCP(gripperTCP_A * gripPosition)
        self.moveArm(preparationPosition)
        self.moveArm(belowPreDropoffPoint)
        self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
        dropoff(dropoffPoint , preDropoffPoint , "A", gripperOpenPosition, postInsertExtraPushDistanceA, cableIntoRobot.forceModeTravelDistance)
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        self.moveArm(postDropoffSidewaysAndDown)


        ## dropoff end2, gripperB
        # gripperRotationPosition = makeTransform([horizontalOffsetForRotationBetweenAandB,0,0]) * belowPreDropoffPoint
        gripperRotationPosition = postDropoffSidewaysAndDown * makeTransform([0, 0, 0, 0, 0, computeAngleToRotate(dropoffPoint)])
        self.moveArm(gripperRotationPosition) # move back slightly to allow gripper to be rotated
        self.setTCP(gripperTCP_B * gripPosition * makeTransform([0,0,0,0,0,math.radians(-90)])) # the second grip position is 180deg from the first - this prevent the whole gripper having to flip over between picking up the first and second ends
        self.moveArm(gripperRotationPosition) # just rotate to the new TCP
        dropoffPoint = assemblyFixture.currentPosition * cableIntoRobot.transformEnd2
        dropoffPoint = dropoffPoint * makeTransform([0, 0, 0, 0, 0, computeAngleToRotate(dropoffPoint)])
        preDropoffPoint = dropoffPoint * makeTransform([0,0,preDropoffDistance])
        belowPreDropoffPoint = makeTransform([0,0,-belowPreDropoffDistance]) * preDropoffPoint
        belowPostDropoffPoint = makeTransform([0,0,-belowPostSecondDropoffDistance]) * dropoffPoint
        postInsertionReturnPosition = makeTransform([-0.25,0.1,-0.08]) * assemblyFixture.originNoRotation * makeTransform([0,0,0,0,math.radians(-90),0])
        self.moveArm(belowPreDropoffPoint)
        self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
        dropoff(dropoffPoint, preDropoffPoint , "B", gripperOpenPosition, postInsertExtraPushDistanceB, cableIntoRobot.forceModeTravelDistance)
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        self.moveArm(belowPostDropoffPoint)
        self.moveArm(postInsertionReturnPosition)


        self.setMoveSpeed ( self.speedValueNormal )



