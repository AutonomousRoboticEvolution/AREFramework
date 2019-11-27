#include "ARE/EA.h"

#include <memory>

using namespace are;

EA::EA(const settings::ParametersMapPtr& param){
    parameters = param;
//    if(!load_fct_exp_plugin<Genome::Factory>
//            (createGenome,settings->exp_plugin_name,"genomeFactory"))
//        exit(1);
}

EA::~EA()
{
    randomNum.reset();
    parameters.reset();
    for(auto& ind : population)
        ind.reset();
}


void EA::setSettings(const settings::ParametersMapPtr &param,const misc::RandNum::Ptr &rn)
{
    parameters = param;
	randomNum = rn;
}


void EA::epoch(){
    evaluation();
    selection();
    replacement();
    crossover();
    mutation();
}


Individual::Ptr EA::getIndividual(size_t index)
{
    return population[index];
}

//void EA::loadIndividual(int individualNum, int sceneNum) {

//}

//void EA::savePopFitness(int generation, std::vector<float> popfit, std::vector<int> popIndividuals) {
//    std::cout << "SAVING GENERATION" << std::endl << std::endl;
//    std::ofstream savePopFile;
//    std::string saveFileName;

//    saveFileName = settings::cast<settings::String>(parameters->at("#repository"))->value + "/SavedGenerations" + std::to_string(settings->sceneNum) + ".csv";
//    savePopFile.open(saveFileName.c_str(), std::ios::out | std::ios::ate | std::ios::app);
//	savePopFile << "generation " << generation << ": ,";
//	for (int i = 0; i < popfit.size(); i++) {
//		savePopFile /*<< " ind " << i << ": " */ << popfit[i] << ",";
//	}
//	float avgFitness = 0;
//	for (int i = 0; i < popfit.size(); i++) {
//		avgFitness += popfit[i];
//	}
//	avgFitness = avgFitness / popfit.size();
//	savePopFile << "avg: ," << avgFitness << ",";
//	int bestInd = 0;
//	float bestFitness = 0;
//	for (int i = 0; i < popfit.size(); i++) {
//		if (bestFitness < popfit[i]) {
//			bestFitness = popfit[i];
//			bestInd = i;
//		}
//	}
//	savePopFile << "ind: ," << popIndividuals[bestInd] << ",";
//	savePopFile << "fitness: ," << bestFitness << ",";
//	savePopFile << "individuals: ,";
//	for (int i = 0; i < popIndividuals.size(); i++) {
//		savePopFile << popIndividuals[i] << ",";
//	}
//    savePopFile << std::endl;
//	savePopFile.close();
//    std::cout << "GENERATION SAVED" << std::endl;
//}


//void EA::loadPopulationGenomes()
//{

//	for (int i = 0; i < settings->indNumbers.size(); i++) {
//        std::cout << "loading individual " << settings->indNumbers[i] << std::endl;
//        populationGenomes.push_back(createGenome(1, randomNum, settings));
//		populationGenomes[i]->loadGenome(settings->indNumbers[i], settings->sceneNum);
//		//cout << "Make sure the following is correct" << endl;
//        populationGenomes[i]->set_fitness(settings->indFits[i]);
//        populationGenomes[i]->set_individualNumber(settings->indNumbers[i]);
//		//cout << "called fitness = " << popFitness[i] << endl;
//	}

//}
