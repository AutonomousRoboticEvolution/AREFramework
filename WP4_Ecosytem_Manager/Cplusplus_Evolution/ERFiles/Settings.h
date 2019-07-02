#pragma once

#include <iostream>
#include <string>
#include <fstream> // ifstream
#include <list>
#include <vector>

/* Dynamixel artefact : To be used in experimental version
//#include "../dynamixel/c++/src/dynamixel_sdk.h"
#define PROTOCOL_VERSION1               1.0                 // See which protocol version is used in the Dynamixel
#define PROTOCOL_VERSION2               2.0
#define DEVICENAME                      "\\\\.\\COM26"      //"/dev/ttyUSB0"      // Check which port is being used on your controller
*/

using namespace std;

class Settings
{
public:
	Settings();
	virtual ~Settings();
	enum FitnessType {
		MOVE = 0,
		SOLAR = 1,
		SWIM = 2,
		MOVE_AND_SOLAR = 3,
		FITNESS_JUMP = 4
	};

	enum EnvironmentType {
		DEFAULT_ENV = 0,
		SUN_BASIC = 1,
		SUN_CONSTRAINED = 2,
		SUN_BLOCKED = 7,
		SUN_MOVING = 3,
		SUN_CONSTRAINED_AND_MOVING = 4,
		WATER_ENV = 6,
		ROUGH = 5,
		CONSTRAINED_MOVING_SUN = 8,
		ENV_FOURSUNS = 9,
		ENV_SWITCHOBJECTIVE = 10,
		ENV_PHOTOTAXIS = 11
		//		DEFAULT = 0,
		//		WALLS = 2,
		//		MOVEMENT = 3,
		//		JUMP = 4
	};

	enum SelectionType {
		RANDOM_SELECTION,
		PROPORTIONATE_SELECTION
	};

	enum ReplacementType {
		RANDOM_REPLACEMENT = 0,
		REPLACE_AGAINST_WORST = 1,
		PARETOMORPH_REPLACEMENT = 2
	};

	enum Colorization {
		COLOR_LSYSTEM = 0,
		COLOR_NEURALNETWORK = 1,
		COLOR_NEURALNETWORKANDLSYSTEM = 2
	};

	enum MoveDirection {
		DISTANCE_X = 0,
		DISTANCE_Y = 1,
		DISTANCE_Z = 2, // jump
		DISTANCE_XY = 3,
		DISTANCE_XZ = 4,
		DISTANCE_YZ = 5,
		DISTANCE_XYZ = 6,
		FORWARD_X = 7,
		FORWARD_Y = 8,
		FORWARD_XY = 9,
		FORWARD_NEGX = 10,
		FORWARD_NEGY = 11,
		FORWARD_NEGXY = 12
	};

	enum MorphologyType
	{
		CAT_MORPHOLOGY = 0,
		MODULAR_LSYSTEM = 1,
		NO_MORPHOLOGY = 2, // I don't know why you would want this though
		MODULAR_CPPN = 3,
		MODULAR_DIRECT = 4,
		CUSTOM_MORPHOLOGY = 5,
		CUSTOM_MODULAR_MORPHOLOGY = 6, // not working 
		QUADRUPED_GENERATIVE = 7, // head missing
		QUADRUPED_DIRECT = 8, // not working. 
		CUSTOM_SOLAR_GENERATIVE = 9,
		TISSUE_DIRECT = 20,
		TISSUE_GMX = 21,
		INTEGRATION = 22,
		MODULAR_PHENOTYPE = 11,
	};

	enum ControlType {
		ANN_DEFAULT = 0,
		ANN_CUSTOM = 1,
		ANN_CPPN = 2,
	//	ANN_NEAT = 4,
		ANN_DISTRIBUTED_UP = 3,
		ANN_DISTRIBUTED_DOWN = 4,
		ANN_DISTRIBUTED_BOTH = 5,
		ANN_NEAT = 6
		//old
//		DEFAULT_CONTROL = 1,
//		STATIONARY_CONTROL = 2,
//		DEFAULT_ANN_CONTROL = 3,
//		LEAKY_ANN_CONTROL = 4,
//		NEAT_CONTROL = 5
	};

	enum EvolutionType {
		RANDOM_SEARCH = 0,
		STEADY_STATE = 1,
		GENERATIONAL = 2,
		EMBODIED_EVOLUTION = 4,
		EMPTY_RUN = 5,
		AFPO
	};

	enum InstanceType {
		INSTANCE_REGULAR = 0,
		INSTANCE_SERVER = 1,
		INSTANCE_DEBUGGING = 2
	};
	
	// Can be used when there are differences between operating systems. Not encountered yet.
	enum OS {
		WINDOWS,
		LINUX
	};

	InstanceType instanceType = INSTANCE_REGULAR;		// Whether the code runs in client or server mode
	EvolutionType evolutionType = STEADY_STATE;			// Type of evolutionary algorithm used
	FitnessType fitnessType = MOVE;						// Fitness type
	EnvironmentType environmentType = DEFAULT_ENV;		// Environment type
	MoveDirection moveDirection = DISTANCE_XY;			// This is used to modify the fitness function by specifying the direction. By default 
														// it is set to DISTANCE_XY which is the horizontal distance moved
	MorphologyType morphologyType = CAT_MORPHOLOGY;		// Type of morphology used
	ControlType controlType = ANN_DEFAULT;				// Specifies the type of controller used
	SelectionType selectionType = RANDOM_SELECTION;		// What is the selection operator (TODO: Not used yet but adviced)
	ReplacementType replacementType = RANDOM_REPLACEMENT;// What is the replacement operator (TODO: Not used yet but adviced)

