#include "ER_CPPN_Interpreter.h"
#include <iostream>
#include <cmath>


ER_CPPN_Interpreter::ER_CPPN_Interpreter()
{
	modular = true;
}

ER_CPPN_Interpreter::~ER_CPPN_Interpreter()
{
	createdModules.clear();
}

void ER_CPPN_Interpreter::createAtPosition(float x, float y, float z) {
	cout << "x, y, z: " << x << ", " << y << ", " << z << endl; 
	float position[3];
	setColors();
	position[0] = x;
	position[1] = y;
	position[2] = z;
	initializeCPPNEncoding(position); // / amount increment is not in genome anymore
}

bool ER_CPPN_Interpreter::checkJointModule() {
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->type == 4) {
			return true;
		}
	}
	simStopSimulation();
	return false;
}

void ER_CPPN_Interpreter::printSome() {
	BaseMorphology::printSome();
	cout << "printing some from ER_CPPN_Interpreter" << endl;
}

bool ER_CPPN_Interpreter::checkLCollisions(shared_ptr<ER_Module> module, vector<int> exceptionHandles) {
	bool collision = true;
//	cout << "objectHandles.size = " << module->objectHandles.size() << endl;
//	cout << "createdModules.size = " << createdModules.size()  << endl;
	for (int n = 0; n < module->objectHandles.size(); n++) {
		if (simGetObjectType(module->objectHandles[n]) == sim_object_shape_type) {
			for (int i = 0; i < createdModules.size() - 1; i++) {
				for (int j = 0; j < createdModules[i]->objectHandles.size(); j++) {
					//	cout << "mh: " << module->objectHandles[n] << ", cr: " << createdModules[i]->objectHandles[j] << endl;
					if (simCheckCollision(module->objectHandles[n], createdModules[i]->objectHandles[j]) == true) {
						//	cout << "mh: " << module->objectHandles[n] << ", cr: " << createdModules[i]->objectHandles[j] << endl;
						for (int k = 0; k < exceptionHandles.size(); k++) {
							//	cout << "mh: " << module->objectHandles[n] << ", cr: " << createdModules[i]->objectHandles[j] << " and eh: " << exceptionHandles[k] << endl;
							if (createdModules[i]->objectHandles[j] != exceptionHandles[k]) {
								return true;
							}
						}
					}
				}
			}
			// check collision with environment
//			cout << "objectHandles.size() = " << environment->envObjectHandles.size() << endl;
			for (int i = 0; i < settings->envObjectHandles.size(); i++) {
				if (module->objectHandles[n] != settings->envObjectHandles[i]) {
					for (int k = 0; k < exceptionHandles.size(); k++) {
						//	cout << "mh: " << module->objectHandles[n] << ", cr: " << createdModules[i]->objectHandles[j] << " and eh: " << exceptionHandles[k] << endl;
						if (simCheckCollision(module->objectHandles[n], settings->envObjectHandles[i]) == true) {
							if (createdModules[i]->objectHandles[n] != exceptionHandles[k]) {
								return true;
							}
						}
					}
					//if (simCheckCollision(module->objectHandles[n], environment->envObjectHandles[i]) == true) {
					//	cout << "object collided with object from environment" << endl;
					//	return true;
					//}
				}
			}
			// check to see if positions are not the same
			float handleNPos[3];
			float handleIJPos[3];
			simGetObjectPosition(module->objectHandles[n], -1, handleNPos);
			for (int i = 0; i < createdModules.size() - 1; i++) {
				for (int j = 0; j < createdModules[i]->objectHandles.size(); j++) {
					simGetObjectPosition(createdModules[i]->objectHandles[j], -1, handleIJPos);
					if (handleNPos[0] > handleIJPos[0] - 0.001 && handleNPos[0] < handleIJPos[0] + 0.001
						&& handleNPos[1] > handleIJPos[1] - 0.001 && handleNPos[1] < handleIJPos[1] + 0.001
						&& handleNPos[2] > handleIJPos[2] - 0.001 && handleNPos[2] < handleIJPos[2] + 0.001) {
						//		cout << "modules are at the same position" << endl; 
						return true;
					}
				}
			}

			// checks if collition with floor happens. Is replaced with setting the robot position higher depending on the lowest coordinate + 0.0001
			// don't delete this function!
			if (createdModules[0]->type == 8) {
				if (checkCollisionBasedOnRotatedPoints(module->objectHandles[n]) == true) {
					return true;
				}
			}
		}
	}
	return false; 
}

