#pragma once
#ifndef EA_CGA_H
#define EA_CGA_H

#include "ARE/EA.h"
#include <memory>


namespace are {

class CGA : public EA
{
public:
    CGA();
    CGA(const settings::ParametersMapPtr& param) : EA(param){}
    ~CGA();

    void setFitness(int individual, float fitness);


    // base functions of EA
    void init();
    void selection();
    void replacement();
    void mutation();

    void initializePopulation();
    void selectIndividuals(); // random tournament
    void replaceIndividuals(); // random
    void replaceNewIndividual(int indNum, int sceneNum, float fitness);
    void createIndividual(int indNum);

    virtual std::shared_ptr<Morphology> getMorph() { return std::shared_ptr<Morphology>(); };  // not used in this class // NEAT hack
    virtual void loadBestIndividualGenome(int sceneNum) {}; // not used
    void createNewGenRandomSelect();
    void replaceNewPopRandom();
    std::shared_ptr<Morphology> copyMorphologyGenome(std::shared_ptr<Morphology> parentMorphology);
private:
    std::vector<Individual::Ptr> childrens;

};

}//are

#endif //EA_GENERATIONAL_H
