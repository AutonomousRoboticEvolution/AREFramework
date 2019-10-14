#include "MorphologyFactory.h"
#include <iostream>

#include "BaseMorphology.h"
#include "FixedBaseMorphology.h"
#include "CAT.h"
#include "ER_VoxelInterpreter.h"

MorphologyFactory::MorphologyFactory()
{
}


MorphologyFactory::~MorphologyFactory()
{
//	cout << endl << "DELETED MORPHOLOGYFACTORY" << endl << endl; 
}


std::shared_ptr<Morphology>
MorphologyFactory::createMorphologyGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{

    if(type == 0)
    {
        std::cout << "Morphology type: CAT" << std::endl;
        std::shared_ptr<Morphology> m_morph(new CAT);
        m_morph->randomNum = rn;
        m_morph->settings = st;
        return m_morph;
    }
    else
    {
        std::shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
        return fixedBaseMorph;
    }

//	switch (type){
//        case 0:
//        {
//            std::cout << "Morphology type: CAT" << std::endl;
//            std::shared_ptr<Morphology> m_morph(new CAT);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 1:
//        {
//            std::shared_ptr<Morphology> m_morph(new ER_LSystem);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 2:
//        {
//            std::shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
//            return fixedBaseMorph;
//        }
//        case 3:
//        {
//            std::shared_ptr<Morphology> m_morph(new ER_CPPN_Encoding);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 4:
//        {
//            std::shared_ptr<Morphology> morph(new ER_DirectEncoding);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        case 5:
//        {
//            std::shared_ptr<CustomMorphology> morph(new CustomMorphology);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        case 6:
//        {
//            // TODO EB -  This needs yet to be written before being used!
//            //shared_ptr<Morphology> morph(new VoxelMorphology);
//            //morph->randomNum = rn;
//            //morph->settings = st;
//            //return morph;
//        }
//        case 11:
//        {
//            std::shared_ptr<Morphology> morph(new ER_DirectEncoding);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        default:
//        {
//            std::shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
//            return fixedBaseMorph;
//        }

}

std::shared_ptr<Morphology> MorphologyFactory::copyMorphologyGenome(std::shared_ptr<Morphology> parent)
{
    std::shared_ptr<Morphology> cloneMorphology;
	cloneMorphology = parent->clone();
	//	cout << "return clone" << endl; 
	return cloneMorphology;
}

std::shared_ptr<Morphology>MorphologyFactory::createBaseMorphologyGenome()
{
    std::shared_ptr<Morphology> baseMorph(new BaseMorphology);
	return baseMorph; 
}
