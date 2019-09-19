/**
	@file EA_MultiNEAT.h
    @authors Matteo de Carlo and Edgar Buchanan
	@brief This class handles evolution using MultiNEAT.
*/
#pragma once
#include "EA.h"
#include <multineat/src/MultiNEAT.h>

class EA_MultiNEAT : public EA{
public:
    EA_MultiNEAT();
    ~EA_MultiNEAT() override;
    // Basic functions from EA class
    void init() override;
    void selection() override;
    void replacement() override;
    void mutation() override;
    void update() override;
    void end() override;
    void createIndividual(int ind) override;
    void loadBestIndividualGenome(int sceneNum) override;
    void setFitness(int ind, float fit) override;
    std::shared_ptr<Morphology> getMorph() override;
};
