#pragma once

#include <iostream>
#include <string>
#include <fstream> // ifstream
#include <list>
#include <vector>
//#include "../dynamixel/c++/src/dynamixel_sdk.h"

#define PROTOCOL_VERSION1               1.0                 // See which protocol version is used in the Dynamixel
#define PROTOCOL_VERSION2               2.0
#define DEVICENAME                      "\\\\.\\COM26"      //"/dev/ttyUSB0"      // Check which port is being used on your controller

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
		ENV_SWITCHOBJECTIVE = 10
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
		EDGARSAMAZINGMORPHOLOGY = 10,
		TISSUE_DIRECT = 20,
		TISSUE_GMX = 21,
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

	enum OS {
		WINDOWS,
		LINUX
	};

	/*enum LSystemType{
	DEFAULT_LSYSTEM = 0,
	CUBE_LSYSTEM = 1,
	JOINT_AND_CUBE_LSYSTEM = 2,
	MODULAR_LSYSTEM = 3,
	LIGHT_LSYSTEM = 4,
	};*/

	InstanceType instanceType = INSTANCE_REGULAR;
	EvolutionType evolutionType = STEADY_STATE;//EMPTY_RUN; // STEADY_STATE; //STEADY_STATE;//EMBODIED;
	FitnessType fitnessType = MOVE;
	EnvironmentType environmentType = DEFAULT_ENV;
	MoveDirection moveDirection = DISTANCE_XY;
	MorphologyType morphologyType = CAT_MORPHOLOGY; // CUSTOM_MODULAR_MORPHOLOGY; //MODULAR_LSYSTEM;//CAT_MORPHOLOGY;// MODULAR_LSYSTEM;
	ControlType controlType = ANN_DEFAULT;
	SelectionType selectionType = RANDOM_SELECTION;
	ReplacementType replacementType = RANDOM_REPLACEMENT;

//	EvolutionType evolutionType = GENERATIONAL; // not implemented yet

	int indCounter;
	int sceneNum;
	bool sendGenomeAsSignal = false;
	bool client;
	float mutationRate = 0.1;
	float morphMutRate = 0.1;
	int generation = 0;
	int maxGeneration = 1000;
	int xGenerations = 4;
	int populationSize = 3;
	int individualCounter = 0;
	int crossover = 0;
	float crossoverRate = 0;
	int ageInds = 0;
	int maxAge = 0; // generations
	int minAge = 0; // age after which death can occur
	float deathProb = 0; // by default no death
	vector<int> indNumbers;
	vector<float> indFits;
	int seed = 0;
	// modular parameters
	int amountModules = 5;
	int useVarModules = 0;
	float maxForce = 1.5;
	float maxForceSensor = 80; // N*m
	int consecutiveThresholdViolations = 10;
	int maxAmountModules = 20;
	vector<int> moduleTypes;
	vector<vector<int> > maxModuleTypes;
	string repository = "files";
	int initialModuleType = 1; // initial module for direct encoding, similar to axiom of L-System
	float energyDissipationRate = 0.00;
	int lIncrements = 3;
	int bestIndividual = 0;
	int loadInd = 0;
	bool verbose = false;
	Colorization colorization = COLOR_NEURALNETWORK;
	
	bool killWhenNotConnected = true;
	bool shouldReopenConnections = false;

	// neural network parameters
	int initialInputNeurons = 3;
	int initialInterNeurons = 1;
	int initialOutputNeurons = 3;
	int initialAmountConnectionsNeurons = 3;
	int	maxAddedNeurons = 4;
	bool savePhenotype = true;
	vector<int> envObjectHandles;

	void openPort();

//	dynamixel::PacketHandler *packetHandler1;
//	dynamixel::PacketHandler *packetHandler2;
//	dynamixel::PortHandler *portHandler;

	void readSettings();
	void saveSettings();
	bool portOpen = false;

	// SETTERS
	void setRepository(std::string repository)
	{
		this->repository = repository; 
		std::cout << "setting repository to " << repository << std::endl;
	}

private:
	void split_line(string& line, string delim, list<string>& values);
};
