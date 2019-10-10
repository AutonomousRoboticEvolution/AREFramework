## This is a separate file from the rest, whose purpose is just to create a configuration JSON file. This file will then
## be loaded by RoboFab.py, so there is no need to run this file every time, unless you have changed one of the settings
## contained in it.
## Hopefully, this should provide an easier place to find and change parameters such as the location of things

import json
from helperFunctions import makeTransform
import math
import numpy as np

def makeFile(location="BRL"):

	# location specific variables:
	if location == "BRL":
		ASSEMBLY_FIXTURE_ORIGIN = makeTransform([4.02 / 1000 , -0.67789 , 0.748-0.0005 , math.radians(180),0,0 ]).tolist() #6.315
		PRINTER_1_ORIGIN = makeTransform( [ 0.3345 , 0.106, 0.0819 , 0 , 0 , math.radians( -90 ) ] ) .tolist()
		CABLE_BANK_1_ORIGIN = (makeTransform([0.5912 , -0.0014 , 0.5168 , math.radians(60) , 0 , math.radians(-90) ]) * makeTransform([-0.002,-0.001,-0.005]) ).tolist()
		ORGAN_BANK_1_ORIGIN = makeTransform([-0.400 , -0.1598 , -7.4/1000 , 0 , 0 ,math.radians(90) ]).tolist()
	else:
		raise Exception("unknown location given for making configuration file")

	# using a loop to make all the cables in the cable bank, this will be added into the structure later:
	cable_bank_cables_temp = []
	x_spacing = 0.0336
	y_spacing = 0.0330
	for y_pos in [0, 1, 2]:
		for x_pos in [0, 1]:
			cable_bank_cables_temp.append([makeTransform([x_spacing * 2 * x_pos, y_spacing * y_pos, 0]).tolist() ,
										   makeTransform([x_spacing * (2 * x_pos + 1), y_spacing * y_pos, 0]).tolist()])



	data = {

		"location":location,

		"network":{
			"COMPUTER_ADDRESS":"192.168.2.253",
			"PORT_FOR_UR5":30000
		},

		"UR5":{
			"HOME_POSITIONS":{
				"home" : [math.radians(-90) , math.radians(-136.23) , math.radians(125.09) , math.radians(-162.86) , math.radians(-90) , math.radians(90)] ,
				# "organ_bank" : [math.radians(0) , math.radians(-97) , math.radians(84) , math.radians(-77) , math.radians(-89) , math.radians(0)] ,
				"organ_bank" : [math.radians(-15.69) , math.radians(-90.61) , math.radians(84.61) , math.radians(-127.85) , math.radians(-78.71) , math.radians(78.83)] ,
				"AF_core" : [math.radians(75.05) , math.radians(-44.69) , math.radians(97.83) , math.radians(-157.51) , math.radians(47.12) , math.radians(-159.55)] ,
				"AF_cables" : [math.radians(84.06) , math.radians(-58.19) , math.radians(111.59) , math.radians(-131.29) , math.radians(84.94) , math.radians(12.73)] ,
				# "AF" : [math.radians(80) , math.radians(-134) , math.radians(135) , math.radians(-180) , math.radians(-80) , math.radians(-3)] ,
				"AF" : [math.radians(85.09) , math.radians(-103.06) , math.radians(139.4) , math.radians(-215.35) , math.radians(-85.22) , math.radians(-2.8)] ,
				# "printer" : [math.radians(155-360) , math.radians(-113) , math.radians(98) , math.radians(-80) , math.radians(-88) , math.radians(3)] ,
				"printer" : [math.radians(-160.71) , math.radians(-117.80) , math.radians(92.97) , math.radians(-47.22) , math.radians(-131.13) , math.radians(-154.05)] ,
				# "printer" : [math.radians(-164.89) , math.radians(-103.86) , math.radians(84.42) , math.radians(-56.00) , math.radians(-133.00) , math.radians(200.74-360)] ,
				"cable_bank": [math.radians(-195.94), math.radians(-116.06), math.radians(100.78), math.radians(-180.28), math.radians(-74.41), math.radians(94.49-180)],
				"cable_preparation": [math.radians(25), math.radians(-54.82), math.radians(83.03), math.radians(-118.29), math.radians(90), math.radians(-38.83+180)]
			},
			"speedValueNormal":200,
			"speedValueSlow":80,
			"speedValueReallySlow":20,
			"disableServoOnFinish":False
		},

		"gripper":{
			"EXPECTED_STARTUP_MESSAGE":"I am the transmitter",
			"TCP_A": (makeTransform([0.184766,0,0.141915,math.radians(45),0,math.radians(90)])*makeTransform([0.003-0.89/1000+0.0025,-0.006+0.24/1000,6.875/1000])).tolist(),
			# "TCP_A": (makeTransform( [0,0,0, math.radians(45),0,math.radians(-270)] ) * makeTransform([0.002,-30/1000,231/1000])).tolist(),
			"TCP_B": (makeTransform( [0,0,0, math.radians(-45),0,math.radians(90)] ) * makeTransform([0,0,0,0,0,math.radians(180)]) * makeTransform([-2/1000,19/1000,261/1000]) * makeTransform([0,0.001,0.006]) ).tolist(),
			"READY_MESSAGE":"ready",
			"SUCCESS_MESSAGE":"success",
			"min_servo_value":900,
			"max_servo_value":2100,
			"A_open_position":2100,
			"A_closed_position":1220,
			"B_open_position":2100,
			"B_closed_position":1350
		},

		# Defines properties of each organ type
		"dictionaryOfOrganTypes":{
			0: {
				"organType": 0,
				"friendlyName": "Core organ",
				"forceModeTravelDistance": 20 / 1000,
				"postInsertExtraPushDistance": 9.0 / 1000, # positive for extra pushing when attaching organ to skeleton
				"pickupExtraPushDistance": -5.0 / 1000, # offset in organ frame z direction just when picking up organ
				"transformOrganOriginToGripper": np.linalg.inv(makeTransform ( [ 0, 0, 70.8 / 1000, 0, math.radians ( 180 ), 0 ] ) ).tolist(),
				"transformOrganOriginToClipCentre": makeTransform().tolist(),
				"transformOrganOriginToCableSocket": [
					(makeTransform([ -22.93/1000, 23.45/1000 , 62.77/1000 , 0,0,math.radians(-90)])).tolist (),
					(makeTransform([ -22.93/1000, 12.2/1000 , 62.77/1000 , 0,0,math.radians(-90)])).tolist (),
					(makeTransform([ -22.93/1000, -16.2/1000 , 62.77/1000 , 0,0,math.radians(-90)])).tolist (),
					(makeTransform([ -22.93/1000, -28.3/1000 , 62.77/1000 , 0,0,math.radians(-90)])).tolist ()
				],
				"gripperOpeningFraction": 0.3,
				"gripperClosedFraction": 0.9,
				"USE_FORCE_MODE":False
			},
			1: { "organType": 1,
				 "friendlyName": "Wheel organ",
				 "forceModeTravelDistance": 4 / 1000,
				 "postInsertExtraPushDistance": 4.0 / 1000, #5
				"pickupExtraPushDistance": 0 / 1000, # positive value will make it hold further onto wheel organ
				 # "gripPosition": makeTransform ([ 5.89 / 1000, 29.06 / 1000, 15.37 / 1000, 0, math.radians ( 180 ), math.radians ( 90 ) ] ).tolist(),
				"transformOrganOriginToGripper": makeTransform([6.8/1000,0,-2.858/1000,0,0,math.radians(-90)]).tolist(),
				# "transformOrganOriginToClipCentre": makeTransform([0,-5.9/1000,5.12/1000 , 0, 0,0]).tolist(),
				"transformOrganOriginToClipCentre": makeTransform([0,0,5.12/1000 , math.radians(180), 0,math.radians(90)]).tolist(),
				"transformOrganOriginToCableSocket": [
					makeTransform ([-5.6/1000,10.4/1000,-20.45/1000,0,math.radians(90),math.radians(90)]).tolist (),
					makeTransform ([13.35/1000,-10.75/1000,20/4/1000,0,math.radians(-90),math.radians(90)]).tolist ()
													  ],
				"gripperOpeningFraction": 0.2,
				"gripperClosedFraction": 0.6,
				"USE_FORCE_MODE":False
				 },
			2: {"organType": 2,
				"friendlyName": "Sensor organ",
				"forceModeTravelDistance": 5 / 1000,
				"postInsertExtraPushDistance": 5.0 / 1000, #2
				"pickupExtraPushDistance": 0 / 1000,
				 # "gripPosition": makeTransform ([ 5.89 / 1000, 29.06 / 1000, 15.37 / 1000, 0, math.radians ( 180 ), math.radians ( 90 ) ] ).tolist(),
				"transformOrganOriginToGripper": (makeTransform([ 0,0, 21.1/1000 , 0,0,0 ])).tolist(),
				"transformOrganOriginToClipCentre": makeTransform([ 0,0,5.123/1000 , math.radians(180),0,math.radians(90) ]).tolist(),
				"transformOrganOriginToCableSocket": [makeTransform([5.55/1000 , 26.1/1000 , -4.7/1000 , math.radians(90),0,math.radians(180)]).tolist ()],
				 "gripperOpeningFraction": 0.0,
				"gripperClosedFraction": 0.2,
				"USE_FORCE_MODE":False
				 }
		},

		"ASSEMBLY_FIXTURE":{
			"ARDUINO_SUCCESS_MESSAGE":"OK",
			"TIMEOUT_SECONDS":60,
			"WAIT_FOR_OK_REPLY":False,
			"ORIGIN":ASSEMBLY_FIXTURE_ORIGIN,
			"FIND_HOME_AT_STARTUP":True,
			"HOME_BETWEEN_EVERY_MOVE":False,
			"CORE_ORGAN_ATTACHMENT_Z_OFFSET": 2.0/1000,
			"EXPECTED_STARTUP_MESSAGE":"I am the Assembly Fixture"
		},

		"PRINTER_1":{
		# "ORIGIN":makeTransform( [ 0.487 , -0.043, 0.083 , 0 , 0 , math.radians( -90 ) ] ) .tolist(),
		"ORIGIN":PRINTER_1_ORIGIN,
		# "skeletonPositionOnPrintbed":makeTransform([0.1489,0.150,0]).tolist()
		"skeletonPositionOnPrintbed":makeTransform([0.1575,0.168,0]).tolist()
		},

		"CABLE_BANK_1":{
		"ORIGIN": CABLE_BANK_1_ORIGIN,
		"CABLE_GRIP_POINT":  makeTransform ( [ 0, 0, 0.0055, -math.radians(180), 0,math.radians(-180) ] ).tolist(),
		"CABLE_GRIPPER_OPEN_POWER":  0.65,
		"CABLE_CONTENTS":cable_bank_cables_temp,
		"ORGAN_CONTENTS":[] # no organs
		},

		"ORGAN_BANK_1":{
		"ORIGIN":ORGAN_BANK_1_ORIGIN,
		"ORGAN_CONTENTS":[ # a list of lists, each sub-list represents an organ, in the same format as a blueprint row: [0 , <organ type> , <x (mm)> , <y (mm)> , <z (mm)> , <rx (radians)> , <ry (radians)> , <rz (radians)>]
			[0,0, (204.06 -2 ) / 1000, 286.06 / 1000, 1.5 / 1000, 0, 0, 0 ], # Core organ
			[0,1, 71.16 / 1000, 81.06 / 1000, 9 / 1000, 0, math.radians ( 180 ) , 0 ], # wheel 1
			[0,1, 71.16 / 1000, (81.06+82) / 1000, 9 / 1000, 0, math.radians ( 180 ) , 0 ], # wheel 2
			[0,2, (163.06-2) / 1000, 81.06 / 1000, 6 / 1000, 0, math.radians ( 180 ) , 0 ], # sensor 1
			[0,2, (163.06+82-2) / 1000, 81.06 / 1000, 6 / 1000, 0, math.radians ( 180 ) , 0 ] # sensor 2
		],
		"CABLE_CONTENTS":[] # no cables
		}
	}
	filename = "configuration_" + data["location"] + ".json"
	with open(filename, "w") as write_file:
		json.dump(data, write_file , indent=4)


if __name__ == "__main__":
	makeFile()