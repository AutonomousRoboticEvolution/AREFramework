## contains classes for the different components of the RoboFab:
#  AssemblyFixture
#  Bank (can be used for organ and/or cable banks
# This file does NOT include the UR5, which has it's own file (UR5_host.py) because it's long and complicated!


import numpy as np
import math, time, serial
from helperFunctions import debugPrint, makeTransformInputFormatted, connect2USB, makeTransform
from robotComponents import Organ, Cable


## Object to deal with the rotating assembly fixture
class AssemblyFixture:
    def __init__ ( self, configData ):

        # extract some settings / configuration from the configurationData, which was loaded directly from the JSON
        self.arduino_success_message = configData [ "ARDUINO_SUCCESS_MESSAGE" ]
        self.WAIT_FOR_OK_REPLY = configData [ "WAIT_FOR_OK_REPLY" ]
        self.ARDUINO_SUCCESS_MESSAGE = configData [ "ARDUINO_SUCCESS_MESSAGE" ]
        self.HOME_BETWEEN_EVERY_MOVE = configData [ "HOME_BETWEEN_EVERY_MOVE" ]
        self.COMMUNICATION_TIMEOUT = configData [ "TIMEOUT_SECONDS" ]
        self.CORE_ORGAN_ATTACHMENT_Z_OFFSET = configData [ "CORE_ORGAN_ATTACHMENT_Z_OFFSET" ]
        self.CLEAR_Z_HEIGHT =  configData ["CLEAR_Z_HEIGHT"]

        # originNoRotation is the position, in the Base frame, when the angle is zero
        self.originNoRotation = np.matrix ( configData [ "ORIGIN" ] )

        self.currentPosition = self.originNoRotation
        self.positionSteps = 0
        self.messageToSend = " "
        self.lastSerialMessageReceived = ''
        self.isEnabled = False

        # tries different USB ports until connected
        self.arduino = connect2USB (configData [ "EXPECTED_STARTUP_MESSAGE" ])

        # settings for arduino serial comms:
        self.arduino.write_timeout = 1

        #startup - enable motor and find home position for stepper
        if configData [ "FIND_HOME_AT_STARTUP" ]:
            self.homeStepperMotor ()


    # send the AF to a demanded angular position. This will also update the returned value of AF.currentPosition
    def setPosition ( self, positionDemandRadians ):
        debugPrint ( "AF: going to position: " + str ( positionDemandRadians ) ,messageVerbosity= 1)
        if not self.isEnabled:
            self.enableStepperMotor()
        if self.HOME_BETWEEN_EVERY_MOVE:
            self.homeStepperMotor()
        self.messageToSend = str ( math.degrees ( positionDemandRadians ) )
        self.currentPosition = self.originNoRotation * makeTransformInputFormatted ([0, 0, 0, 0, 0, positionDemandRadians])
        self._updateSerial ()

    def turnElectromagnetsOn ( self ):
        debugPrint ( "AF: I've been told to turn on the electromagnets." )
        self.messageToSend = "h"  # hold (h) robot
        self._updateSerial ()

    def turnElectromagnetsOff ( self ):
        debugPrint ( "AF: I've been told to turn off the electromagnets." )
        self.messageToSend = "r"  # release (r) robot
        self._updateSerial ()

    def enableStepperMotor ( self ):
        debugPrint ( "AF: I've been told to enable stepper motor." )
        self.messageToSend = "e"  # enable (e) stepper motor
        self._updateSerial ()
        self.isEnabled = True

    def disableStepperMotor ( self ):
        debugPrint ( "AF: I've been told to disable stepper motor." )
        self.messageToSend = "o"  # unable (o) stepper motor
        self._updateSerial ()
        self.isEnabled = False

    def homeStepperMotor ( self ):
        debugPrint ( "AF: I've been told to find origin.")
        if not self.isEnabled:
            self.enableStepperMotor()
        time.sleep(1)
        self.messageToSend = "f"  # find (f) origin
        self._updateSerial ()

    def resetMemory ( self ):
        debugPrint ( "AF: Reset memory." )
        self.messageToSend = "m"
        self._updateSerial ()

    def _updateSerial (self):
        self._sendMessage ()
        if self.WAIT_FOR_OK_REPLY:
            in_string = str ( self.arduino.readline () )
            startOfSerialMessageTime = time.time ()
            while not self.ARDUINO_SUCCESS_MESSAGE in in_string:
                in_string = str ( self.arduino.readline () )
                if time.time () - startOfSerialMessageTime > self.COMMUNICATION_TIMEOUT:
                    raise serial.SerialException ( "AF communication timeout" )

    #lower level function - do not call this directly
    def _sendMessage (self):
        debugPrint ( "Sending message: " + self.messageToSend ,messageVerbosity=3 )
        self.arduino.write ( self.messageToSend.encode () )
        debugPrint ( "Sent" ,messageVerbosity=3 )

## Object that deals with a "bank", i.e. the storage place for organs and/or cables before they are put into a robot.
class Bank:
    def __init__ ( self, configData , dictionaryOfAllOrganTypes=None ):
        ## Bank.origin is a pose defining the point relative to which the organ and cable storage positions are defined
        self.origin = np.matrix ( configData [ "ORIGIN" ] )

        ## Bank.organsList is a list of all the organs currently in the bank. When an organ is removed (to put into a robot) is should be deleted from this list.
        self.organsList = [ ]
        for thisOrganDataRow in configData["ORGAN_CONTENTS"]:
            self.organsList.append ( Organ(
                organType=thisOrganDataRow[0],
                positionTransform=makeTransform(
                    x=thisOrganDataRow[ 1 ],
                    y=thisOrganDataRow [ 2 ],
                    z=thisOrganDataRow [ 3 ],
                    rotX=thisOrganDataRow [ 4 ],
                    rotY=thisOrganDataRow [ 5 ],
                    rotZ=thisOrganDataRow [ 6 ]
                ),
                i2cAddress=thisOrganDataRow[7],
                dictionaryOfAllOrganTypes=dictionaryOfAllOrganTypes,
                isInBankFlag = True
            ))

        ## Bank.cablesList is a list of all the cable currently in the bank. When an cable is removed (to put into a robot) is should be deleted from this list.
        self.cablesList = [ ]
        for thisCableEndPoints in configData["CABLE_CONTENTS"]:
            self.cablesList.append ( Cable(thisCableEndPoints , gripPoint=np.matrix(configData["CABLE_GRIP_POINT"] )))

        if "CABLE_GRIPPER_OPEN_POWER" in configData.keys(): # cable bank
            self.GripperPowerForOpenPosition = configData["CABLE_GRIPPER_OPEN_POWER"]


    ## locate an organ of the desired type, remove it from organsList and return it
    def findAnOrgan ( self, desiredTypeID ):
        for i in range ( len ( self.organsList ) ):
            if self.organsList [ i ].organType == desiredTypeID:  # bingo!
                return self.organsList.pop ( i )
        # not found
        raise (RuntimeError ( "Bank does not contain an organ of type " + str ( desiredTypeID ) ))

    ## locate a cable (they are all the same), remove it from organsList and return it
    def findACable ( self ):
        if len ( self.cablesList ) == 0:
            raise RuntimeError ( "No cables left in bank" )
        cableOut = self.cablesList.pop ()
        return cableOut