	int indCounter;					// record the number of individuals
	int sceneNum;					// Experiment ID
	bool sendGenomeAsSignal = false;// When false, genome is saved as csv and loaded in server. true opens a port and passes the genome as a stringstream from client to server
	bool client;					// Stating whether the instance is in client mode. DEPRECATED
	float mutationRate = 0.1; 		// mutation rate...
	float morphMutRate = 0.1;		// morphology mutation rate...
	int generation = 0;				// Tracks the generation. Used in single thread.
	int maxGeneration = 1000;		// maximum number of generations
	int xGenerations = 4;			// for debugging, this specifies after how many generations the simulator should quit and reopen
	int populationSize = 3; 		// Population size
	int individualCounter = 0;		// This variable counts the total number of individuals that have been evaluated during one evolutionary runs
	int crossover = 0;				// Using crossover (int value for potentially using different crossover strategies)
	float crossoverRate = 0;		// Rate at which crossover happens. Variable can be used like mutation rate
	
	int ageInds = 0;				// Whether age should be used (deprecated) (TODO: DELETE : DEPRECATED)
	int maxAge = 0;					// An absolute maximum age could be set. (TODO: DELETE : DEPRECATED)
	int minAge = 0;					// Minimum age, death cannot occur before this (TODO: DELETE : DEPRECATED)
	float deathProb = 0;			// Probability of individuals being removed at random from the population (TODO: DELETE : DEPRECATED)
	
	vector<int> indNumbers;			// Vector storing the IDs of the individuals in the current population. This vector can be used to load all individuals from a specific generation
	vector<float> indFits;			// Vector storing the fitness values of the individuals in the current population
	int seed = 0;					// Random seed passed to the random number generator (TODO: random number generator names are slightly different)

	/// modular parameters
	int useVarModules = 0;			// There was an option to mutate the number of modules in the L-System (TODO: DELETE : DEPRECATED)
	float maxForce = 1.5;			// Maximum force to be set on all joints
	float maxForceSensor = 80;		// Maximum force set on all force sensors (N*m)
	int consecutiveThresholdViolations = 10; // Determining how many consecutive force sensor threshold violations will lead to the module breaking
	int maxNumberModules = 20;		// Maximum number of modules allowed
	vector<int> moduleTypes;		// Vector storing the module types
	vector<vector<int> > maxModuleTypes; /* Vector storing how many of each module type can be expressed 
										 NOTE: this takes into account how many times the module type is expressed in total! So if you choose to append module 
										 type 4 to be appended twice in module Types, both of them will be counted when checking for the maximum module types */

	int numberOfModules = 5;		// The number of module types used by the L-System (note: not maximum number of modules!!!)


	string repository = "files";	/* The repository seen from the working directory to look for the settings.csv and the genomes.
									NOTE: the morphologies subfolders are not automatically created! */
	int initialModuleType = 1;		// initial module for direct encoding, similar to axiom of L-System. This is the cube module by default

	float energyDissipationRate = 0.00;// Can be used to simulate how much energy is consumed by specific modules (TODO: DELETE : DEPRECATED)
	int lIncrements = 3;			// Specifies how many times the L-System is iterated over or specifies 
	int bestIndividual = 0;			// Tracks the best individual of every generation
	int loadInd = 0;				// Specifies if an individual should be loaded (used in client-server mode)
	bool verbose = false;			// Specifies whether to output print statement. 
	Colorization colorization = COLOR_NEURALNETWORK; // To specify how to color the robot morphology. This is useful for debugging. 
	
	bool killWhenNotConnected = true;// Whether to send a message to shutdown V-REP servers if the connection with the client is lost
	bool shouldReopenConnections = false;// Sometimes, connections between client and servers are lost, this specifies whether the connections should be reopened. 
	bool createPatternNeurons = false;// Pattern neurons are basically sinusoidal wave functions now

	// neural network parameters
	int initialInputNeurons = 3;	// Initial number of input neurons
	int initialInterNeurons = 1;	// Initial number of interneurons
	int initialOutputNeurons = 3;	// Initial number of output neurons
	int initialAmountConnectionsNeurons = 3; // Initial number of connections for each neuron
	int	maxAddedNeurons = 4;		// The maximum number of neurons that can be added in one call of the mutation operator
	bool savePhenotype = true;		// Whether to save the phenotype or not
	vector<int> envObjectHandles;	// Object handles of the environment objects (TODO: DELETE : DEPRECATED)

	void openPort();				// Opening serial port (TODO: DELETE: DEPRECATED)

	/* Duynamixel artefact: useful in experimental version 
//	dynamixel::PacketHandler *packetHandler1;
//	dynamixel::PacketHandler *packetHandler2;
//	dynamixel::PortHandler *portHandler;
	*/

	void readSettings();			// Reading a .csv settings file
	void saveSettings();			// Saving a .csv settings file
	bool portOpen = false;			// (TODO: DELETE : DEPRECATED)

	/**
		@brief Set the repository for saving data
	*/
	void setRepository(std::string repository)
	{
		this->repository = repository; 
		std::cout << "setting repository to " << repository << std::endl;
	}

private:
	void split_line(string& line, string delim, list<string>& values);
};
