import json
import os
import subprocess
import time
import tkinter as tk
import numpy as n
import datetime

from helperFunctions import convertTransformToPose
from printer import Printer
from RoboFab import RoboFab_host

NUMBER_OF_PRINTERS = 2


class RobofabGUI:
    def __init__(self, configurationData):

        print("Configured for {}".format(configurationData["location"]))

        # extract information from configuration data:
        self.configurationData = configurationData
        self.logDirectory = configurationData["logDirectory"]
        self.coppeliasimFolder = configurationData["coppeliasimFolder"]

        # start setup of main window:
        self.mainWindow = tk.Tk()
        self.mainWindow.protocol("WM_DELETE_WINDOW", self.windowCloseHandler) # function windowCloseHandler() will be called when the GUI is closed
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
            self.printingInProgress.append(False)
            self.printingDone.append(False)


        #  UR5 startup section:
        self.robofabObject = None
        frame_startupUR5 = tk.Frame(width=25, borderwidth=1, relief=tk.RAISED)
        frame_startupUR5.grid(row=2, column=NUMBER_OF_PRINTERS+1)
        self.button_startupUR5 = tk.Button( text="Initialise UR5", width=20, height=2,
                                            command=self.buttonHandlerStartUR5,
                                            master=frame_startupUR5 )
        self.label_ur5Status = tk.Label( text="", master=frame_startupUR5 )

        self.button_organBankManager = tk.Button(text="Organ Bank Manager", width=20, height=2,
                                             command=self.organBankManager,
                                             master=frame_startupUR5)
        self.button_releaseGripper = tk.Button(text="Release gripper", width=20, height=2,
                                             command=self.buttonHandlerReleaseGripper,
                                             master=frame_startupUR5)

        # arrange withing UR5 startup section frame:
        self.button_startupUR5.pack()
        self.label_ur5Status.pack()
        self.button_organBankManager.pack()
        self.button_releaseGripper.pack()

        # print in terminal the available robots:
        os.makedirs("{}/waiting_to_be_built".format(self.logDirectory), exist_ok=True)  # make folder if it doesn't already exist
        os.makedirs("{}/waiting_to_be_evaluated".format(self.logDirectory), exist_ok=True)  # make folder if it doesn't already exist
        print("Blueprints available:")
        for filename in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
            if filename.startswith("blueprint_"):
                print(filename)
        print("")

        # start GUI:
        self.mainWindow.after(1, self.timedUpdateButtons())
        tk.mainloop()

    def windowCloseHandler(self):
        if not self.robofabObject is None:
            self.robofabObject.disconnectAll()
        self.mainWindow.destroy()

    def timedUpdateButtons(self):
        self.updateButtonColours()
        self.mainWindow.after(100, self.timedUpdateButtons)

    def handlerButtonAssemble(self, printer_number):
        if self.robofabObject is None:
            self.label_printerStatus[printer_number]["text"] = "Can't assemble,\nRoboFab not initialised"
        if self.printerObjects[printer_number] is None:
            self.label_printerStatus[printer_number]["text"] = "Can't assemble,\nPrinter not initialised"

        else: # UR5 and printer are both initialised, so we can do the assembly
            self.robofabObject.setupRobotObject( robotID=self.robotID_loaded[printer_number], printer=self.printerObjects[printer_number])
            if self.robofabObject.checkBankHasEnoughOrgans():
                self.label_printerStatus[printer_number]["text"] = "*busy assembling*\n"
                self.mainWindow.update()
                self.robofabObject.buildRobot( printer=self.printerObjects[printer_number]) # do the assembly
                self.label_printerStatus[printer_number]["text"] = "Assembly done\n"
                self.saveOrganBankToFile()
                self.mainWindow.update()
            else:
                self.label_printerStatus[printer_number]["text"] = "Can't assemble,\nBank missing organs"
                print("Can't assemble, Bank missing organs")
                self.mainWindow.update()


    def buttonHandlerStartUR5( self ):
        if not self.robofabObject is None:
            print("UR5 already started")
            return
        else:
            self.robofabObject = RoboFab_host ( self.configurationData )

    def buttonHandlerReleaseGripper(self):
        if self.robofabObject is None:
            print("Can't release gripper since UR5 not initialised")
        else:
            self.robofabObject.UR5.gripper.disableServos()


    def updateButtonColours( self ):
        self.label_globalErrors["text"] = self.globalErrorText

        # if UR5 not yet initialised, active button
        if self.robofabObject is None:
            self.button_startupUR5["bg"]="yellow"
            self.button_organBankManager["bg"] = "grey"
        else:
            self.button_startupUR5["bg"]="green"
            self.button_organBankManager["bg"] = "yellow"

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
                    self.button_initPrinter[i]["bg"]="grey"
                    self.button_slice[i]["bg"]="grey"
                    self.button_print[i]["bg"]="grey"
                    self.button_assemble[i]["bg"]="grey"
            else: #printing not in progress

                # find colour for "select robot ID" button
                if self.entry_robotID[i].get()=="":
                    #  no text in input field
                    self.button_printerLoadFile[i]["bg"] = "red"
                elif self.robotID_loaded[i] == self.entry_robotID[i].get():
                    #  ID already loaded
                    self.button_printerLoadFile[i]["bg"] = "green"
                elif "morph_genome_{}".format(self.entry_robotID[i].get()) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)):
                    #  the new ID exists in the waiting_to_be_built folder
                    self.button_printerLoadFile[i]["bg"] = "yellow"
                else:
                    #  the selected robot ID does not exist in the waiting_to_be_built folder
                    self.button_printerLoadFile[i]["bg"] = "red"

                # find colour for init printer button:
                if self.printerObjects[i] is None:
                    # not yet initialised
                    self.button_initPrinter[i]["bg"] = "yellow"
                else:
                    # is already initialised
                    self.button_initPrinter[i]["bg"] = "green"

                # find colour for "slice" button:
                if "mesh_{}.gcode".format(self.robotID_loaded[i]) in os.listdir("./gcode/"):
                    # gcode is already done
                    self.button_slice[i]["bg"] = "green"
                elif not "blueprint_{}.csv".format(self.robotID_loaded[i]) in os.listdir("{}/waiting_to_be_built".format(self.logDirectory)) or self.printerObjects[i] is None:
                    # haven't made the blueprint yet or printer not initialised
                    self.button_slice[i]["bg"] = "grey"
                else:
                    # blueprint exists but hasn't been sliced, so ready to go!
                    self.button_slice[i]["bg"] = "yellow"

                # find colour for print button:
                if self.printingDone[i]:
                    self.button_print [ i ] [ "bg" ] = "green"
                elif "mesh_{}.gcode".format ( self.robotID_loaded [ i ] ) in os.listdir ( "./gcode/"  ) and not self.printerObjects[i] is None:
                    # slicing done and printer is connected
                    self.button_print [ i ] [ "bg" ] = "yellow"
                else:
                    self.button_print [ i ] [ "bg" ] = "grey"

                # find colour for assemble button:
                if "list_of_organs_addresses_{}.csv".format ( self.robotID_loaded[i]) in os.listdir (
                    "{}/waiting_to_be_evaluated".format ( self.logDirectory ) ):
                    # organs list already exists, so has been assembled
                    self.button_assemble [ i ] [ "bg" ] = "green"
                elif self.printingDone[i] and not self.robofabObject is None:
                    # printing done, so ready to do assembly (but not yet done):
                    self.button_assemble [ i ] [ "bg" ] = "yellow"
                else:
                    self.button_assemble [ i ] [ "bg" ] = "grey"


    def handlerButtonPrint(self, printer_number):
        self.label_printerStatus[printer_number]["text"] = "*setting print*\n"
        self.mainWindow.update()

        filename = "mesh_{}".format(self.robotID_loaded[printer_number])
        self.printerObjects[printer_number].uploadGCodeToOctoPrint("gcode/{}".format(filename))
        self.printerObjects[printer_number].printFileOnOctoprint(filename)
        # work out and show the estimated finishing time
        estimated_finish_time = datetime.datetime.now() + datetime.timedelta(seconds= self.printerObjects[printer_number].print_time_estimate_seconds )
        if self.printerObjects[printer_number].print_time_estimate_seconds>0:
            self.label_printerStatus [ printer_number ] [ "text" ] = "Printing, estimated finish:\n{}".format(estimated_finish_time.strftime("%H:%M:%S"))
            print("Printing, estimated finish:\n{}".format(estimated_finish_time.strftime("%H:%M:%S")))
        else:
            self.label_printerStatus [ printer_number ] [ "text" ] = "Printing, estimated finish:\ntime unknown"
            print("Printing, estimated finish:\ntime unknown")
        self.printingInProgress [printer_number] = True

    def handlerButtonSlice(self, printer_number):
        self.label_printerStatus[printer_number]["text"] = "*busy slicing*\n"
        self.mainWindow.update()

        robot_ID = self.robotID_loaded[printer_number]
        print("Trying to slice robot with ID {}".format(robot_ID))
        self.printerObjects[printer_number].createSTL(robot_ID)
        self.printerObjects[printer_number].slice("mesh_" + robot_ID)

        self.label_printerStatus[printer_number]["text"] = "Slicing done\n"

    def handlerButtonPrinterInit( self, printer_number ):
        # check the status is acceptable:
        if not self.printerObjects[printer_number] is None:
            self.label_printerStatus[printer_number]["text"] = "Already initialised\n"
            print("Already initialised")
        else:
            self.label_printerStatus[printer_number]["text"] = "*trying to connect*\n"
            self.mainWindow.update()
            # setup printer object:
            IPAddress = self.configurationData["network"]["PRINTER{}_IP_ADDRESS".format(printer_number)]
            try:
                self.printerObjects[printer_number] = Printer(IPAddress=IPAddress, configurationData=configurationData, printer_number=printer_number)
                # self.printerObjects[printer_number] = Printer(IPAddress=None, configurationData=configurationData, printer_number=printer_number)
                # print("Skipping printer connection")
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
        self.label_printerStatus [ printer_number ] [ "text" ] = "Robot selected\n"
        if not f"blueprint_{self.robotID_loaded[printer_number]}.csv" in os.listdir(f"{self.logDirectory}/waiting_to_be_built"):
            self.label_printerStatus [ printer_number ] [ "text" ] = "WARNING: blueprint not present\n"
        if not f"mesh_{self.robotID_loaded[printer_number]}.stl" in os.listdir(f"{self.logDirectory}/waiting_to_be_built"):
            self.label_printerStatus [ printer_number ] [ "text" ] = "WARNING: mesh file not present\n"

        self.printingDone[printer_number]=False

    def organBankManager(self):
        if self.robofabObject is None:
            print("UR5 not initialised, can't manage organ bank")
            return 1

        # print("Manage organ bank...")

        self.window_bankManager = tk.Tk()
        frame = tk.Frame(master=self.window_bankManager, width=1200, height=750)
        tk.Button(master=self.window_bankManager, text="Update addresses and close", bg="yellow", command=self.updateI2CAddressesAndClose).pack()
        frame.pack()

        self.listOfButtons_slots = []
        self.listOfTextboxes_slots = []
        for i, organ in enumerate(self.robofabObject.organBank.organsList):
            text = organ.friendlyName.replace(" organ", "")
            # # add the address to the text:
            # try:
            #     text = text + "\n"+hex(int(organ.I2CAddress))
            # except ValueError:
            #     text = text+"\n"+organ.I2CAddress # I2CAddress is actually an IP address

            colour = "green" if organ.isInBank else "red"

            width=15 if organ.organType==0 else 10

            self.listOfButtons_slots.append(
                tk.Button(master=frame, text=text, bg=colour, height = 1, width=width,
                          command=lambda i=i: self.handlerButtonBankSlot(i)
                          )
            )
            self.listOfTextboxes_slots.append(
                tk.Entry(master=frame, width=width)
            )
            try: # if I2CAddress is a number, then display as hex
                self.listOfTextboxes_slots[-1].insert(0, hex(int(organ.I2CAddress)))
            except ValueError: # not a number, so display as string:
                self.listOfTextboxes_slots[-1].insert(0,organ.I2CAddress)

            if organ.transformOrganOriginToMaleCableSocket is None:
                organ_plotting_position = organ.positionTransformWithinBankOrRobot
            else:
                organ_plotting_position = organ.positionTransformWithinBankOrRobot * organ.transformOrganOriginToMaleCableSocket
            xPosition = organ_plotting_position[0,3]*1800
            yPosition=(0.4 - organ_plotting_position[1,3])*1500
            self.listOfButtons_slots[i].place(x=xPosition, y=yPosition)
            self.listOfTextboxes_slots[i].place(x=xPosition, y=yPosition+30)

        self.window_bankManager.protocol("WM_DELETE_WINDOW", self.updateI2CAddressesAndClose) # function windowCloseHandler() will be called when the GUI is closed

    def updateI2CAddressesAndClose(self):
        for i, organ in enumerate(self.robofabObject.organBank.organsList):
            # update organ's address with the value from textbox
            input_text = self.listOfTextboxes_slots[i].get()
            if input_text.startswith("0x"):
                # is a hex value, so convert to a string of this value in decimal:
                organ.I2CAddress = str( int(input_text,16) )
            else:
                # isn't a hex value, so just save the string as input:
                organ.I2CAddress = input_text

        self.saveOrganBankToFile()
        self.window_bankManager.destroy() # close window

    def handlerButtonBankSlot(self,i):
        # print("Swapping state of "+self.robofabObject.organBank.organsList[i].friendlyName)

        if self.robofabObject.organBank.organsList[i].isInBank:
            self.robofabObject.organBank.organsList[i].isInBank = False
            self.listOfButtons_slots[i]["bg"] = "red"
        else:
            self.robofabObject.organBank.organsList[i].isInBank = True
            self.listOfButtons_slots[i]["bg"] = "green"

    def saveOrganBankToFile(self):
        # creates a text file, with a line per organ, with each line in this format:
        # [<organ type> , 16 values for position transform , <i2c or IP Address>, isInBank]

        string_to_save=""
        for organ in self.robofabObject.organBank.organsList:
            if len(string_to_save)>0: string_to_save+="\n"
            string_to_save += str(organ.organType) + ","
            for i in range(4):
                for j in range(4):
                    string_to_save += str( organ.positionTransformWithinBankOrRobot[i,j] ) + ","

            string_to_save+= organ.I2CAddress + ","
            string_to_save+= str(organ.isInBank)

        with open("organBankContents.txt","w") as outputFile:
            outputFile.write(string_to_save)

if __name__ == "__main__":
    # Make the settings file then extract the settings from it
    from makeConfigurationFile import makeFile as makeConfigurationFile

    # Make the settings file then extract the settings from it
    with open('location.txt') as f:
        location = f.read().replace("\n", "")
        print("location: {}".format(location))
    makeConfigurationFile(location=location) # <--- change this depending on if you're in York or BRL
    configurationData = json.load(open('configuration_{}.json'.format(location)))  # <--- change this depending on if you're in York or BRL

    gui = RobofabGUI(configurationData)
