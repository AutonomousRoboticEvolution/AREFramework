#pragma once
#include "BaseMorphology.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

class FixedBaseMorphology :
	public BaseMorphology
{
public:
    FixedBaseMorphology(){}
    FixedBaseMorphology(const Settings &st) : BaseMorphology(st){}
    ~FixedBaseMorphology(){}
	void init();
	void addDefaultMorphology();
	void saveGenome(int indNum,float fitness);

};
