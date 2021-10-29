import json
import os
import subprocess
import time
import tkinter as tk

from printer import Printer
from RoboFab import RoboFab_host

NUMBER_OF_PRINTERS = 3


class RobofabGUI:
    def __init__(self, configurationData):

        # extract information from configuration data:
        self.configurationData = configurationData
        self.logDirectory = configurationData["logDirectory"]
        self.parametersFileForGenerate = configurationData["parametersFile"]
        self.coppeliasimFolder = configurationData["coppeliasimFolder"]

        # start setup of main window:
        self.mainWindow = tk.Tk()
        label_title = tk.Label(text="RoboFab GUI", font='Helvetica 18 bold')
        label_title.grid(row=0, column=0, columnspan=4)
        self.label_globalErrors = tk.Label(fg="red")
        self.label_globalErrors.grid(row=1, column=0, columnspan=4)
        self.globalErrorText = ""

        # lists of things that appear in each printer frame:
        printer_frame = []
        self.label_printer_title = []
        self.button_printerLoadFile = []
        self.button_initPrinter = []
        self.button_slice = []
        self.button_print = []
        self.button_assemble = []
        self.button_generate = []
        self.label_robotID = []
        self.entry_robotID = []
        self.robotID_loaded = []
        self.label_printerStatus = []

        self.printerStatus = []  # string for "file_selected",
        self.printerObjects = []  # list of printer objects
        self.printingInProgress = [] # flags for when a print is currently happening on this printer
        self.printingDone = [] # flags for after a print has finished (so we can assemble)

        # loop for the printer frames:
        for i in range(NUMBER_OF_PRINTERS):
            printer_frame.append(tk.Frame(width=25, borderwidth=1, relief=tk.RAISED))
            self.label_printer_title.append(
                tk.Label(text="Printer {}".format(i), master=printer_frame[i], font='Helvetica 12 bold'))
            self.label_printerStatus.append(tk.Label(text="\n",master=printer_frame[i]))
            self.label_robotID.append(tk.Label(text="Robot ID: ", master=printer_frame[i]))
            self.button_printerLoadFile.append(
                tk.Button(text="Select robot ID", width=20, height=2, bg="LightCyan",
                          command=lambda i=i: self.handlerButtonSelectRobotID(i),
                          master=printer_frame[i]))
            self.entry_robotID.append(tk.Entry(width=20, master=printer_frame[i]))
            self.button_initPrinter.append(tk.Button( text="Connect to printer", width=20, height=2,
                                                      command=lambda i=i: self.handlerButtonPrinterInit( i ),
                                                      master=printer_frame[i] ))
            self.button_slice.append(
                tk.Button(text="Slice", width=20, height=2, command=lambda i=i: self.handlerButtonSlice(i),
                          master=printer_frame[i]))
            self.button_print.append(
                tk.Button(text="Print", width=20, height=2, command=lambda i=i: self.handlerButtonPrint(i),
                          master=printer_frame[i]))
            self.button_assemble.append(
                tk.Button(text="Assemble", width=20, height=2, command=lambda i=i: self.handlerButtonAssemble(i),
                          master=printer_frame[i]))
            self.button_generate.append(
                tk.Button(text="Run are-generate", width=20, height=2, command=lambda i=i: self.buttonHandlerRunGenerate(i),
                          master=printer_frame[i]))

            printer_frame[i].grid(row=2, column=i)  # arrange this frame in the overall window
            # pack frame:
            self.label_printer_title[i].pack()
            self.label_printerStatus[i].pack()
            self.label_robotID[i].pack()
            self.entry_robotID[i].pack()
            self.button_printerLoadFile[i].pack()
            self.button_generate[i].pack()
            self.button_initPrinter[i].pack()
            self.button_slice[i].pack()
            self.button_print[i].pack()
            self.button_assemble[i].pack()

            self.robotID_loaded.append(
                "")  # this is a list of strings that represent the IDs of currently loaded robots
            self.printerStatus.append("none")  # this is a list of strings that represent the status of each printer
            self.printerObjects.append(None)
            self.printingInProgress.append(False)
            self.printingDone.append(False)


        #  UR5 startup section:
        self.robofabObject = None
        frame_startupUR5 = tk.Frame(width=25, borderwidth=1, relief=tk.RAISED)
        frame_startupUR5.grid(row=2, column=3)
        self.button_startupUR5 = tk.Button( text="Initialise UR5", width=20, height=2,
                                            command=self.buttonHandlerStartUR5,
                                            master=frame_startupUR5 )
        self.label_ur5Status = tk.Label( text="", master=frame_startupUR5 )

        button_organBankManager = tk.Button(text="Organ Bank Manager", width=20, height=2,
                                             command=self.organBankManager,
                                             master=frame_startupUR5)

        # arrange withing ARE-generate frame:
        self.button_startupUR5.pack()
        self.label_ur5Status.pack()
        button_organBankManager.pack()

        # start GUI:
        self.mainWindow.after(1, self.timedUpdateButtons())
        tk.mainloop()

    def timedUpdateButtons(self):
        self.updateButtonColours()
        self.mainWindow.after(100, self.timedUpdateButtons)

    def handlerButtonAssemble(self, printer_number):
        if not self.printingDone [printer_number]:
            self.label_printerStatus[printer_number]["text"] = "Can't assemble,\nprinting not done"
        elif self.robofabObject is None:
            self.label_printerStatus[printer_number]["text"] = "Can't assemble,\nRoboFab not initialised"
        else:
            self.label_printerStatus[printer_number]["text"] = "*busy assembling*\n"
            self.mainWindow.update()

            self.robofabObject.setupRobotObject( robotID=self.robotID_loaded[printer_number], printer=self.printerObjects[printer_number])
            self.robofabObject.buildRobot( printer=self.printerObjects[printer_number])


    def buttonHandlerStartUR5( self ):
        if not self.robofabObject is None:
            print("UR5 already started")
            return
        else:
            self.robofabObject = RoboFab_host ( self.configurationData )



    def updateButtonColours( self ):
        self.label_globalErrors["text"] = self.globalErrorText

        # if UR5 not yet initialised, active button
        if self.robofabObject is None:
            self.button_startupUR5["bg"]="yellow"
        else:
            self.button_startupUR5["bg"]="green"


        for i in range(NUMBER_OF_PRINTERS):

            # if the printer is currently printing, check if it's finished:
            if self.printingInProgress[i] == True:
                if self.printerObjects[i].checkIfPrintingFinished():
                    # print is now finished
                    self.printingInProgress [ i ] = False
                    self.printingDone [ i ] = True
                    self.label_printerStatus[i]["text"] = "Ready to assemble"
                    self.button_print[i]["bg"]="green"
                    self.button_assemble[i]["bg"]="yellow"
                else:
                    # printing is still ongoing, grey out all buttons:
                    self.button_printerLoadFile[i]["bg"]="grey"
                    self.button_generate[i]["bg"]="grey"
                    self.button_initPrinter[i]["bg"]="grey"
                    self.button_slice[i]["bg"]="grey"
                    self.button_print[i]["bg"]="grey"
                    self.button_assemble[i]["bg"]="grey"
            else:
                #printing not in progress

                # find colour for "select robot ID" button
                if self.robotID_loaded[i] == self.entry_robotID[i].get():
                    #  ID already loaded
                    self.button_printerLoadFile[i]["bg"] = "green"
                elif "morphGenome_{}".format(self.entry_robotID[i].get()) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
                    #  the new ID exists in the waiting_to_be_built folder
                    self.button_printerLoadFile[i]["bg"] = "yellow"
                else:
                    #  the selected robot ID does not exist in the waiting_to_be_built folder
                    self.button_printerLoadFile[i]["bg"] = "red"

                # find colour for are-generate button:
                if "blueprint_{}.csv".format(self.robotID_loaded[i]) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
                    # have made the blueprint already
                    self.button_generate[i]["bg"] = "green"
                elif "morphGenome_{}".format(self.robotID_loaded[i]) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
                    # no blueprint, but there is a morphGenome, so this button needs pressing:
                    self.button_generate[i]["bg"] = "yellow"
                else:
                    self.button_generate[i]["bg"] = "grey"

                # find colour for init printer button:
                if self.printerObjects[i] is None:
                    # not yet initialised
                    self.button_initPrinter[i]["bg"] = "yellow"
                else:
                    # is already initialised
                    self.button_initPrinter[i]["bg"] = "green"

                # find colour for "slice" button:
                if self.printerObjects[i] is None:
                    # printer not initialised
                    self.button_slice[i]["bg"] = "grey"
                elif "mesh_{}.gcode".format ( self.robotID_loaded [ i ] ) in os.listdir ( "./gcode/"  ):
                    # gcode is already done
                    self.button_slice [ i ] [ "bg" ] = "green"
                elif not "blueprint_{}.csv".format(self.robotID_loaded[i]) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
                    # haven't made the blueprint yet
                    self.button_slice[i]["bg"] = "grey"
                else:
                    # blueprint exists but hasn't been sliced, so ready to go!
                    self.button_slice[i]["bg"] = "yellow"

                # find colour for print button:
                if self.printingDone[i]:
                    self.button_print [ i ] [ "bg" ] = "green"
                elif "mesh_{}.gcode".format ( self.robotID_loaded [ i ] ) in os.listdir ( "./gcode/"  ):
                    # slicing done, ready to print
                    self.button_print [ i ] [ "bg" ] = "yellow"
                else:
                    self.button_print [ i ] [ "bg" ] = "grey"

                # find colour for assemble button:
                if "list_of_organs_{}.csv".format ( self.entry_robotID [ i ].get () ) in os.listdir (
                    "{}/waiting_to_be_evaluated".format ( self.logDirectory ) ):
                    # organs list already exists, so has been assembled
                    self.button_assemble [ i ] [ "bg" ] = "green"
                elif self.printingDone[i]:
                    # printing done, so ready to do assembly (but not yet done):
                    self.button_assemble [ i ] [ "bg" ] = "yellow"
                else:
                    self.button_assemble [ i ] [ "bg" ] = "grey"


    def handlerButtonPrint(self, printer_number):
        self.label_printerStatus[printer_number]["text"] = "*busy setting print*\n"
        self.mainWindow.update()

        filename = "mesh_{}".format(self.robotID_loaded[printer_number])
        self.printerObjects[printer_number].uploadGCodeToOctoPrint("gcode/{}".format(filename))
        self.printerObjects[printer_number].printFileOnOctoprint(filename)
        self.label_printerStatus[printer_number]["text"] = "Printing in progress"
        self.printingInProgress [printer_number] = True

    def handlerButtonSlice(self, printer_number):
        self.label_printerStatus[printer_number]["text"] = "*busy slicing*\n"
        self.mainWindow.update()

        robot_ID = self.robotID_loaded[printer_number]
        print("Trying to slice robot with ID {}".format(robot_ID))
        self.printerObjects[printer_number].createSTL(robot_ID)
        self.printerObjects[printer_number].slice("mesh_" + robot_ID)

        self.label_printerStatus[printer_number]["text"] = "Slicing done"

    def handlerButtonPrinterInit( self, printer_number ):
        # check the status is acceptable:
        if not self.printerObjects[printer_number] is None:
            self.label_printerStatus[printer_number]["text"] = "Already initialised\n"
            print("Already initialised")
        else:
            self.label_printerStatus[printer_number]["text"] = "*busy*\n"
            self.mainWindow.update()
            # setup printer object:
            IPAddress = self.configurationData["network"]["PRINTER{}_IP_ADDRESS".format(printer_number)]
            try:
                # self.printerObjects[printer_number] = Printer(IPAddress=IPAddress, config=configurationData["PRINTER_{}".format(printer_number)])
                self.printerObjects[printer_number] = Printer(IPAddress=None, configurationData=configurationData,
                                                              printer_number=printer_number)
                print("Skipping printer connection")
            except Exception as inst:
                self.globalErrorText = "Failed to initialise printer {} at {}".format(printer_number, IPAddress)
                raise inst

            self.label_printerStatus[printer_number]["text"] = "Printer ready\n"

    def handlerButtonSelectRobotID(self, printer_number):
        # get and save the new robot ID:
        self.robotID_loaded[printer_number] = self.entry_robotID[printer_number].get()
        # display new robot ID:
        self.label_robotID[printer_number]["text"] = "Robot ID: {}".format(
            self.robotID_loaded[printer_number])

        # change status:
        self.label_printerStatus[printer_number]["text"] = "Robot selected\n"
        self.printingDone=False

    def buttonHandlerRunGenerate(self, printer_number):
        # self.robotIDLoadedGenerate = self.entryIDForGenerate.get()  # get and save the new robot ID
        # self.entryIDForGenerate.delete(0, tk.END)  # clear the text input
        robotID = self.robotID_loaded[printer_number]

        # check the morphGenome exists:
        if not "morphGenome_{}".format(robotID) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
            self.label_printerStatus[printer_number]["text"] = "error!\nmorphGenome_{} does not exist".format(
                robotID)
            return 1
        self.label_printerStatus[printer_number]["text"] = "generating {}\nplease wait...".format(robotID)
        self.mainWindow.update()

        setIndividualToLoadInParametersFileForGenerate(self.parametersFileForGenerate, robotID)
        runAreGenerate(self.coppeliasimFolder, self.parametersFileForGenerate)

        if not "blueprint_{}.csv".format(robotID) in [x for x in os.listdir(
                "{}/waiting_to_be_built".format(self.logDirectory)) if x.startswith("blueprint_")]:
            self.label_printerStatus[printer_number]["text"] = "ran generate for {}\nbut blueprint not generated".format(
                robotID)
        else:
            self.label_printerStatus[printer_number]["text"] = "Generated blueprint for {}\nsuccess".format(robotID)

    def organBankManager(self):
        # if self.robofabObject is None:
        #     print("UR5 not initialised, can't manage organ bank")
        #     return 1

        print("Manage organ bank...")

        window_bankManager = tk.Tk()
        frame = tk.Frame(master=window_bankManager, width=750, height=500)
        tk.Button(master=window_bankManager, text="Close", command=window_bankManager.destroy).pack()
        frame.pack()

        self.listOfButtons_slots = []
        for i, organ in enumerate(self.robofabObject.organBank.organsList):
            text = organ.friendlyName.replace(" organ", "")
            # add the address to the text:
            try:
                text = text + "\n"+hex(int(organ.I2CAddress))
            except ValueError:
                text = text+"\n"+organ.I2CAddress # I2CAddress is actually an IP address

            colour = "green" if organ.isInBank else "red"

            self.listOfButtons_slots.append(
                tk.Button(master=frame, text=text, bg=colour,
                          command=lambda i=i: self.handlerButtonBankSlot(i)
                          )
            )
            xPosition=organ.positionTransformWithinBankOrRobot[0,3]*1500
            yPosition=(0.4-organ.positionTransformWithinBankOrRobot[1,3])*1000
            self.listOfButtons_slots[i].place(x=xPosition, y=yPosition)

    def handlerButtonBankSlot(self,i):
        print("Swapping state of "+self.robofabObject.organBank.organsList[i].organsList[i].friendlyName)

        if self.robofabObject.organBank.organsList[i].organsList[i].isInBank:
            self.robofabObject.organBank.organsList[i].organsList[i].friendlyName = False
            self.listOfButtons_slots[i]["bg"] = "red"
        else:
            self.robofabObject.organBank.organsList[i].organsList[i].friendlyName = True
            self.listOfButtons_slots[i]["bg"] = "green"


