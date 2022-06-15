# Instructions:
# - set the logDirectory in the configuration file to be the experiment folder withing are-logs that you want to use
# - put the mesh and blueprint into the waiting_to_be_built folder within the logDirectory
# - run this script - it will ask you to input the ID of the robot you want to create the gcode for
# (note - it can take a while to run, be patient!)
# - the new mesh (with clips) will be created in evolutionary_robotics_framework/RoboFab/meshes
# - the gcode will be created in evolutionary_robotics_framework/RoboFab/gcode

from printer import *
from robotComponents import Robot
# from makeConfigurationFile import makeFile
import json
from helperFunctions import makeTransformMillimetersDegrees

# we need to make a config dictionary. The actual values for the printer origin. temperature and API_KEY won't be used
config = {"logDirectory":"/home/leni/are-logs/amsterdam_physical",
          "PRINTER_0":{
		"ORIGIN":0,
        "BED_COOLDOWN_TEMPERATURE":30,
		"API_KEY":0
		},
          "dictionaryOfOrganTypes": {
              '0': {
                  "organType": 0,
                  "friendlyName": "Head organ",
                  # "forceModeTravelDistance": 20.0 / 1000,
                  "forceModeTravelDistance": 18 / 1000,
                  # "postInsertExtraPushDistance": 3.0 / 1000, # positive for extra pushing when attaching organ to skeleton
                  "postInsertExtraPushDistance": 0.0 / 1000,  # positive for extra pushing when attaching organ to skeleton
                  "pickupExtraPushDistance": 2 / 1000,  # offset in gripper frame z direction just when picking up organ
                  # "transformOrganOriginToGripper": np.linalg.inv(makeTransformInputFormatted ( [ 0, 0, 72 / 1000, 0, math.radians ( 180 ), math.radians(180) ] ) ).tolist(), # Head v0.1
                  "transformOrganOriginToGripper": np.linalg.inv ( makeTransformMillimetersDegrees ( z=85, rotX=180 ) ).tolist (),  # Head v0.2
                  "transformOrganOriginToClipCentre": makeTransformInputFormatted ().tolist (),
                  "transformOrganOriginToFemaleCableSocket": [
                      (makeTransformMillimetersDegrees ( x=-24, y=-34.67, z=107.49, rotX=180, rotZ=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=-8, y=-34.67, z=107.49, rotX=180, rotZ=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=8, y=-34.67, z=107.49, rotX=180, rotZ=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=24, y=-34.67, z=107.49, rotX=180, rotZ=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=-24, y=34.57, z=107.49, rotX=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=-8, y=34.57, z=107.49, rotX=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=8, y=34.57, z=107.49, rotX=180 )).tolist (),
                      (makeTransformMillimetersDegrees ( x=24, y=34.57, z=107.49, rotX=180 )).tolist ()
                  ],
                  "transformOrganOriginToMaleCableSocket": None,
                  "gripperOpeningFraction": 0.4,
                  "gripperClosedFraction": 1.0,
                  "USE_FORCE_MODE": True
              },
              '1': { "organType": 1,
                   "friendlyName": "Wheel organ",
                   "forceModeTravelDistance": 30 / 1000,
                   "postInsertExtraPushDistance": 2.0 / 1000,
                   "pickupExtraPushDistance": 0 / 1000,  # positive value will make it hold further onto organ
                   "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees ( x=0, y=56, z=4.5, rotX=0, rotY=90, rotZ=90 ).tolist (),
                   "transformOrganOriginToGripper": None,  # makeTransformMillimetersDegrees(x=0, y=-19.338, z=-8, rotX=90, rotY=0, rotZ=0).tolist(),
                   "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees ( x=0, y=35, z=34.4, rotX=180, rotY=0, rotZ=0 ).tolist (),
                   "transformOrganOriginToFemaleCableSocket": None,  # female socket(s)
                   "gripperOpeningFraction": 0.0,
                   "gripperClosedFraction": 0.45,
                   "USE_FORCE_MODE": True
                   },
              '2': { "organType": 2,
                   "friendlyName": "Sensor organ",
                   "forceModeTravelDistance": 5 / 1000,
                   "postInsertExtraPushDistance": 2.0 / 1000,
                   "pickupExtraPushDistance": 0 / 1000,
                   "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees ( x=0, y=42.57, z=-7.7, rotX=0, rotY=90, rotZ=90 ).tolist (),
                   "transformOrganOriginToGripper": None,  # makeTransformMillimetersDegrees(x=-6, y=0, z=-9.579, rotX=180, rotY=0, rotZ=90).tolist(),
                   "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees ( x=0, y=21.57, z=22.2, rotX=180, rotY=0, rotZ=0 ).tolist (),
                   "transformOrganOriginToFemaleCableSocket": None,  # female socket(s)
                   "gripperOpeningFraction": 0.0,
                   "gripperClosedFraction": 0.6,
                   "USE_FORCE_MODE": True,
                   },
              '3': { "organType": 3,  # minimally tested!
                   "friendlyName": "Joint organ",
                   "forceModeTravelDistance": 30 / 1000,
                   "postInsertExtraPushDistance": 2.0 / 1000,
                   "pickupExtraPushDistance": 0 / 1000,
                   "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees ( x=0, y=47.741, z=-30, rotX=0, rotY=90, rotZ=90 ).tolist (),
                   "transformOrganOriginToGripper": makeTransformMillimetersDegrees ( x=0, y=37.741, z=-35.238, rotX=180, rotY=0, rotZ=0 ).tolist (),
                   "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees ( x=8, y=12.741, z=0, rotX=180, rotY=0, rotZ=0 ).tolist (),
                   "transformOrganOriginToFemaleCableSocket": [ makeTransformMillimetersDegrees ( x=-12.217, y=-35.292, z=3.5, rotX=180, rotY=0, rotZ=0 ).tolist () ],  # female socket(s)
                   "gripperOpeningFraction": 0.0,
                   "gripperClosedFraction": 0.5,
                   "USE_FORCE_MODE": True
                   },
              '4': { "organType": 4,
                   "friendlyName": "Castor organ",
                   "forceModeTravelDistance": 5 / 1000,
                   "postInsertExtraPushDistance": 2.0 / 1000,
                   "pickupExtraPushDistance": 0 / 1000,
                   "transformOrganOriginToGripper": (makeTransformMillimetersDegrees ( x=0, y=6, z=18.312, rotX=180 )).tolist (),
                   "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees ( x=0, y=16, z=20.1, rotX=0, rotY=180, rotZ=90 ).tolist (),
                   "transformOrganOriginToFemaleCableSocket": None,  # female socket(s)
                   "gripperOpeningFraction": 0.6,
                   "gripperClosedFraction": 0.8,
                   "USE_FORCE_MODE": True,
                   "transformOrganOriginToMaleCableSocket": None
                   }
          }
          }

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
blueprint_filename = "blueprint_{}.csv".format(robotID)
mesh_filename = "mesh_{}.csv".format(robotID)

#printer = Printer ( None, config )
#printer.createSTL(robotID,True)
#printer.slice("mesh_{}".format(robotID))

myRobot=Robot(ID=robotID)
print("{}/{}".format(waiting_to_be_built_folder_path,blueprint_filename))
myRobot.createOrgansFromBlueprint("{}/{}".format(waiting_to_be_built_folder_path,blueprint_filename), config["dictionaryOfOrganTypes"], makeTransformInputFormatted())
myRobot.drawRobot(waiting_to_be_built_folder_path+"/../")