void ER_CPPN_Interpreter::checkForceSensors() {
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->broken != true) {
			for (int j = 0; j < createdModules[i]->objectHandles.size(); j++) {
				if (simGetObjectType(createdModules[i]->objectHandles[j]) == sim_object_forcesensor_type) {
					int force = simReadForceSensor(createdModules[i]->objectHandles[j], NULL, NULL);
					if (force != 0 && force != -1) {
						if (force == 3) {
							cout << "force sensor is broken" << endl;
							cout << "module " << i << " is broken" << endl;
							createdModules[i]->broken = true;
						}
					}
				}
			}
		}
	}
}



bool ER_CPPN_Interpreter::loadGenome(std::istream &input, int individualNumber)
{
	bool load = ER_CPPN_Encoding::loadGenome(input, individualNumber);
	cout << "ADJUSTING CPPN GENOME" << endl;
	unique_ptr<ModuleFactory> mf = unique_ptr<ModuleFactory>(new ModuleFactory);
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		shared_ptr<ER_Module> mod = mf->createModuleGenome(genome->moduleParameters[i]->type,randomNum,settings);
		mod->state = i; 
//		mod->type = settings->moduleTypes[i];
		mod->control = genome->moduleParameters[i]->control;
		modules.push_back(mod);
	}
	cout << "Loaded all modules" << endl;
	mf.reset();
	return load;
}

int ER_CPPN_Interpreter::initializeCPPNEncoding(float initialPosition[3]) {
//	simSetInt32Parameter(sim_intparam_dynamic_step_divider, 25);
	if (settings->verbose) {
		cout << "initializing CPPN encoding interpreter" << endl;
	}
	createdModules.clear();
	for (int i = 1; i < genome->moduleParameters.size(); i++) {
		genome->moduleParameters[i]->expressed = false;
	}
	createdModules.push_back(moduleFactory->copyModuleGenome(modules[0]));
	int parentHandle = -1;
	vector<float> configuration;
	configuration.resize(6);
	configuration[0] = initialPosition[0];
	configuration[1] = initialPosition[1];
	configuration[2] = initialPosition[2];
	configuration[3] = 0;
	configuration[4] = 0;
	configuration[5] = 0;
	createdModules[0]->createModule(configuration, -1, parentHandle);

	for (int i = 1; i < modules.size(); i++) {
		if (modules[i]->parent == 0) {
			modules[i]->parentModulePointer = createdModules[0];
		}
	}
	for (int i = 0; i < modules.size(); i++) {
		modules[i]->moduleID = i;
	}
	//cout << "looping through modules" << endl;
	for (int i = 1; i < modules.size(); i++) {
		int parentNr = genome->moduleParameters[i]->parent;
		int parentSite = genome->moduleParameters[i]->parentSite;
		int orien = genome->moduleParameters[i]->orientation;
		int createdParentNumber = -1;
		for (int n = 0; n < createdModules.size(); n++)
		{
			if (createdModules[n]->moduleID == parentNr) {
				createdParentNumber = n;
			}
		}
		if (createdModules.size() < settings->maxAmountModules) {
			if (modules[i]->parentModulePointer != NULL && createdParentNumber > -1 && createdModules[createdParentNumber] != NULL) {
				if (genome->moduleParameters[parentNr]->expressed == true) {
					if (createdModules[createdParentNumber]->siteConfigurations.size() == 0) {
					//	cout << "state " << createdModules[createdParentNumber]->state << ",";
					//	cout << "id " << createdModules[createdParentNumber]->moduleID << ",";
						//	cout << "id " << createdModules[parentNr]-> << endl;
					}
					// parentHandle = createdModules[i]->parentModulePointer->;
					//cout << "should create module : " << i << ",";
					shared_ptr<ER_Module> parentModulePointer = modules[i]->parentModulePointer;
					if (parentModulePointer == NULL) {
						cout << ", null , ";
					}
					//cout << "parentModulePointer = " << parentModulePointer << ", ";
					//cout << "parentSite = " << parentSite << ", ";
					//cout << " .. " << parentModulePointer->moduleID << ",";
					//cout << " pType " << parentModulePointer->type << ",";

					parentHandle = parentModulePointer->siteConfigurations[parentSite][0]->parentHandle;
					int relativePositionHandle = parentModulePointer->siteConfigurations[parentSite][0]->relativePosHandle;
					//cout << " 1 ,";
					createdModules.push_back(moduleFactory->copyModuleGenome(modules[i]));
					createdModules[createdModules.size() - 1]->parent = createdParentNumber;
					//createdModules[createdModules.size() - 1]->parentSite = parentSite;
					//cout << " 1.1, ";
					int createdModulesSize = createdModules.size();
					vector<float> siteConfiguration;
					//cout << "pnr:" << createdParentNumber << ", cMs: " << createdModules.size() << ", or " << orien << ",";
					//cout << createdModules[createdParentNumber]->type << ",";
					//cout << "sc.size: " << createdModules[createdParentNumber]->siteConfigurations.size() << ",";
					//cout << "orien = " << orien << ",";
					//cout << "parentSite = " << parentSite << ",";

					//if (createdModules[createdParentNumber]->siteConfigurations.size() < 1) {
					//	cout << "ERROR" << endl;
					//}
					siteConfiguration.push_back(createdModules[createdParentNumber]->siteConfigurations[parentSite][orien]->x);
					siteConfiguration.push_back(createdModules[createdParentNumber]->siteConfigurations[parentSite][orien]->y);
					siteConfiguration.push_back(createdModules[createdParentNumber]->siteConfigurations[parentSite][orien]->z);
					siteConfiguration.push_back(createdModules[createdParentNumber]->siteConfigurations[parentSite][orien]->rX);
					siteConfiguration.push_back(createdModules[createdParentNumber]->siteConfigurations[parentSite][orien]->rY);
					siteConfiguration.push_back(createdModules[createdParentNumber]->siteConfigurations[parentSite][orien]->rZ);
					//cout << " a ,";
					createdModules[createdModulesSize - 1]->createModule(siteConfiguration, relativePositionHandle, parentHandle);
					createdModulesSize = createdModules.size();
					//cout << " b ,";

					// set color
					// createdModules[createdModulesSize - 1]->colorModule(genome->moduleParameters[i]->color, 1.0);

					vector<int> exception;
					exception.push_back(parentHandle);
					for (int p = 0; p < createdModules[createdModulesSize - 1]->objectHandles.size(); p++) {
						exception.push_back(createdModules[createdModulesSize - 1]->objectHandles[p]);
					}

					if (checkLCollisions(createdModules[createdModulesSize - 1], exception) == true) {
						createdModules.erase(createdModules.begin() + (createdModulesSize - 1));
						//		genome->moduleParameters[i]->expressed = false;
					}
					else {
						for (int n = 0; n < modules.size(); n++) {
							if (modules[n]->parent == i) {
								modules[n]->parentModulePointer = createdModules[createdModulesSize - 1];
							}
						}
						genome->moduleParameters[i]->expressed = true;
					}
					//cout << "created Module" << endl;
				}
			}
		}
		else {
			for (int j = 0; j < genome->moduleParameters.size(); j++) {
			//	cout << "pi: " << genome->moduleParameters[j]->parent << endl;
			}
			// cout << "ERROR: " << "No parent Module Pointer or module not actually created" << endl;
		} 
	}
	if (settings->verbose) {
		cout << "shifting robot position" << endl;
	}
	if (createdModules[0]->type != 8) {
		shiftRobotPosition();
	}
//	float pos[3];
//	simGetObjectPosition(createdModules[0]->objectHandles[0], -1, pos);
//	cout << pos[0] << "," << pos[1] << "," << pos[2] << endl;
//	update();
	return 1;
}