def setIndividualToLoadInParametersFileForGenerate(filepath, robotID="0_0"):
    genToLoad = robotID.split("_")[0]
    indToLoad = robotID.split("_")[1]
    print("Changing the file: {}\n to use the robot from generation: {}, individual: {}".format(filepath,
                                                                                                robotID.split("_")[0],
                                                                                                robotID.split("_")[1]))

    os.rename(filepath, filepath + "_bak")
    string_to_write = ""
    with open(filepath + "_bak", "r") as file:
        for line in file:
            if line.startswith("#genToLoad"):
                string_to_write = string_to_write + "#genToLoad,int," + str(genToLoad) + "\n"
            elif line.startswith("#indToLoad"):
                string_to_write = string_to_write + "#indToLoad,int," + str(indToLoad) + "\n"
            else:
                string_to_write = string_to_write + line
    with open(filepath, "w") as file:
        file.write(string_to_write)


def runAreGenerate(coppeliasimFolder, parametersFilepath):
    print("Trying to run ARE-generate...")
    terminalCommand = "./are_sim.sh generate -h -g{}".format(parametersFilepath)
    output = str(
        subprocess.check_output(terminalCommand, cwd="{}".format(coppeliasimFolder), shell=True,
                                stderr=subprocess.STDOUT))
    # print("Output from command was:\n{}".format(output))


if __name__ == "__main__":
    # Make the settings file then extract the settings from it
    from makeConfigurationFile import makeFile as makeConfigurationFile

    makeConfigurationFile(location="BRL")  # <--- change this depending on if you're in York or BRL
    configurationData = json.load(
        open('configuration_BRL.json'))  # <--- change this depending on if you're in York or BRL

    gui = RobofabGUI(configurationData)
