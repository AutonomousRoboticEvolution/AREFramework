from subprocess import Popen
import time
import socket # for talking directly to gripper over ethernet

from helperFunctions import debugPrint

class GripperHandler:

    def __init__ ( self, configurationData ):

        self.MIN_SERVO_VALUE = float ( configurationData["gripper"][ "min_servo_value" ] )  # absolute lowest the servo will accept
        self.MAX_SERVO_VALUE = float ( configurationData ["gripper"][ "max_servo_value" ] )  # absolute highest the servo will accept
        self.gripperAOpen = float ( configurationData ["gripper"][ "A_open_position" ] )
        self.gripperAClosed = float ( configurationData ["gripper"][ "A_closed_position" ] )
        self.gripperBOpen = float ( configurationData ["gripper"][ "B_open_position" ] )
        self.gripperBClosed = float ( configurationData ["gripper"][ "B_closed_position" ] )

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

        debugPrint("attempting to connect...",messageVerbosity=2)
        self.sock.connect((UR5Address,gripperPort))

        # debugPrint("sending start message...",messageVerbosity=2)
        # time.sleep(0.1)
        # tmp = self.sock.sendall(( configurationData ["gripper"][ "STARTUP_MESSAGE" ] ).encode('utf-8'))
        # debugPrint("sending: "+str(tmp),messageVerbosity=3)

        # debugPrint("waiting for reply...",messageVerbosity=2)
        # time.sleep(0.1)
        # data_in = self.sock.recv(4096)
        # debugPrint("reply was : "+str(data_in),messageVerbosity=2)
        # if str(data_in)==configurationData ["gripper"][ "EXPECTED_STARTUP_REPLY" ]:
        #     debugPrint("that's good",messageVerbosity=2)
        # else:
        #     debugPrint("that's bad",messageVerbosity=2)



    def saturate ( self, val ):
        if val > self.MAX_SERVO_VALUE:
            return self.MAX_SERVO_VALUE
        elif val < self.MIN_SERVO_VALUE:
            return self.MIN_SERVO_VALUE
        else:
            return val

    def disableServos(self): # puts the servo motors into "idle", so they won't make a noise or apply any force
        self.isEnabled = False
        self.updateSerial()

    def stop(self): # close the socket that connects to the gripper via UR5
        self.sock.send(b'stop')
        self.sock.close()

    def updateSerial ( self ):
        debugPrint("Updating Gripper",messageVerbosity=2)
        if self.isEnabled:
            rawServoValueA = self.saturate (
                int ( self.gripperAOpen + self.currentPowerGripperA * (self.gripperAClosed - self.gripperAOpen) ) )
            rawServoValueB = self.saturate(
                int(self.gripperBOpen + self.currentPowerGripperB * (self.gripperBClosed - self.gripperBOpen)))
        else: # sending zero causes the servos to turn off (and stop buzzing!)
            rawServoValueA = 0
            rawServoValueB = 0

        debugPrint ( "Raw Servo A: " + str ( rawServoValueA ) ,messageVerbosity=3)
        debugPrint ( "Raw Servo B: " + str ( rawServoValueB ) ,messageVerbosity=3 )

        debugPrint(
            "I've been told to send values to the serial port of " + str(rawServoValueA) + " and " + str(rawServoValueB),
            messageVerbosity=3)
        messageToSend = 'a' + str(rawServoValueA) + 'b' + str(rawServoValueB) + '\r'
        debugPrint ( "Sending message: " + str ( messageToSend.encode () ) ,messageVerbosity=3)
        self.sock.sendall(messageToSend.encode('utf-8'))

        time.sleep(0.1)

        # data_in = self.sock.recv(4096)

        # data_in = bytearray()
        # while True:
        #     c=self.sock.recv(1024)
        #     print(c)
        #     print(chr(0xff))
        #     if c=='\n' or c=='':
        #         break
        #     else:
        #         data_in += c
        #
        # debugPrint("reply was : " + str(data_in), messageVerbosity=2)
        # if str(data_in) == configurationData["gripper"]["SUCCESS_MESSAGE"]:
        #     debugPrint("that's good", messageVerbosity=2)
        # else:
        #     debugPrint("that's bad", messageVerbosity=2)

    def setGripperPosition (self, newGripperPower:float, AorB:str = 'A'):
        # set newGripperPower: 0.0 = fully open, 1.0 = fully closed
        if AorB == 'A':
            self.currentPowerGripperA = newGripperPower
        elif AorB == 'B':
            self.currentPowerGripperB = newGripperPower
        else:
            raise ValueError("AorB should be 'A' or 'B', was: "+AorB)
        self.isEnabled = True
        self.updateSerial ()
        time.sleep(0.5)

## Test setup
if __name__ == "__main__":

    # Make the settings file then extract the settings from it
    from makeConfigurationFile import makeFile as makeConfigurationFile
    import json
    makeConfigurationFile(location="BRL")
    configurationData = json.load(open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

    gripper = GripperHandler(configurationData)

    time.sleep(0.1)
    print("1...")
    gripper.setGripperPosition(1.0,"A")
    gripper.setGripperPosition(1.0,"B")

    time.sleep(1)
    print("2...")
    gripper.setGripperPosition(0.0,"A")
    gripper.setGripperPosition(0.0,"B")


    time.sleep(1)
    print("3...")
    gripper.setGripperPosition(0.5, "A")
    gripper.setGripperPosition(0.5, "B")


    time.sleep(1)
    gripper.disableServos()
    gripper.stop()
    print("done")