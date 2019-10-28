#pragma once
#ifndef FIXEDBASEMORPHOLOGY_H
#define FIXEDBASEMORPHOLOGY_H

#include "ARE/BaseMorphology.h"
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

#endif //FIXEDBASEMORPHOLOGY_H
