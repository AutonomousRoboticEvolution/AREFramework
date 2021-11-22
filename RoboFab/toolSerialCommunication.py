from subprocess import Popen
import time
import socket # for talking directly to gripper over ethernet

from helperFunctions import debugPrint

## Handles all the communication with the gripper (via the serial connection of the UR5 tool).
class GripperHandler:

    def __init__ ( self, configurationData ):

        self.MIN_SERVO_VALUE = int ( configurationData["gripper"][ "min_servo_value" ] )  # absolute lowest the servo will accept
        self.MAX_SERVO_VALUE = int ( configurationData ["gripper"][ "max_servo_value" ] )  # absolute highest the servo will accept
        self.gripperAOpen = int ( configurationData ["gripper"][ "A_open_position" ] )
        self.gripperAClosed = int ( configurationData ["gripper"][ "A_closed_position" ] )
        self.gripperBOpen = int ( configurationData ["gripper"][ "B_open_position" ] )
        self.gripperBClosed = int ( configurationData ["gripper"][ "B_closed_position" ] )

        self.currentPowerGripperA = 0.5
        self.currentPowerGripperB = 0.5
        self.isEnabled = True

        debugPrint("Starting gripper connection",messageVerbosity=1)

        ## direct socket method:
        PCAddress = configurationData["network"]["COMPUTER_ADDRESS"]
        UR5Address = configurationData["network"]["ROBOT_IP_ADDRESS"]
        gripperPort = configurationData["network"]["PORT_FOR_GRIPPER"]

        self.sock = socket.socket()
        self.sock.settimeout(10)

        debugPrint("attempting to connect to gripper, at {} port {}...".format(UR5Address,gripperPort),messageVerbosity=2)
        self.sock.connect((UR5Address,gripperPort))

    ## quick helper function. Returns the input value is it is in the range between MIN_SERVO_VALUE and MAX_SERVO_VALUE, otherwise returns the relevant MIN or MAX value.
    def saturate ( self, val:int ):
        if val > self.MAX_SERVO_VALUE:
            return self.MAX_SERVO_VALUE
        elif val < self.MIN_SERVO_VALUE:
            return self.MIN_SERVO_VALUE
        else:
            return val

    ## puts the servo motors into "idle", so they won't make a noise or apply any force
    def disableServos(self):
        self.isEnabled = False
        self.updateSerial()
        time.sleep(0.1)
    ## turns the servo motors back on from "idle"
    def enableServos(self):
        self.isEnabled = True
        self.updateSerial()
        time.sleep(1)

    ## close the socket that connects to the gripper via UR5
    def stop(self):
        self.sock.send(b'stop')
        self.sock.close()

    ## Actually send the current values to the gripper.
    def updateSerial ( self ):
        debugPrint("Updating Gripper",messageVerbosity=2)
        if self.isEnabled:
            rawServoValueA:int = self.saturate (int( self.gripperAOpen + self.currentPowerGripperA * (self.gripperAClosed - self.gripperAOpen) ) )
            rawServoValueB:int = self.saturate (int( self.gripperBOpen + self.currentPowerGripperB * (self.gripperBClosed - self.gripperBOpen) ) )
        else: # sending zero causes the servos to turn off (and stop buzzing!)
            rawServoValueA:int = 0
            rawServoValueB:int = 0

        debugPrint ( "Raw Servo A: " + str ( rawServoValueA ) ,messageVerbosity=3)
        debugPrint ( "Raw Servo B: " + str ( rawServoValueB ) ,messageVerbosity=3 )

        debugPrint(
            "I've been told to send values to the serial port of " + str(rawServoValueA) + " and " + str(rawServoValueB),
            messageVerbosity=3)
        messageToSend = 'a' + str(rawServoValueA) + 'b' + str(rawServoValueB) + '\r'
        debugPrint ( "Sending message: " + str ( messageToSend.encode () ) ,messageVerbosity=3)
        self.sock.sendall(messageToSend.encode('utf-8'))

        time.sleep(0.1)


    ## Used to set a value to a gripper. The input newGripperPower: 0.0 = fully open, 1.0 = fully closed
    def setGripperPosition (self, newGripperPower:float, AorB:str = 'B'):
        if AorB == 'A':
            self.currentPowerGripperA = newGripperPower
        elif AorB == 'B':
            self.currentPowerGripperB = newGripperPower
        else:
            raise ValueError("AorB should be 'A' or 'B', was: "+AorB)
        self.isEnabled = True
        self.updateSerial ()
        time.sleep(1.0)

## Test setup
if __name__ == "__main__":

    # Make the settings file then extract the settings from it
    from makeConfigurationFile import makeFile as makeConfigurationFile
    import json
    makeConfigurationFile(location="BRL")
    configurationData = json.load(open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

    gripper = GripperHandler(configurationData)

    time.sleep(0.1)
    gripper.setGripperPosition(1.0 ,"B")

    # time.sleep(1)
    # gripper.setGripperPosition(0.0,"B")

    # time.sleep(1)
    # gripper.setGripperPosition(0.0,"A")

    time.sleep(1)
    gripper.disableServos()
    # gripper.stop()
    print("done")