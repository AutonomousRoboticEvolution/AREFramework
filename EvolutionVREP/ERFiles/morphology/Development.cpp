#include "Development.h"
#include <iostream>
#include <algorithm>

Development::Development()
{
	modular = true;
}


Development::~Development()
{
	createdModules.clear();
}

int Development::getAmountBrokenModules()
{
	int amountBrokenModules = 0;
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->broken == true) {
			amountBrokenModules++;
		}
	}
	return amountBrokenModules;
}

//std::vector <std::shared_ptr<ER_Module>> Development::getCreatedModules() {
//	return createdModules;
//}

void Development::createAtPosition(float x, float y, float z)
{
    std::cout << "x, y, z: " << x << ", " << y << ", " << z << std::endl;
	float position[3];

	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Development::crossover(std::shared_ptr<Morphology> partnerMorph, float cr)
{

}

void Development::printSome()
{
    std::cout << "printing some from Development" << std::endl;
}


void Development::saveGenome(int indNum, float fitness)
{
    std::ofstream genomeFile;
    std::ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
//	genomeFileName << indNum << ".csv";
    // std::std::cout << "Saving genome to " << genomeFileName.str() << std::std::std::endl;
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
        std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
	}

	const std::string genomeText = this->generateGenome(indNum, fitness);
    genomeFile << genomeText << std::endl;
	genomeFile.close();
}


bool Development::loadGenome(int individualNumber, int sceneNum)
{
    std::ostringstream genomeFileName;

	if (settings->morphologyType == settings->MODULAR_PHENOTYPE) {
		genomeFileName << settings->repository + "/morphologies" << sceneNum << "/phenotype" << individualNumber << ".csv";
        std::cout << "Loading phenotype" << genomeFileName.str() << std::endl;
        std::ifstream genomeFile(genomeFileName.str());
		if (!genomeFile) {
            std::cerr << "Could not load " << genomeFileName.str() << std::endl;
			return false;
			//		std::exit(1);
		}
		bool load = this->loadGenome(genomeFile, individualNumber);
		genomeFile.close();
        return load;
	}
	else {
		genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
        std::cout << "Loading genome" << genomeFileName.str() << std::endl;
        std::ifstream genomeFile(genomeFileName.str());
		if (!genomeFile) {
            std::cerr << "Could not load " << genomeFileName.str() << std::endl;
			return false;
			//		std::exit(1);
		}
		bool load = this->loadGenome(genomeFile, individualNumber);
		genomeFile.close();
        return load;
	}
	

}

bool Development::loadGenome(std::istream &input, int individualNumber)
{
    std::cout << "Cannot instantiate development class, also, should become abstract" << std::endl;
	return false;
}


void Development::init_noMorph()
{
}

/*!
 * Get how many modules can be attached to the child. Should be improved, quick hack.
 * 
 * \param t
 * \return 
 */
int Development::getMaxChilds(int t)
{
	if (t == 4 || t == 9) {
		return 3; 
	}
	else if (t == 6 || t == 3) {
		return 0;
	}
	else if (t == 1) {
		return 5;
	}
	else if (t == 14 || t == 15) {
		return 0;
	}
	else if (t == 16 || t == 18) {
		return 1;
	}
	else if (t == 17) {
		return 5;
	}
	else if (t == 13) {
		return 20;
	}
	else {
		return 0;
	}
}

//std::vector<std::shared_ptr<ER_Module>> Development::loadPhenotype(int indNum) {
//}

