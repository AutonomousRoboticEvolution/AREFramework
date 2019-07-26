from tkinter import *
from tkinter import ttk
import csv

# variables
serverMode = 0
evolutionType = 0
fitnessType = 0
morphologyType = 0
selectionType = 0
replacementType = 0
controlType = 0 
generation = 0
maxGeneration = 1000 # default
generationInterval = 0
indCounter = 0
populationSize = 20
mutationRate = 0.2 # control
morphMutRate = 0.15
crossover = 0
crossoverRate = 0.1
energyDissipationRate = 0 # not used anymore?
maxage = 50
minage = 10


def save(location, nr):
	return 0
def load(location, nr):
	return 0


class Settings(Frame):
	def __init__(self,parent, directory):
		# Button(parent, text="Simulation", height = buttonSize[1], width = buttonSize[0]).grid(row=1, column=0, sticky=W)
		self.nb = ttk.Notebook(parent)
		self.directory = directory
		f1 =  ttk.Frame(self.nb)
		f2 =  ttk.Frame(self.nb)
		f3 =  ttk.Frame(self.nb)
		f4 =  ttk.Frame(self.nb)
		f5 =  ttk.Frame(self.nb)
		f6 =  ttk.Frame(self.nb)
		f7 =  ttk.Frame(self.nb)
		f8 =  ttk.Frame(self.nb)
		f9 =  ttk.Frame(self.nb)
		self.nb.add(f1, text="General")
		self.nb.add(f2, text="EA")
		self.nb.add(f3, text="Morphology")
		self.nb.add(f4, text="Encoding")
		self.nb.add(f5, text="Modules")
		self.nb.add(f6, text="Environment")
		self.nb.add(f7, text="Controller")
		self.nb.add(f8, text="Components")
		self.nb.add(f9, text="Viability")
		self.saveSettingsButton = Button(parent, text="Save Settings", command=self.saveCSV)
		self.saveSettingsButton_R = Button(parent, text="Save Settings Real World", command=self.saveCSV_R)
		self.saveSettingsButton.grid(column=1, row=0);
		self.saveSettingsButton_R.grid(column=1, row=1);
				
		self.generalParameters = GeneralParameters(f1, self.directory)
		self.ea = EAParameters(f2)
		self.morphology = MorphologyParameters(f3, self)
		self.encoding = EncodingParameters(f4)
		self.module = ModuleParameters(f5)
		self.environment = EnvironmentParameters(f6)
		self.control = ControlParameters(f7)
		self.component = ComponentParameters(f8)
		self.viability = ViabilityParameters(f9)
		self.nb.select(f2)
		self.nb.enable_traversal()
		self.nb.grid(column=0, row=1);

	
	def saveCSV(self):
		# open csv
		data = self.generalParameters.getSettings()
		data += self.ea.getSettings()
		data += self.morphology.getSettings()
		data += self.encoding.getSettings()
		data += self.module.getSettings()
		data += self.environment.getSettings()
		data += self.control.getSettings()
		data += self.viability.getSettings()
		for dat in data:
			print(dat)
		repo = self.directory + '/files/'
		with open(repo + "settings0.csv", 'w') as csvfile: 
			csvwriter = csv.writer(csvfile, delimiter=',')
			csvwriter.writerows(data)
	def saveCSV_R(self):
		# open csv
		data = self.generalParameters.getSettings()
		data += self.ea.getSettings()
		data += self.morphology.getSettings()
		data += self.encoding.getSettings()
		data += self.module.getSettings()
		data += self.environment.getSettings()
		data += self.control.getSettings()
		for dat in data:
			print(dat)
		repo = self.directory + '/files/'
		with open(repo + "settings98.csv", 'w') as csvfile: 
			csvwriter = csv.writer(csvfile, delimiter=',')
			csvwriter.writerows(data)