float ER_CPPN_Interpreter::getFitness() {
//	environment->fitnessFunction(*this) = 0;
	return fitness;
}

void ER_CPPN_Interpreter::init_noMorph() {

}


void ER_CPPN_Interpreter::setPhenValue() {
	float val = 0.0;
	for (int i = 0; i < createdModules.size(); i++) {
		float pos[3];
	//	cout << "phenValNOW = " << createdModules[i]->phenV << endl;
		val += createdModules[i]->phenV;
//		simGetObjectPosition(createdModules[i]->phenV), -1, pos);
//		val += pos[0];
//		val += pos[1];
//		val += pos[2];
	//	cout << "val: " << val <<  endl;
	}
	phenValue = val; 
}

void ER_CPPN_Interpreter::init() { 
	if (ER_CPPN_Encoding::genome == NULL) {
		// only initialize when this hasn't been done already
		ER_CPPN_Encoding::init();
	}
	// from the CPPN create a robot
	if (settings->verbose) {
		cout << "creating CPPN robot" << endl;
	}
	unique_ptr<ModuleFactory> mf = unique_ptr<ModuleFactory>(new ModuleFactory);
	unique_ptr<ControlFactory> cf = unique_ptr<ControlFactory>(new ControlFactory);
	genome->moduleParameters.clear();
	int moduleAmount = settings->moduleTypes.size();
	// create axiom module
	genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
	genome->moduleParameters[0]->type = settings->initialModuleType;
	genome->moduleParameters[0]->maxChilds = 5;
	genome->moduleParameters[0]->parent = -1;
	genome->moduleParameters[0]->parentSite = -1;
	genome->moduleParameters[0]->orientation = 0;
	// create axiom module control which is actually not used... 
	genome->moduleParameters[0]->control = cf->createNewControlGenome(settings->controlType, randomNum, settings);
	genome->moduleParameters[0]->control->init(1, 1, 1);
	if (settings->verbose) {
		cout << "quereuing cppn" << endl;
	}
	for (int i = 0; i < maxIterations; i++)
	{
		// query CPPN a few times. 
		if (settings->verbose) {
			cout << "query: " << i << endl;
		}
		int sizeIt = genome->moduleParameters.size();
		for (int n = 0; n < sizeIt; n++) {
			if (genome->moduleParameters[n]->queried == false) {
				genome->moduleParameters[n]->queried = true;
				for (int m = 0; m < genome->moduleParameters[n]->maxChilds; m++) {
					vector<float> inputs;
					inputs.push_back(genome->moduleParameters[n]->type / 10);
					if (genome->moduleParameters[n]->orientation < 0.5) {
						inputs.push_back(1);
						inputs.push_back(0);
						inputs.push_back(0);
						inputs.push_back(0);
					}
					else if (genome->moduleParameters[n]->orientation < 1.5) {
						inputs.push_back(0);
						inputs.push_back(1);
						inputs.push_back(0);
						inputs.push_back(0);
					}
					else if (genome->moduleParameters[n]->orientation < 2.5) {
						inputs.push_back(0);
						inputs.push_back(0);
						inputs.push_back(1);
						inputs.push_back(0);
					}
					else {
						inputs.push_back(0);
						inputs.push_back(0);
						inputs.push_back(0);
						inputs.push_back(1);
					}
					inputs.push_back(genome->moduleParameters[n]->orientation / 4);
					inputs.push_back(genome->moduleParameters[n]->parentSite / genome->moduleParameters[n]->maxChilds);
					inputs.push_back(i / maxIterations);
					//cout << "updating cppn" << endl;
					vector<float> moduleTypeFloat = cppn->update(inputs);
					//cout << "cppn updated" << endl;
					if (moduleTypeFloat[5] > 0.5) {
						// only create module if output is above certain threshold
						int typeM = (int)moduleTypeFloat[0] * (moduleAmount - 1);
						if (typeM < 0) {
							typeM = 0;
						}
						//cout << "typeM:  " << typeM << endl;
						int mt = settings->moduleTypes[typeM];
						genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
						genome->moduleParameters[genome->moduleParameters.size() - 1]->type = mt;
						//genome->moduleParameters[n]->childSiteStates[m] = settings->moduleTypes[(int)((moduleAmount + 1) * moduleTypeFloat[0]) - (1.0 / (moduleAmount + 1))];
						int mn = genome->moduleParameters.size() - 1;
						//cout << "GHerp: " << mn << endl;
						if (settings->moduleTypes[typeM] == 4 || settings->moduleTypes[typeM] == 9) {
							genome->moduleParameters[mn]->maxChilds = 3;
						}
						else if (settings->moduleTypes[typeM] == 6 || settings->moduleTypes[typeM] == 3) {
							genome->moduleParameters[mn]->maxChilds = 0;
						}
						else if (settings->moduleTypes[typeM] == 1) {
							genome->moduleParameters[mn]->maxChilds = 5;
						}
						else {
							genome->moduleParameters[mn]->maxChilds = 3;
						}
						//						genome->moduleParameters[mn]->currentState = n;
						genome->moduleParameters[mn]->parent = n;
						genome->moduleParameters[mn]->parentSite = m;
						int ori = (int)(floor(moduleTypeFloat[1] * 3.99));
						if (ori < 0) { 
							ori = 0; 
						}
						//cout << "ORI:" << ori << endl;
						genome->moduleParameters[mn]->orientation = ori;
						genome->moduleParameters[mn]->control = cf->createNewControlGenome(1, randomNum, settings);
						genome->moduleParameters[mn]->control->init(1, 1, 1);
						vector<float> controlValues;
						controlValues.push_back(moduleTypeFloat[2]);
						controlValues.push_back(moduleTypeFloat[3]);
						controlValues.push_back(moduleTypeFloat[4]);
						genome->moduleParameters[mn]->control->setFloatParameters(controlValues);
					}
				}
			}
		}

	}
	if (settings->verbose) {
		cout << "Done querying" << endl;
	}
	/*for (int i = 0; i < genome->moduleParameters.size(); i++) {
		modules.push_back(mf->createModuleGenome(genome->moduleParameters[i]->type, randomNum, settings));
		modules[i]->state = i;
		modules[i]->type = genome->moduleParameters[i]->type;
		modules[i]->control = genome->moduleParameters[i]->control;
		modules[i]->parent = genome->moduleParameters[i]->parent;
		modules[i]->parentSite = genome->moduleParameters[i]->parentSite;
		modules[i]->orientation = genome->moduleParameters[i]->orientation;
	}*/
	mf.reset();
	cf.reset();
	//initializeCPPNEncoding(positionFirstObject); 	
	//setPhenValue(); // temp test function
}

