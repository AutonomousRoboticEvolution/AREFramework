import tkinter as tk
from tkinter import *
from tkinter import ttk
import subprocess
import os
import numpy as np
import robot_connection
import Blueprint as bp
import csv
import SettingsModule as settings
import matplotlib.pyplot as plt
import matplotlib
from shutil import copyfile
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
#from matplotlib.backends.backend_tkagg import FigureCanvasAgg
import webbrowser


# TODO: variables should be put somewhere else
directory = os.getcwd()
fileRepository = "files"
runNumber = 0
buttonSize = [16,1]
individualCounter = 0


if (os.name == 'nt'):
    print("Running windows")
    directory = "C:/Program Files/V-REP3/V-REP_PRO_EDU"
else:
    directory = "/home/ebb505/ARE_Project/vrep"
    print("Not running windows")
# set directory here for linux machines

def make_label(parent, img):
    label = Label(parent, image=img)
    label.pack()

# Information that can be used to store for loading an individual.
class Individual:
    def __init__(self, num):
        self.num = num;
        self.genome = np.random.rand(1,10)
        self.genomeInformation = []
        self.blueprint = bp.Blueprint(num)
        ## This information is going to be used in the blueprint
        self.componentType = []
        self.componentAbsPos = []
        self.componentAbsOri = []
    def save(self, path):
        # TODO open csv
        a = 0
    ## This method is going to load parameters from the specified phenotype file
    def load(self):
        # File path
        pathGen = directory + '/' + fileRepository + '/morphologies98/' + 'phenotype' + str(self.num) +'.csv'
        print(pathGen)
        # Open csv
        data = []
        with open(pathGen, 'rt') as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',')
            for row in csvreader:
                data.append(row)
                #print(row)
                if row:
                    # Store information for blueprint
                    if row[0] == '#ModuleType:':
                        self.componentType.append(int(row[1]))
                    if row[0] == '#AbsPosition:':
                        self.componentAbsPos.append(list(map(float,row[1:4])))
                    if row[0] == '#AbsOrientation:':
                        self.componentAbsOri.append(list(map(float,row[1:4])))

# Object to store individuals for a queue
class PopulationQueue:
    def __init__(self):
        self.individuals = []
    def addIndividualToQueue():
        self.individuals.append(Individual)

# Frame containing data
class DataFrame(Frame):
    def __init__(self,parent):
        self.master = parent
        # load the maximum and average fitness of an evolutionary run
        Button(parent, text="Load Progression", height = buttonSize[1], command=self.plotRun).grid(row=1, column=0, sticky=W)
        self.imglabel = Label(self.master)
        self.imglabel.grid(row=2, column = 0)
        Button(parent, text="Location in phenotype space", height = buttonSize[1], command=self.plotRun).grid(row=2, column=0, sticky=W)
        self.imglabel = Label(self.master)
        self.imglabel.grid(row=3, column = 0)
        Button(parent, text="Magnitude of change", height = buttonSize[1], command=self.plotRun).grid(row=3, column=0, sticky=W)
        self.imglabel = Label(self.master)
        self.imglabel.grid(row=4, column = 0)


    # lpots a specific run
    def plotRun(self):
        tk.Frame.__init__(self, self.master)
        # TODO: integrated frame for plots
        # label = tk.Label(self, text="Graph Page!")
        # label.pack(pady=10,padx=10)
        # button1 = ttk.Button(self, text="Back to Home",
        #                    command=lambda: controller.show_frame(StartPage))
        # button1.pack()

        data = []
        dataMax = []
        data25 = []
        data75 = []
        dataMin = []
        with open(directory + "/files/" + "SavedGenerations0.csv") as csv_file:
            csv_reader = csv.reader(csv_file, delimiter = ',')
            for row in csv_reader:
                dataAll = []
                loadInd = True;
                for i, gib in enumerate(row):
                    if (gib == "avg: "):
                        data.append(float(row[i+1]))
                        loadInd = False
                    if gib == "fitness: ":
                        dataMax.append(float(row[i+1]))
                    if (i> 0 and loadInd == True):
                        dataAll.append(float(row[i]))
                dataMin.append(min(dataAll))
            # load individuals

        fig = plt.figure()
        ax = fig.add_subplot(1,1,1)
        ax.plot(data)
        ax.plot(dataMax)
        ax.plot(dataMin)
        plt.show()

    def draw_figure(self,canvas, figure, loc=(0, 0)):
        """ Draw a matplotlib figure onto a Tk canvas

        loc: location of top-left corner of figure on canvas in pixels.
        Inspired by matplotlib source: lib/matplotlib/backends/backend_tkagg.py
        """
        figure_canvas_agg = FigureCanvasTkAgg(figure)
        figure_canvas_agg.draw()
        figure_x, figure_y, figure_w, figure_h = figure.bbox.bounds
        figure_w, figure_h = int(figure_w), int(figure_h)
        photo = tk.PhotoImage(master=canvas, width=figure_w, height=figure_h)

        # Position: convert from top-left anchor to center anchor
        canvas.create_image(loc[0] + figure_w/2, loc[1] + figure_h/2, image=photo)

        # Unfortunately, there's no accessor for the pointer to the native renderer
        tkagg.blit(photo, figure_canvas_agg.get_renderer()._renderer, colormode=2)

        # Return a handle which contains a reference to the photo object
        # which must be kept live or else the picture disappears
        return photo