class EnvironmentParameters(Frame):
	def __init__(self,parent):
		fitnessTypeText = "Fitness type"
		environmentTypeText = "Environment type"
		self.fitnessTypeLabel = Label(parent,text=fitnessTypeText)
		self.fitnessTypeLabel.grid(column = 0, row = 0)
		self.fitnessTypes= ['Movement', 'Flight'] 
		self.fitnessTVar = StringVar(parent)
		self.fitnessTVar.set('Movement')
		self.fitmenu = OptionMenu(parent,self.fitnessTVar,*self.fitnessTypes)
		self.fitmenu.grid(column=1,row=0)
		
		self.environmentTypeLabel = Label(parent,text=environmentTypeText)
		self.environmentTypeLabel.grid(column = 0, row = 1)
		self.envTypes= ['plane','rough'] 
		self.envVar = StringVar(parent)
		self.envVar.set('plane')
		self.envmenu = OptionMenu(parent,self.envVar,*self.envTypes)
		self.envmenu.grid(column=1,row=1)
		
	def update():
		print("TODO: set variables")
	def getSettings(self):
		data = []
		row = []
		row.append('#fitnessType')
		if (self.fitnessTVar == 'Movement'):
			row.append('0')
		else: 
			row.append('NAN') # Not set
		data.append(row)
		row = []
		row.append('#environmentType')
		if (self.envVar.get() == 'plane'):
			row.append('0')
		else:
			row.append('5')
		data.append(row)
		return data

class ViabilityParameters(Frame):
	def __init__(self,parent):
		self.gndLabel = Label(parent,text="Organs below printing bed")
		self.gndLabel.grid(column = 0, row = 1)
		self.esCheckGndVar = IntVar()
		self.esCheckGnd = Checkbutton(parent, variable = self.esCheckGndVar)
		self.esCheckGnd.grid(column = 1, row = 1)

		self.collLabel = Label(parent,text="Colliding organs")
		self.collLabel.grid(column = 0, row = 2)
		self.esCheckCollVar = IntVar()
		self.esCheckColl = Checkbutton(parent, variable = self.esCheckCollVar)
		self.esCheckColl.grid(column = 1, row = 2)

		self.oriLabel = Label(parent,text="Non-printable organ orientations")
		self.oriLabel.grid(column = 0, row = 3)
		self.esCheckOriVar = IntVar()
		self.esCheckOri = Checkbutton(parent, variable = self.esCheckOriVar)
		self.esCheckOri.grid(column = 1, row = 3)

		self.manLabel = Label(parent,text="Manufacturability")
		self.manLabel.grid(column = 0, row = 4)
		self.manCheckVar = IntVar()
		self.manCheck = Checkbutton(parent, variable = self.manCheckVar)
		self.manCheck.grid(column = 1, row = 4)

		self.esLabel = Label(parent,text="Essential Organ")
		self.esLabel.grid(column = 0, row = 5)
		self.esCheckVar = IntVar()
		self.esCheck = Checkbutton(parent, variable = self.esCheckVar)
		self.esCheck.grid(column = 1, row = 5)

		self.printVolLabel = Label(parent,text="Max Print Volume")
		self.printVolLabel.grid(column = 0, row = 6)
		self.printVolVar = IntVar(parent,3)
		#self.printVolVar.trace("w", self.moduleSizeChanged)
		self.printVolScale = Scale(parent, from_=0, to=10, orient=HORIZONTAL,resolution=1, variable = self.printVolVar)
		self.printVolScale.grid(column = 1, row = 6)
		self.printVolText=Entry(parent, textvariable = self.printVolVar)
		self.printVolText.grid(column = 2, row = 6)

		self.orgInLabel = Label(parent,text="Organ Insertion Sequence")
		self.orgInLabel.grid(column = 0, row = 7)

		self.behaviorTestLabel = Label(parent,text="Behavior Test")
		self.behaviorTestLabel.grid(column = 0, row = 8)
		self.beCheckVar = IntVar()
		self.beCheck = Checkbutton(parent, variable = self.beCheckVar)
		self.beCheck.grid(column = 1, row = 8)

	def getSettings(self):
		data = []
		row = []
		row.append("#collidingorgans")
		row.append(self.esCheckGndVar.get())
		data.append(row)
		row = []
		row.append("#organsbelowprintingbed")
		row.append(self.esCheckCollVar.get())
		data.append(row)
		row = []
		row.append("#nonprintableorientations")
		row.append(self.esCheckOriVar.get())
		data.append(row)
		return data


