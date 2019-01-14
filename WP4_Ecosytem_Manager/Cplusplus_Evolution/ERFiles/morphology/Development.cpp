#include "Development.h"
#include <iostream>

Development::Development()
{
	modular = true;
}


Development::~Development()
{
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

vector <shared_ptr<ER_Module>> Development::getCreatedModules() {
	return createdModules;
}

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


void Development::saveGenome(int indNum, int sceneNum, float fitness) {

}

float Development::callFitness() {
	return fitness;
}

void Development::loadGenome(int individualNumber, int sceneNum) {
}


void Development::init_noMorph() {

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
