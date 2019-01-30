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

float Development::callFitness() {
	return fitness;
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
	else {
		return 3;
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

}

void Development::mutate() {
	mutateERLGenome(settings->morphMutRate);
}

int Development::getMainHandle() {
	if (createdModules.size() > 0) {
		return createdModules[0]->objectHandles[1];
	}
	else {
		cout << "ERROR: No module could be created, check initial position of the first module. " << endl;
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
