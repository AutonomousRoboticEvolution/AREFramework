## This is a separate file from the rest, whose purpose is just to create a configuration JSON file. This file will then
## be loaded by RoboFab.py, so there is no need to run this file every time, unless you have changed one of the settings
## contained in it.
## Hopefully, this should provide an easier place to find and change parameters such as the location of things

import json
from helperFunctions import makeTransformInputFormatted, makeTransformMillimetersDegrees
import math
import numpy as np

LOG_FOLDER = "/home/robofab/are-logs/test_are_generate"
COPPELIASIM_FOLDER = "/home/robofab/CoppeliaSim_Edu_V4_2_0_Ubuntu18_04"
SOFTWARE_PARAMETERS_FILE= "/home/robofab/evolutionary_robotics_framework/experiments/test_are_generate/parameters.csv"

def makeFile(location):

	# location specific variables:
	if location == "BRL":
		# ASSEMBLY_FIXTURE_ORIGIN = makeTransformInputFormatted([4.02 / 1000 , -0.67789 , 0.748 , math.radians(180), 0, math.radians(90)]).tolist()  # 6.315
		ASSEMBLY_FIXTURE_ORIGIN = makeTransformMillimetersDegrees(x=69.87 , y=-570.36 , z=72.8 , rotY=0.59 , rotZ=90-0.40488).tolist()
		PRINTER_0_ORIGIN = makeTransformInputFormatted([0.3345 , 0.106 , 0.0816 , 0 , 0 , math.radians(-90)]) .tolist()
		PRINTER_1_ORIGIN = makeTransformInputFormatted().tolist()
		PRINTER_2_ORIGIN = makeTransformInputFormatted().tolist()
		ORGAN_BANK_1_ORIGIN = makeTransformInputFormatted([-0.400 , -0.1598 , -10.12 / 1000 , math.radians(0.245) , 0 , math.radians(90)]).tolist()

		gripper_TCP_A = makeTransformMillimetersDegrees(x=0.0, y=7.6, z=215, rotZ=180).tolist() # single gripper

		# define what constitutes "open" and "closed" for the servos on the gripper:
		A_open_position = 2400
		A_closed_position = 1350
		B_open_position = 2100
		B_closed_position = 1350


	elif location == "YRK":
		ASSEMBLY_FIXTURE_ORIGIN =  makeTransformMillimetersDegrees(x=67.8, y=-710.9, z= 70.9, rotZ=90).tolist()  #  NEEDS UPDATING
		PRINTER_0_ORIGIN = makeTransformMillimetersDegrees(x=536.0, y=-83.6, z=80.5, rotZ=-90) .tolist()
		PRINTER_1_ORIGIN = makeTransformMillimetersDegrees() .tolist()
		PRINTER_2_ORIGIN = makeTransformMillimetersDegrees() .tolist()
		ORGAN_BANK_1_ORIGIN = makeTransformMillimetersDegrees(x=-373.3, y=-197.0, z=-7.8).tolist()
		# gripper_TCP_A = (makeTransformInputFormatted([0.19326, 0.00401, 0.14262, math.radians(45), 0, math.radians(90)])).tolist()
		gripper_TCP_A = makeTransformMillimetersDegrees(x=-0.9, y=30.7, z=209.9, rotZ=181.94).tolist() # single gripper

		# define what constitutes "open" and "closed" for the servos on the gripper:
		A_open_position = 1800
		A_closed_position = 1125
		B_open_position = 2100
		B_closed_position = 1350

	else:
		raise Exception("unknown location given for making configuration file")

	# using a loop to make all the cables in the cable bank, this will be added into the structure later:
	cable_bank_cables_temp = []
	x_spacing = 0.0336
	y_spacing = 0.0330
	for y_pos in [0, 1, 2]:
		for x_pos in [0, 1]:
			cable_bank_cables_temp.append([makeTransformInputFormatted([x_spacing * 2 * x_pos, y_spacing * y_pos, 0]).tolist() ,
                                           makeTransformInputFormatted([x_spacing * (2 * x_pos + 1), y_spacing * y_pos, 0]).tolist()])



	data = {

		"location":location,

		"logDirectory":LOG_FOLDER,
		"parametersFile":SOFTWARE_PARAMETERS_FILE,
		"coppeliasimFolder":COPPELIASIM_FOLDER,

		"network":{
			"COMPUTER_ADDRESS":"192.168.2.253", # the RoboFab PC (that runs this code)
			"ROBOT_IP_ADDRESS":"192.168.2.252", # UR5
			"PORT_FOR_UR5":30000,
			"PORT_FOR_GRIPPER":54321,
			"PRINTER0_IP_ADDRESS":"192.168.2.251",
			"PRINTER1_IP_ADDRESS":"192.168.2.250",
			"PRINTER2_IP_ADDRESS":"192.168.2.249"
		},

		"UR5":{
			"HOME_POSITIONS":{
				# "home" : [math.radians(-90) , math.radians(-136.23) , math.radians(125.09) , math.radians(-162.86) , math.radians(-90) , math.radians(90)] ,
				"home" : [math.radians(-90) , math.radians(-131.31) , math.radians(88.06) , math.radians(-90) , math.radians(-90) , math.radians(90)] ,
				"organ_bank" : [math.radians(-19.04) , math.radians(-98.38) , math.radians(66.56) , math.radians(-58.10) , math.radians(-89.90) , math.radians(70.80)] ,
				"limb_assembly_fixture" : [math.radians(40) , math.radians(-90.61) , math.radians(84.61) , math.radians(-127.85) , math.radians(-78.71) , math.radians(78.83)] ,
				"AF" :        [math.radians(83.7) , math.radians(-106.51) , math.radians(84.60) , math.radians(-67.99) , math.radians(-89.85) , math.radians(83.52)] ,
				"printer" : [math.radians(-189) , math.radians(-115) , math.radians(84) , math.radians(-60) , math.radians(-90) , math.radians(81)] ,
				# "printer" : [math.radians(-160.71) , math.radians(-117.80) , math.radians(92.97) , math.radians(-47.22) , math.radians(-131.13) , math.radians(-154.05)] ,
				"cable_bank": [math.radians(-195.94), math.radians(-116.06), math.radians(100.78), math.radians(-180.28), math.radians(-74.41), math.radians(94.49-180)],
				"cable_preparation": [math.radians(25), math.radians(-54.82), math.radians(83.03), math.radians(-118.29), math.radians(90), math.radians(-38.83+180)]
			},
			"speedValueNormal":200,
			"speedValueSlow":80,
			"speedValueReallySlow":20,
			"disableServoOnFinish":False
		},

		"gripper":{
			"STARTUP_MESSAGE":"a0b0",
			"EXPECTED_STARTUP_REPLY":"I am the gripper",
			"TCP_A": gripper_TCP_A,
			# "TCP_A": (makeTransformInputFormatted( [0,0,0, math.radians(45),0,math.radians(-270)] ) * makeTransformInputFormatted([0.002,-30/1000,231/1000])).tolist(),
			"TCP_B": (makeTransformInputFormatted([0, 0, 0, math.radians(-45), 0, math.radians(90)]) * makeTransformInputFormatted([0, 0, 0, 0, 0, math.radians(180)]) * makeTransformInputFormatted([-2 / 1000, 19 / 1000, 261 / 1000]) * makeTransformInputFormatted([0, 0.001, 0.006])).tolist(),
			"SUCCESS_MESSAGE":"OK",
			"min_servo_value":900,
			"max_servo_value":2100,
			"A_open_position":A_open_position,
			"A_closed_position":A_closed_position,
			"B_open_position":B_open_position,
			"B_closed_position":B_closed_position
		},

		# Defines properties of each organ type
		"dictionaryOfOrganTypes":{
			0: {
				"organType": 0,
				"friendlyName": "Head organ",
				# "forceModeTravelDistance": 20.0 / 1000,
				"forceModeTravelDistance": 18/1000,
				# "postInsertExtraPushDistance": 3.0 / 1000, # positive for extra pushing when attaching organ to skeleton
				"postInsertExtraPushDistance": 0.0 / 1000, # positive for extra pushing when attaching organ to skeleton
				"pickupExtraPushDistance": 2 / 1000, # offset in gripper frame z direction just when picking up organ
				# "transformOrganOriginToGripper": np.linalg.inv(makeTransformInputFormatted ( [ 0, 0, 72 / 1000, 0, math.radians ( 180 ), math.radians(180) ] ) ).tolist(), # Head v0.1
				"transformOrganOriginToGripper": np.linalg.inv(makeTransformMillimetersDegrees (z=85 , rotX=180 )).tolist(), # Head v0.2
				"transformOrganOriginToClipCentre": makeTransformInputFormatted().tolist(),
				"transformOrganOriginToCableSocket": [
					(makeTransformMillimetersDegrees(x=-24 , y=-34.67 , z=107.49 , rotX=180,rotZ=180)).tolist (),
					(makeTransformMillimetersDegrees(x=-8 , y=-34.67 , z=107.49 , rotX=180,rotZ=180)).tolist (),
					(makeTransformMillimetersDegrees(x=8 , y=-34.67 , z=107.49 , rotX=180,rotZ=180)).tolist (),
					(makeTransformMillimetersDegrees(x=24 , y=-34.67 , z=107.49 , rotX=180,rotZ=180)).tolist (),
					(makeTransformMillimetersDegrees(x=-24 , y=34.57 , z=107.49 , rotX=180)).tolist (),
					(makeTransformMillimetersDegrees(x=-8 , y=34.57 , z=107.49 , rotX=180)).tolist (),
					(makeTransformMillimetersDegrees(x=8 , y=34.57 , z=107.49 , rotX=180)).tolist (),
					(makeTransformMillimetersDegrees(x=24 , y=34.57 , z=107.49 , rotX=180)).tolist ()
				],
				"transformOrganOriginToMaleCableSocket": None,
				"gripperOpeningFraction": 0.4,
				"gripperClosedFraction": 1.0,
				"USE_FORCE_MODE":True
			},
			1: { "organType": 1,
				"friendlyName": "Wheel organ",
				"forceModeTravelDistance": 30 / 1000,
				"postInsertExtraPushDistance": 4.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000, # positive value will make it hold further onto organ
                 "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0, y=56, z=4.5, rotX=0, rotY=90, rotZ=90).tolist(),
                 "transformOrganOriginToGripper": None, #makeTransformMillimetersDegrees(x=0, y=-19.338, z=-8, rotX=90, rotY=0, rotZ=0).tolist(),
                 "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees(x=0, y=35, z=34.4, rotX=180, rotY=0, rotZ=0).tolist(),
                "transformOrganOriginToCableSocket": None, # female socket(s)
				"gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.45,
				"USE_FORCE_MODE":True
				 },
			2: {"organType": 2,
				"friendlyName": "Sensor organ",
				"forceModeTravelDistance": 5 / 1000,
				"postInsertExtraPushDistance": 2.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000,
                "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0, y=42.57, z=-7.7, rotX=0, rotY=90, rotZ=90).tolist(),
                "transformOrganOriginToGripper": None, #makeTransformMillimetersDegrees(x=-6, y=0, z=-9.579, rotX=180, rotY=0, rotZ=90).tolist(),
                "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees(x=0, y=21.57, z=22.2, rotX=180, rotY=0, rotZ=0).tolist(),
				"transformOrganOriginToCableSocket": None, # female socket(s)
				 "gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.6,
				"USE_FORCE_MODE":True,
				 },
			3: {"organType": 3,
				"friendlyName": "Castor organ",
				"forceModeTravelDistance": 5 / 1000,
				"postInsertExtraPushDistance": 2.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000,
				 # "gripPosition": makeTransformInputFormatted ([ 5.89 / 1000, 29.06 / 1000, 15.37 / 1000, 0, math.radians ( 180 ), math.radians ( 90 ) ] ).tolist(),
				"transformOrganOriginToGripper": (makeTransformInputFormatted([0, 0, 7.79 / 1000 , 0, 0, math.radians(90)])).tolist(),
				"transformOrganOriginToClipCentre": makeTransformInputFormatted([0, 0, 12.62 / 1000 , math.radians(180), 0, 0]).tolist(),
				"transformOrganOriginToCableSocket": None,  # female socket(s)
				"gripperOpeningFraction": 0.6,
				"gripperClosedFraction": 0.8,
				"USE_FORCE_MODE":True,
				"transformOrganOriginToMaleCableSocket": None
				 },
			4: {"organType": 4, # minimally tested!
				"friendlyName": "Joint organ",
				"forceModeTravelDistance": 30 / 1000,
				"postInsertExtraPushDistance": 1.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000,
                "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0, y=11.3, z=15, rotX=0, rotY=-90, rotZ=0).tolist(),
                "transformOrganOriginToGripper": makeTransformMillimetersDegrees(x=0, y=2, z=29.779, rotX=0, rotY=0, rotZ=0).tolist(),
                "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees(x=-9, y=-5, z=-14.5, rotX=0, rotY=0, rotZ=0).tolist(),
				"transformOrganOriginToCableSocket": None,  # female socket(s)
				"gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.5,
				"USE_FORCE_MODE":True
				 }
		},

		"ASSEMBLY_FIXTURE":{
			"ARDUINO_SUCCESS_MESSAGE":"OK",
			"TIMEOUT_SECONDS":60,
			"WAIT_FOR_OK_REPLY":True,
			"ORIGIN":ASSEMBLY_FIXTURE_ORIGIN,
			"HOME_BETWEEN_EVERY_MOVE":False,
			"CORE_ORGAN_ATTACHMENT_Z_OFFSET": 0/1000,
			"EXPECTED_STARTUP_MESSAGE":"I am the Assembly Fixture",
			"CLEAR_Z_HEIGHT": 0.42 # global coordinates z value which is guaranteed to be above all parts of the robot when it is on the assembly fixture
		},

		"PRINTER_0":{
		"ORIGIN":PRINTER_0_ORIGIN,
        "BED_COOLDOWN_TEMPERATURE":30
		},
		"PRINTER_1":{
		"ORIGIN":PRINTER_1_ORIGIN,
        "BED_COOLDOWN_TEMPERATURE":30
		},
		"PRINTER_2":{
		"ORIGIN":PRINTER_2_ORIGIN,
        "BED_COOLDOWN_TEMPERATURE":30
		},

		"ORGAN_BANK_1":{
		"ORIGIN":ORGAN_BANK_1_ORIGIN,
		"ORGAN_CONTENTS":[ # a list of lists, each sub-list represents an organ: [<organ type> , <x (m)> , <y (m)> , <z (m)> , <rx (radians)> , <ry (radians)> , <rz (radians)>, i2c or IP Address]
			[0, 204.06/1000, 286.06/1000, 3/1000 , 0, 0, 0 , "192.168.20.101"], # Head
			[1, 2/1000,  46.061/1000, 10/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x60)], # wheel
			[1, 94.811/1000,  81.061/1000, 10/1000, math.radians(0), math.radians(0), math.radians(90) , str(0x61)], # wheel
			[1, 2/1000,  (46.061+82)/1000, 10/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x62)], # wheel
			[1, 94.811/1000,  (81.061+82)/1000, 10/1000, math.radians(0), math.radians(0), math.radians(90) , str(0x63)], # wheel
			[1, 2/1000,  (46.061+82*2)/1000, 10/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x64)], # wheel
			[1, 94.811/1000,  (81.061+82*2)/1000, 10/1000, math.radians(0), math.radians(0), math.radians(90) , str(0x65)], # wheel
			[1, 2/1000,  (46.061+82*3)/1000, 10/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x66)], # wheel
			[1, 94.811/1000,  (81.061+82*3)/1000, 10/1000, math.radians(0), math.radians(0), math.radians(90) , str(0x67)], # wheel
			[2, 164.631/1000, 30.5/1000, 22.2/1000, 0, 0, math.radians(90) , str(0x30)], # sensor
			[2, 164.631/1000, (30.5+41)/1000, 22.2/1000, 0, 0, math.radians(90) , str(0x32)], # sensor
			[2, 164.631/1000, (30.5+41*2)/1000, 22.2/1000, 0, 0, math.radians(90) , str(0x34)], # sensor
			[2, 164.631/1000, (30.5+41*3)/1000, 22.2/1000, 0, 0, math.radians(90) , str(0x36)], # sensor
			[2, 191.491/1000, 30.5/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x38)], # sensor
			[2, 191.491/1000, (30.5+41)/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x3A)], # sensor
			[2, 191.491/1000, (30.5+41*2)/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x3C)], # sensor
			[2, 191.491/1000, (30.5+41*3)/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x3E)], # sensor
			# [3 , 154.56/1000, 33.66/1000, 17.5/1000, math.radians(180), 0, math.radians(90), ""], #castor
			[4 , 300.061/1000, 74.881/1000 , 28.5/1000, 0, 0, math.radians(90), str(0x08)] #joint
			],
		"CABLE_CONTENTS":[] # no cables
		}
	}
	filename = "configuration_" + data["location"] + ".json"
	with open(filename, "w") as write_file:
		json.dump(data, write_file , indent=4)


if __name__ == "__main__":
	makeFile()