class ComponentParameters(Frame):
	def __init__(self,parent):
		self.parent = parent
		# component list
		
		self.maxNModulesTypesLabel = Label(parent,text="Component Types")
		self.maxNModulesTypesLabel.grid(column = 0, row = 0)
		
		self.mmVar = IntVar(parent,3)
		self.mmVar.trace("w", self.moduleSizeChanged)
		self.mmScale = Scale(parent, from_=0, to=10, orient=HORIZONTAL,resolution=1, variable = self.mmVar)
		self.mmScale.grid(column = 1, row = 0)
		self.mmText=Entry(parent, textvariable = self.mmVar)
		self.mmText.grid(column = 2, row = 0)
		#self.mcVar = IntVar(parent,5)
		#self.mcVar.trace("w", self.mcSizeChanged)
		#self.mcScale = Scale(parent, from_=0, to=10, orient=HORIZONTAL,resolution=1, variable = self.mcVar)
		#self.mcScale.grid(column = 1, row = 0)
		#self.mcText=Entry(parent, textvariable = self.mcVar)
		#self.mcText.grid(column = 2, row = 0)
		self.typecompsLabel = Label(parent,text="Component Type")
		self.typecompsLabel.grid(column = 1, row = 1)
		self.maxcompsLabel = Label(parent,text="Max components")
		self.maxcompsLabel.grid(column = 2, row = 1)
		
		currentRow = 2
		self.moduleTypesText = ['0','1','2','3','4','5','6','7','8','9']
		self.moduleTypeOptionsText = ['0','1','2','3','4','5','6','7','8','9']
		self.moduleMaxTypeOptionsText = ['0','1','2','3','4','5','6','7','8','9']
		self.moduleLabels = []
		self.moduleTypes = []
		self.moduleTypeEntries = []
		self.moduleMaxs = []
		self.moduleMaxEntries = []
		for i in range(len(self.moduleTypesText)):
			self.moduleLabels.append(Label(parent,text=self.moduleTypesText[i]))
			self.moduleLabels[i].grid(column = 0, row = currentRow + i)
			if i == 0:
				self.moduleTypes.append(IntVar(parent,0))
				self.moduleMaxs.append(IntVar(parent,1))
			else:
				self.moduleTypes.append(IntVar(parent,i))
				self.moduleMaxs.append(IntVar(parent,10))
			self.moduleTypeEntries.append(Entry(parent,textvariable = self.moduleTypes[i]))
			self.moduleTypeEntries[i].grid(column = 1, row = currentRow + i)
			self.moduleMaxEntries.append(Entry(parent,textvariable = self.moduleMaxs[i]))
			self.moduleMaxEntries[i].grid(column = 2, row = currentRow + i)
		self.moduleSizeChanged()
		self.directBarsWidthLabel = Label(parent,text="Direct Bars Width")
		self.directBarsWidthLabel.grid(column = 0, row = 14)
		self.directBarsHeightLabel = Label(parent,text="Direct Bars Height")
		self.directBarsHeightLabel.grid(column = 0, row = 15)
	
		self.bhScale = Scale(parent, from_=0, to=1.0,resolution=0.01, orient=HORIZONTAL)
		self.bhScale.set(0.15)
		self.bhScale.grid(column=1,row=14)
	
		self.bwScale = Scale(parent, from_=0, to=1.0,resolution=0.01, orient=HORIZONTAL)
		self.bwScale.set(0.1)
		self.bwScale.grid(column=1,row=15)
	
	def moduleSizeChanged(self, *args):
		for i in range(10):
			if (i < self.mmVar.get()):
				self.moduleTypeEntries[i].config(state = 'normal')
				self.moduleMaxEntries[i].config(state = 'normal')
			else:
				self.moduleTypeEntries[i].config(state = 'disabled')
				self.moduleMaxEntries[i].config(state = 'disabled')
		# voxel parameters
		#	resolution

		#TODO

		# type and max number of components
		# type can be organ or skeleton
		# columnWidth

		def mcSizeChanged(self, *args):
			print("mc changed")

		# Don't use OpenScad


	def getSettings(self):
		data = []
		row = []
		data.append(row)
		return data
	


