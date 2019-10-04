#pragma once

#include <iostream>
#include <string>
#include <fstream> // ifstream
#include <list>
#include <vector>
#include "Settings.h"


class SettingsERClient : public Settings // This settings class does not contain any dynamixel dependencies. Used for cluster evolution
{
public:
	SettingsERClient();
	virtual ~SettingsERClient();
};