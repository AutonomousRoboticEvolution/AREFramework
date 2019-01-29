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
		populationGenomes[i]->loadMorphologyGenome(popIndNumbers[i], scenenum);
		populationGenomes[i]->fitness = settings->indFits[i]; // indFits has to be saved now. 
	}
}

void EA_SteadyState::createNewGenRandomSelect() {
	nextGenGenomes.clear();
	//	nextGenFitness.clear();
	shared_ptr<MorphologyFactory> mfact(new MorphologyFactory);
	for (int i = 0; i < populationGenomes.size(); i++) {
		int parent = randomNum->randInt(populationGenomes.size(), 0);
		if (settings->verbose) {
			std::cout << "Selecting parent " << parent << " with fitness " << populationGenomes[i]->fitness << " individual number is " << populationGenomes[i]->individualNumber << std::endl;
			std::cout << "^ will get ind number " << i + settings->indCounter << std::endl;
		}
		//nextGenFitness.push_back(-100.0);
		nextGenGenomes.push_back(unique_ptr<DefaultGenome>(new DefaultGenome(randomNum, settings)));
		if (settings->verbose) {
			std::cout << "About to deep copy genome" << endl;
		}
		nextGenGenomes[i]->morph = mfact->copyMorphologyGenome(populationGenomes[parent]->morph->clone()); // deep copy
		if (settings->verbose) {
			std::cout << "Done with deep copy genome" << endl;
		}
		nextGenGenomes[i]->individualNumber = i + settings->indCounter;
		nextGenGenomes[i]->fitness = 0; // Ensure the fitness is set to zero. 
		nextGenGenomes[i]->isEvaluated = false; // This should also be set, just to be sure. 
	}
	if (settings->verbose) {
		std::cout << "Mutating next gen genomes of size: " << nextGenGenomes.size() << std::endl;
	}
	mutation();
	// saving genomes
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		nextGenGenomes[i]->saveGenome(nextGenGenomes[i]->individualNumber);
		// Used to debug
		// populationGenomes[i]->saveGenome(-populationGenomes[i]->individualNumber);
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

bool compareByFitness(const shared_ptr<Genome> a, const shared_ptr<Genome> b)
{
	return a->fitness > b->fitness;
}

void EA_SteadyState::replaceNewRank()
{
	vector<shared_ptr<Genome>> populationGenomesBuffer;
	// create one big population. Just store the pointers in the buffer. 
	for (int i = 0; i < populationGenomes.size(); i++) {
		populationGenomesBuffer.push_back(populationGenomes[i]);
	}
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		populationGenomesBuffer.push_back(nextGenGenomes[i]);
	}

	// There were some individuals that got an odd fitness value, this should not happen anymore, but just in case, added a cerr.
	for (int i = 0; i < populationGenomes.size(); i++) {
		if (populationGenomesBuffer[i]->fitness > 100) {
			std::cerr << "ERROR: Note that the fitness wasn't set correctly. Giving individual " << populationGenomes[i]->individualNumber << " a fitness of 0" << std::endl;
			populationGenomesBuffer[i]->fitness = 0;
		}
	}

	// sort population on fitness
	std::sort(populationGenomesBuffer.begin(), populationGenomesBuffer.end(), compareByFitness);

	// remove all individuals on the bottom of the list. 
	while (populationGenomesBuffer.size() > populationGenomes.size()) {
		populationGenomesBuffer.pop_back(); 
	}

	// The buffer can now replace the existing populationGenomes
	populationGenomes = populationGenomesBuffer;  
	// ^ This swap should kill all objects no referenced to anymore. Without smart pointers this looks dangerous as hell. 

	// Now delete all nextGenGenomes that didn't make it in the populationGenomes
	for (int i = 0; i < nextGenGenomes.size(); i++) {
		bool deleteGenome = true;
		for (int j = 0; j < populationGenomes.size(); j++) { // I guess I could just compare pointers instead.
			if (nextGenGenomes[i]->individualNumber == populationGenomes[j]->individualNumber) {
				// this particular next genome is in the population so it doesn't need to be deleted. 
				deleteGenome = false;
				break;
			}
		}
		// if the if statement in the previous look was never true, the genome will now be deleted. 
		if (deleteGenome == true) {
			// delete genome file
			stringstream ss;
			ss << settings->repository + "/morphologies" << settings->sceneNum << "/genome" << nextGenGenomes[i]->individualNumber << ".csv";
			string genomeFileName = ss.str();
			stringstream ssp;
			ssp << settings->repository + "/morphologies" << settings->sceneNum << "/phenotype" << nextGenGenomes[i]->individualNumber << ".csv";
			string phenotypeFileName = ssp.str();
			// genomeFileName << indNum << ".csv";
			// cout << "Removing " << nextGenGenomes[i]->individualNumber << endl;
			remove(genomeFileName.c_str());
			remove(phenotypeFileName.c_str());
		}
	}
	populationGenomesBuffer.clear();
	cout << "REPLACED POP RANKED" << endl;
}
