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

int Development::getAmountBrokenModules() {
	int amountBrokenModules = 0;
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->broken == true) {
			amountBrokenModules++;
		}
	}
	return amountBrokenModules;
}

//vector <shared_ptr<ER_Module>> Development::getCreatedModules() {
//	return createdModules;
//}

void Development::createAtPosition(float x, float y, float z) {
	cout << "x, y, z: " << x << ", " << y << ", " << z << endl;
	float position[3];

	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Development::crossover(shared_ptr<Morphology> partnerMorph, float cr) {

}

void Development::printSome() {
	cout << "printing some from Development" << endl;
}


void Development::saveGenome(int indNum, float fitness)
{
	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
//	genomeFileName << indNum << ".csv";
	// std::cout << "Saving genome to " << genomeFileName.str() << std::std::endl;
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
	ostringstream genomeFileName;

	if (settings->morphologyType == settings->MODULAR_PHENOTYPE) {
		genomeFileName << settings->repository + "/morphologies" << sceneNum << "/phenotype" << individualNumber << ".csv";
		std::cout << "Loading phenotype" << genomeFileName.str() << std::endl;
		ifstream genomeFile(genomeFileName.str());
		if (!genomeFile) {
			std::cerr << "Could not load " << genomeFileName.str() << std::endl;
			return false;
			//		std::exit(1);
		}
		bool load = this->loadGenome(genomeFile, individualNumber);
		return load;
		genomeFile.close();

	}
	else {
		genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
		std::cout << "Loading genome" << genomeFileName.str() << std::endl;
		ifstream genomeFile(genomeFileName.str());
		if (!genomeFile) {
			std::cerr << "Could not load " << genomeFileName.str() << std::endl;
			return false;
			//		std::exit(1);
		}
		bool load = this->loadGenome(genomeFile, individualNumber);
		return load;
		genomeFile.close();

	}
	

}

bool Development::loadGenome(std::istream &input, int individualNumber)
{
	std::cout << "Cannot instantiate development class, also, should become abstract" << endl;
	return false;
}


void Development::init_noMorph() {

}

/*!
 * Get how many modules can be attached to the child. Should be improved, quick hack.
 * 
 * \param t
 * \return 
 */

int Development::getMaxChilds(int t) {
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
		return 1;
	}
	else if (t == 13) {
		return 4;
	}
	else {
		return 0;
	}
}

//vector<shared_ptr<ER_Module>> Development::loadPhenotype(int indNum) {
	
//}

void Development::savePhenotype(vector<shared_ptr<BASEMODULEPARAMETERS>> createdModules, int indNum, float fitness)
{
	if (settings->verbose) {
		cout << "saving direct phenotype genome " << endl << "-------------------------------- " << endl;
	}
	//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	int amountExpressedModules = createdModules.size();
	
	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << indNum << ".csv";

	genomeFile.open(genomeFileName.str());
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	genomeFile << "#AmountExpressedModules:," << amountExpressedModules << "," << endl << endl;
	//	cout << "#AmountStates:," << amountStates << "," << endl << endl;

	genomeFile << "Module Parameters Start Here: ," << endl << endl;
	for (int i = 0; i < createdModules.size(); i++) {
		genomeFile << "#Module:," << i << endl;
		genomeFile << "#ModuleType:," << createdModules[i]->type << endl;
		
		genomeFile << "#ModuleParent:," << createdModules[i]->parent << endl;
		genomeFile << "#ParentSite:," << createdModules[i]->parentSite << endl;
		genomeFile << "#Orientation:," << createdModules[i]->orientation << endl;

		genomeFile << "#ControlParams:," << endl;
		genomeFile << createdModules[i]->control->getControlParams().str();
		genomeFile << "#EndControlParams" << endl;
		genomeFile << "#EndOfModule," << endl << endl;
	}
	genomeFile << "End Module Parameters" << endl;
	genomeFile.close();
}

