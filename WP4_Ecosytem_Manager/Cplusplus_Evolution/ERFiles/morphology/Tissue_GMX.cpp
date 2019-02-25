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
}

/*!
 * 
 * 
 * \param individualNumber
 * \param sceneNum
 */

bool Tissue_GMX::loadGenome(int individualNumber, int sceneNum)
{

}
