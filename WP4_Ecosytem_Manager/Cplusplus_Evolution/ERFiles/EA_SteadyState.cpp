#include "EA_SteadyState.h"
#include <algorithm>


EA_SteadyState::EA_SteadyState()
{
}


EA_SteadyState::~EA_SteadyState()
{
}

void EA_SteadyState::split_line(string& line, string delim, list<string>& values)
{
	size_t pos = 0;
	while ((pos = line.find(delim, (pos + 0))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}
	while ((pos = line.find(delim, (pos + 1))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}

void EA_SteadyState::init()
{
	gf = unique_ptr<GenomeFactory>(new GenomeFactory);
	initializePopulation();
}

void EA_SteadyState::selection()
{
	createNewGenRandomSelect();
}

void EA_SteadyState::replacement()
{
	if (nextGenGenomes.size() > 0) {
		// number of attempts means how many times the new individuals should be checked against the existing population
		//replaceNewPopRandom(1); // int is amount trials comparing offspring to existing population
		replaceNewRank(); 
	}
}

void EA_SteadyState::mutation() {
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		nextGenGenomes[i]->mutate();
	}
}

void EA_SteadyState::initializePopulation()
{
	//if (settings->client) {
		for (int i = 0; i < settings->populationSize; i++)
		{
			populationGenomes.push_back(gf->createGenome(1, randomNum, settings));
			populationGenomes[i]->fitness = 0;
			populationGenomes[i]->individualNumber = i;
			// for easy access of fitness values (used by client-server)
			//popFitness.push_back(0);
		}
	//}
	//else {
	//	cout << "Cannot create VREP dependent genome. Use EA_SteadyState_VREP for online evolution" << endl;
	//}
}

void EA_SteadyState::selectIndividuals()
{
}

void EA_SteadyState::replaceNewIndividual(int indNum, int sceneNum, float fitness) {
	// random check if individual is better and put it in the population
	int ind = rand() % populationGenomes.size();
	if (fitness >= populationGenomes[ind]->fitness) {
		populationGenomes[ind] = nextGenGenomes[indNum]->clone();
	}
};


void EA_SteadyState::replaceIndividuals()
{
}

void EA_SteadyState::loadPopulationGenomes(int scenenum)
{
	vector<int> popIndNumbers = settings->indNumbers;
//	popFitness = settings->indFits;
	std::cout << "Loading population" << std::endl;
	for (int i = 0; i < popIndNumbers.size(); i++) {
		cout << "loading individual " << popIndNumbers[i] << endl;
		//populationGenomes[i]->init_noMorph();
		// bug??
		populationGenomes[i]->loadMorphologyGenome(popIndNumbers[i], scenenum);
		//	populationFitness[i] = populationGenomes[i]->morph->getFitness();
//		cout << "called fitness = " << popFitness[i] << endl;
	}
}

void EA_SteadyState::createNewGenRandomSelect() {
	nextGenGenomes.clear();
//	nextGenFitness.clear();
	shared_ptr<MorphologyFactory> mfact(new MorphologyFactory);
	for (int i = 0; i < populationGenomes.size(); i++) {
		int parent = randomNum->randInt(populationGenomes.size(), 0);
		if (populationGenomes[parent]->fitness < 0) {
			cout << "The dead cannot reproduce" << endl;
			i--;
		}
		else {
			//nextGenFitness.push_back(-100.0);
			nextGenGenomes.push_back(unique_ptr<DefaultGenome>(new DefaultGenome(randomNum, settings)));
			nextGenGenomes[i]->individualNumber = i + settings->indCounter;
			nextGenGenomes[i]->morph = mfact->copyMorphologyGenome(populationGenomes[parent]->morph->clone());
			nextGenGenomes[i]->fitness = 0; // Ensure the fitness is set to zero. 
			// artefact, use for morphological protection
			// nextGenGenomes[i]->parentPhenValue = populationGenomes[parent]->morph->phenValue;
			// cout << "..";
			// TODO Fix crossover for direct encoding. 
			//if (settings->morphologyType != settings->MODULAR_DIRECT) { // cannot crossover direct encoding
			//	if (settings->crossoverRate > 0) {
			//		int otherParent = randNum->randInt(populationGenomes.size(), 0);
			//		while (otherParent == parent) { /* parents should always be different, this while loop makes sure of that
			//										* Unless you want to let it mate with itself of course... */
			//			otherParent = randNum->randInt(populationGenomes.size(), 0);
			//		}
			//		// crossover not working in this version
			//		// crossoverGenerational(i, otherParent);
			//	}
			//}
		}
	}
	mutation();
	// saving genomes
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		nextGenGenomes[i]->saveGenome(nextGenGenomes[i]->individualNumber);
	}
	mfact.reset();
}

void EA_SteadyState::replaceNewPopRandom(int numAttempts)
{
	cout << "REPLACINGPOP::::::::::::::" << endl;
	for (int p = 0; p < populationGenomes.size(); p++) {
		for (int n = 0; n < numAttempts; n++) {
			int currentInd = randomNum->randInt(populationGenomes.size(), 0);
			// cout << currentInd << endl;
			//for (int i = 0; i < populationGenomes.size(); i++) {
			//	if (populationFitness[i] == 0) {
			//		currentInd = i;
			//		break;
			//	}
			//}
			if (nextGenGenomes[p]->fitness >= populationGenomes[currentInd]->fitness) {
				cout << "replacement: " << nextGenGenomes[p]->individualNumber << " replaces " << populationGenomes[currentInd]->individualNumber << endl;
				cout << "replacement: " << nextGenGenomes[p]->fitness << " replaces " << populationGenomes[currentInd]->fitness << endl;
				// save the genome again, but this time save its fitness as well
				// populationGenomes[currentInd]->morph->saveGenome(nextGenGenomes[p]->individualNumber, sceneNum, nextGenFitness[p]); NO, THIS WILL GO WRONG
				populationGenomes[currentInd].reset();
				populationGenomes[currentInd] = nextGenGenomes[p]->clone(); // new DefaultGenome();
				//populationGenomes[currentInd]->fitness = nextGenGenomes[p]->fitness;
				//popIndNumbers[currentInd] = nextGenGenomes[p]->individualNumber;
				break;
			}
			else if (n == (numAttempts - 1)) {
				// delete genome file
				stringstream ss;
				ss << settings->repository + "/morphologies" << settings->sceneNum << "/genome" << nextGenGenomes[p]->individualNumber << ".csv";
				string genomeFileName = ss.str();
				stringstream ssp;
				ssp << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << nextGenGenomes[p]->individualNumber << ".csv";
				string phenotypeFileName = ssp.str();	
				//	genomeFileName << indNum << ".csv";
				cout << "Removing " << nextGenGenomes[p]->individualNumber << endl;
				remove(genomeFileName.c_str());
				remove(phenotypeFileName.c_str());
			}
		}
	}
	cout << "REPLACED POP" << endl;
}

struct indFit {
	float fitness;
	int ind;
};

bool compareByFitness(const shared_ptr<indFit> a, const shared_ptr<indFit> b)
{
	return a->fitness > b->fitness;
}

void EA_SteadyState::replaceNewRank()
{
	// create one big population. 
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		populationGenomes.push_back(nextGenGenomes[i]);
	}
	// sort population on fitness
	vector<shared_ptr<indFit>> fitnesses;

	for (int i = 0; i < populationGenomes.size(); i++) {
		fitnesses.push_back(shared_ptr<indFit>(new indFit));
		if (populationGenomes[i]->fitness > 10) {
			std::cout << "ERROR: The fitness wasn't set correctly" << std::endl;
			fitnesses[i] = 0;
		}
		else {
			fitnesses[i]->fitness = populationGenomes[i]->fitness;
		}
		fitnesses[i]->ind = populationGenomes[i]->individualNumber; // not individual number!
	}
	
	std::sort(fitnesses.begin(), fitnesses.end(), compareByFitness);

	// remove all individuals on the bottom of the list. 
	for (int i = settings->populationSize; i < fitnesses.size(); i++) {
		for (int j = 0; j < populationGenomes.size(); j++) {
			if (fitnesses[i]->ind == populationGenomes[j]->individualNumber) {
				populationGenomes[j].reset(); // all objects within class should be deleted since they are smart pointers. 
				populationGenomes.erase(populationGenomes.begin() + j);
				break;
			}
		}
	}

	// remove next gen files that didn't make it. 
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		bool deleteGenome = true;
		for (int j = 0; j < populationGenomes.size(); j++) {
			if (nextGenGenomes[i]->individualNumber == populationGenomes[j]->individualNumber) {
				deleteGenome = false;
				break;
			}
		}
		if (deleteGenome == true) {
			// delete genome file
			stringstream ss;
			ss << settings->repository + "/morphologies" << settings->sceneNum << "/genome" << nextGenGenomes[i]->individualNumber << ".csv";
			string genomeFileName = ss.str();
			stringstream ssp;
			ssp << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << nextGenGenomes[i]->individualNumber << ".csv";
			string phenotypeFileName = ssp.str();
			//	genomeFileName << indNum << ".csv";
			cout << "Removing " << nextGenGenomes[i]->individualNumber << endl;
			remove(genomeFileName.c_str());
			remove(phenotypeFileName.c_str());
		}
	}

	cout << "REPLACED POP RANKED" << endl;
}
