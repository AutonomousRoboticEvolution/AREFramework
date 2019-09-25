/**
	@file ER_VREP.h
    @authors Frank Veenstra, Edgar Buchanan, Matteo de Carlo and Wei Li
	@brief .
*/

#pragma once

#include <memory>
#include <string>
#include "ER.h"
#include "env/Environment.h"
#include "env/EnvironmentFactory.h"
#include "v_repLib.h"
#include "EA_Factory.h"

class ER_VREP : public CER
{
public:
	ER_VREP();
	virtual ~ER_VREP();
	typedef std::shared_ptr<Morphology> MorphologyPointer;

	/// This method reads the instanceType member from settings class. According to its value ER initializes as a
	/// server or as server-client.
	void initialize() override;  // This function overrides the CER function

	/// This method Instatiates genome factory, enviroment and EA. Also settings for the enviroment and EA are loaded.
	/// Finally, this method initializes EA and environment.
	void initializeSimulation() override; // Loading Environment

	/// Initializes ER as a server to accept genomes from client.
	void initializeServer();

	//! TODO: Check with FV. This method is not defined!
	void initializeRobot(); // For connecting to actual robot.

	/// Calculate the fitness value of the robot
	float fitnessFunction(const MorphologyPointer& morph);

	/// Mark the simulation time step
	float simulationTime = 0;
	/// Pointer to the Environment class
	std::shared_ptr<Environment> environment;

    /// Reference to a morphology object that can be updated in V-REP (phenotype)
    std::shared_ptr<Morphology> currentMorphology;
	int individualToBeLoaded = -1;
	/// Get the morphology reference
    std::shared_ptr<Morphology> getMorphology(Genome* g);

    std::shared_ptr<EA> ea;
	///store the current genome
    std::shared_ptr<Genome> currentGenome;
	///used to create a genome
    std::shared_ptr<GenomeFactoryVREP> genomeFactory;

	/// Logs information about the fitness of each individual, best individual and list of individuals.
	void saveSettings() override;

	/// These functions used to be in newGenome
	/// Loads specific individual.
	bool loadIndividual(int individualNum);

	/// Loads the best individual of a specific scene.
	void loadBestIndividualGenome(int sceneNum);

    ///////////////////////
    /// V-REP Functions ///
    ///////////////////////
    /// Initializes ER as a server to accept genomes from client. If framework is server than just hold information for
    /// one genome. Else, initilizes the first population of individuals.
    void startOfSimulation();

    /// This function is called every simulation step. Note that the behavior of the robot drastically changes when
    /// slowing down the simulation since this function will be called more often. All simulated individuals will be
    /// updated until the maximum simulation time, as specified in the environment class, is reached.
    void handleSimulation();

    /// At the end of the simulation the fitness value of the simulated individual is retrieved and stored in the
    /// appropriate files.
    void endOfSimulation();
};
