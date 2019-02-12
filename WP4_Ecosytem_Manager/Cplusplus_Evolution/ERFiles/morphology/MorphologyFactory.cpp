#pragma once
#include "MorphologyFactory.h"
#include <iostream>

using namespace std;

MorphologyFactory::MorphologyFactory()
{
}


MorphologyFactory::~MorphologyFactory()
{
//	cout << endl << "DELETED MORPHOLOGYFACTORY" << endl << endl; 
}


shared_ptr<Morphology> MorphologyFactory::createMorphologyGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	switch (type){
	case 0:
	{
		cout << "Creating cat genome" << endl;
		shared_ptr<Morphology> m_morph(new CAT);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	case 1: {
//		shared_ptr<Morphology> m_morph(new MultiPurposeLSystemV2);
		shared_ptr<Morphology> m_morph(new ER_LSystem);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	case 2: {
		shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
		//	fixedBaseMorph->init();
		return fixedBaseMorph;
	}
	case 3: {
		shared_ptr<Morphology> m_morph(new ER_CPPN_Encoding);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	case 4: {
		shared_ptr<Morphology> morph(new ER_DirectEncoding);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 5: {
		shared_ptr<CustomMorphology> morph(new CustomMorphology);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 7: {
		shared_ptr<Morphology> morph(new ER_LSystem);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 8: {
		shared_ptr<Morphology> morph(new ER_DirectEncoding);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 9: {
		shared_ptr<Morphology> morph(new ER_LSystem);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 11: {
		shared_ptr<Morphology> morph(new ER_DirectEncoding);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 20: {
		shared_ptr<Morphology> morph(new Tissue_DirectBars);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 21: {
		shared_ptr<Morphology> morph(new Tissue_GMX);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	default: {
		shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
		return fixedBaseMorph;
	}
	}
//	Morphology *plant = new PlantMorphology;
//	return plant;	
}

shared_ptr<Morphology> MorphologyFactory::copyMorphologyGenome(shared_ptr<Morphology> parent)
{
	shared_ptr<Morphology> cloneMorphology;
	cloneMorphology = parent->clone();
	//	cout << "return clone" << endl; 
	return cloneMorphology;
}

shared_ptr<Morphology>MorphologyFactory::createBaseMorphologyGenome()
{
	shared_ptr<Morphology> baseMorph(new BaseMorphology);
	return baseMorph; 
}