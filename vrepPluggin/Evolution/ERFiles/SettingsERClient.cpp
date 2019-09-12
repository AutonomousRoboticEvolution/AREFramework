#pragma once
#include "SettingsERClient.h"

SettingsERClient::SettingsERClient() {
	// set default module parameters
	moduleTypes.push_back(1);
	moduleTypes.push_back(4);
	moduleTypes.push_back(4);
	moduleTypes.push_back(1);
	moduleTypes.push_back(1);

	for (int i = 0; i < moduleTypes.size(); i++) {
		vector <int> tmpMaxModuleTypes;
		tmpMaxModuleTypes.push_back(moduleTypes[i]);
		tmpMaxModuleTypes.push_back(50);
		maxModuleTypes.push_back(tmpMaxModuleTypes);
	}
	maxModuleTypes[0][1] = 50; // one base module
}

SettingsERClient::~SettingsERClient() {

}