void Development::savePhenotype(std::vector<std::shared_ptr<BASEMODULEPARAMETERS>> createdModules, int indNum, float fitness)
{
	if (settings->verbose) {
        std::cout << "saving direct phenotype genome " << std::endl << "-------------------------------- " << std::endl;
	}
	//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	int amountExpressedModules = createdModules.size();
	
    std::ofstream phenotypeFile;
    std::ostringstream phenotypeFileName;
	phenotypeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << indNum << ".csv";

	phenotypeFile.open(phenotypeFileName.str());
    phenotypeFile << "#Individual:" << indNum << std::endl;
    phenotypeFile << "#Fitness:," << fitness << std::endl;
    phenotypeFile << "#AmountExpressedModules:," << amountExpressedModules << "," << std::endl << std::endl;
    //	std::cout << "#AmountStates:," << amountStates << "," << std::endl << std::endl;

    phenotypeFile << "Module Parameters Start Here: ," << std::endl << std::endl;
	for (int i = 0; i < createdModules.size(); i++) {
        phenotypeFile << "#Module:," << i << std::endl;
        phenotypeFile << "#ModuleType:," << createdModules[i]->type << std::endl;
		
        phenotypeFile << "#ModuleParent:," << createdModules[i]->parent << std::endl;
        phenotypeFile << "#ParentSite:," << createdModules[i]->parentSite << std::endl;
        phenotypeFile << "#Orientation:," << createdModules[i]->orientation << std::endl;

		// TODO: EB: This shouldn't be here. This offsets the coordinates so it can be printabled.
        if(createdModules[i]->absPos[0] > 0) createdModules[i]->absPos[0] += 0.003;
        else createdModules[i]->absPos[0] -= 0.003;
        if(createdModules[i]->absPos[1] > 0) createdModules[i]->absPos[1] += 0.003;
        else createdModules[i]->absPos[1] -= 0.003;
        createdModules[i]->absPos[2] += 0.002;

        phenotypeFile << "#AbsPosition:," << createdModules[i]->absPos[0] << "," << createdModules[i]->absPos[1] << "," << createdModules[i]->absPos[2] << std::endl;
        phenotypeFile << "#AbsOrientation:," << createdModules[i]->absOri[0] << "," << createdModules[i]->absOri[1] << "," << createdModules[i]->absOri[2] << std::endl;

        phenotypeFile << "#ControlParams:," << std::endl;
		phenotypeFile << createdModules[i]->control->getControlParams().str();
        phenotypeFile << "#EndControlParams" << std::endl;
        phenotypeFile << "#EndOfModule," << std::endl << std::endl;
	}
    phenotypeFile << "End Module Parameters" << std::endl;
	phenotypeFile.close();
}

std::vector<std::shared_ptr<Development::BASEMODULEPARAMETERS>> Development::loadBasePhenotype(int indNum)
{
    std::cout << "loading direct phenotype genome " << std::endl << "-------------------------------- " << std::endl;

    std::vector<std::shared_ptr<BASEMODULEPARAMETERS>> bmp;
    std::ostringstream phenotypeFileName;
	phenotypeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << indNum << ".csv";
	//	phenotypeFileName << "files/morphologies0/genome9137.csv";
    std::cout << phenotypeFileName.str() << std::endl;
    std::ifstream phenotypeFile(phenotypeFileName.str());
    std::string value;
    std::list<std::string> values;
	while (phenotypeFile.good()) {
		getline(phenotypeFile, value, ',');
        //		std::cout << value << ",";
        if (value.find('\n') != std::string::npos) {
			split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
	int moduleNum;
    std::vector<std::string> moduleValues;
    std::vector<std::string> controlValues;
	bool checkingModule = false;
	bool checkingControl = false;

    std::list<std::string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
        std::string tmp = *it;
		if (settings->verbose) {
            std::cout << "," << tmp;
		}
		if (checkingModule == true) {
			moduleValues.push_back(tmp);
		}
		if (checkingControl == true) {
			controlValues.push_back(tmp);
		}
		if (tmp == "#Fitness:") {
			it++;
			tmp = *it;
			fitness = atof(tmp.c_str());
            //		std::cout << "Fitness was " << fitness << std::endl;
		}
		if (tmp == "#phenValue;") {
			it++;
			tmp = *it;
			phenValue = atof(tmp.c_str());
		}
		if (tmp == "#Module:") {
			it++;
			tmp = *it;
            bmp.push_back(std::shared_ptr<BASEMODULEPARAMETERS>(new BASEMODULEPARAMETERS));
			moduleNum = bmp.size() - 1;// atoi(tmp.c_str());
			checkingModule = true;
            //		std::cout << "moduleNum set to " << moduleNum << std::endl;
		}
		else if (tmp == "#ModuleParent:")
		{
			it++;
			tmp = *it;
			bmp[moduleNum]->parent = atoi(tmp.c_str());
		}
		else if (tmp == "#ModuleType:")
		{
			it++;
			tmp = *it;
            //genome->moduleParameters.push_back(std::shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
			bmp[moduleNum]->type = atoi(tmp.c_str());
		}
		else if (tmp == "#ParentSite:")
		{
			it++;
			tmp = *it;
			bmp[moduleNum]->parentSite = atoi(tmp.c_str());
		}
		else if (tmp == "#Orientation:")
		{
			it++;
			tmp = *it;
			bmp[moduleNum]->orientation = atoi(tmp.c_str());
		}
		
		if (tmp == "#ControlParams:") {
			checkingControl = true;
		}
		else if (tmp == "#EndOfModule") {
			//			lGenome->lParameters[moduleNum]->module->setModuleParams(moduleValues);
			moduleValues.clear();
			if (checkingControl == true) {
                std::unique_ptr<ControlFactory> controlFactory(new ControlFactory);
				bmp[moduleNum]->control = controlFactory->createNewControlGenome(atoi(controlValues[2].c_str()), randomNum, settings); // 0 is ANN
			//	lGenome->lParameters[moduleNum]->control->init(1, 2, 1);
				bmp[moduleNum]->control->setControlParams(controlValues);
				checkingControl = false;
				controlValues.clear();
				controlFactory.reset();
			}
			moduleNum++;
			checkingModule = false;
		}
	}
	return bmp;
}


void Development::init()
{
}

void Development::initCustomMorphology()
{
}


std::shared_ptr<Morphology> Development::clone() const
{
	BaseMorphology::clone();
    return std::make_unique<Development>(*this);
}

void Development::update()
{

}

int Development::mutateERLGenome(float mutationRate)
{
	return 1;
}

int Development::mutateControlERLGenome(float mutationRate)
{
	return 1;
}

void Development::create()
{
    std::cout << "CANNOT CREAT DEVELOPMENT GENOME" << std::endl;
}

void Development::mutate()
{
	mutateERLGenome(settings->morphMutRate);
}

void Development::updateCreatedModules()
{
	// TODO temporary location of function below
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->parentModulePointer) {
			createdModules[i]->parentModulePointer->updateMorph(createdModules[i]->parentSite);
		}
	}
}