void ER_CPPN_Interpreter::initCustomMorphology() {
	initializeGenomeCustom(0);
	float position[3] = { 0, 0, 0.1 };
	initializeCPPNEncoding(position); // amount increment is not in genome anymore
}

shared_ptr<Morphology> ER_CPPN_Interpreter::clone() const {
	BaseMorphology::clone();
	// shouldn't clone this?
	shared_ptr<ER_CPPN_Interpreter> ur = make_unique<ER_CPPN_Interpreter>(*this);
	for (int i = 0; i < ur->genome->moduleParameters.size(); i++) {
		ur->genome->moduleParameters[i] = ur->genome->moduleParameters[i]->clone();
	}
	ur->genome = ur->genome->clone();
	return ur;
}

void ER_CPPN_Interpreter::update() {	
//	vector<float> input;
//	for (int i = 0; i < createdModules.size(); i++) {
//		createdModules[i]->updateModule(input);
//	}
//	checkForceSensors(); 
	vector<float> input;
	for (int i = 0; i < createdModules.size(); i++) {
		//float outputModule = 
		vector<float> moduleInput;
		if (settings->controlType == settings->ANN_CUSTOM) {
			createdModules[i]->updateModule(input);
			moduleInput.push_back(0.0);
		}
		if (settings->controlType == settings->ANN_DEFAULT) {
			createdModules[i]->updateModule(input);
			moduleInput.push_back(0.0);
		}
		if (settings->controlType == settings->ANN_DISTRIBUTED_UP
			|| settings->controlType == settings->ANN_DISTRIBUTED_DOWN
			|| settings->controlType == settings->ANN_DISTRIBUTED_BOTH) {
			moduleInput = createdModules[i]->updateModule(input);
		}
		if (settings->controlType == settings->ANN_DISTRIBUTED_UP) {
			if (createdModules[i]->parentModulePointer) { // if a parent pointer is stored
				int parent = createdModules[i]->parent;
				createdModules[i]->parentModulePointer->addInput(moduleInput);
				//	createdModules[i]->addInput(createdModules[parent]->moduleOutput);
			}
		}
		else if (settings->controlType == settings->ANN_DISTRIBUTED_DOWN) {
			if (createdModules[i]->parentModulePointer) { // if a parent pointer is stored
				int parent = createdModules[i]->parent;
				createdModules[i]->addInput(createdModules[i]->parentModulePointer->moduleOutput);
			}
		}
		else if (settings->controlType == settings->ANN_DISTRIBUTED_BOTH) {
			if (createdModules[i]->parentModulePointer) { // if a parent pointer is stored
				//int parent = createdModules[i]->parent;
				createdModules[i]->parentModulePointer->addInput(moduleInput);
				createdModules[i]->addInput(createdModules[i]->parentModulePointer->moduleOutput);
			}
		}

	}
//	updateColors();
	checkForceSensors();
}

