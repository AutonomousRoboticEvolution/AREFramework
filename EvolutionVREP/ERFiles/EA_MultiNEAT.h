/**
	@file EA_MultiNEAT.h
    @authors Matteo de Carlo and Edgar Buchanan
	@brief This class handles evolution using MultiNEAT.
*/
#pragma once

#include "EA.h"
#include <multineat/Parameters.h>

class EA_MultiNEAT : public EA
{
public:
    EA_MultiNEAT();
    EA_MultiNEAT(const Settings&);
    ~EA_MultiNEAT() override;

    // Basic functions from EA class
    void init() override;

    void epoch();

    void selection() override;

    void replacement() override;

    void mutation() override;

    void update() override;

    void end() override;

    void createIndividual(int ind) override;

    void loadBestIndividualGenome(int sceneNum) override;

    void setFitness(int ind, float fit) override;

    std::shared_ptr<Morphology> getMorph() override;

    void savePopulation(const std::string& );

    void savePopFitness(int generation) override;

private:
    void initializePopulation(const NEAT::Parameters &params);

private:
    std::unique_ptr<NEAT::Population> population;
    std::shared_ptr<Morphology> morph;
};
