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
