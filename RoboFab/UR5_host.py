##

#! /usr/bin/env python
# -*- coding: utf-8 -*-
import socket # for talking to UR5 over ethernet
import serial # for talking to arduino to connect to RF gripper
import time
import threading
import math
import numpy as np
from helperFunctions import debugPrint, makeTransformInputFormatted, convertPoseToTransform, convertTransformToPose, findAngleBetweenTransforms, makeTransform, makeTransformMillimetersDegrees, changeCoordinateValue, applyCeiling, findDisplacementBetweenTransforms
from toolSerialCommunication import GripperHandler


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
        self.homePosition = makeTransformInputFormatted()
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
        # self.moveArmToJointAngles( self.HOME_POSITIONS[ "home" ] )

    ## function to make the large movements between different areas ("stations"), e.g. printer, assembly fixture, bank etc.
    # This will do a joni-space move between the pre-defined home positions for each station (defined as joint values in the config file)
    # The input should be a string, the name of the station to move to, as defined in the config file
    def moveBetweenStations( self , stationToMoveTo="home" ):
        debugPrint("moving from station "+str(self.currentStation)+" to station "+str(stationToMoveTo),messageVerbosity=1)

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
        time.sleep ( 0.1 )


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

    ##"floating" for a few (2) seconds with force mode used to apply zero force in x,y,z.
    #  this could be useful just after attaching/inserting something, to allow it to settle into its natural position
    def forceModeFloat(self):
        self.waitForArmToBeReady ()
        self.sendString("force_mode_float")  # command for "floating"

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

    ## Set newGripperPower: 0.0 = fully open, 1.0 = fully closed.
    # This function just passes the values down to the gripper, after ensuring the UR5 is in position using the sendNothingToArm() function
    def setGripperPosition (self, newGripperPower:float, AorB:str = 'B'):
        time.sleep(0.1)
        self.sendNothingToArm()
        time.sleep(0.1)
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

    ## pull back (negative z TCP frame) with a set force, intended to pull the skeleton off the print bed
    ## returns true if the thing we were pulling (the robot) moved more than 2cm, false otherwise
    def printBedPull(self):
        startingPostition = self.getCurrentPosition()
        self.sendString("print_bed_pull")
        finishPosition = self.getCurrentPosition()
        if findDisplacementBetweenTransforms(startingPostition, finishPosition)["magnitude"] > 0.02:
            # has moved, so assume it has released from bed
            return True
        else:
            # hasn't moved (much), so return to the starting position
            self.moveArm(startingPostition)
            return False


    ##  Handles receipt of messages from the UR5.
    #  Should be started as a thread after the socket has been set up.
    def __threadedListener ( self ):
        self.UR5isConnected = False # start not connected
        while True:
            msg = self.c.recv ( 1024 )
            debugPrint ( "Received message: " + str ( msg ) ,messageVerbosity=3)
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
                # raise RuntimeError("Disconnected from UR arm")
                break


    ## Pickup an organ and insert it
    #
    #  A high-level function to perform an organ insertion.
    #  Will find a suitable organ in the bank, pick it up, insert it into the right location and then send UR home.
    #  Takes inputs:
    #  bank: where to search for the organ
    #  robot: the robot class being inserted into
    def insertOrganUsingGripperFeature (self, organInRobot, bank, assemblyFixture, gripperTCP):

        vertical_clearance = 0.20 # how high vertically above the pickup and preDropoff to be for pre-pickup and abovePreDropoff
        preDropoffDistanceUp = 0.04 # how far back in the end effector frame to go just before pushing in the organ
        FLOAT_ON_INSERTS = True

        # organInRobot = robot.getNextOrganToInsert ()  ## get the next organ which needs to be inserted
        if (organInRobot.organType == 0) or (assemblyFixture is None):
            raise Exception("This function cannot be used for the Head organ")

        if organInRobot.flipGripperOrientation:
            gripperTCP = gripperTCP * makeTransformInputFormatted([0, 0, 0, 0, 0, math.radians(180)])

        debugPrint ( "Inserting organ of type: " + str (organInRobot.friendlyName) ,messageVerbosity=2)

        distanceForCompliantMove = organInRobot.forceModeTravelDistance  # distance at end of insertion for which "force mode" is used on UR5
        postInsertExtraPushDistance = organInRobot.postInsertExtraPushDistance
        debugPrint("postInsertExtraPushDistance: "+str(postInsertExtraPushDistance),messageVerbosity=3)
        # gripPosition = organInRobot.gripPosition  # transform of organ origin to required point of gripper tip
        # if gripPosition is None:
        #     RuntimeError("no grip Position set")

        self.moveBetweenStations("organ_bank")
        self.setGripperPosition (organInRobot.gripperOpenPosition)

        # rotate assembly fixture
        assemblyFixture.setPosition(organInRobot.requiredAssemblyFixtureRotationRadians)
        originPosition = assemblyFixture.currentPosition * makeTransform( rotZ= organInRobot.AssemblyFixtureRotationOffsetFudgeAngle)

        # first we need to find a suitable organ in the bank.
        organFromBank = bank.findAnOrgan(organInRobot.organType)
        if organFromBank is None:
            raise (RuntimeError("Bank does not contain an organ of type " + str(organInRobot.organType)+ ": " + str ( organInRobot.friendlyName ) ))
        organInRobot.I2CAddress = organFromBank.I2CAddress   # transfer over the i2c address

        # compute all relevant locations. The "prePickup" and "preDropoff" are so the end effector approaches from the right direction.
        # note these are all transform matrices (not poses) and the desired position of the organ origin at these points
        pickupPoint = bank.origin * organFromBank.positionTransformWithinBankOrRobot * organInRobot.transformOrganOriginToGripper * makeTransform(z=organFromBank.pickupExtraPushDistance)
        prePickupPoint = makeTransform (z=vertical_clearance) * pickupPoint  # point above pickup point by vertical_clearance
        dropoffPoint = originPosition * organInRobot.positionTransformWithinBankOrRobot *makeTransformMillimetersDegrees(z=-2) *np.linalg.inv(organInRobot.transformOrganOriginToClipCentre) * organInRobot.transformOrganOriginToGripper # note positionTransformWithinBankOrRobot is the position specified in the blueprint, i.e. the clip centre position not organ origin position
        dropoffPointWithExtraPush = dropoffPoint * makeTransformInputFormatted([0, 0, postInsertExtraPushDistance])
        preDropoffPoint = dropoffPoint * makeTransform(z=-preDropoffDistanceUp)  # point back from (in gripper's frame) final position so we approach from right direction
        preForceModePoint = dropoffPoint * makeTransform (z=-distanceForCompliantMove )  # point back from final position from which to start force mode
        abovePreDropoffPoint = makeTransform (z=vertical_clearance) * preDropoffPoint  # point above (in world frame) predropoff point by vertical_clearance

        self.setTCP(gripperTCP)

        # pickup:
        # self.setTCP(gripperTCP * np.linalg.inv(organInRobot.transformOrganOriginToGripper))  # TCP for pickup is organ origin
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

        self.moveBetweenStations("AF")

        # dropoff:
        # self.setTCP(gripperTCP * np.linalg.inv(organInRobot.transformOrganOriginToGripper) * organInRobot.transformOrganOriginToClipCentre)  # TCP for insertion is centre of clip
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        debugPrint("move to abovePreDropoffPoint",messageVerbosity=2)
        # self.moveArmLinearInJointSpace ( abovePreDropoffPoint )  # above dropoff
        self.moveArm ( abovePreDropoffPoint )  # above dropoff
        debugPrint("move to preDropoffPoint",messageVerbosity=2)
        self.moveArm ( preDropoffPoint )

        # force mode:
        debugPrint("Organ insertion",messageVerbosity=1)
        if organInRobot.useForceMode:
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
        # self.moveArm ( dropoffPoint )
        if FLOAT_ON_INSERTS:
            self.forceModeFloat()
        organInRobot.actualDropoffPosition=self.getCurrentPosition ()  # for seeing how far off the expected position we were

        self.setGripperPosition (organInRobot.gripperOpenPosition)
        self.setMoveSpeed ( self.speedValueSlow )  # slow
        self.moveArm ( preDropoffPoint )  # back off
        self.moveArm ( abovePreDropoffPoint  )  # up to clear height
        self.setMoveSpeed ( self.speedValueNormal )

        debugPrint("Open gripper",messageVerbosity=2)
        self.setGripperPosition(0.0) # open

        # insertionMeasuredMisalignment = convertTransformToPose(actualDropoffPosition) - convertTransformToPose(dropoffPoint) # how far from the expected dropoffpoint we ended up - a measure of how misaligned things were
        # misalignment = [ a_i - b_i for a_i, b_i in zip ( convertTransformToPose ( actualDropoffPosition ),
        #                                                  convertTransformToPose ( dropoffPoint ) ) ]
        return organInRobot

        # else:
        #     debugPrint("Organ insert skipped",messageVerbosity=1)

    ## Pickup the Head organ and insert it
    #
    #  Will find a suitable organ in the bank, pick it up, insert it into the skeleton onf the printbed
    #  Takes inputs:
    #  bank: where to search for the organ
    #  robot: the robot class being inserted into
    def insertHeadOrgan(self, bank, printer, robot, gripperTCP , assemblyFixture):

        verticalClearancePreInsert = 0.28  # how high vertically above the pickup and preDropoff to for pre-pickup
        insertRotation = math.radians(20) # the amount of rotation needed for the Head clipping mechanism
        postInsertExtraRotation = math.radians(5)
        BedPullUpDistance = 50/1000
        postBedRemovalSidewaysClearance = 0.1 # after pulling the skeleton off the bed, we should move horizontally out to avoid the skeleton hitting the frame of the printer

        organInRobot = robot.organsList[0]  ## get the Head, which is always first object in organsList
        organInRobot.hasBeenInserted=True
        if not(organInRobot.organType == 0):
            raise Exception("Was expecting the Head organ. The insertHeadOrgan function has either been called erroneously, or the first organ in the organList in robot is not the Head.")

        if organInRobot.flipGripperOrientation:
            gripperTCP = gripperTCP * makeTransformInputFormatted([0, 0, 0, 0, 0, math.radians(180)])


        debugPrint("Inserting organ of type: " + str(organInRobot.friendlyName), messageVerbosity=2)
        useForceMode = organInRobot.useForceMode

        distanceForCompliantMove = organInRobot.forceModeTravelDistance  # distance at end of insertion for which "force mode" is used on UR5
        postInsertExtraPushDistance = organInRobot.postInsertExtraPushDistance
        debugPrint("postInsertExtraPushDistance: " + str(postInsertExtraPushDistance), messageVerbosity=3)


        # first we need to find a suitable organ in the bank.
        organFromBank = bank.findAnOrgan(organInRobot.organType)
        if organFromBank is None:
            raise (RuntimeError("Bank does not contain an organ of type " + str(organInRobot.organType)+ ": " + str ( organInRobot.friendlyName ) ))


        organInRobot.I2CAddress = organFromBank.I2CAddress  # since this is the Head, this is actually the IP address not at I2C address
        print("IP address: {}".format(organInRobot.I2CAddress))

        # compute all relevant locations. The "prePickup" and "preDropoff" are so the end effector approaches from the right direction.
        # note these are all transform matrices (not poses) and the desired position of the organ origin at these points
        pickupPoint = bank.origin * organFromBank.positionTransformWithinBankOrRobot * makeTransformInputFormatted(
            [0, 0, -organFromBank.pickupExtraPushDistance])
        prePickupPoint = makeTransformInputFormatted(
            [0, 0, verticalClearancePreInsert]) * pickupPoint  # point above pickup point by verticalClearancePreInsert
        dropoffPoint = robot.origin * organInRobot.positionTransformWithinBankOrRobot * makeTransformInputFormatted([0, 0, -postInsertExtraPushDistance])
        dropoffWithExtraRotation = dropoffPoint * makeTransform(rotZ=postInsertExtraRotation)
        dropoffPointPreRotation = dropoffPoint * makeTransformInputFormatted([0, 0, 0, 0, 0, -insertRotation])
        preDropoffPoint = dropoffPointPreRotation * makeTransformInputFormatted([0, 0, verticalClearancePreInsert])  # point back from (in organ clip frame) final position so we approach from right direction
        preForceModePoint = dropoffPointPreRotation * makeTransformInputFormatted([0, 0, distanceForCompliantMove])  # point back from final position from which to start force mode
        postDropoffPointUp = dropoffPoint * makeTransformInputFormatted([0, 0, BedPullUpDistance])
        postDropoffPointUpAndOut = postDropoffPointUp * makeTransform(y=-postBedRemovalSidewaysClearance)

        ## sequence:
        # [move to organ bank station]
        # -> prePickupPoint
        # -> pickupPoint
        # -> [close gripper]
        # -> prePickupPoint
        # -> [move to printer station]
        # -> preDropoffPoint
        # -> (if using force mode) preForceModePoint
        # -> dropoffPointPreRotation
        # -> dropoffPoint
        # -> postDropoffPointUp (pull skeleton off bed)
        # -> postDropoffPointUpAndOut (clear printer)

        # pickup:
        self.moveBetweenStations("organ_bank")
        self.setGripperPosition(organInRobot.gripperOpenPosition)
        self.setTCP(gripperTCP * np.linalg.inv(organInRobot.transformOrganOriginToGripper))  # TCP for pickup is organ origin
        debugPrint("move to prePickupPoint", messageVerbosity=2)
        self.moveArm(prePickupPoint)  # move to new TCP
        self.setMoveSpeed(self.speedValueSlow)  # slow
        debugPrint("move to pickupPoint", messageVerbosity=2)
        self.moveArm(pickupPoint)  # pickup point
        debugPrint("close gripper", messageVerbosity=2)
        self.setGripperPosition(organInRobot.gripperClosedPosition)
        debugPrint("move to prePickupPoint", messageVerbosity=2)
        self.moveArm(prePickupPoint)
        self.setMoveSpeed(self.speedValueNormal)  # normal

        self.moveBetweenStations("printer")

        # dropoff:
        self.setTCP(gripperTCP * np.linalg.inv(organInRobot.transformOrganOriginToGripper) )  # TCP for insertion is centre of clip
        # debugPrint("move to abovePreDropoffPoint", messageVerbosity=2)
        # self.moveArmLinearInJointSpace(abovePreDropoffPoint)  # above dropoff
        # self.moveArm ( PreDropoffPoint )  # above dropoff
        debugPrint("move to preDropoffPoint", messageVerbosity=2)
        self.moveArm(preDropoffPoint)

        # force mode:
        debugPrint("Organ insertion", messageVerbosity=1)
        if useForceMode:
            self.moveArm(preForceModePoint)
            self.setMoveSpeed(self.speedValueReallySlow)  # dead slow
            debugPrint("move to dropoffPoint (force mode)", messageVerbosity=2)
            self.moveCompliant(
                dropoffPointPreRotation)  # "force mode" compliant move for last section to dropoff point, and a bit beyond to get a good "seat"
        else:
            self.setMoveSpeed(self.speedValueReallySlow)  # dead slow
            debugPrint("move to dropoffPoint", messageVerbosity=2)
            self.moveArm(dropoffPointPreRotation)

        self.moveArm(dropoffWithExtraRotation)
        self.moveArm(dropoffPoint)
        actualDropoffPosition = self.getCurrentPosition()  # for seeing how far off the expected position we were
        organInRobot.actualDropoffPosition = actualDropoffPosition

        ## the Head is now in the skeleton, on the printbed

        ## wait for the bed to cool somewhat, then try pulling. If unsuccessful, wait some more and try again.
        debugPrint("Starting bed pulling procedure",messageVerbosity=0)
        has_pulled_off_bed = False
        temperature_cooling_increment = 2 # degrees of cooling per pull attempt

        temperature_to_cool_to = 44

        while not has_pulled_off_bed:
            temperature_to_cool_to =temperature_to_cool_to - temperature_cooling_increment
            debugPrint("Cooling to {}".format(temperature_to_cool_to),messageVerbosity=1)
            self.gripper.disableServos() # we could be waiting a while, so turn off the gripper servo to prevent it overheating
            if temperature_to_cool_to<printer.defaultBedCooldownTemperature:
                # we don't want to wait for a temperature less than room temperature(!) so if we are below the default value we will wait 30 seconds instead
                time.sleep(30)
            else:
                printer.coolBed(temperature_to_cool_to) # turns off bed heater and waits until it is cooled
            debugPrint("Pulling!",messageVerbosity=1)
            self.gripper.enableServos()
            if self.printBedPull():
                has_pulled_off_bed=True

        self.setMoveSpeed(self.speedValueReallySlow)
        self.moveArm(postDropoffPointUp)
        self.setMoveSpeed(self.speedValueNormal)
        self.moveArm(postDropoffPointUpAndOut)


        ## Now take the robot to the Assembly Fixture (AF), and put it on:
        assemblyFixture.setPosition(0)
        AFVerticalClearanceForInsert = 0.05
        AFPostInsertExtraPushDistance = assemblyFixture.CORE_ORGAN_ATTACHMENT_Z_OFFSET
        AFDistanceForCompliantMove = 0

        AFDropoffPoint = assemblyFixture.currentPosition * organInRobot.positionTransformWithinBankOrRobot * makeTransformInputFormatted(
            [0, 0, -AFPostInsertExtraPushDistance])
        AFPreDropoffPoint = AFDropoffPoint * makeTransformInputFormatted([0, 0, AFVerticalClearanceForInsert])  # point back from (in organ frame) final position
        AFPreForceModePoint = AFDropoffPoint * makeTransformInputFormatted(
            [0, 0, AFDistanceForCompliantMove + AFPostInsertExtraPushDistance])  # point back from final position from which to start force mode

        # go to to AF
        self.moveBetweenStations("AF")
        # now on the Assembly fixture, turn on the magnets
        self.sendNothingToArm()
        assemblyFixture.turnElectromagnetsOn()

        # dropoff:
        self.moveArmLinearInJointSpace(AFPreDropoffPoint)  # above dropoff
        self.setMoveSpeed(self.speedValueSlow)  # slow

        # force mode:
        if AFDistanceForCompliantMove>0:
            self.moveArm(AFPreForceModePoint)
            self.setMoveSpeed(self.speedValueReallySlow)  # dead slow
            self.moveCompliant(AFDropoffPoint)  # "force mode" compliant move for last section to dropoff point

        else:
            self.setMoveSpeed(self.speedValueReallySlow)  # dead slow
            self.moveArm(AFDropoffPoint)

        self.setMoveSpeed(self.speedValueNormal)
        actualDropoffPosition = self.getCurrentPosition()  # for seeing how far off the expected position we were
        self.setGripperPosition(organInRobot.gripperOpenPosition)
        self.setTCP(gripperTCP) # reset to the standard gripper TCP
        # linear move up out of the way of robot :
        self.moveArm(
            changeCoordinateValue( self.getCurrentPosition() , "z", assemblyFixture.CLEAR_Z_HEIGHT)
        )


        return organInRobot


    ## equivalent of insertOrganWithoutCable for cables
    ## modified for use with the coiled cable built into the organ
    def insertOrganWithCable(self, organInRobot, bank, assemblyFixture, gripperTCP):

        vertical_clearance = 0.20  # how high vertically above the pickup and preDropoff to be for pre-pickup and abovePreDropoff
        preDropoffDistanceUpForOrgan = 0.30 # how far back in the end effector frame to go just before pushing in the organ
        postInsertExtraPushDistance=0.001
        CABLE_FORCE_MODE_DISTANCE = 16/1000
        gripperOpenPositionPickup = 0.5  # 0 = fully open, 1= fully closed
        gripperOpenPositionDropoff= 0.76  # 0 = fully open, 1= fully closed
        gripperClosedPosition = 1.0
        cableOriginToGripper = makeTransformMillimetersDegrees(y=3,z=-3)
        FLOAT_ON_INSERTS = True

        # sense check
        if (organInRobot.organType == 0) or (assemblyFixture is None) or (organInRobot.transformOrganOriginToMaleCableSocket is None):
            raise Exception("This function cannot be used for the Head organ or organ without cable")

        debugPrint ( "Inserting organ of type: " + str (organInRobot.friendlyName) ,messageVerbosity=2)
        distanceForCompliantMove = organInRobot.forceModeTravelDistance  # distance at end of insertion for which "force mode" is used on UR5

        self.moveBetweenStations("organ_bank")
        assemblyFixture.setPosition(organInRobot.requiredAssemblyFixtureRotationRadians)
        originPosition = assemblyFixture.currentPosition * makeTransform(rotZ=organInRobot.AssemblyFixtureRotationOffsetFudgeAngle)
        self.setGripperPosition (gripperOpenPositionPickup)
        self.setTCP(gripperTCP)

        # first we need to find a suitable organ in the bank.
        organFromBank = bank.findAnOrgan(organInRobot.organType)
        if organFromBank is None:
            raise (RuntimeError("Bank does not contain an organ of type " + str(organInRobot.organType)+ ": " + str ( organInRobot.friendlyName ) ))
        organInRobot.I2CAddress = organFromBank.I2CAddress   # transfer over the i2c address


        self.setTCP ( gripperTCP )

        pickupPoint = bank.origin * organFromBank.positionTransformWithinBankOrRobot * \
                      organInRobot.transformOrganOriginToMaleCableSocket * cableOriginToGripper
        dropoffCablePoint = assemblyFixture.currentPosition \
                       * organInRobot.cableDestination \
                       * makeTransform(z=postInsertExtraPushDistance) \
                       * cableOriginToGripper
        dropoffOrganPoint = originPosition \
                            * organInRobot.positionTransformWithinBankOrRobot * \
                            makeTransformMillimetersDegrees(z=-2) * \
                            np.linalg.inv(organInRobot.transformOrganOriginToClipCentre) * \
                            organInRobot.transformOrganOriginToMaleCableSocket * \
                            cableOriginToGripper
        # note positionTransformWithinBankOrRobot is the position specified in the blueprint, i.e. the clip centre position not organ origin position

        # sequence is as follows:
        # abovePrePickupPoint
        # prePickupPoint
        # pickupPoint
        # postPickupPoint (force mode)
        # abovePostPickupPoint
        # aboveDropoffPoint
        # preDropoffPoint
        # preForceModeDropoffPoint
        # dropoffCablePoint (force mode)
        # postDropoffPoint

        prePickupPoint = makeTransform(z=vertical_clearance) * pickupPoint  # point above pickup point by vertical_clearance

        dropoffOrganPointWithExtraPush = dropoffOrganPoint * makeTransformInputFormatted([0, 0, organInRobot.postInsertExtraPushDistance])
        preDropoffOrganPoint = dropoffOrganPoint * makeTransform(z=-preDropoffDistanceUpForOrgan)  # point back from (in gripper's frame) final position so we approach from right direction
        preDropoffOrganPoint = applyCeiling(preDropoffOrganPoint, assemblyFixture.CLEAR_Z_HEIGHT)
        abovePreDropoffOrganPoint = changeCoordinateValue(preDropoffOrganPoint, "z", assemblyFixture.CLEAR_Z_HEIGHT)
        preForceModeOrganPoint = dropoffOrganPoint * makeTransform(z=-distanceForCompliantMove)  # point back from final position from which to start force mode

        cablePullOutPoint = dropoffOrganPoint * makeTransform(z=-preDropoffDistanceUpForOrgan)
        cablePullOutPoint = applyCeiling(cablePullOutPoint, assemblyFixture.CLEAR_Z_HEIGHT)
        # aboveCablePullOutPoint = changeCoordinateValue(cablePullOutPoint,"z",assemblyFixture.CLEAR_Z_HEIGHT)
        aboveDropoffPoint = changeCoordinateValue(dropoffCablePoint,"z",assemblyFixture.CLEAR_Z_HEIGHT)
        aboveCablePullOutPoint = changeCoordinateValue(aboveDropoffPoint,"x",cablePullOutPoint[0,3])
        aboveCablePullOutPoint = changeCoordinateValue(aboveCablePullOutPoint,"y",cablePullOutPoint[1,3])

        preForceModeDropoffPoint = dropoffCablePoint * makeTransform(z=-CABLE_FORCE_MODE_DISTANCE)
        postDropoffPoint = changeCoordinateValue(dropoffCablePoint,"z",assemblyFixture.CLEAR_Z_HEIGHT)
        dropoffPointNoExtra = dropoffCablePoint*makeTransform(z=-postInsertExtraPushDistance)


        # # rotate dropoff point so that the cable points back towards the organ (experimental!)
        # from helperFunctions import findDisplacementBetweenTransforms
        # displacement = findDisplacementBetweenTransforms( np.linalg.inv(dropoffCablePoint) * pickupPoint )
        # if organInRobot.flipGripperOrientation:
            # dropoffCablePoint = dropoffCablePoint * makeTransformInputFormatted([0,0,0,0,0 , displacement["direction_about_z"] ]) # rotated towards the organ
            # dropoffCablePoint = dropoffCablePoint * makeTransformInputFormatted([0,0,0,0,0 , math.radians(-90) ])
        # else:
            # dropoffCablePoint = dropoffCablePoint * makeTransformInputFormatted([0,0,0,0,0 , math.pi + displacement["direction_about_z"] ]) # rotated towards the organ
            # dropoffCablePoint = dropoffCablePoint * makeTransformInputFormatted([0,0,0,0,0 , math.radians(90) ])



        self.setMoveSpeed ( self.speedValueNormal )  # normal

        # grab male
        self.setGripperPosition(gripperOpenPositionPickup)
        self.moveArm(prePickupPoint)
        self.moveArm( pickupPoint )
        self.setGripperPosition(gripperClosedPosition)
        self.moveArm(prePickupPoint)

        self.moveBetweenStations("AF")

        # attach organ to skeleton:
        self.moveArm(abovePreDropoffOrganPoint)  # above dropoff
        self.setMoveSpeed(self.speedValueSlow)  # slow
        self.moveArm(preDropoffOrganPoint)  # back from dropoff
        if organInRobot.useForceMode:
            self.moveArm ( preForceModeOrganPoint )
            self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
            debugPrint("move to dropoffPointWithExtraPush (force mode)",messageVerbosity=2)
            # "force mode" compliant move for last section to dropoff point, and a bit beyond to get a good "seat":
            self.moveCompliant ( dropoffOrganPointWithExtraPush )
        else:
            self.setMoveSpeed ( self.speedValueReallySlow )  # dead slow
            debugPrint("move to dropoffPointWithExtraPush",messageVerbosity=2)
            self.moveArm(dropoffOrganPointWithExtraPush )
        if FLOAT_ON_INSERTS:
            self.forceModeFloat()
        organInRobot.actualDropoffPosition = self.getCurrentPosition()  # for seeing how far off the expected position we were

        # pull out cable
        # self.moveCompliant(postPickupPoint)
        self.setMoveSpeed(self.speedValueSlow)  # slow
        self.moveArm(cablePullOutPoint)
        self.setMoveSpeed ( self.speedValueNormal )
        self.moveArmLinearInJointSpace( aboveCablePullOutPoint )

        # put cable into Head:
        self.moveArm( aboveDropoffPoint )
        self.moveArm( preForceModeDropoffPoint )
        self.setMoveSpeed ( self.speedValueSlow )  # dead slow
        self.moveCompliant( dropoffCablePoint )
        self.moveArm( dropoffPointNoExtra )
        if FLOAT_ON_INSERTS:
            self.forceModeFloat()
        self.setGripperPosition(gripperOpenPositionDropoff)
        self.setMoveSpeed ( self.speedValueNormal )
        self.moveArm( postDropoffPoint )

        self.moveBetweenStations("AF")