class MorphologyParameters(Frame):
	def __init__(self,parent, sett):
		self.parent = parent
		self.settings = sett
		morphologyTypeText = "Morphology type"

		self.morphLabel = Label(parent,text="Evolution type")
		self.morphLabel.grid(column = 0, row = 0)
		self.morphOptions = {'Modular','ARE', 'NAO', 'CAT'}
		self.morphVar = StringVar(parent)
		self.morphVar.trace("w", self.updateSet)
		self.morphVar.set('ARE')
		self.morphMenu = OptionMenu(parent,self.morphVar,*self.morphOptions)
		self.morphMenu.grid(column = 1, row = 0, columnspan = 1)

	def updateSet(self, *args):
		#print(args)
		#print('set')
		if(self.morphVar.get() != 'Modular'):
			self.settings.nb.tab(3, state="disabled")
			self.settings.nb.tab(4, state="disabled")
			self.settings.nb.tab(7, state="disabled")
		else:
			self.settings.nb.tab(3, state="normal")
			self.settings.nb.tab(4, state="normal")
			self.settings.nb.tab(7, state="disabled")

		if(self.morphVar.get() == 'ARE'):
			self.settings.nb.tab(3, state="normal")
			self.settings.nb.tab(4, state="disabled")
			self.settings.nb.tab(7, state="normal")


	def getSettings(self):
		data = []
		row = []
		row.append('#morphologyType')
		if (self.morphVar.get() == 'Modular'):
			if (self.settings.encoding.encodingVar.get() == 'L-System'):
				row.append('1')
			elif (self.settings.encoding.encodingVar.get() == 'CPPN'):
				row.append('3')
			elif (self.settings.encoding.encodingVar.get() == 'Direct'):
				row.append('4')
		elif(self.morphVar.get() == 'ARE'):
			row.append('99')
		elif(self.morphVar.get() == 'NAO'):
			row.append('5')
		elif(self.morphVar.get() == 'CAT'):
			row.append('0')
		data.append(row)
		row = []
		if (self.morphVar.get() == 'Modular'):
			row.append('#lIncrements')
			row.append(str(self.settings.encoding.treeDepthScale.get()))

		data.append(row)
		return data

class ControlParameters(Frame):
	def __init__(self,parent):
		self.parent = parent
		controlTypeText = "Control type"
		self.controlTypeLabel = Label(parent,text=controlTypeText)
		self.controlTypeLabel.grid(column = 0, row = 0)
		self.controlTypes= ['ANN','Custom ANN']
		self.controlVar = StringVar(parent)
		self.controlVar.set('ANN')
		self.controlMenu = OptionMenu(parent,self.controlVar,*self.controlTypes)
		self.controlMenu.grid(column=1,row=0)
	def getSettings(self):
		data = []
		row = []
		row.append('#controlType')
		if (self.controlVar == 'ANN'):
			row.append('1')
		else:
			row.append('1')
		data.append(row)
		return data
class EncodingParameters(Frame):
	def __init__(self,parent):
		self.parent = parent

		self.encodingLabel = Label(parent,text='Encoding')
		self.encodingLabel.grid(column = 0, row = 0)
		self.encodingOptions = {'Direct','L-System','CPPN'}
		self.encodingVar = StringVar(parent)
		self.encodingVar.set('Direct')
		self.encodingMenu = OptionMenu(parent,self.encodingVar,*self.encodingOptions)
		self.encodingMenu.grid(column = 1, row = 0, columnspan = 1)

		self.treeDepthLabel = Label(parent,text='Tree depth')
		self.treeDepthLabel.grid(column = 0, row = 1)
		self.treeDepthScale = Scale(parent, from_=0, to=20, orient=HORIZONTAL)
		self.treeDepthScale.set(4)
		self.treeDepthScale.grid(column=1,row=1)

	def getSettings(self):
		data = []
		row = []
		data.append(row)
		return data

