#pragma once
#include "MorphologyFactoryVREP.h"
#include <iostream>

using namespace std;

MorphologyFactoryVREP::MorphologyFactoryVREP()
{
}


MorphologyFactoryVREP::~MorphologyFactoryVREP()
{
//	cout << endl << "DELETED MORPHOLOGYFACTORY" << endl << endl; 
}


shared_ptr<Morphology> MorphologyFactoryVREP::createMorphologyGenome(int type, shared_ptr<RandNum> rn, shared_ptr<Settings> st)
{
	//cout << "type = " << type << endl;
	switch (type){
	case 0:
	{
		cout << "Creating catVREP" << endl;
		shared_ptr<Morphology> m_morph(new CATVREP);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
//	case PLANT_MORPHOLOGY:{
//		switch (lSystemType) {
//		case 	DEFAULT_LSYSTEM: {
//		cout << "DEFAULT_LSYSTEM not specified yet" << endl;
//		}
//		case	CUBE_LSYSTEM: {
//		cout << "CUBE_LSYSTEM not specified yet" << endl;
//		}
//		case JOINT_AND_CUBE_LSYSTEM: {
//		cout << "initializing jointAndCubeLSystem" << endl;
//			shared_ptr<Morphology> jcMorph(new LSystemJointAndCube);
//			jcMorph->environment = env; 
//			return jcMorph;
////		LMorphology::lSystemJointAndCube(3, mainHandle, 0);
//		}
//		case LIGHT_LSYSTEM: {
//			cout << "initializing light-lSystem" << endl; 
//			shared_ptr<Morphology> lMorph(new LSystemLightSensors);
//			lMorph->environment = env;
//			cout << "light l-system initialized" << endl; 
//			return lMorph;
//		}
//		case MODULAR_LSYSTEM: {
//		cout << "MODULAR_LSYSTEM not specified yet" << endl;
//		}
//		}
//	//	Morphology *plantMorphology = new PlantMorphology;
//	//	plantMorphology->init();
//	//	return plantMorphology;
//	}
	case 1: {
//		shared_ptr<Morphology> m_morph(new MultiPurposeLSystemV2);
		shared_ptr<Morphology> m_morph(new ER_LSystemInterpreter);
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
		shared_ptr<Morphology> m_morph(new ER_CPPN);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	case 4: {
		shared_ptr<Morphology> morph(new ER_DirectEncodingInterpreter);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 5: { // st->CUSTOM_MORPHOLOGY:
		shared_ptr<Morphology> morph(new CustomMorphologyVREP);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	//case 5: {
	//	shared_ptr<Manual_ModularRobot> mr = shared_ptr<Manual_ModularRobot>(new Manual_ModularRobot);
	//	shared_ptr<Morphology> m_morph = mr;
	//	m_morph->randomNum = rn;
	//	m_morph->settings = st;
	//	mr->openPort();
	//	return m_morph;
	//}
	case 7: {
		//		shared_ptr<Morphology> m_morph(new MultiPurposeLSystemV2);
		shared_ptr<Morphology> m_morph(new ER_LSystemInterpreter);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	case 8: {
		shared_ptr<Morphology> morph(new ER_DirectEncodingInterpreter);
		morph->randomNum = rn;
		morph->settings = st;
		return morph;
	}
	case 9: {
		//		shared_ptr<Morphology> m_morph(new MultiPurposeLSystemV2);
		shared_ptr<Morphology> m_morph(new ER_LSystemInterpreter);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	case 10: {
		shared_ptr<Morphology> m_morph(new EdgarsAmazingMorphologyClassVREP);
		m_morph->randomNum = rn;
		m_morph->settings = st;
		return m_morph;
	}
	default: {
		shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
		return fixedBaseMorph;
	}
	}
//	Morphology *plant = new PlantMorphology;
//	return plant;	
}

shared_ptr<Morphology> MorphologyFactoryVREP::copyMorphologyGenome(shared_ptr<Morphology> parent)
{
	shared_ptr<Morphology> cloneMorphology;
	cloneMorphology = parent->clone();
	//	cout << "return clone" << endl; 
	return cloneMorphology;
}

shared_ptr<Morphology>MorphologyFactoryVREP::createBaseMorphologyGenome()
{
	shared_ptr<Morphology> baseMorph(new BaseMorphology);
	return baseMorph; 
}