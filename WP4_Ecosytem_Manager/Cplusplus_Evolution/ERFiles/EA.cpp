#include "EA.h"



EA::EA()
{
}


EA::~EA()
{
}

void EA::split_line(string& line, string delim, list<string>& values)
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

void EA::setSettings(shared_ptr<Settings> st, shared_ptr<RandNum> rn)
{
	settings = st;
	randomNum = rn;
}



void EA::init()
{
	// Nothing to initialize anymore. 
}


void EA::createIndividual(int indNum)
{
	populationGenomes[indNum]->create();
}

void EA::loadIndividual(int individualNum, int sceneNum) {

}

void EA::savePopFitness(int generation, vector<float> popfit, vector<int> popIndividuals) {
	cout << "SAVING GENERATION" << endl << endl;
	ofstream savePopFile;
	string saveFileName;
	saveFileName = settings->repository + "/SavedGenerations" + to_string(settings->sceneNum) + ".csv";
	savePopFile.open(saveFileName.c_str(), ios::out | ios::ate | ios::app);
	savePopFile << "generation " << generation << ": ,";
	for (int i = 0; i < popfit.size(); i++) {
		savePopFile /*<< " ind " << i << ": " */ << popfit[i] << ",";
	}
	float avgFitness = 0;
	for (int i = 0; i < popfit.size(); i++) {
		avgFitness += popfit[i];
	}
	avgFitness = avgFitness / popfit.size();
	savePopFile << "avg: ," << avgFitness << ",";
	int bestInd = 0;
	float bestFitness = 0;
	for (int i = 0; i < popfit.size(); i++) {
		if (bestFitness < popfit[i]) {
			bestFitness = popfit[i];
			bestInd = i;
		}
	}
	savePopFile << "ind: ," << popIndividuals[bestInd] << ",";
	savePopFile << "fitness: ," << bestFitness << ",";
	savePopFile << "individuals: ,";
	for (int i = 0; i < popIndividuals.size(); i++) {
		savePopFile << popIndividuals[i] << ",";
	}
	savePopFile << endl;
	savePopFile.close();
	cout << "GENERATION SAVED" << endl;
}


void EA::savePopFitness(int generation) {
	cout << "SAVING GENERATION" << endl << endl;
	ofstream savePopFile;
	string saveFileName;
	saveFileName = settings->repository + "/SavedGenerations" + to_string(settings->sceneNum) + ".csv";
	savePopFile.open(saveFileName.c_str(), ios::out | ios::ate | ios::app);
	savePopFile << "generation " << generation << ": ,";
	for (int i = 0; i < populationGenomes.size(); i++) {
		savePopFile /*<< " ind " << i << ": " */ << populationGenomes[i]->fitness << ",";
	}
	float avgFitness = 0;
	for (int i = 0; i < populationGenomes.size(); i++) {
		avgFitness += populationGenomes[i]->fitness;
	}
	avgFitness = avgFitness / populationGenomes.size();
	savePopFile << "avg: ," << avgFitness << ",";
	int bestInd = 0;
	float bestFitness = 0;
	for (int i = 0; i < populationGenomes.size(); i++) {
		if (bestFitness < populationGenomes[i]->fitness) {
			bestFitness = populationGenomes[i]->fitness;
			bestInd = i;
		}
	}
	savePopFile << "ind: ," << populationGenomes[bestInd]->individualNumber << ",";
	savePopFile << "fitness: ," << bestFitness << ",";
	savePopFile << "individuals: ,";
	for (int i = 0; i < populationGenomes.size(); i++) {
		savePopFile << populationGenomes[i]->individualNumber << ",";
	}
	savePopFile << endl;
	savePopFile.close();
	cout << "GENERATION SAVED" << endl;
}

void EA::loadPopulationGenomes()
{
	unique_ptr<GenomeFactory> gf = unique_ptr<GenomeFactory>(new GenomeFactory);
	for (int i = 0; i < settings->indNumbers.size(); i++) {
		cout << "loading individual " << settings->indNumbers[i] << endl;
		populationGenomes.push_back(gf->createGenome(1, randomNum, settings));
		populationGenomes[i]->loadMorphologyGenome(settings->indNumbers[i], settings->sceneNum);
		//cout << "Make sure the following is correct" << endl;
		populationGenomes[i]->fitness = settings->indFits[i];
		populationGenomes[i]->individualNumber = settings->indNumbers[i];
		//cout << "called fitness = " << popFitness[i] << endl;
	}
	gf.reset();
}
