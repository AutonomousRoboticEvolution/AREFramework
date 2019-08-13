import paramiko

class RobotConnection:
	def __init__(self, ip_address):
		try:
			# Stablish communication
			self.ssh_client=paramiko.SSHClient()
			self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
			self.ssh_client.connect(hostname='192.168.1.5', username='pi', password='arerobot')
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
	def robotStart(self):
		print("Robot starting")
	## Close connection ports with robots
	def closePort(self):
		if(self.isconnected):
			self.ssh_client.close()
			print("Connection port to robot closed")
		else:
			print("Robot not connected")