class ModuleParameters(Frame):
	def __init__(self,parent):
		self.parent = parent
	
		moduleTypesText = "Module types:"
		maxNModulesTypesText = "Max number module types"
		numberOfModulesText = "Number of Modules"
		maxNumberModulesText = "Max number of Modules"
		maximumForceModulesText = "Max Force Modules"
		maximumForceSensorText = "Max Force Sensor Modules"

		self.maxNModulesTypesLabel = Label(parent,text=maxNModulesTypesText)
		self.maxNModulesTypesLabel.grid(column = 0, row = 0)
		self.mmVar = IntVar(parent,5)
		self.mmVar.trace("w", self.moduleSizeChanged)
		self.mmScale = Scale(parent, from_=0, to=10, orient=HORIZONTAL,resolution=1, variable = self.mmVar)
		self.mmScale.grid(column = 1, row = 0)
		self.mmText=Entry(parent, textvariable = self.mmVar)
		self.mmText.grid(column = 2, row = 0)
		
		#self.numberOfModulesLabel = Label(parent,text=numberOfModulesText)
		#self.numberOfModulesLabel.grid(column = 0, row = 1)
		self.maxNumberModulesLabel = Label(parent,text=maxNumberModulesText)
		self.maxNumberModulesLabel.grid(column = 0, row = 1)
		self.mnVar = IntVar(parent,20)
		#self.mnVar.trace("w")
		self.mnScale = Scale(parent, from_=0, to=200, orient=HORIZONTAL,resolution=1, variable = self.mnVar)
		self.mnScale.grid(column = 1, row = 1)
		self.mnText=Entry(parent, textvariable = self.mnVar)
		self.mnText.grid(column = 2, row = 1)
		
		self.maximumForceModulesLabel = Label(parent,text=maximumForceModulesText)
		self.maximumForceModulesLabel.grid(column = 0, row = 2)
		self.maxFVar = DoubleVar(parent,1.5)
		self.maxFText=Entry(parent, textvariable = self.maxFVar)
		self.maxFText.grid(column = 2, row = 2, columnspan = 2)

		self.maximumForceSensorLabel = Label(parent,text=maximumForceSensorText)
		self.maximumForceSensorLabel.grid(column = 0, row = 3)
		self.maxFSVar = DoubleVar(parent,80)
		self.maxFSText=Entry(parent, textvariable = self.maxFSVar)
		self.maxFSText.grid(column = 2, row = 3)

		self.moduleTypesLabel = Label(parent,text=moduleTypesText)
		self.moduleTypesLabel.grid(column = 0, row = 5)

		currentRow = 6
		self.moduleTypesText = ['0','1','2','3','4','5','6','7','8','9']
		self.moduleTypeOptionsText = ['0','1','2','3','4','5','6','7','8','9']
		self.moduleLabels = []
		self.moduleTypes = []
		self.moduleTypeEntries = []
		for i in range(len(self.moduleTypesText)):
			self.moduleLabels.append(Label(parent,text=self.moduleTypesText[i]))
			self.moduleLabels[i].grid(column = 0, row = currentRow + i)
			if i == 0:
				self.moduleTypes.append(IntVar(parent,1))
			else:
				self.moduleTypes.append(IntVar(parent,4))
			self.moduleTypeEntries.append(Entry(parent,textvariable = self.moduleTypes[i]))
			self.moduleTypeEntries[i].grid(column = 1, row = currentRow + i)
		self.moduleSizeChanged()
		
		
	def moduleSizeChanged(self, *args):
		for i in range(10):
			if (i < self.mmVar.get()):
				self.moduleTypeEntries[i].config(state = 'normal')
			else:
				self.moduleTypeEntries[i].config(state = 'disabled')

	def getSettings(self):
		data = []
		row = []
		row.append('#amountModules')
		row.append(str(self.mmVar.get()))
		data.append(row)
		row = []
		row.append('#maxAmountModules' )
		row.append(str(self.mnVar.get()))
		data.append(row)
		row = []
		row.append('#maxForceModules' )
		row.append(str(self.maxFVar.get()))
		data.append(row)
		row = []
		row.append('#maxForceSensorModules' )
		row.append(str(self.maxFSVar.get()))
		data.append(row)
		row = []
		row.append('#moduleTypes')
		for i in range(self.mmVar.get()):
			row.append(str(self.moduleTypes[i].get()))
		data.append(row)
		return data
		
class GeneralParameters(Frame):
	def __init__(self,parent, directory):
		self.master = parent
		repositoryText = "Repository"
		serverModeText = "Server mode"
		initialSeedText = "Initial seed"
		verboseText = "Verbose"
		sendGenomeAsText = "Send genome as: "
		reopenConnectionsText = "Reopen connections"
		self.repositoryLabel = Label(parent,text=repositoryText)
		self.repositoryLabel.grid(column = 0, row = 0)
		#self.repositoryText = Text(parent)#,Text=directory)
		#self.repositoryText.grid(column = 1, row = 0)
		#self.repositoryText = Label(parent,text=directory)
		#self.repositoryText.grid(column=1, row = 0)
		self.repositoryText=Entry(parent,width=60)
		self.repositoryText.insert(END,directory)	
		self.repositoryText.grid(column=1,row=0, columnspan = 2)
