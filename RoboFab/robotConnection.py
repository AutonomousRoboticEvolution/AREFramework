import paramiko
import time

class RobotConnection:
    def __init__(self, ip_address):
        # try:
        # Establish communication
        self.ssh_client = paramiko.SSHClient()
        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        # self.ssh_client.connect(hostname=ip_address, username='pi', password='raspberry')
        self.ssh_client.connect(hostname=ip_address, username='pi', password='raspberry')
        print("Communication ESTABLISHED with robot")
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
            print("Transferring controller")
            ftp_client = self.ssh_client.open_sftp()
            ftp_client.put('./controllers/' + str(name) + '.csv',
                           'genome.csv')
            ftp_client.close()
            print("Robot starting")
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
            print("Robot not connected")