#pragma once
#include "BaseMorphology.h"
class FixedBaseMorphology :
	public BaseMorphology
{
public:
	FixedBaseMorphology();
	~FixedBaseMorphology();
	void init();
	void addDefaultMorphology();
	void saveGenome(int indNum, int sceneNum,float fitness);

};