#		self.repositoryText.configure(text=directory)	

		self.serverModeLabel = Label(parent,text=serverModeText)
		self.serverModeLabel.grid(column = 0, row = 1)
		self.serverCheckVar = IntVar()
		self.serverModeCheck = Checkbutton(parent, variable = self.serverCheckVar)
		self.serverModeCheck.grid(column = 1, row = 1)

		self.initialSeedLabel = Label(parent,text=initialSeedText)
		self.initialSeedLabel.grid(column = 0, row = 2)
		self.initialSeedText=Entry(parent,width=60)
		self.initialSeedText.insert(END,'0')	
		self.initialSeedText.grid(column=1,row=2, columnspan = 1)
		
		self.verboseLabel = Label(parent,text=verboseText)
		self.verboseLabel.grid(column = 0, row = 3)
		self.verbosity = {'none','error','debug'}
		self.verboseDrop = StringVar(parent)
		self.verboseDrop.set('none')
		self.verboseMenu = OptionMenu(parent,self.verboseDrop,*self.verbosity)
		self.verboseMenu.grid(column = 1, row = 3, columnspan = 1)
		
		self.sendGenomeAsLabel = Label(parent,text=sendGenomeAsText)
		self.sendGenomeAsLabel.grid(column = 0, row = 4)
		self.sendGenomeAsChoices = {'signal','file'}
		self.sendGenomeAsDrop = StringVar(parent)
		self.sendGenomeAsDrop.set('signal')
		self.sendGenomeAsMenu = OptionMenu(parent,self.sendGenomeAsDrop,*self.sendGenomeAsChoices)
		self.sendGenomeAsMenu.grid(column = 1, row = 4, columnspan = 1)
		
		self.reopenConnectionsLabel = Label(parent,text=reopenConnectionsText)
		self.reopenConnectionsLabel.grid(column = 0, row = 5)
		self.reopenConnectionsCheck = Checkbutton(parent)
		self.reopenConnectionsCheck.grid(column = 1, row = 5)

	def getSettings(self):
		data = []
		row = []
		row.append("#repository")
		row.append(self.repositoryText.get())
		data.append(row)
		row = []
		row.append("#servermode")
		row.append(self.serverCheckVar.get())
		data.append(row)
		row = []
		row.append("#initialSeed")
		row.append(self.initialSeedText.get())
		data.append(row)
		row = []
		row.append("#verbose")
		if (self.verboseDrop.get() == 'debug'):
			row.append('1')
		else:
			row.append('0')
		data.append(row)
		row = []
		row.append("#sendGenomeAsSignal")
		if (self.sendGenomeAsDrop.get() == 'signal'):
			row.append('1')
		else:
			row.append('0')
		data.append(row)
		
		return data


