#pragma once

#include <memory>
#include <string>
#include <vector>
#include "EA.h"


class EA_SteadyState : public EA
{
public:
    EA_SteadyState() : EA(){}
    ~EA_SteadyState(){}

	void split_line(std::string & line, std::string delim, std::list<std::string>& values) override;

	// base functions of EA
	void init() override;
	void selection() override;
	void replacement() override;
	void mutation() override;
	void update() override {}; // not used in this class
	void createIndividual(int indNum) override {}; // not used in this class
	/// used for NEAT
	void end() override {}; // not used in this class

	/**
		@brief This method initilizes a population of genomes
	*/
	void initializePopulation();
	virtual void saveGenome(const std::shared_ptr<Genome> &genome);
	void setFitness(int individual, float fitness) override;
	std::shared_ptr<Morphology> getMorph() override { return std::shared_ptr<Morphology>(); };  // not used in this class // NEAT hack
	void loadBestIndividualGenome(int sceneNum) override {}; // not used here
    /**
        @brief This method creates a new morphology base on a randomly selected parent (copy)
    */
	void createNewGenRandomSelect();
    void replaceNewPopRandom(int numAttempts);
	void replaceNewRank();
	void loadPopulationGenomes(int scenenum);
};

