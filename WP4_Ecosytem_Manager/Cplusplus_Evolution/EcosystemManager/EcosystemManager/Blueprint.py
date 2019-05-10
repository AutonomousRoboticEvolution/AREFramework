import csv
from enum import Enum
import random

class Type(Enum):
	SENSOR = 0
	WHEEL = 1
	JOINT = 2
	#BONE: SKELETON = 3
	MISC = 4


class ModuleBlueprintParameters:
	def __init__(self, id, tp, coor):
		self.ID = id
		self.type = tp
		self.coordinates = coor
	def getSpecifications(self):
		return self.ID, self.type, float(self.coordinates[0]), float(self.coordinates[1]), float(self.coordinates[2]), float(self.coordinates[3]), float(self.coordinates[4]), float(self.coordinates[5])

class Blueprint:
	def __init__(self, name):
		self.name = name 
		self.mbps = [] # stores module blueprint parameters (list)

	def init(self): # TODO change to actual robot bp
		# create 3 random modules
		n_joints = 0
		for i in range(10):
			id = n_joints
			tp = random.randint(0,2)#
			if (tp == 2):
				n_joints+=1
			coor = []
			for j in range(6):
				if (j < 2):
					coor.append(random.uniform(-9.0,9.0))
				else:
					coor.append(0)
			self.mbps.append(ModuleBlueprintParameters(id, tp, coor))

	def addModuleBlueprint(self, id, tp, coor):
		mbps.append(ModuleBlueprintParameters(id,tp,coor))
	
	def loadBP(self, path, name): # loads mbps from csv
		self.mbps.clear()
		id = 0
		tp = 0
		coor = []
		with open(path + name + ".csv", 'rb') as csvfile:
			reader = csv.reader(csvfile, delimiter=',')
			for row in reader:
				id = row[0]
				tp = row[1]
				coor = row[2:]
				self.addModuleBlueprint(id,tp,coor)

	def saveBP(self, path, name): # stores mbps
		with open(path + "\\" + str(name) + ".csv", 'w') as csvfile:
			writer = csv.writer(csvfile, delimiter=',')
			for m in self.mbps:
				writer.writerow(m.getSpecifications())
	def getcsv(self):
		dt = []
		writer = csv.writer(csvfile, delimiter=',')
		for m in self.mbps:
			writer.writerow(m.getSpecifications())
		return writer
