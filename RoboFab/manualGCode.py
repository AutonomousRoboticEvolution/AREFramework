# Instructions:
# - set the logDirectory in the configuration file to be the experiment folder withing are-logs that you want to use
# - put the mesh and blueprint into the waiting_to_be_built folder within the logDirectory
# - run this script - it will ask you to input the ID of the robot you want to create the gcode for
# (note - it can take a while to run, be patient!)
# - the new mesh (with clips) will be created in evolutionary_robotics_framework/RoboFab/meshes
# - the gcode will be created in evolutionary_robotics_framework/RoboFab/gcode

from printer import *
# from makeConfigurationFile import makeFile
import json

# we need to make a config dictionary. The actual values for the printer origin. temperature and API_KEY won't be used
config = {"logDirectory":"/home/matt/are-logs/amsterdam/",
          "PRINTER_0":{
		"ORIGIN":0,
        "BED_COOLDOWN_TEMPERATURE":30,
		"API_KEY":0
		} }

waiting_to_be_built_folder_path = "{}/waiting_to_be_built/".format(config["logDirectory"])
files_in_the_folder = os.listdir( waiting_to_be_built_folder_path )

print("Available robot IDs:")
for mesh_filename in files_in_the_folder:
    if mesh_filename.startswith( "mesh_" ):
        robotID = mesh_filename [ len ( "mesh_" ):-len(".stl") ]
        blueprint_filename = "blueprint_{}.csv".format(robotID)
        if blueprint_filename in files_in_the_folder:
            print(robotID)
        else:
            print("{} exists but {} does not".format(mesh_filename,blueprint_filename))

robotID = input("What robot ID do you want to use?")
print("robot ID: {} selected".format(robotID))

printer = Printer ( None, config )
printer.createSTL(robotID,True)
printer.slice("mesh_{}".format(robotID))