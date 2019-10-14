#include "MorphologyFactoryVREP.h"
#include <iostream>

MorphologyFactoryVREP::MorphologyFactoryVREP()
{
}


MorphologyFactoryVREP::~MorphologyFactoryVREP()
{
//	cout << endl << "DELETED MORPHOLOGYFACTORY" << endl << endl; 
}


std::shared_ptr<Morphology> MorphologyFactoryVREP::createMorphologyGenome(int type, std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
    if(type == 0)
    {
        std::cout << "Creating catVREP" << std::endl;
        std::shared_ptr<Morphology> m_morph(new CATVREP);
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
//            cout << "Creating catVREP" << endl;
//            shared_ptr<Morphology> m_morph(new CATVREP);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 1: {
//            shared_ptr<Morphology> m_morph(new ER_LSystemInterpreter);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 2: {
//            shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
//            //	fixedBaseMorph->init();
//            return fixedBaseMorph;
//        }
//        case 3: {
//            shared_ptr<Morphology> m_morph(new ER_CPPN_Interpreter);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 4: {
//            shared_ptr<Morphology> morph(new ER_DirectEncodingInterpreter);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        case 5: {
//            shared_ptr<Morphology> morph(new CustomMorphologyVREP);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        case 7: {
//            shared_ptr<Morphology> m_morph(new ER_LSystemInterpreter);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 8: {
//            shared_ptr<Morphology> morph(new ER_DirectEncodingInterpreter);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        case 9: {
//            shared_ptr<Morphology> m_morph(new ER_LSystemInterpreter);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 12: {
//            shared_ptr<Morphology> m_morph(new NEAT_CPPN_Encoding);
//            m_morph->randomNum = rn;
//            m_morph->settings = st;
//            return m_morph;
//        }
//        case 11: {
//            shared_ptr<Morphology> morph(new ER_DirectEncodingInterpreter);
//            morph->randomNum = rn;
//            morph->settings = st;
//            return morph;
//        }
//        default: {
//            shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
//            return fixedBaseMorph;
//        }
//	}
}

std::shared_ptr<Morphology> MorphologyFactoryVREP::copyMorphologyGenome(std::shared_ptr<Morphology> parent)
{
    std::shared_ptr<Morphology> cloneMorphology;
	cloneMorphology = parent->clone();
	//	cout << "return clone" << endl; 
	return cloneMorphology;
}

std::shared_ptr<Morphology>MorphologyFactoryVREP::createBaseMorphologyGenome()
{
    std::shared_ptr<Morphology> baseMorph(new BaseMorphology);
	return baseMorph; 
}

std::shared_ptr<Morphology> MorphologyFactoryVREP::convertMorph(std::shared_ptr<Morphology> morph)
{


    if(morph->settings->morphologyType == 0)
    {
        std::shared_ptr<CATVREP> placeHolder;
        placeHolder = std::static_pointer_cast<CATVREP>(morph);

        std::shared_ptr<CATVREP> castObject;
        castObject = std::make_shared<CATVREP>();
        castObject->control = placeHolder->control->clone();
        castObject->randomNum = placeHolder->randomNum;
        castObject->settings = placeHolder->settings;
        return std::move(castObject);
        // shared_ptr<CATVREP> catMorph = dynamic_pointer_cast<CATVREP>(morph);
        // shared_ptr<Morphology> m_morph = catMorph;
        // return m_morph;
    }
    else if (morph->settings->morphologyType == 2)
    {
        std::shared_ptr<FixedBaseMorphology> placeHolder;
        placeHolder = std::static_pointer_cast<FixedBaseMorphology>(morph);

        std::shared_ptr<FixedBaseMorphology> castObject;
        castObject = std::make_shared<FixedBaseMorphology>();
        castObject->randomNum = placeHolder->randomNum;
        castObject->settings = placeHolder->settings;
        return std::move(castObject);
    }
    else
    {
        std::shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
        return fixedBaseMorph;
    }

//	switch (morph->settings->morphologyType) {
//	case 0:
//	{
//		std::shared_ptr<CATVREP> placeHolder;
//		placeHolder = std::static_pointer_cast<CATVREP>(morph);

//		shared_ptr<CATVREP> castObject;
//		castObject = make_shared<CATVREP>();
//		castObject->control = placeHolder->control->clone();
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;
//		// shared_ptr<CATVREP> catMorph = dynamic_pointer_cast<CATVREP>(morph);
//		// shared_ptr<Morphology> m_morph = catMorph;
//		// return m_morph;
//	}
//	case 1: {
//		std::shared_ptr<ER_LSystemInterpreter> placeHolder;
//		placeHolder = std::static_pointer_cast<ER_LSystemInterpreter>(morph);

//		shared_ptr<ER_LSystemInterpreter> castObject;
//		castObject = make_shared<ER_LSystemInterpreter>();
//		castObject->lGenome = placeHolder->lGenome;
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;

//	}
//	case 2: {
//		std::shared_ptr<FixedBaseMorphology> placeHolder;
//		placeHolder = std::static_pointer_cast<FixedBaseMorphology>(morph);

//		shared_ptr<FixedBaseMorphology> castObject;
//		castObject = make_shared<FixedBaseMorphology>();
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;
//	}
//	case 3: {
//		// transform ER_CPPN_Encoding to ER_CPPN_Interpreter
//		// first save do a static cast to access ER_CPPN_Encoding values
//		std::shared_ptr<ER_CPPN_Interpreter> placeHolder;
//		placeHolder = std::static_pointer_cast<ER_CPPN_Interpreter>(morph);
		
//		shared_ptr<ER_CPPN_Interpreter> castObject;
//		castObject = make_shared<ER_CPPN_Interpreter>();
//		castObject->genome = placeHolder->genome;
//		castObject->cppn = placeHolder->cppn;
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;
		
//	}
//	case 4: {
//		// transform ER_CPPN_Encoding to ER_CPPN_Interpreter
//		// first save do a static cast to access ER_CPPN_Encoding values
//		std::shared_ptr<ER_DirectEncodingInterpreter> placeHolder;
//		placeHolder = std::static_pointer_cast<ER_DirectEncodingInterpreter>(morph);
		
//		// Make a
//		shared_ptr<ER_DirectEncodingInterpreter> castObject;
//		castObject = make_shared<ER_DirectEncodingInterpreter>();
//		castObject->genome = placeHolder->genome;
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;
//	}
//	case 5: { // st->CUSTOM_MORPHOLOGY:
//		std::shared_ptr<CustomMorphologyVREP> placeHolder;
//		placeHolder = std::static_pointer_cast<CustomMorphologyVREP>(morph);

//		shared_ptr<CustomMorphologyVREP> castObject;
//		castObject = make_shared<CustomMorphologyVREP>();
//        castObject->control = placeHolder->control->clone();
//		//castObject->genome = placeHolder->genome;
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;
//	}
//			//case 5: {
//			//	shared_ptr<Manual_ModularRobot> mr = shared_ptr<Manual_ModularRobot>(new Manual_ModularRobot);
//			//	shared_ptr<Morphology> m_morph = mr;
//			//	m_morph->randomNum = rn;
//			//	m_morph->settings = st;
//			//	mr->openPort();
//			//	return m_morph;
//			//}
//	case 7: {

//	}
//	case 8: {

//	}
//	case 9: {

//	}
//	case 20: {
//		return morph;
//	}
//	case 21: {
//        return morph;
//	}
//	case 11: {
//		// transform ER_CPPN_Encoding to ER_CPPN_Interpreter
//		// first save do a static cast to access ER_CPPN_Encoding values
//		std::shared_ptr<ER_DirectEncodingInterpreter> placeHolder;
//		placeHolder = std::static_pointer_cast<ER_DirectEncodingInterpreter>(morph);

//		shared_ptr<ER_DirectEncodingInterpreter> castObject;
//		castObject = make_shared<ER_DirectEncodingInterpreter>();
//		castObject->genome = placeHolder->genome;
//		castObject->randomNum = placeHolder->randomNum;
//		castObject->settings = placeHolder->settings;
//		return castObject;
//	}
//	default: {
//		shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
//		return fixedBaseMorph;
//	}
}

	// shared_ptr<Morphology> vrepMorph = createMorphologyGenome(morph->settings->morphologyType, morph->randomNum, morph->settings);
	//shared_ptr<Morphology> newMorph = createMorphologyGenome(morph->settings->morphologyType, morph->randomNum, morph->settings);
	//shared_ptr<Morphology> vrepMorph = dynamic_cast<newMorph>();
	//vrepMorph->control = morph->control;
	//vrepMorph = 

