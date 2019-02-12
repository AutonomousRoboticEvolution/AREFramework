#include "Tissue_GMX.h"

Tissue_GMX::Tissue_GMX()
{
}


Tissue_GMX::~Tissue_GMX()
{
}

vector<int> Tissue_GMX::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> Tissue_GMX::getJointHandles(int parentHandle)
{
	return vector<int>();
}

void Tissue_GMX::update() {

}

int Tissue_GMX::getMainHandle()
{
	return mainHandle;
}

void Tissue_GMX::create()
{

}

void Tissue_GMX::init() {
	create();
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	control->init(40, 28, 40);
	control->mutate(0.5);
}

void Tissue_GMX::saveGenome(int indNum, float fitness)
{
	cout << "Saving Edgar's Amazing Genome " << endl << "-------------------------------- " << endl;
	cout << "saving direct genome " << endl << "-------------------------------- " << endl;
	//	int evolutionType = 0; // regular evolution, will be changed in the future. 
	
	ofstream genomeFile;
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << settings->sceneNum << "/genome" << indNum << ".csv";
	//	genomeFileName << indNum << ".csv";
	genomeFile.open(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Error opening file \"" << genomeFileName.str() << "\" to save genome." << std::endl;
	}
	genomeFile << "#Individual:" << indNum << endl;
	genomeFile << "#Fitness:," << fitness << endl;
	//	cout << "#AmountStates:," << amountStates << "," << endl << endl;

	genomeFile << "Morphology starts here: ," << endl << endl;
	genomeFile << "," << "#body" <<  ",";
	for (int i = 0; i < bodies[0].medians.size(); i++) {
		genomeFile << bodies[0].medians[i] << ',';
	}
	for (int i = 0; i < bodies[0].deviations.size(); i++) {
		genomeFile << bodies[0].deviations[i] << ',';
	}
	genomeFile << ",body end," << endl;

	genomeFile << ",#sensors" << ",";
	for (int i = 0; i < sensors.size(); i++) {
		genomeFile << sensors[i].coordinates.x << ",";
		genomeFile << sensors[i].coordinates.y << ",";
		genomeFile << sensors[i].coordinates.z << ",";
	}
	genomeFile << ",sensors end," << endl;

	genomeFile << ",#actuators" << ",";
	for (int i = 0; i < actuators.size(); i++) {
		genomeFile << actuators[i].coordinates.x << ",";
		genomeFile << actuators[i].coordinates.y << ",";
		genomeFile << actuators[i].coordinates.z << ",";
	}
	genomeFile << ",actuators end," << endl;

	genomeFile << "#ControlParams:," << endl;
	genomeFile << control->getControlParams().str();
	genomeFile << "#EndOfControl," << endl << endl;
	genomeFile << "#EndGenome," << endl << endl;
	genomeFile.close();
}

/*!
 * 
 * 
 * \param individualNumber
 * \param sceneNum
 */

bool Tissue_GMX::loadGenome(int individualNumber, int sceneNum)
{
	randomNum->getSeed(); // setSeed(individualNumber);
	if (settings->verbose) {
		cout << "loading edgar genome " << individualNumber << "(ER_Direct)" << endl;
	}
	ostringstream genomeFileName;
	genomeFileName << settings->repository + "/morphologies" << sceneNum << "/genome" << individualNumber << ".csv";
	cout << genomeFileName.str() << endl;
	ifstream genomeFile(genomeFileName.str());
	if (!genomeFile) {
		std::cerr << "Could not load " << genomeFileName.str() << std::endl;
		return false;
		//		std::exit(1);
	}
	string value;
	list<string> values;
	while (genomeFile.good()) {
		getline(genomeFile, value, ',');
		if (value.find('\n') != string::npos) {
			split_line(value, "\n", values);
		}
		else {
			values.push_back(value);
		}
	}
	vector<string> controlValues;
	bool checkingModule = false;
	bool checkingControl = false;

	list<string>::const_iterator it = values.begin();
	for (it = values.begin(); it != values.end(); it++) {
		string tmp = *it;
		if (settings->verbose) {
			cout << "," << tmp;
		}
		if (tmp == "#body")
		{
			// load medians

			// load deviations

		}

		if (tmp == "#sensors")
		{

		}

		if (tmp == "#actuators")
		{

		}
		if (checkingControl == true) {
			controlValues.push_back(tmp);
		}
		if (tmp == "#ControlParams:") {
			checkingControl = true;
		}
		else if (tmp == "#EndGenome") {
			if (checkingControl == true) {
				unique_ptr<ControlFactory> controlFactory(new ControlFactory);
				control = controlFactory->createNewControlGenome(atoi(controlValues[2].c_str()), randomNum, settings); // 0 is ANN
				control->setControlParams(controlValues);
				checkingControl = false;
				controlValues.clear();
				controlFactory.reset();
			}
		}
	}
	return true;
}
