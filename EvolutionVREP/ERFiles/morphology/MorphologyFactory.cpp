#include "MorphologyFactory.h"
#include <iostream>

#include "BaseMorphology.h"
#include "FixedBaseMorphology.h"
#include "ER_LSystem.h"
#include "ER_CPPN.h"
#include "ER_LSystemInterpreter.h"
#include "CAT.h"
#include "ER_DirectEncoding.h"
#include "CustomMorphology.h"
#include "ER_CPPN_Encoding.h"

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
            cout << "Morphology type: CAT" << endl;
            shared_ptr<Morphology> m_morph(new CAT);
            m_morph->randomNum = rn;
            m_morph->settings = st;
            return m_morph;
        }
        case 1:
        {
            shared_ptr<Morphology> m_morph(new ER_LSystem);
            m_morph->randomNum = rn;
            m_morph->settings = st;
            return m_morph;
        }
        case 2:
        {
            shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
            return fixedBaseMorph;
        }
        case 3:
        {
            shared_ptr<Morphology> m_morph(new ER_CPPN_Encoding);
            m_morph->randomNum = rn;
            m_morph->settings = st;
            return m_morph;
        }
        case 4:
        {
            shared_ptr<Morphology> morph(new ER_DirectEncoding);
            morph->randomNum = rn;
            morph->settings = st;
            return morph;
        }
        case 5:
        {
            shared_ptr<CustomMorphology> morph(new CustomMorphology);
            morph->randomNum = rn;
            morph->settings = st;
            return morph;
        }
        case 6:
        {
            // TODO EB -  This needs yet to be written before being used!
            //shared_ptr<VoxelMorphology> morph(new VoxelMorphology);
            //morph->randomNum = rn;
            //morph->settings = st;
            //return morph;
        }
        case 11:
        {
            shared_ptr<Morphology> morph(new ER_DirectEncoding);
            morph->randomNum = rn;
            morph->settings = st;
            return morph;
        }
        default:
        {
            shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
            return fixedBaseMorph;
        }
	}
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
