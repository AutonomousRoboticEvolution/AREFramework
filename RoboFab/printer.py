import subprocess, os, shutil # for running terminal command (for cura-engine)
import time
import requests # for communicating with api over http
from typing import Type
import numpy as np
from helperFunctions import debugPrint, makeTransformInputFormatted

TIDY_UP_FILES = True

## The printer class is for running a 3D printer
## This includes the slicing of an stl and all communication with the Raspberry Pi which runs Octoprint
class Printer:
    def __init__(self, IPAddress, configurationData, printer_number=0):
        debugPrint("Creating a printer object")
        printerConfiguration = configurationData["PRINTER_{}".format(printer_number)]

        # some settings etc for OpenSCAD file handling
        self.openSCADScriptFileName = "skeleton_maker.scad"
        self.openSCADScriptFileNameManualVersion = "skeleton_maker_manual.scad"
        self.openSCADDirectory = "./OpenSCAD/"
        # self.meshesNoClipsDirectory = "./meshes_no_clips/"
        self.meshesNoClipsDirectory = "{}/waiting_to_be_built/".format(configurationData["logDirectory"])
        self.blueprintsDirectory = "{}/waiting_to_be_built/".format(configurationData["logDirectory"])
        self.meshesDirectory = "./meshes/"

        self.IPAddress=IPAddress # IP address of the Raspberry Pi running OctoPrint
        self.origin = np.matrix (printerConfiguration["ORIGIN"])
        self.skeletonPositionOnPrintbed = makeTransformInputFormatted([0.150, 0.150, 0]) # default middle of bed, can be updated
        self.timeout=5 # timeout in seconds for api requests to octoPrint
        self.defaultBedCooldownTemperature = printerConfiguration["BED_COOLDOWN_TEMPERATURE"]
        self.print_time_estimate_seconds=-1

        # response = requests.get("http://"+self.IPAddress+"/api/printer")
        self.apiKeyHeader = { 'X-Api-Key': '{}'.format(printerConfiguration["API_KEY"])}

        if self.IPAddress is not None:
            self.connectOctoPrintToPrinter()
            self.getOctoPiStatus()


    ## higher-level function to slice, upload and start printing a given stl file. The file ./meshes/mesh[IDNumber].stl must exist
    def printARobot(self, IDNumber, FAKE_SLICE_ONLY=False):

        filename = "mesh_" + IDNumber

        if FAKE_SLICE_ONLY:
            self.fakeSlice(filename)
        else:
            debugPrint("Making stl for robot ID: {}".format(IDNumber), messageVerbosity=0)
            self.createSTL(IDNumber)
            debugPrint("Slicing {}".format(filename), messageVerbosity=0)
            self.slice(filename)
        self.uploadGCodeToOctoPrint("gcode/{}".format(filename))
        self.printFileOnOctoprint(filename)
        self.waitForEndOfPrint()

        debugPrint("Printing for {} finished".format(filename), messageVerbosity=0)


    ## run the openSCAD script on the individual specified by ID_number.
    ## assumes the existence of the following files:
    ## ./meshes_no_clips/mesh[ID_number].stl
    ## ./blueprints/blueprint[ID_number].csv
    def createSTL( self, ID_number: str, manualVersion=False ):
        debugPrint("Creating an STL file for individual {}".format(ID_number))
        blueprintFilename = "blueprint_" + ID_number + ".csv"
        debugPrint("using blueprint: {}{}".format(self.blueprintsDirectory, blueprintFilename),messageVerbosity=2)
        debugPrint("and mesh file: {}mesh_{}.stl".format(self.meshesNoClipsDirectory, ID_number),messageVerbosity=2)
        with open("{}{}".format(self.blueprintsDirectory, blueprintFilename), "r") as blueprintFile:
            blueprint_string = blueprintFile.read()
            blueprintFile.close()

        # edit the string into the format needed for OpenSCAD to understand it:
        # - add [ to beginning and ], to end of each line, expect the last which needs no comma.
        # - at end, add: ];
        # - at beginning add: organs_blueprint_list =[
        blueprint_string = "organs_blueprint_list=[[" + blueprint_string.replace("\n", "],[")[:-2] + "];\n"
        debugPrint("Organs:" + blueprint_string, messageVerbosity=2)

        # copy the mesh (with no clips) into the openSCAD directory as inputMesh.stl (a temporary file)
        shutil.copyfile("{}mesh_{}.stl".format(self.meshesNoClipsDirectory, ID_number),
                        "{}inputMesh.stl".format(self.openSCADDirectory))

        # make a new (temporary) openscad file, with the blueprint data at the start:
        with open("{}temporaryScript.scad".format(self.openSCADDirectory), "w") as temporaryFile:
            temporaryFile.write(blueprint_string)
            if manualVersion:
                temporaryFile.write(
                    open("{}{}".format(self.openSCADDirectory, self.openSCADScriptFileNameManualVersion), "r").read())
            else:
                temporaryFile.write(
                    open("{}{}".format(self.openSCADDirectory, self.openSCADScriptFileName), "r").read())
            temporaryFile.close()

        # run the OpenSCAD terminal command
        terminalCommand = "openscad temporaryScript.scad -o outputMesh.stl"
        debugPrint("Running OpenSCAD command: {}".format(terminalCommand), messageVerbosity=1)
        debugPrint("This might take a while...", messageVerbosity=1)
        completed = subprocess.run(terminalCommand, cwd=self.openSCADDirectory, shell=True)
        if completed.returncode != 0:
            raise RuntimeError("OpenSCAD terminal command failed\n{}".format(terminalCommand))

        # copy the resulting mesh (with clips) into the meshes folder
        if os.path.isfile("{}mesh_{}.stl".format(self.meshesDirectory, ID_number)):
            debugPrint("WARNING: mesh file mesh_{}.stl already exists, will be overwritten".format(ID_number))
        shutil.move(self.openSCADDirectory + "outputMesh.stl",
                    "{}mesh_{}.stl".format(self.meshesDirectory, ID_number))


        # tidy up by removing temporary files
        if TIDY_UP_FILES:
            for filename in ["{}temporaryScript.scad".format(self.openSCADDirectory),
                             "{}inputMesh.stl".format(self.openSCADDirectory)]:
                if os.path.isfile(filename):
                    os.remove(filename)

        debugPrint("STL created for mesh{}".format(ID_number), messageVerbosity=0)

    # returns a string representing the current "state" of OctoPrint, e.g.:
    # "Operational"
    # "Connecting"
    # "Offline"
    # "Finishing"
    def getOctoPiStatus(self):
        responseObject = requests.get("http://{}/api/connection".format(self.IPAddress), headers=self.apiKeyHeader, timeout=self.timeout)
        if responseObject.status_code == 200:
            # OctoPrint is happy
            debugPrint("Octoprint Raspberry Pi at address {} is reporting its state is: {}".format(self.IPAddress,responseObject.json()["current"]["state"]),messageVerbosity=2)
            return responseObject.json()["current"]["state"]
        else:
            # OctoPrint is not happy
            debugPrint("Error with Octoprint Raspberry Pi at address {}, http status code: {}".format(self.IPAddress,responseObject.status_code),messageVerbosity=1)
            return "error"


    #returns a dictionary, with keys "bed_actual", "bed_target", "tool_actual" and "tool_target" and values as temperature in degC
    def getTemperatures(self):
        responseObject = requests.get("http://{}/api/printer".format(self.IPAddress), headers=self.apiKeyHeader, timeout=self.timeout)
        print(responseObject, self.IPAddress)
        temperatures = {"bed_actual":responseObject.json()["temperature"]["bed"]["actual"],
                        "bed_target":responseObject.json()["temperature"]["bed"]["target"],
                        "tool_actual":responseObject.json()["temperature"]["tool0"]["actual"],
                        "tool_target":responseObject.json()["temperature"]["tool0"]["target"],
                        }
        debugPrint("Checking printer temperatures",messageVerbosity=3)
        for item in temperatures:
            debugPrint("{}:{}".format(item,temperatures[item]),messageVerbosity=4)

        return temperatures

    #sends the gcode to the OctoPrint Raspberry Pi. Note the gcode will be uploaded but not started
    def uploadGCodeToOctoPrint(self, filenameToUpload: str):
        if not filenameToUpload[-6:] == ".gcode":# check filename end .gcode, if not add it
            filenameToUpload+= ".gcode"
        debugPrint("Trying to upload file '{}' to OctoPrint at IP {}".format(filenameToUpload,self.IPAddress),messageVerbosity=1)

        # check the file exists, if not throw an error
        if not os.path.isfile(filenameToUpload):
            raise Exception("Can't find specified gcode file {}".format(filenameToUpload))

        # make sure OctoPrint is connected to printer
        self.connectOctoPrintToPrinter()


        # print(os.path.basename(filenameToUpload))
        responseObject = requests.post("http://{}/api/files/local".format(self.IPAddress),
                                       headers=self.apiKeyHeader,
                                       files={"file":open(filenameToUpload, 'rb'),
                                              "filename":os.path.basename(filenameToUpload)},
                                       data={"select":"false", "print":"false"})
        if not responseObject.json()["done"]:
            raise RuntimeError("Upload of file {} to Octoprint at IP {} failed".format(filenameToUpload,self.IPAddress))


    # ask the OctoPrint (running on Raspberry Pi) to connect to the TAZ printer over its USB.
    # This will use the default settings for port, baud rate etc. set in OctoPrint, so you can set these using the
    # web interface. Just type the Pi's IP address into your web browser.
    def connectOctoPrintToPrinter(self):

        #first check if we are already connected
        reply = self.getOctoPiStatus() # reply is a string, e.g. "Operational", "Connecting", "Offline" or "error" if we get a http error from OctoPrint

        #if not connected, try to connect
        if not reply in ["Operational", "Printing"]:
            debugPrint("Sending command to OctoPrint to connect to printer",messageVerbosity=2)
            dataToSend = {"command": "connect"}
            requests.post("http://{}/api/connection".format(self.IPAddress),
                                     headers=self.apiKeyHeader,
                                     timeout=self.timeout,
                                     json=dataToSend)

            #wait for connection to happen
            reply = self.getOctoPiStatus()
            retries=0
            while reply =="Connecting":
                retries+=1
                time.sleep(1)
                reply = self.getOctoPiStatus()
                if retries>19:
                    debugPrint("WARNING: trying to connect OctoPrint to printer, {} retries so far".format(retries),messageVerbosity=1)
        else:
            debugPrint("OctoPrint already connected to printer (state {})".format(reply),messageVerbosity=2)


    # sends a message to OctoPi to print the given file, which should have already been uploaded using uploadGCodeToOctoPrint(filename)
    def printFileOnOctoprint(self,filename):
        if not filename[-6:] == ".gcode":# check filename end .gcode, if not add it
            filename+= ".gcode"

        debugPrint("Asking OctoPi at {} to print file called {}".format(self.IPAddress,filename),messageVerbosity=1)
        responseObject = requests.post("http://{}/api/files/local/{}".format(self.IPAddress,filename),
                                       headers=self.apiKeyHeader,
                                       json={
                                           "command":"select",
                                           "print":True
                                       }
                                       )
        if responseObject.status_code == 409:
            raise RuntimeError("409 error selecting file {} - is the printer already running?".format(filename))
        if responseObject.status_code == 404:
            raise RuntimeError("404 error: maybe you tried to select a file ({}) on OctoPi which doesn't exist?".format(filename))

        # wait for printer to finish "Starting":
        while self.getOctoPiStatus() == "Starting":
            time.sleep(1)

    # call CuraEngine to slice the given stl into gcode
    # stl should be in the meshes folder, and the gcode will be placed into the gcode folder
    def slice(self, stl_filename):

        debugPrint("Slicing file {}".format(stl_filename),messageVerbosity=2)
        with open("./printerSettings/curaEngineCommand.txt", "r") as temporaryFile:
            terminalCommand = temporaryFile.read().replace("**stl_filename**",stl_filename)
            temporaryFile.close()

        time.sleep(0.1)

        output = str( subprocess.check_output (terminalCommand, cwd="./printerSettings",shell=True, stderr=subprocess.STDOUT) )
        # extract some meta-data, i.e. the estimated print time and filamanet used
        print_time_location_in_output = output.find("Print time: ")
        self.print_time_estimate_seconds=int(output[print_time_location_in_output +12 : output.find("\\n",print_time_location_in_output)])
        filament_used_location_in_output = output.find("Filament: ")
        filament_used_estimate_seconds=int(output[filament_used_location_in_output +10 : output.find("\\n",filament_used_location_in_output)])

        debugPrint("Estimated print time: {} seconds = {:.2f} hours".format(self.print_time_estimate_seconds,float(self.print_time_estimate_seconds)/(60*60)),0)
        debugPrint("Estimated Filament used (m): {}".format(filament_used_estimate_seconds),0)

    # create a dummy gcode file instead of actually slicing the file, which could be useful for debugging and testing, because you don't have to wait for the actual part to print!
    def fakeSlice(self, filename):
        shutil.copyfile("gcode/dummy.gcode","gcode/{}.gcode".format(filename))

    # returns false if the printer is still active
    def checkIfPrintingFinished(self):
        status = self.getOctoPiStatus()
        if status in["Printing","Finishing","Pausing"]:
            return False
        else:
            return True

    def waitForEndOfPrint(self):
        while not self.checkIfPrintingFinished():
            time.sleep(5)

    # tell the printer to switch off the printbed
    # will only return once the bed is at or below the finishTemperature (dealt 999 so will return immediately)
    def coolBed(self, finishTemperature=None):
        if finishTemperature is None:
            finishTemperature=self.defaultBedCooldownTemperature
        # send command to turn off printbed heater:
        debugPrint("Asking OctoPi at {} to turn of bed heater".format(self.IPAddress), messageVerbosity=1)
        responseObject = requests.post("http://{}/api/printer/bed".format(self.IPAddress),
                                       headers=self.apiKeyHeader,
                                       json={
                                           "command": "target",
                                           "target": 0
                                       }
                                       )

        # wait until actual temperature is at or below the finishTemperature
        debugPrint("Waiting for bed to cool to {} degrees...".format(finishTemperature),messageVerbosity=1)
        while self.getTemperatures()["bed_actual"] > finishTemperature:
            time.sleep(15)
        debugPrint("Bed cooling finished",messageVerbosity=1)



if __name__ == "__main__":
    import json

    '''
    ID_list = [1203,1358,1457,1689,2062,2710,3134,4180,4340]
    ID_list = [1689,2710]
    for ID_num in ID_list:
        robot_ID=str(ID_num)
        print("ID: {}".format(robot_ID))
    '''

    ## to print a hand-made stl file using the same settings as an automated mesh file:
    printer=Printer("192.168.2.251" , json.load(open('configuration_BRL.json')),0)
    printer.slice("testpart")
    printer.uploadGCodeToOctoPrint("gcode/testpart")
    printer.printFileOnOctoprint("testpart")
    printer.coolBed(30)



    # printer=Printer("192.168.2.251" , json.load(open('configuration_BRL.json'))["PRINTER_1"])
    #printer.printARobot("test3", FAKE_SLICE_ONLY=False)
    # printer.coolBed(30)
