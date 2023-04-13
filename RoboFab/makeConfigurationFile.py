## This is a separate file from the rest, whose purpose is just to create a configuration JSON file. This file will then
## be loaded by RoboFab.py, so there is no need to run this file every time, unless you have changed one of the settings
## contained in it.
## Hopefully, this should provide an easier place to find and change parameters such as the location of things

import json
from helperFunctions import makeTransformInputFormatted, makeTransformMillimetersDegrees
import math
import numpy as np



def makeFile(location):

	# location specific variables:
	if location == "BRL":

		LOG_FOLDER = "/home/robofab/are-logs/final_exploration_hybrid/"
		COPPELIASIM_FOLDER = "/home/robofab/CoppeliaSim_Edu_V4_2_0_Ubuntu18_04"
		SOFTWARE_PARAMETERS_FILE = "/home/robofab/parameters.csv"

		ROBOFAB_WEBCAM_PIPE = "/dev/v4l/by-id/usb-046d_Logitech_Webcam_C930e_86CF445E-video-index0"

		# ASSEMBLY_FIXTURE_ORIGIN = makeTransformInputFormatted([4.02 / 1000 , -0.67789 , 0.748 , math.radians(180), 0, math.radians(90)]).tolist()  # 6.315
		ASSEMBLY_FIXTURE_ORIGIN = makeTransformMillimetersDegrees(x=69.87 , y=-570.36 , z=72.8 , rotY=0.59 , rotZ=90-0.40488).tolist()
		PRINTER_0_ORIGIN = makeTransformInputFormatted([0.3345 , 0.106 , 0.0816 , 0 , 0 , math.radians(-90)]) .tolist()
		PRINTER_1_ORIGIN = makeTransformInputFormatted().tolist()
		PRINTER_2_ORIGIN = makeTransformInputFormatted().tolist()
		PRINTER_0_API_KEY = "b76fba867e5ee070caff864d953ed27b"
		PRINTER_1_API_KEY = "change_me"
		ORGAN_BANK_1_ORIGIN = makeTransformMillimetersDegrees( x=-400.07, y=-256.03, z=-9.49, rotX=0.24, rotY=0, rotZ=90 + 0.037 ).tolist()

		gripper_TCP_FOR_CABLES = makeTransformMillimetersDegrees(x=0.0, y=7.6, z=215, rotZ=180).tolist() # single gripper
		gripper_TCP_A = makeTransformMillimetersDegrees(x=0.0, y=7.6, z=215, rotZ=180).tolist() # single gripper

		# define what constitutes "open" and "closed" for the servos on the gripper:
		A_open_position = 2400
		A_closed_position = 1350
		B_open_position = 2100
		B_closed_position = 1400


	elif location == "YRK":
		LOG_FOLDER = "/home/robofab/are-logs/test_are_generate"
		COPPELIASIM_FOLDER = "/home/robofab/vrep"
		SOFTWARE_PARAMETERS_FILE = "/home/robofab/evolutionary_robotics_framework/experiments/test_are_generate/parameters.csv"

		ROBOFAB_WEBCAM_PIPE = "0"

		# ASSEMBLY_FIXTURE_ORIGIN = makeTransformInputFormatted([4.02 / 1000 , -0.67789 , 0.748 , math.radians(180), 0, math.radians(90)]).tolist()  # 6.315
		# ASSEMBLY_FIXTURE_ORIGIN = makeTransformMillimetersDegrees(x=63.1 , y=-578.4 , z=70.9 , rotY=0 , rotZ=90).tolist()
		ASSEMBLY_FIXTURE_ORIGIN = makeTransformMillimetersDegrees(x=104.9 , y= -576.4 , z=71.0 , rotZ=90).tolist()
		PRINTER_0_ORIGIN = makeTransformMillimetersDegrees(x=427.8, y=160.5, z=83, rotZ = -90).tolist()
		# PRINTER_0_ORIGIN = makeTransformInputFormatted([0.390 , 0.1561 , 0.085 , 0 , 0 , math.radians(-90)]) .tolist()
		PRINTER_1_ORIGIN = makeTransformMillimetersDegrees(x=--73.1 ,y=355.2, z=83).tolist()
		# PRINTER_1_ORIGIN = makeTransformInputFormatted([-0.1013, 0.244 , 0.085 , 0 , 0 , 0]).tolist()
		PRINTER_0_API_KEY = "9B987FFCEE3540F796014AA3C96D2CE4"
		PRINTER_1_API_KEY = "FA8E62EF85C74861A40D67EF75E09764"
		# ORGAN_BANK_1_ORIGIN = (makeTransformMillimetersDegrees(x=-335.9, y=-272.5, z=-8.6, rotZ=90) * makeTransformMillimetersDegrees(rotX=0.408, rotY=0.019) ).tolist()
		ORGAN_BANK_1_ORIGIN = (  makeTransformMillimetersDegrees(z=-2.5)  * makeTransformMillimetersDegrees(x=-335.9, y=-272.5, z=-8.6, rotZ=90) * makeTransformMillimetersDegrees(rotX=0.408, rotY=0.019) ).tolist()

		gripper_TCP_A = (makeTransformMillimetersDegrees(x=0, y=9.6, z=216.9, rotZ=180) ).tolist() # single gripper
		gripper_TCP_FOR_CABLES = (makeTransformMillimetersDegrees(x=0, y=9.6, z=216.9, rotZ=180) * makeTransformMillimetersDegrees(rotX=3) ).tolist() # single gripper


		# define what constitutes "open" and "closed" for the servos on the gripper:
		A_open_position = 0
		A_closed_position = 0
		B_open_position = 2100
		B_closed_position = 1080


	else:
		raise Exception("unknown location given for making configuration file")

	data = {

		"location":location,

		"logDirectory":LOG_FOLDER,
		"parametersFile":SOFTWARE_PARAMETERS_FILE,
		"coppeliasimFolder":COPPELIASIM_FOLDER,

		"RoboFabWebcamPipe":ROBOFAB_WEBCAM_PIPE,

		"network":{
			"COMPUTER_ADDRESS":"192.168.2.253", # the RoboFab PC (that runs this code)
			"ROBOT_IP_ADDRESS":"192.168.2.252", # UR5
			"PORT_FOR_UR5":30000,
			"PORT_FOR_GRIPPER":54321,
			"PRINTER0_IP_ADDRESS":"192.168.2.251",
			"PRINTER1_IP_ADDRESS":"192.168.2.250",
		},

		"UR5":{
			"HOME_POSITIONS":{
				# "home" : [math.radians(-90) , math.radians(-136.23) , math.radians(125.09) , math.radians(-162.86) , math.radians(-90) , math.radians(90)] ,
				"home" : [math.radians(-90) , math.radians(-131.31) , math.radians(88.06) , math.radians(-90) , math.radians(-90) , math.radians(90)] ,
				"organ_bank" : [math.radians(-19.04) , math.radians(-98.38) , math.radians(66.56) , math.radians(-58.10) , math.radians(-89.90) , math.radians(70.80)] ,
				"limb_assembly_fixture" : [math.radians(40) , math.radians(-90.61) , math.radians(84.61) , math.radians(-127.85) , math.radians(-78.71) , math.radians(78.83)] ,
				"AF" :        [math.radians(83.7) , math.radians(-106.51) , math.radians(84.60) , math.radians(-67.99) , math.radians(-89.85) , math.radians(83.52)] ,
				"printer_0" : [math.radians(-189) , math.radians(-115) , math.radians(84) , math.radians(-60) , math.radians(-90) , math.radians(81)] ,
				"printer_1" : [math.radians(-90) , math.radians(-131.31) , math.radians(88.06) , math.radians(-90) , math.radians(-90) , math.radians(90)] ,
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
			"TCP_FOR_CABLES": gripper_TCP_FOR_CABLES,
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
				"transformOrganOriginToFemaleCableSocket": [
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
				"postInsertExtraPushDistance": 2.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000, # positive value will make it hold further onto organ
                 "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0, y=47.9, z=-2.9, rotX=0, rotY=90, rotZ=90).tolist(),
                 "transformOrganOriginToGripper": None, #makeTransformMillimetersDegrees(x=0, y=-19.338, z=-8, rotX=90, rotY=0, rotZ=0).tolist(),
                 "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees(x=0, y=35, z=34.4, rotX=180, rotY=0, rotZ=0).tolist(),
                "transformOrganOriginToFemaleCableSocket": None, # female socket(s)
				"gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.45,
				"USE_FORCE_MODE":False
				 },
			2: {"organType": 2,
				"friendlyName": "Sensor organ",
				"forceModeTravelDistance": 5 / 1000,
				"postInsertExtraPushDistance": 2.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000,
                "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0, y=34.47, z=-15, rotX=0, rotY=90, rotZ=90).tolist(),
                "transformOrganOriginToGripper": None, #makeTransformMillimetersDegrees(x=-6, y=0, z=-9.579, rotX=180, rotY=0, rotZ=90).tolist(),
                "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees(x=0, y=21.57, z=22.2, rotX=180, rotY=0, rotZ=0).tolist(),
				"transformOrganOriginToFemaleCableSocket": None, # female socket(s)
				 "gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.6,
				"USE_FORCE_MODE":False,
				 },
			3: {"organType": 3, # minimally tested!
				"friendlyName": "Leg organ",
				"forceModeTravelDistance": 30 / 1000,
				"postInsertExtraPushDistance": 2.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000,
                "transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0, y=39.641, z=-37.29, rotX=0, rotY=90, rotZ=90).tolist(),
                "transformOrganOriginToGripper": makeTransformMillimetersDegrees(x=0, y=37.741, z=-40.238, rotX=180, rotY=0, rotZ=0).tolist(),
                "transformOrganOriginToMaleCableSocket": makeTransformMillimetersDegrees(x=9, y=11.741, z=0, rotX=180, rotY=0, rotZ=0).tolist(),
				"transformOrganOriginToFemaleCableSocket":  [makeTransformMillimetersDegrees(x=-12.217, y=-35.059, z=3.5, rotX=180, rotY=0, rotZ=0).tolist()],  # female socket(s)
				"gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.45,
				"USE_FORCE_MODE":True
				 },
			4: {"organType": 4,
				"friendlyName": "Castor organ",
				"forceModeTravelDistance": 5 / 1000,
				"postInsertExtraPushDistance": 2.0 / 1000,
				"pickupExtraPushDistance": 0 / 1000,
				"transformOrganOriginToGripper": (makeTransformMillimetersDegrees(x=0,y=13.5,z=-14.912,rotZ=180)).tolist(),
				"transformOrganOriginToClipCentre": makeTransformMillimetersDegrees(x=0,y=15.4,z=-9.4,rotX=0,rotY=-90,rotZ=-90).tolist(),
				"transformOrganOriginToFemaleCableSocket": None,  # female socket(s)
				"gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.45,
				"USE_FORCE_MODE":True,
				"transformOrganOriginToMaleCableSocket": None
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
			"CLEAR_Z_HEIGHT": 0.4 # global coordinates z value which is guaranteed to be above all parts of the robot when it is on the assembly fixture
		},

		"PRINTER_0":{
		"ORIGIN":PRINTER_0_ORIGIN,
        "BED_COOLDOWN_TEMPERATURE":30,
		"API_KEY":PRINTER_0_API_KEY
		},
		"PRINTER_1":{
		"ORIGIN":PRINTER_1_ORIGIN,
        "BED_COOLDOWN_TEMPERATURE":30,
		"API_KEY":PRINTER_1_API_KEY
		},

		"ORGAN_BANK_1":{
		"ORIGIN":ORGAN_BANK_1_ORIGIN,
		"ORGAN_CONTENTS":[ # a list of lists, each sub-list represents an organ: [<organ type> , <x (m)> , <y (m)> , <z (m)> , <rx (radians)> , <ry (radians)> , <rz (radians)>, i2c or IP Address]
			[0, 225/1000, 300/1000, 3/1000 , 0, 0, 0 , "192.168.20.101"], # Head
			[1, 20/1000,  75/1000 , 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x60)], # wheel
			[1, 95/1000,  75/1000 , 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x61)], # wheel
			[1, 20/1000,  150/1000, 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x62)], # wheel
			[1, 95/1000,  150/1000, 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x63)], # wheel
			[1, 20/1000,  225/1000, 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x64)], # wheel
			[1, 95/1000,  225/1000, 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x65)], # wheel
			[1, 20/1000,  300/1000, 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x66)], # wheel
			[1, 95/1000,  300/1000, 35/1000, math.radians(0), math.radians(0), math.radians(-90) , str(0x67)], # wheel
			[2, 178.43/1000, (77.5 + 0  )/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x30)], # sensor
			[2, 228.43/1000, (77.5 + 0  )/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x32)], # sensor
			[2, 178.43/1000, (77.5 + 40 )/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x34)], # sensor
			[2, 228.43/1000, (77.5 + 40 )/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x36)], # sensor
			[2, 178.43/1000, (77.5 + 80 )/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x38)], # sensor
			[2, 228.43/1000, (77.5 + 80 )/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x3A)], # sensor
			[2, 178.43/1000, (77.5 + 120)/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x3C)], # sensor
			[2, 228.43/1000, (77.5 + 120)/1000, 22.2/1000, 0, 0, math.radians(-90) , str(0x3E)], # sensor
			[4, (325)/1000, (65 + 0*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 1*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 2*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 3*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 4*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 5*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 6*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[4, (325)/1000, (65 + 7*37.5)/1000 , 17/1000, math.radians(180), math.radians(0), math.radians(0), str(0x00)], #castor
			[3, 400/1000, (106.741-0.5)/1000, 61/1000, math.radians(0), math.radians(0), math.radians(180), str(0x00)], #jointA
			[3, 475/1000, (106.741-0.5)/1000, 61/1000, math.radians(0), math.radians(0), math.radians(180), str(0x00)], #jointA
			[3, 400/1000, (216.741-0.5)/1000, 61/1000, math.radians(0), math.radians(0), math.radians(180), str(0x00)], #jointA
			[3, 475/1000, (216.741-0.5)/1000, 61/1000, math.radians(0), math.radians(0), math.radians(180), str(0x00)]  #jointA
			]
		}
	}
	filename = "configuration_" + data["location"] + ".json"
	with open(filename, "w") as write_file:
		json.dump(data, write_file , indent=4)


if __name__ == "__main__":
	makeFile("YRK")
