#include "SimpleControl.h"

#include <iostream>

using namespace std;

SimpleControl::SimpleControl()
{
}


SimpleControl::~SimpleControl()
{
}

void SimpleControl::init(int input, int inter, int output) {

}

bool SimpleControl::checkControl(vector<string> values) {
	return true;
}

void SimpleControl::makeDependenciesUnique()
{
	//ADD
}


vector<float> SimpleControl::update(vector<float> sensorValues) {

	// deprived
	return vector<float>();
}

shared_ptr<Control> SimpleControl::clone() const {
	
	//return unique_ptr<SimpleControl>(new SimpleControl(const *this));
	//return new SimpleControl(*this);
	
	return shared_ptr<Control>(new SimpleControl(*this));
}

void SimpleControl::deleteControl() {
	delete this;
}

void SimpleControl::mutate(float mutationRate) {

}

stringstream SimpleControl::getControlParams() {
	stringstream ss;
	ss << "#SimpleControl," << -1 << endl;
	ss << endl;
	return ss;
}

void SimpleControl::setControlParams(vector<string> values) {

}