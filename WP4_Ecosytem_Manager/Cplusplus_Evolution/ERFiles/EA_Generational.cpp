#include "EA_Generational.h"



EA_Generational::EA_Generational()
{
}


EA_Generational::~EA_Generational()
{
}



void EA_Generational::init()
{
	initializePopulation();
}

void EA_Generational::selection()
{
	createNewGenRandomSelect();
}

void EA_Generational::replacement()
{
}

void EA_Generational::mutation()
{
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		nextGenGenomes[i]->mutate();
	}
}

void EA_Generational::initializePopulation()
{
	unique_ptr<GenomeFactory> gf = unique_ptr<GenomeFactory>(new GenomeFactory);

	if (settings->client) {
		for (int i = 0; i < settings->populationSize; i++)
		{
			populationGenomes.push_back(gf->createGenome(1, randomNum, settings));
			populationGenomes[i]->fitness = 0;
		}
	}
	else {
		cout << "Cannot create VREP dependent genome. Use EA_Generational_VREP for online evolution" << endl;
		//for (int i = 0; i < st->populationSize; i++)
		//{
		//	populationGenomes.push_back(gf->createGenome(1, randomNum, st));
		//	populationGenomes[i]->fitness = 0;
		//	// for easy access of fitness values (used by client-server)
		//	popFitness.push_back(0);
		//}
	}
	gf.reset();
}

void EA_Generational::selectIndividuals()
{
}



void EA_Generational::replaceIndividuals()
{
}

void EA_Generational::createIndividual(int indNum)
{
	populationGenomes[indNum]->create();
}



void EA_Generational::createNewGenRandomSelect() {
	nextGenGenomes.clear();
	//nextGenFitness.clear();

	shared_ptr<MorphologyFactory> mfact(new MorphologyFactory);
	for (int i = 0; i < populationGenomes.size(); i++) {
		int parent = i;
		//nextGenFitness.push_back(-100.0);
		nextGenGenomes.push_back(unique_ptr<DefaultGenome>(new DefaultGenome(randomNum, settings)));
		nextGenGenomes[i]->individualNumber = i + settings->indCounter;
		nextGenGenomes[i]->morph = mfact->copyMorphologyGenome(populationGenomes[parent]->morph->clone());
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
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		nextGenGenomes[i]->fitness = 0; // setting their fitness to zero since they haven't been evaluated yet. 
		nextGenGenomes[i]->saveGenome(nextGenGenomes[i]->individualNumber);
	}
	mfact.reset();
}

// replace entire population with new population
void EA_Generational::replaceNewPopRandom()
{
	for (int p = 0; p < populationGenomes.size(); p++) {
		int currentInd = p;
		populationGenomes[currentInd].reset();
		populationGenomes[currentInd] = nextGenGenomes[p]->clone(); // new DefaultGenome();
//		popFitness[currentInd] = nextGenFitness[p];
//		popIndNumbers[currentInd] = nextGenGenomes[p]->individualNumber;
	}
}