#pragma once
#ifndef SETTINGSERCLIENT_H
#define SETTINGSERCLIENT_H

#include <iostream>
#include <string>
#include <fstream> // ifstream
#include <list>
#include <vector>
#include "ARE/Settings.h"


class SettingsERClient : public Settings // This settings class does not contain any dynamixel dependencies. Used for cluster evolution
{
public:
	SettingsERClient();
	virtual ~SettingsERClient();
};

#endif //SETTINGSERCLIENT_H