class SimulationFrame(Frame):
    def __init__(self,parent):
        # Row is just defines the position of the buttons
        row = 0
        self.master = parent # parent frame
        # Button to run V-REP single thread, used later
        #self.runB = Button(parent, text="Run Evolution", command=self.clickRun, height = buttonSize[1], width = buttonSize[0])
        #self.runB.grid(column=0, row=row);
        #row += 1
        # Button to load the best individual
        self.recallB = Button(parent, text="Recall Best", command=self.clickRecallBest, height = buttonSize[1], width = buttonSize[0])
        self.recallB.grid(column=0, row=row);
        row += 1
        # Button to run in parallel
        self.runPB = Button(parent, text="Run Parallel", command=self.clickRunParallel, height = buttonSize[1], width = buttonSize[0])
        self.runPB.grid(column=0,row=row);
        row += 1
        # Button to run on cluster TODO: add command function that can be used
        self.runPC = Button(parent, text="Run on Cluster", height = buttonSize[1], width = buttonSize[0],bg="orange2")
        self.runPC.grid(column=0,row=row);
        row += 1
        # Specify the path to the savedGenerations .csv file
        self.pathGen = directory + '/' + fileRepository + '/' + 'SavedGenerations' + str(runNumber) +'.csv'
        # Loading a population
        self.loadPopulationButton = Button(parent, text="Load Population", command=self.loadPopulation, height = buttonSize[1], width = buttonSize[0])
        self.loadPopulationButton.grid(column=0, row=row);

        # Specifying the run number of a population to load (/save) TODO: actually implement it
        self.popNumVar = IntVar(parent,0)
        self.popNumVar.trace("w", self.callback)
        self.popNumScale = Scale(parent, from_=0, to=99, orient=HORIZONTAL, variable = self.popNumVar)
        self.popNumScale.grid(column = 1, row = row)
        self.popNumText=Entry(parent, textvariable = self.popNumVar)
        self.popNumText.grid(column = 2, row = row)

        # Run evolution
        row += 1
        self.newRunNum = Button(parent, text="Run Evolution", command=self.clickRun, height = buttonSize[1], width = buttonSize[0])
        self.newRunNum.grid(column=0, row=row);

        # Run evolution with individuals evolved from physical environment (popqueue)
        row += 1
        self.runNewHQ = Button(parent, text="Run with seeded pop", command=self.clickRunHQ, height = buttonSize[1], width = buttonSize[0])
        self.runNewHQ.grid(column=0, row=row);

        # Specify the number of cores of the machine.
        row += 1
        self.coreLabel = Label(parent, text="Number of cores")
        self.coreLabel.grid(column = 0, row = row)
        self.coreButton = Scale(parent, from_=0, to=32, orient=HORIZONTAL)
        self.coreButton.set(4)
        # row +=1
        self.coreButton.grid(column = 1, row = row)
        row += 1
        # For the repository
        # TODO: add a command that sets the repository
        self.repLab = Label(parent,text='repository')
        self.repLab.grid(column=0, row = row)
        self.repository=Entry(parent,width=30)
        row+=1
        self.repository.grid(column=0,row=row, columnspan = 1)
        self.repository.configure(text=directory)
        row += 1

        # Just for some output statements
        self.con = Label(parent,text="Output")
        self.con.grid(column=0, row =row)
        #self.n_cores = 6
        #self.runB = Button(parent, text="Run Evolution", command=self.clickRun, height = buttonSize[1], width = buttonSize[0])

        # for queueing a population TODO
        self.popQueue = PopulationQueue()
        self.popQueueList = Listbox(parent, width =60)
        self.popQueueList.grid(column=3,row=2, columnspan = 4, rowspan = 6, sticky=E+W)
        self.popQueueList.bind('<<ListboxSelect>>',self.CurSelect)

        # For queueing individuals from the physical and simulated world? TODO: exact mechanism still needs to be designed
        self.hybridLabel = Label(parent,text="Hybrid Queue")
        self.hybridLabel.grid(column = 3, row = 11)
        self.hybridQueue = PopulationQueue()
        self.hybridQueue = Listbox(parent, width =60)
        self.hybridQueue.grid(column=3,row=12, columnspan = 4, rowspan = 3, sticky=E+W)

        # Inserting individuals in the real world queue. (cuts and pastes)
        self.insert = Button(parent, text="Insert as seed in real", command=self.insertReal, height = buttonSize[1], width = buttonSize[0])
        self.insert.grid(column=3, row=0);
        # TODO: placeholder for inspecting the genome based on whatever. This should call another python script but currently just loads an image
        self.inspect = Button(parent, text="Inspect Genome", command=self.inspectGenome, height = buttonSize[1], width = buttonSize[0])
        self.inspect.grid(column=4, row=0);
        # Loads the specific individual in V-REP
        self.inspect = Button(parent, text="Inspect in VREP", command=self.clickRecallBest, height = buttonSize[1], width = buttonSize[0])
        self.inspect.grid(column=3, row=1);
        # Pushes individuals in a queue file that can be read by an evolutionary run
        self.insert = Button(parent, text="Insert In Queue", command=self.insertQueue, height = buttonSize[1], width = buttonSize[0])
        self.insert.grid(column=4, row=1);

        # tracks which individual is selected
        self.selectedInd = self.popQueueList.curselection()

    #self.imglabel = Label(parent, text="Nothing to see here")
    #self.imageIsLoaded = False
    #self.imglabel.grid(column=4, row=2)

    def callback(self,*args):
        print("variable changed! (TODO)")
    def loadPopulation(self):
        # empty existing queue
        self.popQueue.individuals.clear()
        self.popQueueList.delete(0,self.popQueueList.size())
        self.pathGen = directory + '/' + fileRepository + '/' + 'SavedGenerations' + str(self.popNumVar.get()) +'.csv'

        # open csv and push the individuals to the individuals list
        data = []
        with open(self.pathGen, 'rt') as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',')
            for row in csvreader:
                data.append(row)
        #	print(row)
        finalDat = data[-1]
        readIndividual = False
        individuals = []
        for point in finalDat:
            if (readIndividual == True and len(point) > 0):
                #print(point)
                individuals.append(int(point))
            if (point == 'individuals: '):
                readIndividual = True

        # store genomes loaded from last population


        # Add the genomes to the simulated population queue
        for ind in individuals:
            self.popQueue.individuals.append(Individual(ind))
        for ind in self.popQueue.individuals:
            self.popQueueList.insert(END, "Genome " + str(ind.num)+ " - Fitness " + " ... DM ... Size ... ")

        print(individuals)

    def CurSelect(self, evt):
        value=str(self.popQueueList.get(self.popQueueList.curselection()))
        #print(value)
        self.selectedInd = self.popQueueList.curselection()

    def clickRunHQ(self):
        # make directory
        path = directory + "/files/morphologies" + "99"
        if os.path.exists(path):
            # remove all files within folder
            for file in os.listdir(path):
                file_path = os.path.join(path,file)
                if os.path.isfile(file_path):
                    os.unlink(file_path)
        else:
            os.mkdir(path)

        # copy the loaded individuals to the folder
        copyfile(directory + "/files/Settings0.csv", directory + "/files/settings99.csv")
        for ind in self.popQueue.individuals:
            copyfile(directory + "/files/morphologies0/genome"+str(ind.num) + ".csv", directory + "/files/morphologies99/genome"+str(ind.num)+".csv")

        # Open settings99 (real world evolution placeholder)
        settingsdata = []
        with open(directory + "/files/settings99.csv", 'r') as csvfile:
            filereader = csv.reader(csvfile, delimiter = ',')
            for row in filereader:
                settingsdata.append(row)
        # Load all the stored individuals
        for row in settingsdata:
            for k,gib in enumerate(row):
                if gib == "#individuals":
                    row.clear()
                    row.append("#individuals")
                    for ind in self.popQueue.individuals:
                        row.append(ind.num)
                if gib == "#populationSize":
                    row[k+1] = len(self.popQueue.individuals)
        with open(directory + "/files/settings99.csv", 'w') as csvfile:
            docwriter = csv.writer(csvfile, delimiter = ',')
            for row in settingsdata:
                docwriter.writerow(row)
        # Open V-REP as subprocess
        if (os.name == 'nt'):
            subprocess.Popen([r""+ directory + "/vrep.exe", "-g99", "-g2", "-gfiles"])
        else:
            subprocess.Popen([r""+ directory + "/vrep.sh", "-g99", "-g2", "-gfiles"])


    def setRealWorldPointer(self, p):
        self.realWorldPointer = p

    # function to insert genomes in the real world
    def insertReal(self):
        print("Currently selected: " + str(self.selectedInd))
        # the individual to be moved (based on selection)
        moveInd = int(self.selectedInd[0])
        # the number of the individual as stored in the population queue
        indNum = self.popQueue.individuals[moveInd].num
        # Add the individual from the simulation (this) queue to the real world queue
        # For this the individual in the queue and the popqueuelist entry need to be moved:
        self.realWorldPointer.popQueue.individuals.append(self.popQueue.individuals[moveInd])
        item = self.popQueueList.get(self.popQueueList.curselection())
        self.realWorldPointer.popQueueList.insert(END, item)
        # The individuals shoul now also exit from the existing population;
        # Note that the individuals could also just be copied but then there is the issue of having multiple individuals
        # with the same name existing in the population.
        self.popQueue.individuals.remove(self.popQueue.individuals[moveInd])
        self.popQueueList.delete(self.selectedInd[0])
        path = directory + "/files/morphologies" + "98"
        if (os.path.exists(path)):
            print("path exists")
        else:
            os.mkdir(path)
        # copy files in the population queueto this folder
        copyfile(directory + "/files/morphologies0/genome"+str(indNum) + ".csv", directory + "/files/morphologies98/genome"+str(indNum)+".csv")
        copyfile(directory + "/files/morphologies0/phenotype"+str(indNum) + ".csv", directory + "/files/morphologies98/phenotype"+str(indNum)+".csv")
    def insertQueue(self):
        self.insertReal()
        # update Queue List
        self.saveQueue()

    def saveQueue(self):
        with open(directory + "/files/queue98.csv", 'w') as csvfile:
            docwriter = csv.writer(csvfile, delimiter = ',')
            self.popQueue
            inds = []
            for ind in self.realWorldPointer.popQueue.individuals:
                row = []
                row.append(ind.num)
                docwriter.writerow(row)

    def inspectGenome(self):
        print(self.popQueue.individuals[self.selectedInd[0]].genomeInformation)
    # TODO: Add function that will be useful to add a genome

    def onFrameConfigure(self, event):
        '''Reset the scroll region to encompass the inner frame'''
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))

    def clickRun(self):
        self.con.configure(text="Starting Evolution")
        self.repository.configure(text=directory)
        # TODO: connect headless variable to running the program in headless mode or not.
        if (os.name == 'nt'):
            subprocess.Popen([r""+ directory + "/vrep.exe", "-g0", "-g2", "-gfiles"])
        else:
            subprocess.Popen([r""+ directory + "/vrep.sh", "-g0", "-g2", "-gfiles"])
    # file = 'C:\\Program Files\\V-REP3\\V-REP_PRO_EDU\\vrep.exe'
    # os.system('"'+ file + '"' + ' -g10 - g2 -gfiles')

    def clickRecallBest(self):
        # Loads the best individual
        self.con.configure(text="RECALLING BEST")
        self.repository.configure(text=directory)
        if (os.name == 'nt'):
            subprocess.Popen([r""+ directory + "/vrep.exe", "-g0", "-g9", "-gfiles"])
        else:
            subprocess.Popen([r""+ directory + "/vrep.sh", "-g0", "-g9", "-gfiles"])

    def clickRunParallel(self):
        # Runs evolution in parallel
        self.con.configure(text="Starting Parallel Evolution")
        self.repository.configure(text=directory)
        for i in range(self.coreButton.get()-1):
            #arguments = "-h -g" + str(i) + " -g2 -gfiles -gREMOTEAPISERVERSERVICE_"+ str(i + 1) +"_FALSE_FALSE"
            if (os.name == 'nt'):
                subprocess.Popen([r""+directory+"/vrep.exe","-h", "-g0" ,"-g1" ,"-gfiles", "-gREMOTEAPISERVERSERVICE_"+str(i+104000)+"_TRUE_TRUE"])
            else:
                subprocess.Popen([r""+directory+"/vrep.sh" ,"-h","-g0" ,"-g1" ,"-gfiles", "-gREMOTEAPISERVERSERVICE_"+str(i+104000)+"_TRUE_TRUE"])
        #-gREMOTEAPISERVERSERVICE_"+ str(i+1040000) + "_FALSE_FALSE"])
        if (os.name == 'nt'):
            # subprocess.Popen([r""+directory+"/programming/v_repExtER_Integration/x64/Debug/ERClient.exe",directory+"/files" ,"0" ,str(self.coreButton.get())])
            subprocess.Popen([r""+directory+"/programming/v_repExtER_Integration/x64/Debug/ERClient.exe",directory+"/files" ,"0" ,str(self.coreButton.get())])
        else:
            subprocess.Popen([r""+directory+"/programming/Evolution/build/ERClient/ERClient", directory+"/files" ,"0" ,str(self.coreButton.get())])


