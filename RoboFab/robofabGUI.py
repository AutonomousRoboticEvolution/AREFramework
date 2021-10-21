import json
import os
import subprocess
import time
import tkinter as tk

NUMBER_OF_PRINTERS=3

class RobofabGUI:
    def __init__(self, configurationData):

        # extract information from configuration data:
        self.logDirectory = configurationData [ "logDirectory" ]
        self.parametersFileForGenerate = configurationData["parametersFile"]
        self.coppeliasimFolder = configurationData["coppeliasimFolder"]

        # start setup of main window:
        self.mainWindow = tk.Tk()
        label_title = tk.Label(text="RoboFab GUI", font='Helvetica 18 bold')
        label_title.grid(row=0, column=0, columnspan=4)

        # lists of things that appear in each printer frame:
        printer_frame = []
        self.label_printer_title = []
        self.button_loadFile = []
        self.button_initPrinter = []
        self.button_slice = []
        self.button_print = []
        self.label_robotID = []
        self.entry_robotID = []
        self.robotID_loaded = []
        self.printerStatus = [] #  string for "file_selected",
        self.label_printerStatus=[]

        # loop for the printer frames:
        for i in range(NUMBER_OF_PRINTERS):
            printer_frame.append(tk.Frame(width=25, borderwidth=1, relief=tk.RAISED))
            self.label_printer_title.append(tk.Label(text="Printer {}".format(i), master=printer_frame[i], font='Helvetica 12 bold'))
            self.label_printerStatus.append(tk.Label(master=printer_frame[i]))
            self.label_robotID.append(tk.Label(text="Robot ID: ", master=printer_frame[i]))
            self.button_loadFile.append(
                tk.Button( text="Select robot ID", width=20, height=2, bg="LightCyan",
                           command=lambda i=i: self.handlerButtonSelectRobotID( i ),
                           master=printer_frame[i] ))
            self.entry_robotID.append(tk.Entry(width=20, master=printer_frame[i]))
            self.button_initPrinter.append(tk.Button( text="Initialise printer", width=20, height=2, bg="LightCyan", command=lambda i=i: self.handlerButtonInitPrinter( i ), master=printer_frame[i] ))
            self.button_slice.append(tk.Button( text="Slice", width=20, height=2, bg="LightCyan", command=lambda i=i: self.handlerButtonSlice( i ), master=printer_frame[i] ))
            self.button_print.append(tk.Button( text="Print", width=20, height=2, bg="LightCyan", command=lambda i=i: self.handlerButtonPrint( i ), master=printer_frame[i] ))
            self.entry_robotID.append(tk.Entry(width=20, master=printer_frame[i]))

            printer_frame[i].grid(row=1, column=i)  # arrange this frame in the overall window
            # pack frame:
            self.label_printer_title[i].pack()
            self.label_printerStatus[i].pack()
            self.label_robotID[i].pack()
            self.button_loadFile[i].pack()
            self.entry_robotID[i].pack()

            # this is a list of strings that represent the IDs of currently loaded robots:
            self.robotID_loaded.append("")
            self.printerStatus.append ( "none" )
        self.updatePrinterStatuses()

        #  "run are-generate" section:
        self.robotIDLoadedGenerate=""
        frame_runGenerate = tk.Frame(width=25, borderwidth=1, relief=tk.RAISED)
        frame_runGenerate.grid(row=1, column=3)
        self.button_runGenerate = tk.Button( text=" Run are-generate", width=20, height=2, bg="DarkOrange",
                                             command=self.buttonHandlerRunGenerate,
                                             master=frame_runGenerate )
        labelGenerateTitle = tk.Label(text="Enter robot ID:", master=frame_runGenerate)
        self.label_generateStatus = tk.Label( text="Status: waiting\n", master=frame_runGenerate )
        self.entryIDForGenerate = tk.Entry(width=20, master=frame_runGenerate)

        # arrange withing ARE-generate frame:
        labelGenerateTitle.pack()
        self.entryIDForGenerate.pack()
        self.button_runGenerate.pack()
        self.label_generateStatus.pack()

        # start GUI:
        tk.mainloop()

    def handlerButtonInitPrinter ( self, printer_number ):
        self.label_printerStatus[printer_number] [ "text" ] = "*busy*"
        self.mainWindow.update ()
        time.sleep(2)
        pass
        self.printerStatus[printer_number] = "Initialised, ready to select"
        self.updatePrinterStatuses()

    def handlerButtonSlice ( self ):
        pass

    def handlerButtonPrint ( self ):
        pass

    def updatePrinterStatuses( self ):
        for i in range ( NUMBER_OF_PRINTERS ):
            self.label_printerStatus [ i ]["text"] = "Status: {}".format(self.printerStatus[i])

    def handlerButtonSelectRobotID( self, printer_number ):
        self.robotID_loaded[printer_number] = self.entry_robotID[printer_number].get()  # get and save the new robot ID
        self.label_robotID[printer_number]["text"] = "Robot ID: {}".format(
            self.robotID_loaded[printer_number])  # display new robot ID
        self.entry_robotID[printer_number].delete(0, tk.END)  # clear the text input

        self.label_printerStatus[printer_number]="robot_selected"
        self.updatePrinterStatuses()

    def buttonHandlerRunGenerate( self ):
        self.robotIDLoadedGenerate = self.entryIDForGenerate.get()  # get and save the new robot ID
        self.entryIDForGenerate.delete(0, tk.END)  # clear the text input

        # check the morphGenome exists:
        if not "morphGenome_{}".format(self.robotIDLoadedGenerate) in [x for x in os.listdir( "{}/waiting_to_be_built".format ( self.logDirectory ) ) if x.startswith("morphGenome_")]:
            self.label_generateStatus[ "text" ]= "Status: error!\nmorphGenome_{} does not exist".format( self.robotIDLoadedGenerate )
            return 1
        self.label_generateStatus [ "text" ] = "Status: generating {}\nplease wait".format( self.robotIDLoadedGenerate )
        self.mainWindow.update()

        setIndividualToLoadInParametersFileForGenerate(self.parametersFileForGenerate, self.robotIDLoadedGenerate)
        runAreGenerate(self.coppeliasimFolder,self.parametersFileForGenerate)

        if not "blueprint_{}.csv".format ( self.robotIDLoadedGenerate ) in [ x for x in os.listdir (
                "{}/waiting_to_be_built".format ( self.logDirectory ) ) if x.startswith ( "blueprint_" ) ]:
            self.label_generateStatus [ "text" ] = "Status: finished {}\nwarning:blueprint not generated".format( self.robotIDLoadedGenerate )
        else:
            self.label_generateStatus [ "text" ] = "Status: finished {}\nsuccess".format (
                self.robotIDLoadedGenerate )