void ER_CPPN_Interpreter::updateColors() {
	// not working for direct encoding yet
	for (int i = 0; i < createdModules.size(); i++) {
		float alpha = createdModules[0]->energy;
		if (alpha > 1.0) {
			alpha = 1.0;
		}
		else if (alpha < 0.4) {
			alpha = 0.4;
		}
		//	cout << "alpha = " << alpha << endl;
		createdModules[i]->colorModule(genome->moduleParameters[createdModules[i]->state]->color, alpha);
	}
}

void ER_CPPN_Interpreter::setColors() {
	for (int i = 0; i < genome->amountModules; i++) {
		float red[3] = { 1.0, 0, 0 };
		float blue[3] = { 0.0, 0.0, 1.0 };
		float yellow[3] = { 1.0, 1.0, 0.0 };
		float green[3] = { 0.0, 0.3, 0.0 };
		float orange[3] = { 1.0, 0.5, 0.0 };
		float orangePlus[3] = { 1.0, 0.6, 0.2 };
		float black[3] = { 0.0, 0.0, 0 };
		float grey[3] = { 0.45,0.45,0.45 };

		switch (i) {
		case 0:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = red[j];
			}
			break;
		case 1:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = blue[j];
			}
			break;
		case 2:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = yellow[j];
			}
			break;
		case 3:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = green[j];
			}
			break;
		case 4:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = orange[j];
			}
			break;
		case 5:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = orangePlus[j];
			}
			break;
		default:
			for (int j = 0; j < 3; j++) {
				genome->moduleParameters[i]->rgb[j] = grey[j];
			}
			break;
		}
	}
}

