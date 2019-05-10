import socket
BUFFER_SIZE = 1024
ip_address = "10.2.166.80"
#ip_address = "0.0.0.0"
port = 3010

class RobotConnection:
	def __init__(self, ip_address, port):
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.s.connect((ip_address, port))
		# TODO return failed connection
	def sendMessage(self, message):
		self.s.send(bytes(message, 'utf-8'))
	def receiveMessage(self):
		return self.s.recv(BUFFER_SIZE).decode()
	def closePort(self):
		self.s.close()
