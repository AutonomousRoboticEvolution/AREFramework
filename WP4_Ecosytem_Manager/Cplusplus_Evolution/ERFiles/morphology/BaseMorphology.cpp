#include "BaseMorphology.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>

using namespace std;

// for reading csv file
void BaseMorphology::split_line(string& line, string delim, list<string>& values)
{
	size_t pos = 0;
	while ((pos = line.find(delim, (pos + 0))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}
	while ((pos = line.find(delim, (pos + 1))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}

BaseMorphology::BaseMorphology()
{
}


BaseMorphology::~BaseMorphology()
{
}

void BaseMorphology::mutate(){
	cout << "Cannot Mutate Base Morphology: check settings" << endl; 
}

bool BaseMorphology::loadGenome(int individualNumber, int sceneNum)
{
	std::cout << "cannot load basemorph" << std::endl;
	return false;
}

bool BaseMorphology::loadGenome(std::istream &input, int individualNumber)
{
	std::cout << "cannot load basemorph" << std::endl;
	return false;
}

vector <shared_ptr<ER_Module>> BaseMorphology::getCreatedModules() {
	vector <shared_ptr<ER_Module>> vec;
	return vec;
}

void BaseMorphology::setPhenValue()
{
	phenValue = 1;
}


vector<int> BaseMorphology::getObjectHandles(int)
{
	return vector<int>();
}

vector<int> BaseMorphology::getJointHandles(int)
{
	return vector<int>();
}

vector<int> BaseMorphology::getAllHandles(int)
{
	return vector<int>();
}

void BaseMorphology::init() {
// physically construct the initial parameters
	cout << "NOTE: BaseMorphology::init() is deprecated" << endl; 
}

void BaseMorphology::create() {
	cout << "NOTE: BaseMorphology::create() is deprecated" << endl;


}

void BaseMorphology::printSome() {
// function deprived
}

void BaseMorphology::createAtPosition(float x, float y, float z) {
	
}


void BaseMorphology::update() {
	if (control) {
		vector<float> input; // get sensor parameters
		input.push_back(1);
		control->update(input);
	}
}


void BaseMorphology::saveBaseMorphology(int indNum, float fitness)
{
}

void BaseMorphology::loadBaseMorphology(int indNum, int sceneNum)
{
}

void BaseMorphology::clearMorph(){
	// function deprived
}

void BaseMorphology::setMainHandlePosition(float position[3]) {
	cout << "setMainHandlePosition is deprived" << endl;
}

shared_ptr<Morphology> BaseMorphology::clone() const {
	return shared_ptr<Morphology>(new BaseMorphology(*this));
}

int BaseMorphology::getAmountBrokenModules() { 
	cout << "BaseMorphology cannot return broken modules, check code" << endl;
	return 0; 
}

int BaseMorphology::getMainHandle() {
	return mainHandle; 
}

float BaseMorphology::getFitness() {
	cout << "Basemorphology cannot return a fitness, check code" << endl;
	return 0.0; 
}

void BaseMorphology::saveGenome(int indNum, float fitness) {
	cout << "Basemorphology cannot save genome, check code" << endl;
}

const std::string BaseMorphology::generateGenome(int indNum, float fitness) const
{
	std::cout << "Basemorphology cannot generate genome, check code" << std::endl;
	return "Herp derp"; 
}