void ER_CPPN_Interpreter::create() {
	init();
//	cout << "creating" << endl;
	setColors();
//	cout << "color set" << endl;
	unique_ptr<ModuleFactory> mf = unique_ptr<ModuleFactory>(new ModuleFactory);
	modules.clear();
//	cout << "modules cleared" << endl;
	for (int i = 0; i < genome->moduleParameters.size(); i++) {
		modules.push_back(mf->createModuleGenome(genome->moduleParameters[i]->type, randomNum, settings));
		modules[i]->state = i;
		modules[i]->type = genome->moduleParameters[i]->type;
		modules[i]->control = genome->moduleParameters[i]->control;
		modules[i]->parent = genome->moduleParameters[i]->parent;
		modules[i]->parentSite = genome->moduleParameters[i]->parentSite;
		modules[i]->orientation = genome->moduleParameters[i]->orientation;
	}
	mf.reset();
	initializeCPPNEncoding(positionFirstObject); // amount increment is not in genome anymore
	checkJointModule(); // quits simulator when no joint found. 
}

int ER_CPPN_Interpreter::getMainHandle() {
	if (createdModules.size() > 0) {
		return createdModules[0]->objectHandles[1];
	}
	else {
		cout << "ERROR: No module could be created, check initial position of the first module. " << endl;
	}
}

void ER_CPPN_Interpreter::savePhenotype(int ind, float fitness)
{
	// change createdModules back to a moduleParameters array. 
	vector<shared_ptr<BASEMODULEPARAMETERS>> bmp;
	for (int i = 0; i < createdModules.size(); i++) {
		bmp.push_back(shared_ptr<BASEMODULEPARAMETERS>(new BASEMODULEPARAMETERS));
		bmp[i]->control = createdModules[i]->control; // no deep copy needed since created modules will be deleted
		bmp[i]->type = createdModules[i]->type;
		bmp[i]->parent = createdModules[i]->parent;
		bmp[i]->parentSite = createdModules[i]->parentSite;
		bmp[i]->orientation = createdModules[i]->orientation;
	}
	Development::savePhenotype(bmp, ind, fitness);
}

int ER_CPPN_Interpreter::getAmountBrokenModules() {
	int amountBrokenModules = 0;
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->broken == true) {
			amountBrokenModules++;
		}
	}
	return amountBrokenModules;
}

float ER_CPPN_Interpreter::checkArea(float interSection[3], float pps[4][3]) {

	float alphaD = sqrt(powf((pps[0][0] - pps[1][0]), 2) + powf((pps[0][1] - pps[1][1]), 2) + powf((pps[0][2] - pps[1][2]), 2));
	float betaD = sqrt(powf((pps[0][0] - pps[2][0]), 2) + powf((pps[0][1] - pps[2][1]), 2) + powf((pps[0][2] - pps[2][2]), 2));
	//	cout << "alpha and beta should be the same:" << endl;
	//	cout << "alphaD = " << alphaD << endl;
	//	cout << "betaD = " << betaD << endl;
	float areaX = alphaD * betaD;

	float tLine1 = sqrt(powf((pps[0][0] - interSection[0]), 2) + powf((pps[0][1] - interSection[1]), 2) + powf((pps[0][2] - interSection[2]), 2));
	float tLine2 = sqrt(powf((pps[1][0] - interSection[0]), 2) + powf((pps[1][1] - interSection[1]), 2) + powf((pps[1][2] - interSection[2]), 2));
	float tLine3 = sqrt(powf((pps[2][0] - interSection[0]), 2) + powf((pps[2][1] - interSection[1]), 2) + powf((pps[2][2] - interSection[2]), 2));
	float tLine4 = sqrt(powf((pps[3][0] - interSection[0]), 2) + powf((pps[3][1] - interSection[1]), 2) + powf((pps[3][2] - interSection[2]), 2));

	float area1 = 0.5 * tLine1 * tLine2;
	float area2 = 0.5 * tLine2 * tLine3;
	float area3 = 0.5 * tLine3 * tLine4;
	float area4 = 0.5 * tLine4 * tLine1;
	//	cout << "interSection Point = " << interSection[0] << ", " << interSection[1] << ", " << interSection[2] << endl;
	//	cout << "triangle areas are: " << area1 << ", " << area2 << ", " << area3 << ", " << area4 << endl;
	float areaBound = area1 + area2 + area3 + area4;

	//	cout << "areas are: " << endl;
	//	cout << "	areaX = " << areaX << endl;
	//	cout << "	areaBound =" << areaBound << endl;
	return (1 / areaX * areaBound);
}

