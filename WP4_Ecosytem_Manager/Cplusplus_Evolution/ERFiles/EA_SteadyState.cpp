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
	initializePopulation();
}

void EA_SteadyState::selection()
{
	createNewGenRandomSelect();
}

void EA_SteadyState::replacement()
{
	if (populationGenomes.size() > 0) {
		// number of attempts means how many times the new individuals should be checked against the existing population
		// replaceNewPopRandom(1); // int is amount trials comparing offspring to existing population
		replaceNewRank(); 
	}
	else {
		populationGenomes = nextGenGenomes;
	}
}

void EA_SteadyState::mutation() {
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		nextGenGenomes[i]->mutate();
	}
}

void EA_SteadyState::initializePopulation()
{
	unique_ptr<GenomeFactory> gf = unique_ptr<GenomeFactory>(new GenomeFactory);
	for (int i = 0; i < settings->populationSize; i++)
	{
		nextGenGenomes.push_back(gf->createGenome(1, randomNum, settings));
		nextGenGenomes[i]->fitness = 0;
		nextGenGenomes[i]->individualNumber = i;
	}
	gf.reset();
}

void EA_SteadyState::loadPopulationGenomes(int scenenum)
{
	vector<int> popIndNumbers = settings->indNumbers;
	std::cout << "Loading population" << std::endl;
	for (int i = 0; i < popIndNumbers.size(); i++) {
		std::cout << "loading individual " << popIndNumbers[i] << std::endl;
		nextGenGenomes[i]->loadMorphologyGenome(popIndNumbers[i], scenenum);
		nextGenGenomes[i]->fitness = settings->indFits[i]; // indFits has to be saved now. 
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
			nextGenGenomes[i]->morph = mfact->copyMorphologyGenome(populationGenomes[parent]->morph->clone());
			nextGenGenomes[i]->individualNumber = i + settings->indCounter;
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
			if (nextGenGenomes[p]->fitness >= populationGenomes[currentInd]->fitness) {
				cout << "replacement: " << nextGenGenomes[p]->individualNumber << " replaces " << populationGenomes[currentInd]->individualNumber << endl;
				cout << "replacement: " << nextGenGenomes[p]->fitness << " replaces " << populationGenomes[currentInd]->fitness << endl;
				populationGenomes[currentInd].reset();
				populationGenomes[currentInd] = nextGenGenomes[p]->clone(); // new DefaultGenome();
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
