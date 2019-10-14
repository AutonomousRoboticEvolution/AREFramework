#pragma once

#include <vector>
#include <memory>
#include "v_repLib.h"
#include "../module/ER_Module.h"


class VREPUI
{
public:
	VREPUI();
	~VREPUI();

	int nNUIHandle;
	int morphologyUIHandle;
	int createNNUI(std::vector<std::shared_ptr<ER_Module>> createdModules);
	int createMorphUI(std::vector<std::shared_ptr<ER_Module>> createdModules);
	void deleteNNUI();
	void deleteMorphUI();
};