void ER_CPPN_Interpreter::shiftRobotPosition() {
	float minimumObjectPos = 5.0;
	for (int i = 0; i < createdModules.size(); i++) {
		for (int n = 0; n < createdModules[i]->objectHandles.size(); n++) {
			if (simGetObjectType(createdModules[i]->objectHandles[n]) == sim_object_shape_type) {
				float objectOrigin[3];
				simGetObjectPosition(createdModules[i]->objectHandles[n], -1, objectOrigin);
				float size[3];
				float rotationOrigin[3] = { 0,0,0 };
				simGetObjectFloatParameter(createdModules[i]->objectHandles[n], 18, &size[0]);
				simGetObjectFloatParameter(createdModules[i]->objectHandles[n], 19, &size[1]);
				simGetObjectFloatParameter(createdModules[i]->objectHandles[n], 20, &size[2]);
				for (int i = 0; i < 3; i++) {
					size[i] = size[i] * 2;
				}

				vector<vector<float>> cubeVertex; // 8 points in 3d space
				vector<vector<float>> points;
				points.resize(8);

				float objectMatrix[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

				simGetObjectMatrix(createdModules[i]->objectHandles[n], -1, objectMatrix);
				//for (int i = 0; i < 12; i++) {
				//	cout << objectMatrix[i] << ", ";
				//} cout << endl;

				points[0].push_back(rotationOrigin[0] + (0.5 * size[0]));
				points[0].push_back(rotationOrigin[1] + (0.5 * size[1]));
				points[0].push_back(rotationOrigin[2] + (0.5 * size[2]));

				points[1].push_back(rotationOrigin[0] - (0.5 * size[0]));
				points[1].push_back(rotationOrigin[1] + (0.5 * size[1]));
				points[1].push_back(rotationOrigin[2] + (0.5 * size[2]));

				points[2].push_back(rotationOrigin[0] - (0.5 * size[0]));
				points[2].push_back(rotationOrigin[1] - (0.5 * size[1]));
				points[2].push_back(rotationOrigin[2] + (0.5 * size[2]));

				points[3].push_back(rotationOrigin[0] + (0.5 * size[0]));
				points[3].push_back(rotationOrigin[1] - (0.5 * size[1]));
				points[3].push_back(rotationOrigin[2] + (0.5 * size[2]));

				points[4].push_back(rotationOrigin[0] + (0.5 * size[0]));
				points[4].push_back(rotationOrigin[1] + (0.5 * size[1]));
				points[4].push_back(rotationOrigin[2] - (0.5 * size[2]));

				points[5].push_back(rotationOrigin[0] - (0.5 * size[0]));
				points[5].push_back(rotationOrigin[1] + (0.5 * size[1]));
				points[5].push_back(rotationOrigin[2] - (0.5 * size[2]));

				points[6].push_back(rotationOrigin[0] - (0.5 * size[0]));
				points[6].push_back(rotationOrigin[1] - (0.5 * size[1]));
				points[6].push_back(rotationOrigin[2] - (0.5 * size[2]));

				points[7].push_back(rotationOrigin[0] + (0.5 * size[0]));
				points[7].push_back(rotationOrigin[1] - (0.5 * size[1]));
				points[7].push_back(rotationOrigin[2] - (0.5 * size[2]));

				vector<vector<float>> rotatedPoints;
				rotatedPoints.resize(8);
				for (int i = 0; i < 8; i++) {
					rotatedPoints[i].push_back((points[i][0] * objectMatrix[0]) + (points[i][1] * objectMatrix[1]) + (points[i][2] * objectMatrix[2]));
					rotatedPoints[i].push_back((points[i][0] * objectMatrix[4]) + (points[i][1] * objectMatrix[5]) + (points[i][2] * objectMatrix[6]));
					rotatedPoints[i].push_back((points[i][0] * objectMatrix[8]) + (points[i][1] * objectMatrix[9]) + (points[i][2] * objectMatrix[10]));
					rotatedPoints[i][0] += objectOrigin[0];
					rotatedPoints[i][1] += objectOrigin[1];
					rotatedPoints[i][2] += objectOrigin[2];
					if (rotatedPoints[i][2] < minimumObjectPos) {
						minimumObjectPos = rotatedPoints[i][2];
					}
				}
			}
		}
	}
	float tmpPos[3];
	float newRobotPos[3];
	mainHandle = getMainHandle();
	simGetObjectPosition(mainHandle, -1, tmpPos);
	newRobotPos[0] = tmpPos[0];
	newRobotPos[1] = tmpPos[1];
	newRobotPos[2] = -minimumObjectPos + 0.001;
	simSetObjectPosition(mainHandle, mainHandle, newRobotPos);
	float postpos[3];
	simGetObjectPosition(mainHandle, -1, postpos);
	if (settings->verbose) {
		cout << "postpos: " << postpos[2] << endl;
	}
}

bool ER_CPPN_Interpreter::checkCollisionBasedOnRotatedPoints(int objectHandle) {
	float objectOrigin[3];
	simGetObjectPosition(objectHandle, -1, objectOrigin);
	float size[3];
	float rotationOrigin[3] = { 0,0,0 };
	simGetObjectFloatParameter(objectHandle, 18, &size[0]);
	simGetObjectFloatParameter(objectHandle, 19, &size[1]);
	simGetObjectFloatParameter(objectHandle, 20, &size[2]);
	for (int i = 0; i < 3; i++) {
		size[i] = size[i] * 2;
	}

	vector<vector<float>> cubeVertex; // 8 points in 3d space
	vector<vector<float>> points;
	points.resize(8);

	float objectMatrix[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	simGetObjectMatrix(objectHandle, -1, objectMatrix);
	//for (int i = 0; i < 12; i++) {
	//	cout << objectMatrix[i] << ", ";
	//} cout << endl;

	points[0].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[0].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[0].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[1].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[1].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[1].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[2].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[2].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[2].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[3].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[3].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[3].push_back(rotationOrigin[2] + (0.5 * size[2]));

	points[4].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[4].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[4].push_back(rotationOrigin[2] - (0.5 * size[2]));

	points[5].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[5].push_back(rotationOrigin[1] + (0.5 * size[1]));
	points[5].push_back(rotationOrigin[2] - (0.5 * size[2]));

	points[6].push_back(rotationOrigin[0] - (0.5 * size[0]));
	points[6].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[6].push_back(rotationOrigin[2] - (0.5 * size[2]));

	points[7].push_back(rotationOrigin[0] + (0.5 * size[0]));
	points[7].push_back(rotationOrigin[1] - (0.5 * size[1]));
	points[7].push_back(rotationOrigin[2] - (0.5 * size[2]));

	vector<vector<float>> rotatedPoints;
	rotatedPoints.resize(8);
	for (int i = 0; i < 8; i++) {
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[0]) + (points[i][1] * objectMatrix[1]) + (points[i][2] * objectMatrix[2]));
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[4]) + (points[i][1] * objectMatrix[5]) + (points[i][2] * objectMatrix[6]));
		rotatedPoints[i].push_back((points[i][0] * objectMatrix[8]) + (points[i][1] * objectMatrix[9]) + (points[i][2] * objectMatrix[10]));
		rotatedPoints[i][0] += objectOrigin[0];
		rotatedPoints[i][1] += objectOrigin[1];
		rotatedPoints[i][2] += objectOrigin[2];
		if (rotatedPoints[i][2] < minimumHeight) {
			return true; 
		}
	}
	return false; 
}