class RealWorldFrame(Frame):
    def __init__(self,parent):
        # button refers to command that enables a user to move a genome from the simulation to the real world
        self.insert = Button(parent, text="Insert in Simulation", command=self.insertSimulation)
        self.insert.grid(column=3, row=0);
        self.popLabel = Label(parent, text="Queue(from virtual world)", bg="orange2")
        self.popLabel.grid(column=3,row=1, sticky=E+W)
        # placeholder for a queue, waiting for inividuals to be pushed in the real world.
        self.popQueue = PopulationQueue()
        # popQueue is the object, popQueueList is the actual Tkinter GUI list.
        self.popQueueList = Listbox(parent, width =60)
        self.popQueueList.grid(column=3,row=2, columnspan = 2, rowspan = 6, sticky=E+W)
        for ind in self.popQueue.individuals:
            self.popQueueList.insert(END, "Genome " + str(ind.num))
        self.popQueueList.bind('<<ListboxSelect>>',self.CurSelect)

        # placeholder for a queue with evaluated individuals.
        self.evalLabel = Label(parent, text="evaluatedIndividuals (current population)", bg="green")
        self.evalLabel.grid(column=3,row=8, sticky=E+W)
        self.evaluatedQueue = PopulationQueue()
        self.evaluatedQueueList = Listbox(parent, width =60)
        self.evaluatedQueueList.grid(column=3,row=9, columnspan = 2, rowspan = 3, sticky=E+W)
        self.evaluatedQueueList.insert(END, "p-genome1" + "\t" + "fitness x" + "\t\t"+ " size y")
        self.evaluatedQueueList.insert(END, "p-genome2" + "\t" + "fitness a" + "\t\t"+ " size b")
        self.evaluatedQueueList.insert(END, "p-genome3" + "\t" + "fitness c" + "\t\t"+ " size d")
        self.pushGenomeButton = Button(parent, text="Push selected to virtual queue", height = buttonSize[1], width = 25,activebackground="red")
        self.pushGenomeButton.grid(column=3, row=12)

        # Placeholder for potential connected robots
        self.robots = []

        # Buttons with various functions:
        row = 0;
        self.conLabel = Label(parent, text="For connecting to real robot (deprecated)")
        self.conLabel.grid(column = 0, row = row)
        # connects to a robot via serial
        row += 1;
        self.runRobotButton = Button(parent, text="Open Port", command=self.openPort, height = buttonSize[1], width = buttonSize[0])
        self.runRobotButton.grid(column=0, row=row)
        # send genome via serial
        row += 1;
        self.sendGenomeButton = Button(parent,text="Activate Robot", command = self.sendGenome, height = buttonSize[1], width = buttonSize[0])
        self.sendGenomeButton.grid(column=0, row=row);
        # checks whether the robots are done being evaluated (placeholder)
        row += 1;
        self.checkDoneButton = Button(parent,text="Check Robots", command = self.checkRobots, height = buttonSize[1], width = buttonSize[0])
        self.checkDoneButton.grid(column=0, row=row);
        # close serial port
        row += 1;
        self.closeConButton = Button(parent,text="Close Connections", command = self.closeConnections, height = buttonSize[1], width = buttonSize[0])
        self.closeConButton.grid(column=0, row=row);
        # to open physical evolution, now just runs a simulation
        row += 1;
        self.newLabel = Label(parent, text="---------------")
        self.newLabel.grid(column = 0, row = row)
        row +=1;
        self.runPhysicalEvolution = Button(parent,text="Run Physical", comman = self.clickRun, height = buttonSize[1], width = buttonSize[0])
        self.runPhysicalEvolution.grid(column=0, row=row);
        # loads a population to be loaded for the physical EA run
        row += 1;
        self.loadPopulationButton = Button(parent, text="Load Population", command=self.loadPopulation, height = buttonSize[1], width = buttonSize[0])
        self.loadPopulationButton.grid(column=0, row=row);
        # path to the SavedGenerations98 (physical evolution)
        self.pathGen = directory + '/' + fileRepository + '/' + 'SavedGenerations98.csv'
        # placeholder for a selection probability mechanism # TODO: add proper functionality in plugin
        row += 1;
        self.selLabel = Label(parent, text="Queue Selection Probability",bg="orange2")
        self.selLabel.grid(column = 0, row = row)
        self.selButton = Scale(parent, from_=0, to=1, resolution=0.1, orient=HORIZONTAL,bg="orange2")
        self.selButton.set(1)
        self.selButton.grid(column = 1, row = row)
        row +=1
        #self.popQueueList.bind('<<ListboxSelect>>',self.CurSelect)
        self.selectedInd = self.popQueueList.curselection()
        self.saveBPButton = Button(parent,text="Save Genome Blueprint", command = self.saveBlueprint);
        self.saveBPButton.grid(column=0, row=row);

    def loadPopulation(self):
        # empty existing queue
        self.evaluatedQueue.individuals.clear()
        self.evaluatedQueueList.delete(0,self.popQueueList.size())

        # open csv of evolutionary run
        data = []
        with open(self.pathGen, 'rt') as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',')
            for row in csvreader:
                data.append(row)
        #	print(row)
        finalDat = data[-1]
        readIndividual = False
        individuals = []
        for point in finalDat:
            if (readIndividual == True and len(point) > 0):
                #print(point)
                individuals.append(int(point))
            if (point == 'individuals: '):
                readIndividual = True

        # store genomes loaded from last population
        # Add the genomes to the simulated population queue
        for ind in individuals:
            self.popQueue.individuals.append(Individual(ind))
        for ind in self.popQueue.individuals:
            self.popQueueList.insert(END, "Genome " + str(ind.num) + " - Fitness ... DM ... Size ... ")

        print(individuals)

    def openPort(self):
        # Connects to a robot
        if (len(self.robots) >1):
            print("Cannot connect to more than one robot for now")
            return -1
        #IP address: device specific
        self.robots.append(robot_connection.RobotConnection('192.168.1.101'))

    def sendGenome(self):
        if (len(self.robots)<1):
            print("ERROR: not connected to any robots")
            return -1
        for rob in self.robots:
            #rob.sendMessage("2")
            ind = int(self.selectedInd[0])
            rob.robotStart(self.popQueue.individuals[ind].num)

    def checkRobots(self):
        # Check connectivity
        if (len(self.robots)<1):
            print("ERROR: not connected to any robots")
            return -1
        for rob in self.robots:
            print(rob.receiveMessage())
    def closeConnections(self):
        if (len(self.robots)<1):
            print("ERROR: not connected to any robots")
            return -1
        for rob in self.robots:
            rob.closePort()

    def CurSelect(self, evt):
        # Which individual is currently selected
        value=str(self.popQueueList.get(self.popQueueList.curselection()))
        self.selectedInd = self.popQueueList.curselection()

    def insertSimulation(self):
        # insert the selected individual in simulation (similar to insertReal function in simulation tab)
        print("Currently selected: " + str(self.selectedInd))
        moveInd = int(self.selectedInd[0])
        self.simulationPointer.popQueue.individuals.append(self.popQueue.individuals[moveInd])
        item = self.popQueueList.get(self.popQueueList.curselection())
        self.simulationPointer.popQueueList.insert(END, item)
        self.popQueue.individuals.remove(self.popQueue.individuals[moveInd])
        self.popQueueList.delete(self.selectedInd[0])

    def setSimulationPointer(self, p):
        self.simulationPointer = p

    def saveBlueprint(self):
        print("Currently selected: " + str(self.selectedInd))
        ind = int(self.selectedInd[0])
        print(self.popQueue.individuals[ind].num)
        # Load parameters from phenotype file
        self.popQueue.individuals[ind].load()
        # Initialize blueprint with parameters
        self.popQueue.individuals[ind].blueprint.init(self.popQueue.individuals[ind].componentType,self.popQueue.individuals[ind].componentAbsPos,self.popQueue.individuals[ind].componentAbsOri) # TODO
        # Save blueprint file
        self.popQueue.individuals[ind].blueprint.saveBP(self.popQueue.individuals[ind].num)
        # Generate mesh file
        subprocess.call([os.getcwd() + '/meshGeneratorScript.sh', str(self.popQueue.individuals[ind].num)])

    def clickRun(self):
        #self.repository.configure(text=directory)
        # change population size in the settings file
        settingsdata = []
        if (os.path.isfile(directory + "/files/settings98.csv") == False):
            print("Note, could not find a settings file (settings98.csv) for the physical run")
        if len(self.popQueue.individuals) == 0:
            print("There is no individual in your physical run")
            return 0
        with open(directory + "/files/settings98.csv", 'r') as csvfile:
            filereader = csv.reader(csvfile, delimiter = ',')
            for row in filereader:
                settingsdata.append(row)
        for row in settingsdata:
            for k,gib in enumerate(row):
                if gib == "#generation":
                    row[k+1] = 0;
                if gib == "#individuals":
                    row.clear()
                    row.append("#individuals")
                    for ind in self.popQueue.individuals:
                        row.append(ind.num)
                if gib == "#populationSize":
                    row[k+1] = len(self.popQueue.individuals)
        with open(directory + "/files/settings98.csv", 'w') as csvfile:
            docwriter = csv.writer(csvfile, delimiter = ',')
            for row in settingsdata:
                docwriter.writerow(row)

        if (os.name == 'nt'):
            subprocess.Popen([r""+ directory + "/vrep.exe", "-g98", "-g2", "-gfiles"])
        else:
            subprocess.Popen([r""+ directory + "/vrep.sh", "-g98", "-g2", "-gfiles"])
