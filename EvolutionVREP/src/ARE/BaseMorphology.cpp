#include "ARE/BaseMorphology.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>

// for reading csv file
void BaseMorphology::split_line(std::string& line, std::string delim, std::list<std::string>& values)
{
	size_t pos = 0;
    while ((pos = line.find(delim, (pos + 0))) != std::string::npos) {
        std::string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}
    while ((pos = line.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}


void BaseMorphology::mutate(){
    std::cout << "Cannot Mutate Base Morphology: check settings" << std::endl;
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

std::vector <std::shared_ptr<ER_Module>> BaseMorphology::getCreatedModules() {
    std::vector <std::shared_ptr<ER_Module>> vec;
	return vec;
}

void BaseMorphology::setPhenValue()
{
	phenValue = 1;
}


std::vector<int> BaseMorphology::getObjectHandles(int)
{
    return std::vector<int>();
}

std::vector<int> BaseMorphology::getJointHandles(int)
{
    return std::vector<int>();
}

std::vector<int> BaseMorphology::getAllHandles(int)
{
    return std::vector<int>();
}

void BaseMorphology::init() {
// physically construct the initial parameters
    std::cout << "NOTE: BaseMorphology::init() is deprecated" << std::endl;
}

void BaseMorphology::create() {
    std::cout << "NOTE: BaseMorphology::create() is deprecated" << std::endl;
}

void BaseMorphology::printSome() {
// function deprived
}

void BaseMorphology::createAtPosition(float x, float y, float z) {
	
}


void BaseMorphology::update() {
	if (control) {
        std::vector<float> input; // get sensor parameters
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
    std::cout << "setMainHandlePosition is deprived" << std::endl;
}

std::shared_ptr<Morphology> BaseMorphology::clone() const {
    return std::shared_ptr<Morphology>(new BaseMorphology(*this));
}

int BaseMorphology::getAmountBrokenModules() { 
    std::cout << "BaseMorphology cannot return broken modules, check code" << std::endl;
	return 0; 
}

int BaseMorphology::getMainHandle() {
	return mainHandle; 
}

float BaseMorphology::getFitness() {
    std::cout << "Basemorphology cannot return a fitness, check code" << std::endl;
	return 0.0; 
}

void BaseMorphology::saveGenome(int indNum, float fitness) {
    std::cout << "Basemorphology cannot save genome, check code" << std::endl;
}

const std::string BaseMorphology::generateGenome(int indNum, float fitness) const
{
    std::cout << "Basemorphology cannot generate genome, check code" << std::endl;
	return "Herp derp"; 
}
