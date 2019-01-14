#pragma once

#include "v_repLib.h"
#include "../morphology/Modular_Morphology.h"
#include <vector>

class VREPUI
{
public:
	VREPUI();
	~VREPUI();

	int nNUIHandle;
	int morphologyUIHandle;
	int createNNUI(vector<shared_ptr<ER_Module>> createdModules);
	int createMorphUI(vector<shared_ptr<ER_Module>> createdModules);
	void deleteNNUI();
	void deleteMorphUI();
};