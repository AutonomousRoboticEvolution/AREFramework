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

	/// This method reads the instanceType member from settings class. According to its value ER initializes as a server or as server-client.
	void initialize();  // This function overrides the CER function

	/**
		@brief This method Instatiates genome factory, enviroment and EA. Also settings for the enviroment and EA are loaded. Finally, this method initializes EA and environment.
	*/
	void initializeSimulation(); // Loading Environment


	/**
		@brief Initializes ER as a server to accept genomes from client.
	*/
	void initializeServer();

	//! TODO: Check with FV. This method is not defined!
	void initializeRobot(); // For connecting to actual robot.

	// V-REP
	/**
		@brief Initializes ER as a server to accept genomes from client. If framework is server than just hold information for one genome. Else, initilizes the first population of individuals.
	*/
	void startOfSimulation();

	/**
		@brief This function is called every simulation step. Note that the behavior of the robot drastically changes when slowing down the simulation since this function will be called more often. All simulated individuals will be updated until the maximum simulation time, as specified in the environment class, is reached.
	*/
	void handleSimulation();

	/**
		@brief At the end of the simulation the fitness value of the simulated individual is retrieved and stored in the appropriate files.
	*/
	void endOfSimulation();

	/**
		@brief Calculate the fitness value of the robot
		@param morph The pointer of the robot morphology
	*/
	float fitnessFunction(MorphologyPointer morph);

	///mark the simulation time step
	float simulationTime = 0;
	/// Pointer to the Environment class
	std::shared_ptr<Environment> environment;

	// To keep track of the position of the robot
    /// deprecated
    std::string mainHandleName;
    /// Reference to a morphology object that can be updated in V-REP (phenotype)
    std::shared_ptr<Morphology> currentMorphology;
	int individualToBeLoaded = -1;
	/**
		@brief Get the morphology reference
		@param g the reference of the Genome
	*/
    std::shared_ptr<Morphology> getMorphology(Genome* g);

    std::shared_ptr<EA> ea;
	///store the current genome
    std::shared_ptr<Genome> currentGenome;
	///used to create a genome
    std::shared_ptr<GenomeFactoryVREP> genomeFactory;

	/**
		@brief Logs information about the fitness of each individual, best individual and list of individuals.
	*/
	void saveSettings();

	// These functions used to be in newGenome
	/**
		@brief Loads specific individual.
		@param invidualNum represent the ID of the robot.
	*/
	bool loadIndividual(int individualNum);

	/**
		@brief Loads the best individual of a specific scene.
		@param sceneNum represents the scene from where the best individual will be loaded.
	*/
	void loadBestIndividualGenome(int sceneNum);
};