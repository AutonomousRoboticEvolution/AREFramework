#include "CustomMorphology.h"



CustomMorphology::CustomMorphology()
{
}


CustomMorphology::~CustomMorphology()
{
}

vector<int> CustomMorphology::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> CustomMorphology::getJointHandles(int parentHandle)
{
	return vector<int>();
}

void CustomMorphology::update() {

}

int CustomMorphology::getMainHandle()
{
	return mainHandle;
}

void CustomMorphology::create()
{

}

void CustomMorphology::init() {
	create();
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	control->init(40, 28, 40);
	control->mutate(0.5);
}