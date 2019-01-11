#include "VREPUI.h"

VREPUI::VREPUI()
{
}

VREPUI::~VREPUI()
{
}

int VREPUI::createNNUI(vector<shared_ptr<ER_Module>> createdModules)
{
	int buttonHandles[3] = {1, 2, 3};
	int uiSize[2] = { 7,15 };
	int cellSize[2] = { 24, 24 };
	int uiHandle = simCreateUI("NetworkUI", sim_ui_menu_title, uiSize, cellSize, buttonHandles);
	//button1
	int button1pos[2] = { 0,1 };
	int button1Size[2] = { 10,2 };
	vector<int> uiSliders;
	uiSliders.push_back(simCreateUIButton(uiHandle, button1pos, button1Size, sim_buttonproperty_slider));
	//button2
	int button2pos[2] = { 0,3 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button2pos, button1Size, sim_buttonproperty_slider | sim_buttonproperty_enabled));
	//button3
	int button3pos[2] = { 0,5 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button3pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button4
	int button4pos[2] = { 0,7 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button4pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button5
	int button5pos[2] = { 0,9 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button5pos, button1Size, sim_buttonproperty_enabled | sim_buttonproperty_slider));
	//button6
	int button6pos[2] = { 0,11 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button6pos, button1Size, sim_buttonproperty_slider));
	//button7
	int button7pos[2] = { 0,13 };
	uiSliders.push_back(simCreateUIButton(uiHandle, button7pos, button1Size, sim_buttonproperty_slider));

	return uiHandle;
}

int VREPUI::createMorphUI(vector<shared_ptr<ER_Module>> createdModules)
{
	int uiSizeY = createdModules.size();
	float black[3] = { -250,-250,-250 };
	float white[3] = { 250,250,250 };

	int buttonHandles[3] = { 1, 2, 3 };
	int uiSize[2] = { 25,uiSizeY };
	int cellSize[2] = { 24, 24 };
	int uiHandle = simCreateUI("NetworkUI", sim_ui_menu_title, uiSize, cellSize, buttonHandles);
	// Module 1 is attached to module ... in orientation ...
	for (int i = 0; i < uiSizeY; i++) {
		int buttonPos[2] = { 0, i + 1 };
		int buttonSize[2] = { 25,1 };
		int labelHandle = simCreateUIButton(uiHandle, buttonPos, buttonSize, sim_buttonproperty_label);
		string text = "Module " + to_string(i) + " is connected to module " + 
			to_string(createdModules[i]->parent) + " at site " + to_string(createdModules[i]->parentSite)
			+ " in orientation " + to_string(createdModules[i]->orientation)
			+ ", ID: " + to_string(createdModules[i]->moduleID);

		simSetUIButtonLabel(uiHandle, labelHandle, text.c_str(), "hello");
		simSetUIButtonColor(uiHandle, labelHandle, NULL, NULL, white);
	}
	return uiHandle;
}

void VREPUI::deleteNNUI()
{
	simRemoveUI(nNUIHandle);
}

void VREPUI::deleteMorphUI()
{
	simRemoveUI(morphologyUIHandle);
}
