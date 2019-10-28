#include "ARE/EA.h"

#include <memory>


EA::EA(const Settings& st){
    settings = std::make_shared<Settings>(st);
    if(!load_fct_exp_plugin<Genome::Factory>
            (createGenome,settings->exp_plugin_name,"genomeFactory"))
        exit(1);
}

EA::~EA()
{
}

void EA::split_line(std::string& line, std::string delim, std::list<std::string>& values)
{
	size_t pos = 0;
    while ((pos = line.find(delim, (pos + 0))) != std::string::npos) {
        std::string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}
    while ((pos = line.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}

void EA::setSettings(std::shared_ptr<Settings> st, std::shared_ptr<RandNum> rn)
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

void EA::savePopFitness(int generation, std::vector<float> popfit, std::vector<int> popIndividuals) {
    std::cout << "SAVING GENERATION" << std::endl << std::endl;
    std::ofstream savePopFile;
    std::string saveFileName;
    saveFileName = settings->repository + "/SavedGenerations" + std::to_string(settings->sceneNum) + ".csv";
    savePopFile.open(saveFileName.c_str(), std::ios::out | std::ios::ate | std::ios::app);
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
    savePopFile << std::endl;
	savePopFile.close();
    std::cout << "GENERATION SAVED" << std::endl;
}


void EA::savePopFitness(int generation)
{
    std::cout << "SAVING GENERATION" << std::endl << std::endl;
    std::ofstream savePopFile;
    std::string saveFileName;
    saveFileName = settings->repository + "/SavedGenerations" + std::to_string(settings->sceneNum) + ".csv";
    savePopFile.open(saveFileName.c_str(), std::ios::out | std::ios::ate | std::ios::app);
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
    savePopFile << std::endl;
	savePopFile.close();
    std::cout << "GENERATION SAVED" << std::endl;
}

void EA::loadPopulationGenomes()
{

	for (int i = 0; i < settings->indNumbers.size(); i++) {
        std::cout << "loading individual " << settings->indNumbers[i] << std::endl;
        populationGenomes.push_back(createGenome(1, randomNum, settings));
		populationGenomes[i]->loadGenome(settings->indNumbers[i], settings->sceneNum);
		//cout << "Make sure the following is correct" << endl;
		populationGenomes[i]->fitness = settings->indFits[i];
		populationGenomes[i]->individualNumber = settings->indNumbers[i];
		//cout << "called fitness = " << popFitness[i] << endl;
	}

}