void ER_CPPN_Interpreter::symmetryMutation(float mutationRate) {
	/*for (int i = 0; i < genome->moduleParameters.size(); i++) {
		if (randomNum->randFloat(0.0, 1.0) < mutationRate) {
			int amountChilds = genome->moduleParameters[i]->childSites.size();
			if (amountChilds != 0) {
				int chosenOne = randomNum->randInt(amountChilds, 0);
				int mirrorType = randomNum->randInt(3, 0);
				vector<int> mirrorSite = modules[i]->getMirrorSite(genome->moduleParameters[i]->childSites[chosenOne], genome->moduleParameters[i]->childConfigurations[chosenOne], mirrorType);
				int chosenSite = genome->moduleParameters[i]->childSites[chosenOne];
				int chosenCon = genome->moduleParameters[i]->childConfigurations[chosenOne];
				int chosenState = genome->moduleParameters[i]->childSiteStates[chosenOne];
				int newPos = randomNum->randInt(genome->moduleParameters[i]->maxChilds, 0);

				if (newPos >= amountChilds) {
					genome->moduleParameters[i]->childSites.push_back(mirrorSite[0]);
					genome->moduleParameters[i]->childConfigurations.push_back(mirrorSite[1]);
					genome->moduleParameters[i]->childSiteStates.push_back(genome->moduleParameters[i]->childSiteStates[chosenOne]);
				}
				else {
					genome->moduleParameters[i]->childSites[newPos] = mirrorSite[0];
					genome->moduleParameters[i]->childConfigurations[newPos] = mirrorSite[1];
					genome->moduleParameters[i]->childSiteStates[newPos] = genome->moduleParameters[i]->childSiteStates[chosenOne];
				}
				genome->moduleParameters[i]->amountChilds = genome->moduleParameters[i]->childSites.size();
			}
		}
	}*/
	cout << "Done with symmetry mutation" << endl;
}