void Development::shiftRobotPosition()
{
	updateCreatedModules();

	float minimumObjectPos = 50.0;
	float minimumYObjectPosition = 50.0;
	float minimumXObjectPosition = 50.0;
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
				for (int j = 0; j < 3; j++) {
					size[j] = size[j] * 2;
				}

                std::vector<std::vector<float>> cubeVertex; // 8 points in 3d space
                std::vector<std::vector<float>> points;
				points.resize(8);

				float objectMatrix[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

				simGetObjectMatrix(createdModules[i]->objectHandles[n], -1, objectMatrix);

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

                std::vector<std::vector<float>> rotatedPoints;
				rotatedPoints.resize(8);
				for (int j = 0; j < 8; j++) {
					rotatedPoints[j].push_back((points[j][0] * objectMatrix[0]) + (points[j][1] * objectMatrix[1]) + (points[j][2] * objectMatrix[2]));
					rotatedPoints[j].push_back((points[j][0] * objectMatrix[4]) + (points[j][1] * objectMatrix[5]) + (points[j][2] * objectMatrix[6]));
					rotatedPoints[j].push_back((points[j][0] * objectMatrix[8]) + (points[j][1] * objectMatrix[9]) + (points[j][2] * objectMatrix[10]));
					rotatedPoints[j][0] += objectOrigin[0];
					rotatedPoints[j][1] += objectOrigin[1];
					rotatedPoints[j][2] += objectOrigin[2];
					if (rotatedPoints[j][2] < minimumObjectPos) {
						minimumObjectPos = rotatedPoints[j][2];
					}
					if (rotatedPoints[j][1] < minimumYObjectPosition) {
						minimumYObjectPosition = rotatedPoints[j][1];
					}
					if (rotatedPoints[j][0] < minimumXObjectPosition) {
						minimumXObjectPosition = rotatedPoints[j][0];
					}
				}
			}
		}
	}
	float tmpPos[3];
	float newRobotPos[3];
	mainHandle = getMainHandle();
	simGetObjectPosition(mainHandle, -1, tmpPos);
	if (settings->environmentType == settings->ROUGH) {
		if (settings->verbose) {
            std::cout << "Shifty Shifter !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }
	    newRobotPos[0] = tmpPos[0];
		newRobotPos[1] = tmpPos[1];
		newRobotPos[2] = (-minimumObjectPos + 0.001) + 0.35;
        std::cout << "newRobotPos = " << newRobotPos[2] << std::endl;
        std::cout << "obPos = " << minimumObjectPos << std::endl;
	}
	else {
		newRobotPos[0] = tmpPos[0];
		newRobotPos[1] = tmpPos[1];
		newRobotPos[2] = -minimumObjectPos + 0.001;
	}

	simSetObjectPosition(mainHandle, mainHandle, newRobotPos);
	//	float postpos[3];
	//	simGetObjectPosition(mainHandle, -1, postpos);
    //	std::cout << "postpos: " << postpos[2] << std::endl;
}

