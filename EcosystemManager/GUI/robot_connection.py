import paramiko
import time
class RobotConnection:
	def __init__(self, ip_address):
		try:
			# Stablish communication
			self.ssh_client=paramiko.SSHClient()
			self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
			self.ssh_client.connect(hostname=ip_address, username='pi', password='arerobot')
			print("Communication STABLISHED with robot")
			self.isconnected = True
		except:
			print("ERROR stablishing communication with robot")
			self.isconnected = False
	## Send genome to robot
	def sendMessage(self, message):
		print("Sending message to robot")
	## Receive message from robots
	def receiveMessage(self):
		print("Receive message to robot")
	## Start program in robot
	def robotStart(self, name):
		if self.isconnected:
			print("Transfering controller")
			ftp_client=self.ssh_client.open_sftp()
			ftp_client.put('../../../../../vrep/files/morphologies98/phenotype' + str(name) +'.csv','testController.csv')
			ftp_client.close()
			print("Robot starting")
			stdin,stdout,stderr = self.ssh_client.exec_command("./simpleController/main")
		else:
			print("ERROR: Robot is not connected")
	## Close connection ports with robots
	def closePort(self):
		if(self.isconnected):
			stdin,stdout,stderr = self.ssh_client.exec_command("sudo killall -9 main")
			stdin,stdout,stderr = self.ssh_client.exec_command("./stopController/main")
			time.sleep(1)
			stdin,stdout,stderr = self.ssh_client.exec_command("sudo poweroff")
			self.ssh_client.close()
			print("Connection port to robot closed")
		else:
			print("Robot not connected")
