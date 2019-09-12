import csv
from enum import Enum

class Type(Enum):
	SENSOR = 0
	WHEEL = 1
	JOINT = 2
	#BONE: SKELETON = 3
	MISC = 3

class ModuleBlueprintParameters:
	def __init__(self, id, moduleType, absPos, absOri):
		self.ID = id
		self.moduleType = moduleType
		self.absPos = absPos
		self.absOri = absOri
	def getSpecifications(self):
		return self.ID, self.moduleType, self.absPos[0], self.absPos[1], self.absPos[2], self.absOri[0], self.absOri[1], self.absOri[2]

class Blueprint:
	def __init__(self, name):
		self.name = name 
		self.mbps = [] # stores module blueprint parameters (list)

	## Loads component type, position and orientation
	def init(self,moduleType,absPos,absOri):
		for i in range(len(moduleType)):
			id = 0
			self.mbps.append(ModuleBlueprintParameters(id, moduleType[i], absPos[i], absOri[i]))

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
	## Save BP file.
	def saveBP(self, name): # stores mbps
		with open("../../../../../robofab/blueprints/BP" + str(name) + ".csv", 'w') as csvfile:
		#with open(path + "/Blueprint" + str(name) + ".csv", 'w') as csvfile:
			writer = csv.writer(csvfile, delimiter=',')
			for m in self.mbps:
				writer.writerow(m.getSpecifications())

	def getcsv(self):
		dt = []
		writer = csv.writer(csvfile, delimiter=',')
		for m in self.mbps:
			writer.writerow(m.getSpecifications())
		return writer
