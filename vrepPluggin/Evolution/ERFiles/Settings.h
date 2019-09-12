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
    /// Type of fitness function
	enum FitnessType {
        /// Movement
		MOVE = 0,
		/// Deprecated
		SOLAR = 1,
        /// Deprecated
		SWIM = 2,
        /// Deprecated
		MOVE_AND_SOLAR = 3,
        /// Deprecated
		FITNESS_JUMP = 4
	};

    enum StartingCondition {
        COND_LOAD_BEST,
        COND_RUN_EVOLUTION_CLIENT,
        COND_RUN_EVOLUTION_SERVER,
        COND_LOAD_SPECIFIC_INDIVIDUAL
    };

    /// This enum defines which environment is loaded
	enum EnvironmentType {
	    /// Default environment
		DEFAULT_ENV = 0,
        /// Deprecated
        SUN_BASIC = 1,
        /// Deprecated
        SUN_CONSTRAINED = 2,
        /// Deprecated
		SUN_BLOCKED = 7,
        /// Deprecated
		SUN_MOVING = 3,
        /// Deprecated
		SUN_CONSTRAINED_AND_MOVING = 4,
        /// Deprecated
		WATER_ENV = 6,
		/// Loads an environment with a heightmap
		ROUGH = 5,
        /// Deprecated
        CONSTRAINED_MOVING_SUN = 8,
        /// Deprecated
		ENV_FOURSUNS = 9,
        /// Deprecated
		ENV_SWITCHOBJECTIVE = 10,
        /// Used for the phototaxis scenario
		ENV_PHOTOTAXIS = 11
	};

	/// Enum defining the selection method used by the EA
	enum SelectionType {
        RANDOM_SELECTION,
		PROPORTIONATE_SELECTION
	};

	enum ReplacementType {
		RANDOM_REPLACEMENT = 0,
		REPLACE_AGAINST_WORST = 1,
		PARETOMORPH_REPLACEMENT = 2
	};

	/// Can be used to color the modules
	enum Colorization {
	    /// This colors the robot based on the L-System genotypes expressed
		COLOR_LSYSTEM = 0,
		/// Colors modules based on the outputs of the neural networks
		COLOR_NEURALNETWORK = 1,
        /// Deprecated
        COLOR_NEURALNETWORKANDLSYSTEM = 2
	};
    /// Specify which direction the fitness function should look at.
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
    /// Used to define which morphology to evolve
	enum MorphologyType
	{
		CAT_MORPHOLOGY = 0,
		MODULAR_LSYSTEM = 1,
		NO_MORPHOLOGY = 2, // I don't know why you would want this though
		MODULAR_CPPN = 3,
		MODULAR_DIRECT = 4,
		CUSTOM_MORPHOLOGY = 5,
        /// Deprecated
        CUSTOM_MODULAR_MORPHOLOGY = 6, // not working
        /// Deprecated
        QUADRUPED_GENERATIVE = 7, // head missing
        /// Deprecated
		QUADRUPED_DIRECT = 8, // not working.
        /// Deprecated
		CUSTOM_SOLAR_GENERATIVE = 9,
		TISSUE_DIRECT = 20,
		TISSUE_GMX = 21,
		INTEGRATION = 22,
		/// Used to load a modular robot from a phenotype file (phenotype<individualNumber>.csv)
		MODULAR_PHENOTYPE = 11,
		CPPN_NEAT_MORPH = 12


	};

	enum ControlType {
	    /// Neural network that changes the topology of the network as well
		ANN_DEFAULT = 0,
		/// Neural network that doesn't change it's topology
		ANN_CUSTOM = 1,
		/// Compositional pattern producing network as interpreted by Frank; not original implementation from stanley (2007)
		ANN_CPPN = 2,
	//	ANN_NEAT = 4,
	    /// Used to define the directionality of communication between neural networks of neighbouring modules
		ANN_DISTRIBUTED_UP = 3,
        /// Used to define the directionality of communication between neural networks of neighbouring modules
		ANN_DISTRIBUTED_DOWN = 4,
        /// Used to define the directionality of communication between neural networks of neighbouring modules
		ANN_DISTRIBUTED_BOTH = 5,
        /// Should be used for calling the ANN used in NEAT
		ANN_NEAT = 6,
		/// Can be used to specify leaky integrators in the ANN
		LEAKY_ANN_CONTROL = 7,  // TODO: adjustable leakiness parameter
        /// Fixed structure ANN
		ANN_FIXED_STRUCTURE = 8
	};

	enum EvolutionType {
	    /// deprecated
		RANDOM_SEARCH = 0,
		STEADY_STATE = 1,
		GENERATIONAL = 2,
        /// deprecated
		EA_NEAT = 3,
		EMBODIED_EVOLUTION = 4,
	};

	/// Instance type defines in what mode the "Evolutionary Robotics" plugin runs // TODO: define name
	enum InstanceType {
	    /// Single thread (local)
		INSTANCE_REGULAR = 0,
		/// Waits for genome signals for using in parallel execution
		INSTANCE_SERVER = 1,
        /// deprecated
        INSTANCE_DEBUGGING = 2
	};

	/// Can be used when there are differences between operating systems. Not encountered yet.
	enum OS {
		WINDOWS,
		LINUX
	};


    StartingCondition startingCondition = COND_RUN_EVOLUTION_CLIENT;
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


	/*enum LSystemType{
	DEFAULT_LSYSTEM = 0,
	CUBE_LSYSTEM = 1,
	JOINT_AND_CUBE_LSYSTEM = 2,
	MODULAR_LSYSTEM = 3,
	LIGHT_LSYSTEM = 4,
	};*/

    /// modular parameters
    int useVarModules = 0;			// There was an option to mutate the number of modules in the L-System (TODO: DELETE : DEPRECATED)
    float maxForce = 1.5;			// Maximum force to be set on all joints
    float maxForceSensor = 80;		// Maximum force set on all force sensors (N*m)
    float maxForce_ForceSensor = 0.01;// Maximum force set on all force sensors (by default) TODO: should replace maxForceSensor
    float maxTorque_ForceSensor = 1000;    // Maximum torque set on all force sensors (by default)

//	EvolutionType evolutionType = GENERATIONAL; // not implemented yet

    int consecutiveThresholdViolations = 10; // Determining how many consecutive force sensor threshold violations will lead to the module breaking
	int maxNumberModules = 20;		// Maximum number of modules allowed in phenotype
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

	void readSettings();			// Reading a .csv settings file
	void saveSettings();			// Saving a .csv settings file
	bool autoDeleteSettings = true;

	/// Viability variables
	// Allow organs above printing bed
	bool bOrgansAbovePrintingBed = false;
	// Allow colliding organs
    bool bCollidingOrgans = false;
    // Allow any orientation for organs
    bool bNonprintableOrientations = false;
    // Any number of organs
    bool bAnyOrgansNumber =false;

	bool loadFromQueue = false;

	/**
		@brief Set the repository for saving data
	*/
    void setRepository(std::string repository);

private:
	void split_line(string& line, string delim, list<string>& values);
};
