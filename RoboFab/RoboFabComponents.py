## contains classes for the different components of the RoboFab:
#  AssemblyFixture
#  Bank (can be used for organ and/or cable banks
# This file does NOT include the UR5, which has it's own file (UR5_host.py) because it's long and complicated!
import os

import numpy as np
import math, time, serial
from helperFunctions import debugPrint, makeTransformInputFormatted, connect2USB, makeTransform, convertPoseToTransform
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
        time.sleep(0.1)
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
        self.organsList = [] # the list of organs, filled below

        ## check if the file "organBankContents.txt" exisits.
        ## if it does, then we will load the data from this.
        ## if it does not, we will load the default organs from the configuration file
        if os.path.isfile("./organBankContents.txt"):
            debugPrint("Loading organ bank contents from file (organBankContents.txt)",messageVerbosity=0)
            # format is an organ per line, with each line formated like this:
            # <organ type> , 16 values for the position transform, <i2c or IP Address>, <isInBank>
            with open("organBankContents.txt", "r") as inputFile:
                lines = inputFile.readlines()
                for line in lines: # each line is an organ
                    data=line.split(",")
                    # remove [ and ] characters

                    index=0

                    # position transform
                    positionTransform=makeTransform()
                    for i in range(4):
                        for j in range(4):
                            index+=1
                            positionTransform[i,j] = data[index]

                    #i2c address
                    index += 1
                    i2cAddress = data[index]

                    # inInBank:
                    index += 1
                    if data[index][-1]=="\n":
                        isInBank= data[index][:-1] == "True" # need to remove the newline character from the end
                    else:
                        isInBank= data[index] == "True"

                    self.organsList.append(Organ(
                        organType=data[0],
                        positionTransform= positionTransform,
                        i2cAddress=i2cAddress,
                        dictionaryOfAllOrganTypes=dictionaryOfAllOrganTypes,
                        isInBankFlag = isInBank
                    ))

        else:
            ## Bank.organsList is a list of all the organ slots in the bank. When an organ is present in the slot, then self.organsList[i].isInBank = True
            debugPrint("Loading organ bank contents from configuration data",messageVerbosity=0)
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

    ## locate an organ of the desired type, remove it from organsList and return it
    def findAnOrgan ( self, desiredTypeID ):
        for i in range ( len ( self.organsList ) ):
            if self.organsList[i].organType == desiredTypeID and self.organsList[i].isInBank==True:  # bingo!
                self.organsList[i].isInBank = False
                return self.organsList[i]
        # not found
        return None
        #raise (RuntimeError ( "Bank does not contain an organ of type " + str ( desiredTypeID ) ))

    ## locate a cable (they are all the same), remove it from organsList and return it
    def findACable ( self ):
        if len ( self.cablesList ) == 0:
            raise RuntimeError ( "No cables left in bank" )
        cableOut = self.cablesList.pop ()
        return cableOut

    def countOrgansAvailable(self):
        dictionaryOfOrgansAvailable = {}
        for organ in self.organsList:
            if organ.isInBank:
                # increment the counter (or, if we haven't started a count for this type of organ, set the counter to 1)
                if organ.organType in dictionaryOfOrgansAvailable:
                    dictionaryOfOrgansAvailable[organ.organType] += 1
                else:
                    dictionaryOfOrgansAvailable[organ.organType] = 1
        return dictionaryOfOrgansAvailable


if __name__ == "__main__":
    # Make the settings file then extract the settings from it
    from makeConfigurationFile import makeFile as makeConfigurationFile
    import json

    makeConfigurationFile(location="BRL")  # <--- change this depending on if you're in York or BRL
    configurationData = json.load(
        open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

    bank = Bank(configurationData["ORGAN_BANK_1"], configurationData["dictionaryOfOrganTypes"])
    print(bank.countOrgansAvailable())