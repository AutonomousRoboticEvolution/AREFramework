## code for establishing an ssh connection to a specific robot, sending the controller to it and starting the controller

import paramiko
import time
from helperFunctions import debugPrint

## create an object for the ssh communication with a particular robot.
# Input ip_address should be a string of the individual robot's IP address, e.g. "192.168.20.101"
class RobotConnection:
    def __init__(self, ip_address):
        # try:
        # Establish communication
        self.ssh_client = paramiko.SSHClient()
        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.ssh_client.connect(hostname=ip_address, username='pi', password='raspberry')
        debugPrint("Communication ESTABLISHED with robot",messageVerbosity=1)
        self.isConnected = True
        # except:
        #     print("ERROR establishing communication with robot")
        #     self.isConnected = False

    ## Send genome to robot
    def sendMessage(self, message):
        print("Sending message to robot")

    ## Receive message from robots
    def receiveMessage(self):
        print("Receive message to robot")

    ## Start program in robot
    def robotStart(self, name):
        if self.isConnected:
            debugPrint("Transferring controller",messageVerbosity=2)
            ftp_client = self.ssh_client.open_sftp()
            ftp_client.put('./controllers/' + str(name) + '.csv',
                           'genome.csv')
            ftp_client.close()
            debugPrint("Robot starting",messageVerbosity=2)
            stdin, stdout, stderr = self.ssh_client.exec_command("./robot_controller_are")
        else:
            print("ERROR: Robot 1 is not connected")

    ## Close connection ports with robots
    def closePort(self):
        if self.isConnected:
            stdin, stdout, stderr = self.ssh_client.exec_command("sudo killall -9 main")
            stdin, stdout, stderr = self.ssh_client.exec_command("./stopController/main")
            time.sleep(5)
            # stdin, stdout, stderr = self.ssh_client.exec_command("sudo poweroff")
            self.ssh_client.close()
            print("Connection port to robot closed")
        else:
            print("Tried to close the connection, but the robot was not connected")