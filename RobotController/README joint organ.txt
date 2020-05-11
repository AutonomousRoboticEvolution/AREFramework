Author: Matt
The joint driver code is very heavily based off the motor driver written by Mike.

Registers:
0x10: TARGET_POSTIION 8 bit value where 0 is 0 degrees, 255 is 180 degrees (set-able)
0x11: JOINT_MODE 8 bit value, but only the last bit is used at the moment, all others should be zero (set-able):
	0 is turn the servo off (so it moves freely)
	1 is turn the servo on (so it ties to move to the target position set by 0x10)
0x12: MEASURED_POSITION current joint position, as measured by the potentiometer (read-able, unsigned 8-bit)