int Development::getMainHandle()
{
    if (createdModules.empty()) {
        std::string error_message = "ERROR: No module could be created, check initial position of the first module. ";
        std::cerr << error_message << std::endl;
        throw std::runtime_error(error_message);
    } else {
        return createdModules[0]->moduleHandle;
    }
}

//std::vector<float> Development::eulerToDirectionalstd::vector(std::vector<float> eulerAngles)
//{
//	Eigen::Matrix3f m;
//	m = AngleAxisf(eulerAngles[0], std::vector3f::UnitX())
//		* AngleAxisf(eulerAngles[1], std::vector3f::UnitY())
//		* AngleAxisf(eulerAngles[2], std::vector3f::UnitZ());
//	//	std::cout << m << std::endl << "is unitary " << m.isUnitary() << std::endl;
//	AngleAxisf AA = AngleAxisf(1 * M_PI, std::vector3f::UnitZ());
//	std::vector<float> directionalstd::vector;
//	directionalstd::vector.push_back(m(6));
//	directionalstd::vector.push_back(m(7));
//	directionalstd::vector.push_back(m(8));
//
//	return directionalstd::vector;
//}

void Development::checkGenome(int individualNumber, int sceneNum)
{

}

void Development::checkControl(int individual, int sceneNum)
{
	checkGenome(individual, sceneNum);
}

/// Check whether two components are colliding
bool Development::checkLCollisions(std::shared_ptr<ER_Module> module, std::vector<int> exceptionHandles)
{
    // TODO EB: Implement a way to ignore visuals.
    for (int n = 0; n < module->objectHandles.size(); n++) {
        if (simGetObjectType(module->objectHandles[n]) == sim_object_shape_type) {
            for (int i = 0; i < createdModules.size() - 1; i++) {
                for (int j = 0; j < createdModules[i]->objectHandles.size(); j++) {
                    if (simCheckCollision(module->objectHandles[n], createdModules[i]->objectHandles[j]) == true) {
                        for (int k = 0; k < exceptionHandles.size(); k++) {
                            if (createdModules[i]->objectHandles[j] != exceptionHandles[k]) {
                                return true;
                            }
                        }
                    }
                }
            }
            // check collision with environment
            for (int i = 0; i < settings->envObjectHandles.size(); i++) {
                if (module->objectHandles[n] != settings->envObjectHandles[i]) {
                    for (int k = 0; k < exceptionHandles.size(); k++) {
                        if (simCheckCollision(module->objectHandles[n], settings->envObjectHandles[i]) == true) {
                            if (createdModules[i]->objectHandles[n] != exceptionHandles[k]) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

/// Check for collisions. If there is a colliding object, remove it from the genome representation.
bool Development::bCheckCollision(int iParentHandle, int createdModulesSize)
{
    bool bViabilityResult;
    std::vector<int> exception;
    exception.push_back(iParentHandle);
    for (int p = 0; p < createdModules[createdModulesSize - 1]->objectHandles.size(); p++) {
        exception.push_back(createdModules[createdModulesSize - 1]->objectHandles[p]);
    }
    if (checkLCollisions(createdModules[createdModulesSize - 1], exception) == false || settings->bCollidingOrgans) {
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename << " Collission check - PASSED." << std::endl;
        }
        bViabilityResult = true;
    }
    else{
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename << " Collission check - FAILED." << std::endl;
        }
        bViabilityResult = false;
    }
    return bViabilityResult;
}

/// Check for ground. If object is above the ground, it can be created
bool Development::bCheckGround(int createdModulesSize)
{
    bool bViabilityResult;
    if(0.0 < createdModules[createdModulesSize - 1]->absPos[2] || settings->bOrgansAbovePrintingBed) {
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename << " Above ground check - PASSED."  << std::endl;
        }
        bViabilityResult = true;
    }
    else{
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename << " Above ground check - FAILED." << std::endl;
        }
        bViabilityResult = false;
    }
    return bViabilityResult;
}

/// Check for orientation. If the orientation of the organ is printable
bool Development::bCheckOrientation(int createdModulesSize)
{
    bool bViabilityResult;
    // If the orientation of the organ is printable
    if (createdModules[createdModulesSize - 1]->type == 14 ||
        createdModules[createdModulesSize - 1]->type == 15) {
        if(
                ((createdModules[createdModulesSize - 1]->absOri[0] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[0] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[1] < 1.65806 && createdModules[createdModulesSize - 1]->absOri[1] > 1.48353) && (createdModules[createdModulesSize - 1]->absOri[2] > -1.65806 && createdModules[createdModulesSize - 1]->absOri[2] < -1.48353)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] > -1.65806 && createdModules[createdModulesSize - 1]->absOri[0] < -1.48353) && (createdModules[createdModulesSize - 1]->absOri[1] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[1] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[2] > -0.0872665 && createdModules[createdModulesSize - 1]->absOri[2] < 0.0872665)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[0] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[1] > -1.65806 && createdModules[createdModulesSize - 1]->absOri[1] < -1.48353) && (createdModules[createdModulesSize - 1]->absOri[2] < 1.65806 && createdModules[createdModulesSize - 1]->absOri[2] > 1.48353)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] < 1.65806 && createdModules[createdModulesSize - 1]->absOri[0] > 1.48353) && (createdModules[createdModulesSize - 1]->absOri[1] > -0.0872665 && createdModules[createdModulesSize - 1]->absOri[1] < 0.0872665) && (abs(createdModules[createdModulesSize - 1]->absOri[2]) < 3.22886 && abs(createdModules[createdModulesSize - 1]->absOri[2]) > 3.05433)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[0] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[1] > -0.0872665 && createdModules[createdModulesSize - 1]->absOri[1] < 0.0872665) && (createdModules[createdModulesSize - 1]->absOri[2] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[2] > -0.0872665)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[0] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[1] > -0.0872665 && createdModules[createdModulesSize - 1]->absOri[1] < 0.0872665) && (createdModules[createdModulesSize - 1]->absOri[2] < 1.65806 && createdModules[createdModulesSize - 1]->absOri[2] > 1.48353)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[0] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[1] > -0.0872665 && createdModules[createdModulesSize - 1]->absOri[1] < 0.0872665) && (createdModules[createdModulesSize - 1]->absOri[2] > -1.65806 && createdModules[createdModulesSize - 1]->absOri[2] < -1.48353)) ||
                ((createdModules[createdModulesSize - 1]->absOri[0] < 0.0872665 && createdModules[createdModulesSize - 1]->absOri[0] > -0.0872665) && (createdModules[createdModulesSize - 1]->absOri[1] > -0.0872665 && createdModules[createdModulesSize - 1]->absOri[1] < 0.0872665) && (abs(createdModules[createdModulesSize - 1]->absOri[2]) > 3.05433 && abs(createdModules[createdModulesSize - 1]->absOri[2]) < 3.22886)) ||
                settings->bNonprintableOrientations){
            if (settings->verbose) {
                std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename
                     << " Good orientation - PASSED." << std::endl;
            }
            bViabilityResult = true;
        }
        else{
            if (settings->verbose) {
                std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename
                     << " Good orientation - FAILED." << std::endl;
            }
            bViabilityResult = false;
        }
    }
    else { // If organ is not brain or sensor
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename
                 << " Good orientation - PASSED." << std::endl;
        }
        bViabilityResult = true;
    }
    return bViabilityResult;
}

