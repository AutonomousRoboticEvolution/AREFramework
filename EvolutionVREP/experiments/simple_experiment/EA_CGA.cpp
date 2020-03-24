#include "EA_CGA.h"

using namespace are;

void CGA::setFitness(int individual, float fitness)
{
    childrens[individual]->setFitness(fitness);
}

void CGA::init()
{

    for (int i = 0; i < settings->populationSize; i++)
    {
        Individual::Ptr ind();

        population.push_back(ind);
        population.back()->fitness = 0;
    }

}

void CGA::selection()
{
    for(auto )
}

void CGA::mutation()
{
    for (auto & child : childrens)
        child->mutate();
}



void CGA::createIndividual(int indNum)
{
	populationGenomes[indNum]->create();
}

void CGA::createNewGenRandomSelect()
{
	nextGenGenomes.clear();
	//nextGenFitness.clear();

//    std::shared_ptr<MorphologyFactory> mfact(new MorphologyFactory);
	for (int i = 0; i < populationGenomes.size(); i++) {
		int parent = i;
		//nextGenFitness.push_back(-100.0);
		nextGenGenomes.push_back(std::make_unique<DefaultGenome>(randomNum, settings));
		nextGenGenomes[i]->individualNumber = i + settings->indCounter;
        nextGenGenomes[i]->morph = copyMorphologyGenome(populationGenomes[parent]->morph->clone());
		// artefact, use for morphological protection
		// nextGenGenomes[i]->parentPhenValue = populationGenomes[parent]->morph->phenValue;
		// Fix crossover for direct encoding. 
		// if (settings->morphologyType != settings->MODULAR_DIRECT) { // cannot crossover direct encoding
		//	 if (settings->crossoverRate > 0) {
		//		int otherParent = randNum->randInt(populationGenomes.size(), 0);
		//		while (otherParent == parent) { /* parents should always be different, this while loop makes sure of that
		//										* Unless you want to let it mate with itself of course... */
		//			otherParent = randNum->randInt(populationGenomes.size(), 0);
		//		}
		//		// crossover not working in this version
		//		// crossoverGenerational(i, otherParent);
		//	}
		//	}
	}
	mutation();
	for (auto & nextGenGenome : nextGenGenomes) {
		nextGenGenome->fitness = 0; // setting their fitness to zero since they haven't been evaluated yet.
		nextGenGenome->saveGenome(nextGenGenome->individualNumber);
	}
//	mfact.reset();
}

// replace entire population with new population
void CGA::replaceNewPopRandom()
{
	for (int p = 0; p < populationGenomes.size(); p++) {
		int currentInd = p;
		populationGenomes[currentInd].reset();
		populationGenomes[currentInd] = nextGenGenomes[p]->clone(); // new DefaultGenome();
//		popFitness[currentInd] = nextGenFitness[p];
//		popIndNumbers[currentInd] = nextGenGenomes[p]->individualNumber;
	}
}

std::shared_ptr<Morphology> EA_Generational::copyMorphologyGenome(std::shared_ptr<Morphology> parent)
{
    std::shared_ptr<Morphology> cloneMorphology;
    cloneMorphology = parent->clone();
    //	cout << "return clone" << endl;
    return cloneMorphology;
}
