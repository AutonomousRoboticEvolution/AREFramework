/**
	@file BatteryMonitor.hpp
	@brief Header and documentation for BatteryMonitor.cpp, for the DS2438Z+ battery monitor chip
	@author Mike Angus
*/

#ifndef BATTERY_MONITOR_HPP
#define BATTERY_MONITOR_HPP

//Includes for direct file-based 1-wire access (for testing)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

//Other includes
#include <wiringSerial.h>

class BatteryMonitor {

	public :
		BatteryMonitor();
		void direct1WireTest();
};

#endif