#include "Tissue_DirectBars.h"

Tissue_DirectBars::Tissue_DirectBars()
{
}


Tissue_DirectBars::~Tissue_DirectBars()
{
}

vector<int> Tissue_DirectBars::getObjectHandles(int parentHandle)
{
	return vector<int>();
}

vector<int> Tissue_DirectBars::getJointHandles(int parentHandle)
{
	return vector<int>();
}

void Tissue_DirectBars::update() {

}

int Tissue_DirectBars::getMainHandle()
{
	return mainHandle;
}

void Tissue_DirectBars::create()
{

}

void Tissue_DirectBars::init() {
	create();
	unique_ptr<ControlFactory> controlFactory(new ControlFactory);
	control = controlFactory->createNewControlGenome(0, randomNum, settings); // ann
	controlFactory.reset();
	control->init(40, 28, 40);
	control->mutate(0.5);
}

void Tissue_DirectBars::saveGenome(int indNum, float fitness)
{

}

/*!
 *
 *
 * \param individualNumber
 * \param sceneNum
 */

bool Tissue_DirectBars::loadGenome(int individualNumber, int sceneNum)
{

}