class EAParameters(Frame):
	def __init__(self,parent):
		self.master = parent
		self.EATypeLabel = Label(parent,text="Evolution type")
		self.EATypeLabel.grid(column = 0, row = 0)
		self.EATypeOptions = {'Steady State','Generational'}
		self.EATypeVar = StringVar(parent)
		self.EATypeVar.set('Steady State')
		self.EATypeMenu = OptionMenu(parent,self.EATypeVar,*self.EATypeOptions)
		self.EATypeMenu.grid(column = 1, row = 0, columnspan = 1)

		self.MutationRateLabel = Label(parent,text="Control mutation rate")
		self.MutationRateLabel.grid(column = 0, row = 1)
		self.mrVar = DoubleVar(parent,0.1)
		self.mrVar.trace("w", self.callback)
		self.mrScale = Scale(parent, from_=0, to=1.0, orient=HORIZONTAL,resolution=0.02, variable = self.mrVar)
		self.mrScale.grid(column = 1, row = 1)
		self.mrText=Entry(parent, textvariable = self.mrVar)
		self.mrText.grid(column = 2, row = 1)

		self.morphMutationRateLabel = Label(parent,text="Morphology mutation rate")
		self.morphMutationRateLabel.grid(column = 0, row = 2)
		self.morphmrVar = DoubleVar(parent,0.1)
		self.morphmrVar.trace("w", self.callback)
		self.morphmrScale = Scale(parent, from_=0, to=1.0, orient=HORIZONTAL,resolution=0.02, variable = self.morphmrVar)
		self.morphmrScale.grid(column = 1, row = 2)
		self.morphmrText=Entry(parent, textvariable = self.morphmrVar)
		self.morphmrText.grid(column = 2, row = 2)

		self.selectionTypeLabel = Label(parent,text="Selection")
		self.selectionTypeLabel.grid(column = 0, row = 4)
		self.selOptions = {'Random','Fitness Proportional', 'Tournament'}
		self.selVar = StringVar(parent)
		self.selVar.set('Random')
		self.selMenu = OptionMenu(parent,self.selVar,*self.selOptions)
		self.selMenu.grid(column = 1, row = 4, columnspan = 1)

		self.replacementTypeLabel = Label(parent,text="Replacement")
		self.replacementTypeLabel.grid(column = 0, row = 5)
		self.repOptions = {'Random','Fitness Proportional', 'Tournament', 'Replace worst'}
		self.repVar = StringVar(parent)
		self.repVar.set('Random')
		self.repMenu = OptionMenu(parent,self.repVar,*self.repOptions)
		self.repMenu.grid(column = 1, row = 5, columnspan = 1)


		self.maxGenLabel = Label(parent,text="Max generations")
		self.maxGenLabel.grid(column = 0, row = 6)
		self.maxGenVar = IntVar(parent,1000)
		self.maxGenVar.trace("w", self.callback)
		self.maxGenScale = Scale(parent, from_=0, to=10000, orient=HORIZONTAL,resolution=100, variable = self.maxGenVar)
		self.maxGenScale.grid(column = 1, row = 6)
		self.maxGenText=Entry(parent, textvariable = self.maxGenVar)
		self.maxGenText.grid(column = 2, row = 6)
		
		self.generationIntervalLabel = Label(parent,text="Generation Interval")
		self.generationIntervalLabel.grid(column = 0, row = 7)

		self.populationSizeLabel = Label(parent,text="Population Size")
		self.populationSizeLabel.grid(column = 0, row = 8)
		self.popVar = IntVar(parent,90)
		self.popVar.trace("w", self.callback)
		self.popScale = Scale(parent, from_=0, to=400, orient=HORIZONTAL,resolution=1, variable = self.popVar)
		self.popScale.grid(column = 1, row = 8)
		self.popText=Entry(parent, textvariable = self.popVar)
		self.popText.grid(column = 2, row = 8)

		self.crossoverLabel = Label(parent,text="Crossover Chance")
		self.crossoverLabel.grid(column = 0, row = 9)
		self.crossVar = DoubleVar(parent,0.0)
		self.crossVar.trace("w", self.callback)
		self.crossScale = Scale(parent, from_=0, to=1, orient=HORIZONTAL,resolution=0.02, variable = self.crossVar)
		self.crossScale.grid(column = 1, row = 9)
		self.crossText=Entry(parent, textvariable = self.crossVar)
		self.crossText.grid(column = 2, row = 9)
			
	def callback(self,*args):
		a = 1
		#print("variable changed!")

	def getSettings(self):
		data = []
		row = []
		row.append('#evolutionType')
		row.append((self.EATypeVar.get()))
		data.append(row)
		
		row = []
		row.append('#mutationRate')
		row.append(str(self.mrVar.get()))
		data.append(row)
		
		row = []
		row.append('#morphMutRate')
		row.append((self.morphmrVar.get()))
		data.append(row)

		row = []
		row.append('#selectionType')
		row.append((self.selVar.get()))
		data.append(row)

		row = []
		row.append('#replacementType')
		row.append((self.repVar.get()))
		data.append(row)

		row = []
		row.append('#maxGeneration')
		row.append(str(self.maxGenVar.get()))
		data.append(row)

		row = []
		row.append('#generationInterval')
		row.append(str(self.maxGenVar.get()))
		data.append(row)

		row = []
		row.append('#populationSize')
		row.append(str(self.popVar.get()))
		data.append(row)

		row = []
		row.append('#crossover')

		if (self.crossVar.get() > 0.0):
			row.append('1')
		else:
			row.append('0')
		data.append(row)
		row = []
		row.append('#crossoverRate')
		row.append(str(self.crossVar.get()))
		data.append(row)

		return data
