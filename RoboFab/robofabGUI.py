import json
import os
import subprocess
import time
import tkinter as tk

from printer import Printer

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
        self.label_robotID = []
        self.entry_robotID = []
        self.robotID_loaded = []
        self.label_printerStatus = []

        self.printerStatus = []  # string for "file_selected",
        self.printerObjects = []  # list of printer objects

        # loop for the printer frames:
        for i in range(NUMBER_OF_PRINTERS):
            printer_frame.append(tk.Frame(width=25, borderwidth=1, relief=tk.RAISED))
            self.label_printer_title.append(
                tk.Label(text="Printer {}".format(i), master=printer_frame[i], font='Helvetica 12 bold'))
            self.label_printerStatus.append(tk.Label(master=printer_frame[i]))
            self.label_robotID.append(tk.Label(text="Robot ID: ", master=printer_frame[i]))
            self.button_printerLoadFile.append(
                tk.Button(text="Select robot ID", width=20, height=2, bg="LightCyan",
                          command=lambda i=i: self.handlerButtonSelectRobotID(i),
                          master=printer_frame[i]))
            self.entry_robotID.append(tk.Entry(width=20, master=printer_frame[i]))
            self.button_initPrinter.append(tk.Button(text="Connect to printer", width=20, height=2,
                                                     command=lambda i=i: self.handlerButtonInitPrinter(i),
                                                     master=printer_frame[i]))
            self.button_slice.append(
                tk.Button(text="Slice", width=20, height=2, command=lambda i=i: self.handlerButtonSlice(i),
                          master=printer_frame[i]))
            self.button_print.append(
                tk.Button(text="Print", width=20, height=2, command=lambda i=i: self.handlerButtonPrint(i),
                          master=printer_frame[i]))
            self.button_assemble.append(
                tk.Button(text="Assemble", width=20, height=2, command=lambda i=i: self.handlerButtonAssemble(i),
                          master=printer_frame[i]))

            printer_frame[i].grid(row=2, column=i)  # arrange this frame in the overall window
            # pack frame:
            self.label_printer_title[i].pack()
            self.label_printerStatus[i].pack()
            self.label_robotID[i].pack()
            self.entry_robotID[i].pack()
            self.button_printerLoadFile[i].pack()
            self.button_initPrinter[i].pack()
            self.button_slice[i].pack()
            self.button_print[i].pack()
            self.button_assemble[i].pack()

            self.robotID_loaded.append(
                "")  # this is a list of strings that represent the IDs of currently loaded robots
            self.printerStatus.append("none")  # this is a list of strings that represent the status of each printer
            self.printerObjects.append(None)

        self.updateButtons()

        #  "run are-generate" section:
        self.robotIDLoadedGenerate = ""
        frame_runGenerate = tk.Frame(width=25, borderwidth=1, relief=tk.RAISED)
        frame_runGenerate.grid(row=2, column=3)
        self.button_runGenerate = tk.Button(text=" Run are-generate", width=20, height=2, bg="DarkOrange",
                                            command=self.buttonHandlerRunGenerate,
                                            master=frame_runGenerate)
        labelGenerateTitle = tk.Label(text="Enter robot ID:", master=frame_runGenerate)
        self.label_generateStatus = tk.Label(text="Status: waiting\n", master=frame_runGenerate)
        self.entryIDForGenerate = tk.Entry(width=20, master=frame_runGenerate)

        # arrange withing ARE-generate frame:
        labelGenerateTitle.pack()
        self.entryIDForGenerate.pack()
        self.button_runGenerate.pack()
        self.label_generateStatus.pack()

        # start GUI:
        self.mainWindow.after(1, self.timedUpdateButtons())
        tk.mainloop()

    def timedUpdateButtons(self):
        self.updateButtons()
        self.mainWindow.after(1000, self.timedUpdateButtons)

    def handlerButtonSlice(self, printer_number):
        # check the status is acceptable:
        if not self.printerStatus[printer_number] == "Robot selected, ready to slice":
            self.globalErrorText = "Select robot first"
            print("Select robot first (status was {})".format(self.printerStatus[printer_number]))
        else:
            self.label_printerStatus[printer_number]["text"] = "*busy*\n"
            self.mainWindow.update()

            robot_ID = self.robotID_loaded[printer_number]
            print("Trying to slice robot with ID {}".format(robot_ID))
            self.printerObjects[printer_number].createSTL(robot_ID)
            self.printerObjects[printer_number].slice("mesh_" + robot_ID)

            self.printerStatus[printer_number] = "Ready to print"

        self.updateButtons()

    def handlerButtonPrint(self, printer_number):
        if not self.printerStatus[printer_number] == "Ready to print":
            self.globalErrorText = "Slice first"
            print("Slice first (status was {})".format(self.printerStatus[printer_number]))
        else:
            self.label_printerStatus[printer_number]["text"] = "*busy setting print*\n"
            self.mainWindow.update()

            filename = "mesh_{}".format(self.robotID_loaded[printer_number])
            self.printerObjects[printer_number].uploadGCodeToOctoPrint("gcode/{}".format(filename))
            self.printerObjects[printer_number].printFileOnOctoprint(filename)
            self.printerStatus[printer_number] == "Printing in progress"

        self.updateButtons()

    def handlerButtonAssemble(self, printer_number):
        if not self.printerStatus[printer_number] == "Ready to assemble":
            self.globalErrorText = "Print first"
            print("Print first (status was {})".format(self.printerStatus[printer_number]))
        else:
            self.label_printerStatus[printer_number]["text"] = "*busy assembling*\n"
            self.mainWindow.update()

        self.updateButtons()

    def updateButtons(self):
        self.label_globalErrors["text"] = self.globalErrorText

        for i in range(NUMBER_OF_PRINTERS):
            status = self.printerStatus[i]

            if self.printerStatus[i] == "Printing in Progress":
                if self.printerObjects[i].checkIfPrintingFinished():
                    # print is now finished
                    self.printerStatus[i] = "Ready to assemble"

            # if the current robot ID is not the same as the text in the input box, we can select new value
            if not self.robotID_loaded[i] == self.entry_robotID[i].get():
                self.button_printerLoadFile[i]["bg"] = "green"
            else:
                self.button_printerLoadFile[i]["bg"] = "grey"

            # check if printer is already initialsied:
            if self.printerObjects[i] is None:
                self.button_initPrinter[i]["bg"] = "green"
            else:
                self.button_initPrinter[i]["bg"] = "grey"

            # display new status:
            self.label_printerStatus[i]["text"] = "Status:\n{}".format(status)
            # set the buttons to active/inactive:
            if status == "none":
                self.button_slice[i]["bg"] = "grey"
                self.button_print[i]["bg"] = "grey"
                self.button_assemble[i]["bg"] = "grey"
            elif status == "Initialised, ready to select":
                self.button_slice[i]["bg"] = "grey"
                self.button_print[i]["bg"] = "grey"
                self.button_assemble[i]["bg"] = "grey"
            elif status == "Robot selected, ready to slice":
                self.button_slice[i]["bg"] = "green"
                self.button_print[i]["bg"] = "grey"
                self.button_assemble[i]["bg"] = "grey"
            elif status == "Ready to print":
                self.button_slice[i]["bg"] = "grey"
                self.button_print[i]["bg"] = "green"
                self.button_assemble[i]["bg"] = "grey"
            elif status == "Printing in progress":
                if self.printerObjects[i].checkIfPrintingFinished():
                    # print is now finished
                    self.printerStatus[i] = "Ready to assemble"
                    self.button_slice[i]["bg"] = "grey"
                    self.button_print[i]["bg"] = "grey"
                    self.button_assemble[i]["bg"] = "green"
                else:
                    # print in progress, nothing should be available
                    self.button_slice[i]["bg"] = "grey"
                    self.button_print[i]["bg"] = "grey"
                    self.button_assemble[i]["bg"] = "grey"


            else:
                print("Unknown status: {}".format(status))

    def handlerButtonInitPrinter(self, printer_number):
        # check the status is acceptable:
        if not self.printerStatus[printer_number] == "none":
            self.globalErrorText = "Already initialised"
            print("Already initialised (status was {})".format(self.printerStatus[printer_number]))
        else:
            self.globalErrorText = ""
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
                self.updateButtons()
                raise inst

            self.printerStatus[printer_number] = "Initialised, ready to select"
        self.updateButtons()

    def handlerButtonSelectRobotID(self, printer_number):
        # get and save the new robot ID:
        self.robotID_loaded[printer_number] = self.entry_robotID[printer_number].get()
        # display new robot ID:
        self.label_robotID[printer_number]["text"] = "Robot ID: {}".format(
            self.robotID_loaded[printer_number])

        # change status:
        self.printerStatus[printer_number] = "Robot selected, ready to slice"
        self.updateButtons()

    def buttonHandlerRunGenerate(self):
        self.robotIDLoadedGenerate = self.entryIDForGenerate.get()  # get and save the new robot ID
        self.entryIDForGenerate.delete(0, tk.END)  # clear the text input

        # check the morphGenome exists:
        if not "morphGenome_{}".format(self.robotIDLoadedGenerate) in [x for x in os.listdir(
                "{}/waiting_to_be_built".format(self.logDirectory)) if x.startswith("morphGenome_")]:
            self.label_generateStatus["text"] = "Status: error!\nmorphGenome_{} does not exist".format(
                self.robotIDLoadedGenerate)
            return 1
        self.label_generateStatus["text"] = "Status: generating {}\nplease wait".format(self.robotIDLoadedGenerate)
        self.mainWindow.update()

        setIndividualToLoadInParametersFileForGenerate(self.parametersFileForGenerate, self.robotIDLoadedGenerate)
        runAreGenerate(self.coppeliasimFolder, self.parametersFileForGenerate)

        if not "blueprint_{}.csv".format(self.robotIDLoadedGenerate) in [x for x in os.listdir(
                "{}/waiting_to_be_built".format(self.logDirectory)) if x.startswith("blueprint_")]:
            self.label_generateStatus["text"] = "Status: finished {}\nwarning:blueprint not generated".format(
                self.robotIDLoadedGenerate)
        else:
            self.label_generateStatus["text"] = "Status: finished {}\nsuccess".format(
                self.robotIDLoadedGenerate)


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
