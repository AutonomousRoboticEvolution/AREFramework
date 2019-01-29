#include "ER_Module.h"



ER_Module::ER_Module()
{
}


ER_Module::~ER_Module()
{
	siteConfigurations.clear();
	controlHandles.clear();
	objectHandles.clear();
	attachHandles.clear();
	attachRefHandles.clear();
}

void ER_Module::colorModule(float color[3], float alpha) {
	float newColor[3] = { color[0] * alpha, color[1] * alpha, color[2] * alpha };
	//	cout << "Color = " << color[0] << ", " << color[1] << ", " << color[2] << endl;
	for (int i = 0; i < 3; i++) {
		//	if (energy > 1.0) {
		//		newColor[i] = color[i] * 1.0;// +(energy - 1);
		//	}
		//	else {
		//		newColor[i] = color[i] * energy;
		//	}
		//	newColor[i] = alpha;// color[i] * alpha;
		if (newColor[i] < 0.0) {
			newColor[i] = 0.0;
		}
		if (newColor[i] > 1.0) {
			newColor[i] = 1.0;
		}
	//	cout << "Color = " << newColor[0] << ", " << newColor[1] << ", " << newColor[2] << ", a: " << alpha << endl;
	}
//	cout << " setting colors" << endl;
	for (int i = 0; i < objectHandles.size(); i++) {
		simSetShapeColor(objectHandles[i], NULL, sim_colorcomponent_ambient_diffuse, newColor);
	}
}

void ER_Module::setControlParams(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#ControlType") {
			it++;
			tmp = values[it];
			int controlType = atoi(tmp.c_str());
			if (controlType != -1) {
				unique_ptr<ControlFactory> controlFactory(new ControlFactory);
				control = controlFactory->createNewControlGenome(controlType, randomNum, settings); // 0 is ANN, 1 is Custom_ANN
				control->init(settings->initialInputNeurons, settings->initialInterNeurons, settings->initialOutputNeurons);
				control->setControlParams(values);
				controlFactory.reset();
			}
		}
	}
}

bool ER_Module::checkControlParams(vector<string> values) {
	if (control) {
		bool checkControlOutput = control->checkControl(values);
		return checkControlOutput;
	}
}




vector<int> ER_Module::getMirrorSite(int site, int configuration, int mirrorAxis) {
	vector<int> mirrored; 
	if (mirrorAxis == 0) { // standard mirror
		switch (site) {
		case 1:
			mirrored.push_back(3);
			break;
		case 2:
			mirrored.push_back(4);
			break;
		case 3:
			mirrored.push_back(1);
			break;
		case 4:
			mirrored.push_back(2);
			break;
		default:
			mirrored.push_back(0);
			break;
		}
		switch (configuration) {
		case 0:
			mirrored.push_back(2);
			break;
		case 1:
			mirrored.push_back(3);
			break;
		case 2:
			mirrored.push_back(0);
			break;
		case 3:
			mirrored.push_back(1);
			break;
		default:
			mirrored.push_back(configuration);
			break;
		}
	}	
	else if (mirrorAxis == 1) { // 
		switch (site) {
		case 1:
			mirrored.push_back(4);
			break;
		case 2:
			mirrored.push_back(1);
			break;
		case 3:
			mirrored.push_back(2);
			break;
		case 4:
			mirrored.push_back(3);
			break;
		default:
			mirrored.push_back(0);
			break;
		}
		switch (configuration) {
		case 0:
			mirrored.push_back(2);
			break;
		case 1:
			mirrored.push_back(3);
			break;
		case 2:
			mirrored.push_back(0);
			break;
		case 3:
			mirrored.push_back(1);
			break;
		default:
			mirrored.push_back(configuration);
			break;
		}
	}
	else if (mirrorAxis == 2) { // 
		switch (site) {
		case 1:
			mirrored.push_back(2);
			break;
		case 2:
			mirrored.push_back(3);
			break;
		case 3:
			mirrored.push_back(4);
			break;
		case 4:
			mirrored.push_back(1);
			break;
		default:
			mirrored.push_back(0);
			break;
		}
		switch (configuration) {
		case 0:
			mirrored.push_back(2);
			break;
		case 1:
			mirrored.push_back(3);
			break;
		case 2:
			mirrored.push_back(0);
			break;
		case 3:
			mirrored.push_back(1);
			break;
		default:
			mirrored.push_back(configuration);
			break;
		}
	}
	if (mirrored.size() != 2) {
		cerr << "ERROR: Mirror mutation not be done properly" << endl;
	}
	return mirrored;
}



vector<float> ER_Module::updateModule(vector<float> input) {
//	energy = energy - energyDissipationRate;
//	diffuseEnergy();
//	cout << "moduleColor = " << moduleColor[0] << ", " << moduleColor[1] << ", " << moduleColor[2] << endl;
	if (settings->colorization == settings->COLOR_NEURALNETWORK) {
		float newModColor[3] = { 0.5,0.5,0.5 };
		colorModule(newModColor, control->cf);
	}
	else {
		colorModule(moduleColor, control->cf);
	}
//	vector<float> output;
//	if (control) {
//		output = control->update(input);
//	}
//	cout << "energy = " << energy << endl;
	if (control) {
		addInput(input);
	}
	if (settings->environmentType == settings->SUN_BASIC
		|| settings->environmentType == settings->SUN_CONSTRAINED
		|| settings->environmentType == settings->SUN_CONSTRAINED_AND_MOVING
		|| settings->environmentType == settings->SUN_MOVING
		|| settings->environmentType == settings->SUN_BLOCKED) {
//		cout << "direct diffusing" << endl;
		directDiffuseEnergy();
	}
	vector<float> output = input;

	if (broken == false) {
		output = control->update(input);
		for (int i = 0; i < output.size(); i++) {
			if (output[i] > 1.0) {
				output[i] = 1.0;
			}
			else if (output[i] < -1.0) {
				output[i] = -1.0;
			}
		}
	}
	return output;
}

void ER_Module::addEnergy(float addedEnergy) {
	energy += addedEnergy;
}

void ER_Module::addInput(vector<float> input) {
	if (control) {
		control->addInput(input);
	}
}

void ER_Module::diffuseEnergy() {
	
	if (parentModulePointer != NULL) {
		float diffEn = parentModulePointer->energy - energy;
		if (diffEn < 0) {
			parentModulePointer->energy += -(diffEn / 2);
			energy += (diffEn / 2);
		}
		else if (diffEn == 0) {
		}
		else {
			parentModulePointer->energy += -(diffEn / 2);
			energy += (diffEn / 2);
		}
	}
}

void ER_Module::directDiffuseEnergy() {
	if (parentModulePointer != NULL) {
//		cout << "energy = " << energy << endl;
		parentModulePointer->energy += energy;
//		cout << "parentEnergy = " << parentModulePointer->energy << endl;
		energy = 0;
	}
}

void ER_Module::setModuleID(int id) {
	moduleID = id;
}