def setIndividualToLoadInParametersFileForGenerate(filepath, robotID="0_0"):
    genToLoad=robotID.split("_")[0]
    indToLoad=robotID.split("_")[1]
    print( "Changing the file: {}\n to use the robot from generation: {}, individual: {}".format( filepath, robotID.split("_")[0], robotID.split("_")[1] ) )

    os.rename ( filepath, filepath + "_bak" )
    string_to_write=""
    with open ( filepath+"_bak", "r" ) as file:
        for line in file:
            if line.startswith("#genToLoad"):
                string_to_write=string_to_write+"#genToLoad,int,"+str(genToLoad)+"\n"
            elif line.startswith("#indToLoad"):
                string_to_write=string_to_write+"#indToLoad,int,"+str(indToLoad)+"\n"
            else:
                string_to_write=string_to_write+line
    with open ( filepath, "w") as file:
        file.write(string_to_write)

def runAreGenerate(coppeliasimFolder,parametersFilepath):
    print("Trying to run ARE-generate...")
    terminalCommand= "./are_sim.sh generate -h -g{}".format(parametersFilepath)
    output = str (
        subprocess.check_output ( terminalCommand, cwd="{}".format(coppeliasimFolder), shell=True , stderr=subprocess.STDOUT) )
    #print("Output from command was:\n{}".format(output))



if __name__ == "__main__":
    # Make the settings file then extract the settings from it
    from makeConfigurationFile import makeFile as makeConfigurationFile
    makeConfigurationFile ( location="BRL" )  # <--- change this depending on if you're in York or BRL
    configurationData = json.load (
        open ( 'configuration_BRL.json' ) )  # <--- change this depending on if you're in York or BRL

    gui = RobofabGUI(configurationData)