std::vector<shared_ptr<Development::BASEMODULEPARAMETERS>> Development::loadBasePhenotype(int indNum)
{
	cout << "loading direct phenotype genome " << endl << "-------------------------------- " << endl;

	vector<shared_ptr<BASEMODULEPARAMETERS>> bmp;
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << indNum << ".csv";
	//	genomeFileName << "files/morphologies0/genome9137.csv";
	cout << genomeFileName.str() << endl;
	ifstream genomeFile(genomeFileName.str());
	string value;
	list<string> values;
	while (genomeFile.good()) {
		getline(genomeFile, value, ',');
		//		cout << value << ",";
		if (value.find('\n') != string::npos) {
			split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
	int moduleNum;
	vector<string> moduleValues;
	vector<string> controlValues;
	bool checkingModule = false;
	bool checkingControl = false;

	list<string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		if (settings->verbose) {
			cout << "," << tmp;
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
			//		cout << "Fitness was " << fitness << endl; 
		}
		if (tmp == "#phenValue;") {
			it++;
			tmp = *it;
			phenValue = atof(tmp.c_str());
		}
		if (tmp == "#Module:") {
			it++;
			tmp = *it;
			bmp.push_back(shared_ptr<BASEMODULEPARAMETERS>(new BASEMODULEPARAMETERS));
			moduleNum = bmp.size() - 1;// atoi(tmp.c_str());
			checkingModule = true;
			//		cout << "moduleNum set to " << moduleNum << endl; 
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
			//genome->moduleParameters.push_back(shared_ptr<MODULEPARAMETERS>(new MODULEPARAMETERS));
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
				unique_ptr<ControlFactory> controlFactory(new ControlFactory);
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


void Development::init() {

}

void Development::initCustomMorphology() {
	float position[3] = { 0, 0, 0.1 };
}


shared_ptr<Morphology> Development::clone() const {
	BaseMorphology::clone();
	return make_unique<Development>(*this);
}

void Development::update() {

}

int Development::mutateERLGenome(float mutationRate) {
	return 1;
}

int Development::mutateControlERLGenome(float mutationRate) {
	return 1;
}

void Development::create() {
    cout << "CANNOT CREAT DEVELOPMENT GENOME" << endl;
}

void Development::mutate() {
	mutateERLGenome(settings->morphMutRate);
}

void Development::updateCreatedModules() {
	// TODO temporary location of function below
	for (int i = 0; i < createdModules.size(); i++) {
		if (createdModules[i]->parentModulePointer) {
			createdModules[i]->parentModulePointer->updateMorph(createdModules[i]->parentSite);
		}
	}
}

void Development::shiftRobotPosition() {
	updateCreatedModules();

	float minimumObjectPos = 50.0;
	float minimumYObjectPosition = 50.0;
	float minimumXObjectPosition = 50.0;
	for (int i = 0; i < createdModules.size(); i++) {
		for (int n = 0; n < createdModules[i]->objectHandles.size(); n++) {
		    if (createdModules[i]->type == 13){
		        cout << "This is the wheel" << endl;
		        cout << createdModules[i]->objectHandles.size() << endl;
		    }
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
					if (rotatedPoints[i][1] < minimumYObjectPosition) {
						minimumYObjectPosition = rotatedPoints[i][1];
					}
					if (rotatedPoints[i][0] < minimumXObjectPosition) {
						minimumXObjectPosition = rotatedPoints[i][0];
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
            cout << "Shifty Shifter !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        }
	    newRobotPos[0] = tmpPos[0];
		newRobotPos[1] = tmpPos[1];
		newRobotPos[2] = (-minimumObjectPos + 0.001) + 0.35;
		cout << "newRobotPos = " << newRobotPos[2] << endl;
		cout << "obPos = " << minimumObjectPos << endl;
	}
	else {
		newRobotPos[0] = tmpPos[0];
		newRobotPos[1] = tmpPos[1];
		newRobotPos[2] = -minimumObjectPos + 0.001;
	}

	simSetObjectPosition(mainHandle, mainHandle, newRobotPos);
	//	float postpos[3];
	//	simGetObjectPosition(mainHandle, -1, postpos);
	//	cout << "postpos: " << postpos[2] << endl;
}

int Development::getMainHandle() {
    if (createdModules.size() > 0) {
        return createdModules[0]->moduleHandle;
    }
    else {
        std::cerr << "ERROR: No module could be created, check initial position of the first module. " << endl;
    }
}

vector<float> Development::eulerToDirectionalVector(vector<float> eulerAngles) {
	Matrix3f m;
	m = AngleAxisf(eulerAngles[0], Vector3f::UnitX())
		* AngleAxisf(eulerAngles[1], Vector3f::UnitY())
		* AngleAxisf(eulerAngles[2], Vector3f::UnitZ());
	//	cout << m << endl << "is unitary " << m.isUnitary() << endl; 
	AngleAxisf AA = AngleAxisf(1 * M_PI, Vector3f::UnitZ());
	vector<float> directionalVector;
	directionalVector.push_back(m(6));
	directionalVector.push_back(m(7));
	directionalVector.push_back(m(8));



	return directionalVector;
}


void Development::checkGenome(int individualNumber, int sceneNum) {

}

void Development::checkControl(int individual, int sceneNum) {
	checkGenome(individual, sceneNum);
}