#		self.simulationPointer.popQueue.individuals.append(self.popQueue.individuals[moveInd])
#		item = self.popQueueList.get(self.popQueueList.curselection())
#		self.simulationPointer.popQueueList.insert(END, item)
#		self.popQueue.individuals.remove(self.popQueue.individuals[moveInd])
#		self.popQueueList.delete(self.selectedInd[0])




class MajorButtons(Frame):
    counter = 0
    def __init__(self,parent):
        Frame.__init__(self,parent)
        self.master = parent

        # Notebook has the four main tabs: simulation, settings, data analysis and real-world
        self.nb = ttk.Notebook(parent, width=800)
        nb = self.nb
        self.simulationFrame =  ttk.Frame(nb)
        self.settingsFrame =  ttk.Frame(nb)
        self.dataAnFrame =  ttk.Frame(nb)
        self.realFrame =  ttk.Frame(nb)
        nb.add(self.simulationFrame, text="Simulation")
        nb.add(self.settingsFrame, text="Settings")
        nb.add(self.dataAnFrame, text="Data Analyser")
        nb.add(self.realFrame, text="Real World")
        nb.select(self.settingsFrame)
        nb.enable_traversal()
        #nb.grid(column=0, row=0);
        nb.pack()

        # Actually instantiating all the windows::::::::::
        # settings is quite large so is imported
        self.settingsWindow = settings.Settings(self.settingsFrame, directory)
        self.simulationWindow = SimulationFrame(self.simulationFrame)
        self.dataWindow = DataFrame(self.dataAnFrame)
        self.realWindow = RealWorldFrame(self.realFrame)
        # The simulation and real world classes need to have a reference to each other.
        self.simulationWindow.setRealWorldPointer(self.realWindow)
        self.realWindow.setSimulationPointer(self.simulationWindow)
    # ::::::::::::::::::::::::::::::::::::::::::::::::

    def create_window(self):
        self.counter += 1
        t = Toplevel(self)
        t.wm_title("Window #%s" % self.counter)
        l = Label(t, text="This is window #%s" % self.counter)
        l.pack(side="top", fill="both", expand=True, padx=100, pady=100)