/// Check of number of organs.
bool Development::bCheckOrgansNumber(int createdModulesSize)
{
    bool bViabilityResult = true;
    int brainCounter = 0;
    int motorCounter = 0;
    int sensorCounter = 0;
    int jointCounter = 0;
    int boneCounter = 0;
    for (int i = 0; i < createdModulesSize; ++i) {
        if (createdModules[createdModulesSize - 1]->expressed){
            switch (createdModules[i]->type){
                case 13:
                    brainCounter++;
                    break;
                case 14:
                    motorCounter++;
                    break;
                case 15:
                    sensorCounter++;
                    break;
                case 16:
                    jointCounter++;
                    break;
                case 17:
                    boneCounter++;
                    break;
                default:
                    std::cout << "ERROR: Organ type does not exist in bCheckOrgansNumber() " << std::endl;
                    break;
            }

        }
    }
    if(((createdModules[createdModulesSize - 1]->type == 14 && motorCounter >= 2) ||
        (createdModules[createdModulesSize - 1]->type == 15 && sensorCounter >= 2)) && !settings->bAnyOrgansNumber){
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename
                 << " Organs number check - FAILED." << std::endl;
        }
        bViabilityResult = false;
    }
    else{
        if (settings->verbose) {
            std::cout << "Component: " << createdModules[createdModulesSize - 1]->filename
                 << " Organs number check - PASSED." << std::endl;
        }
        bViabilityResult = true;
    }
    return bViabilityResult;
}