class Window(Frame):
    def __init__(self, master=None):
        # master should be the TKinter root
        Frame.__init__(self,master)
        self.master = master
        # Major buttons is the root class for all the buttons
        self.mButtons = MajorButtons(self.master)
        self.init_window()

    def init_window(self):
        self.master.title("Ecosystem Manager GUI")
        #self.pack(fill=BOTH, expand=1)

        menu = Menu(self.master)
        self.master.config(menu=menu)

        # Basic menu buttnos on top of the window :::::::
        file = Menu(menu)
        file.add_command(label = "Exit", command = self.client_exit)
        menu.add_cascade(label="File", menu = file)

        edit = Menu(menu)
        edit.add_command(label="Undo")
        menu.add_cascade(label="Edit", menu = edit)

        edit = Menu(menu)
        edit.add_command(label="About")
        edit.add_command(label="Documentation", command = self.dochelper)
        edit.add_command(label="Documentation should open webbrowser")
        menu.add_cascade(label="Help", menu = edit)

    # :::::::::::::::::::::::::::::::::::::::::::::::

    #quitButton = Button(self,text="Quit", command=self.client_exit)
    #quitButton.place(x=0,y=0)
    def client_exit(self):
        exit()
    def dochelper(self):
        url = "https://www.overleaf.com/8988212588bdkjhpfdtckz"
        webbrowser.open(url)

# Page containing information about the settings
class SettingsPage(Frame):
    def __init__(self,parent):
        Frame.__init__(self, parent)
        label = Label(self,text="Settings", font=LARGE_FONT)


if __name__ == "__main__":
    print("directory = " + directory)
    # Initialize root of TKinter GUI
    root = Tk()
    root.geometry("800x600")
    # Window is the main class containing all the functionalities. Can be changed
    app = Window(root)
    root.mainloop()
#window = Tk()
#window.title("ARE Ecosystem Manager")

