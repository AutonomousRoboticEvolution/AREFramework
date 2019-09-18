// ------------------------------------------------------------------
// The header file used for the ENN my master's thesis. 
// Note that sensory neurons, interneurons and motor neurons 
// are called inputNeurons, interNeurons and OutputNeurons instead.
// Some artefacts of the original plugin can still be detected 
// but can be ignored


// creationCounter is not used in this version!
// ------------------------------------------------------------------

// BubbleRob artefact:
// "
// Copyright 2006-2014 Dr. Marc Andreas Freese. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.1.0 on January 20th 2014
// "


#pragma once

#include <vector>
#include "DefaultGenome.h"
#include "VREPFunctions.h"
#include "Environment.h"
#include "Agent.h"
#include "Population.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace std;

enum simulationSetup
{
	EVOLUTION, 
	RECALLCUSTOM,
	COEVOLUTION, 
	OBSTACLEEVOLUTION,
	RECALLBEST,
	RECALLPOP,
	GROWEVOLUTION,
	ROTATE,
	RECALLROTATOR,
	DYNAMIC
};


class CCat
{
public:
	CCat();
	virtual ~CCat();
	//CCatContainer();
	//virtual ~CCatContainer();

	// Following 4 needed to keep a scene object's custom data synchronized with CBubbleRob's data (_maxVelocity, which is stored inside a scene object)
	void synchronizeSceneObject();
	void synchronize();
	static void putCatTagToSceneObject(int objectHandle, float maxVelocity);

	void setMaxVelocity(float maxVel);
	float getMaxVelocity();

	// The three main classes that are influenced by the simulation state of V-REP:
	void startOfSimulation();
	bool endOfSimulation();
	void handleSimulation();
	void initializeParameters();

	// ENN classes:
	void mutation();						// mutates
	void checkMutation();
	void crossover();
	void initializer();						// initializes a new individual
	void activityVisualizer();				// idea was to use bitmaps to track the activity of neurons, altough this was too confusing to implement and not a priority
	
	//	void handleInitializer();				/* this class identifies the joints and limbs of the robot in V-REP, because this class is called fiirst, it is usefull to entail 
	//										functions that require to be called right before or after the robot is identified.  */

	// following code has artefacts of the bubblerob robot
protected:
};

/*
class CAgentBlueprint
{
public:
	CAgentBlueprint();
	virtual ~CAgentBlueprint();
	void getConstructionBlueprint(string, vector<vector<float>>, vector<vector<float>>, vector<vector<float>>, vector<float>, vector<int>, 
		vector <int>, vector<vector<float>>, vector<vector<float>>, vector<vector<float>>, vector<float>, vector<int>, vector <int>,
		vector<vector<float>>, vector<float>, vector<float>);

	string b_mainHandleName;
	vector<vector<float>> a_objectOrientation; // store initial Robot values. 
	vector<vector<float>> a_objectPosition;
	vector<vector<float>> a_objectSizes;
	vector<float> a_objectMass;
	vector<int> a_objectParent;
	vector<int> a_objectType;

	vector<vector<float>> a_jointOrientation; // store initial Robot values. 
	vector<vector<float>> a_jointPosition;
	vector<vector<float>> a_jointSizes;
	vector<float> a_jointMass;
	vector<int> a_jointParent;
	vector<int> a_jointType;
	vector<vector<float>> a_jointMaxMinAngles;
	vector<float> a_jointMaxVelocity;
	vector<float> a_jointMaxForce;
	
	//int agentAmountObjects;					// includes joints
	//vector<int> objectType;					// currently solid cuboid or revolute joint
	//vector<vector<float>> objectPositions;	// 3 dimensional position based on world
//	vector<vector<float>> objectOrientations;	// euler angles based on world
//	vector<vector<float>> objectSize;			// note that joints have only two parameters specifying size 
//	vector<int> agentObjectHandles;				// gives every object a handle ID
//	vector<int> objectParents;					// each object accept the main has a parent and is specified. 
//	vector<int> agentJointHandles;				// the joint handles are used by the ENN 
//	vector<int> agentJointTypes;				// specifying the type for each jointHandle
private:
}; */

/*
class CIndividualGenome
{
public:
	CIndividualGenome();
	~CIndividualGenome();
	// plant
	int amountIncrement = 10; 
	// plant end

	void currentGenomeChecker();
	// functions
	
	vector<float> g_acquiredActivationLevels;
	int g_amountSenseParts;
	vector<int> g_jointHandles;
	vector<int> g_tactileHandles;
	vector<int> g_objectHandles;

	int g_tactileSensingStartsAt;
	int g_proprioceptionStartsAt;
	int g_vestibularSystemStartsAt;
//	CAgentBlueprint agentBlueprint;
//	void storeAgentBlueprint(string, vector<vector<float>>, vector<vector<float>>, vector<vector<float>>, vector<float>, vector<int>,
//		vector <int>, vector<vector<float>>, vector<vector<float>>, vector<vector<float>>, vector<float>, vector<int>, vector <int>,
//		vector<vector<float>>, vector<float>, vector<float>);
//	bool blueprintStored = false; 

	void printIndividualInfo();
	int individualNumber;

	vector<vector<vector<float>>> g_lStateProperties;
	vector<vector<float>> g_lStateChanger;
	vector<vector<float>> g_lStateSequencer;
	vector<bool> leafState;



	// --------------------------------------------------------
	// Initial Parameters: these are saved and stored in .csv files. 
	// --------------------------------------------------------
	vector<float> g_jointActivity;
	// alternative design
	int g_amountNeurons;
	// types can be: input, output, recurrent, patterned, CPG.
//	vector<bool> neuronType;
//	vector<int> neuronConnectionAmount;
//	vector<vector<int>> neuronConnections;
//	vector<vector<float>> neuronWeights;
	
	// future app, neuron development
	
	// placement in genome
//	vector<int> genomeOrder; // = { 0, 1, 2, 3, 4 };

	// classic design
	// input --------------------------------
	// specify amount and create new vector
	int g_amountInputNeurons;
	vector<int> g_inputNeuronConnectionAmount;
	// The connections the input neurons have to other specific neurons
	vector<vector<int>> g_inputNeuronConnections;
	// the weights of all inputneurons
	vector<vector<float>> g_initialInputNeuronWeights;
	vector<float> g_inputActivationThreshold;
	vector<int> g_inputNeuronType;

	// robot parameters ---------------------------------------------

	
	// inter --------------------------------
	// specify amount and create new vector
	int g_amountInterNeurons;
	vector<int> g_interNeuronConnectionAmount;
	// The connections the inter neurons have to other specific neurons
	vector<vector<int>> g_interNeuronConnections;
	// the weights of all interneurons
	vector<vector<float>> g_initialInterNeuronWeights;
	vector<float> g_interNeuronActivationThresholds;
	vector<int> g_interNeuronType;

	// output --------------------------------
	// specify amount and create new vector
	int g_amountOutputNeurons;
	vector<int> g_outputNeuronConnectionAmount;
	// The connections the output neurons have to specific output slots
	vector<vector<int>> g_outputNeuronConnections;
	// the weights of all outputNeurons

	vector<vector<float>> g_initialOutputNeuronWeights;
	vector<float> g_outputActivationThreshold;
	vector<int> g_outputNeuronType;

	// pattern generators, pattern generators are added to the hidden layer together with interneurons although their vectors are defined seperately. 
	int g_amountPatternGenerators;
	vector<int> g_patternGeneratorConnectionAmount;
	vector<vector<int>> g_patternGeneratorConnections;
	vector<vector<float>> g_initialPatternGenerationWeights;
	vector<vector<float>> g_initialPatternGenerationTime;
	// type of pattern generator: 0 = on-off, 1 = sin activation, 2 = sin output
	vector<int> g_patternGenerationType;


	// fertility gene -------------------------------------- 
	// the fertility gene consists of a number of integers, if the integers do not correlate when trying to create an offspring, the fitness of the offspring will be 0
	// in the case that the fertility rates do not match, the newly created individual might only be able to mate with his brothers/sisters
//	vector<int> fertilityGene;

	// specifies sex of an individual (more than two types of sexes can be created)
	int g_individualSex = 0;
	int g_species = 0;
	
	// specifies the age of the individual of the genome, -1 if the age is indefinite. 
	int g_individualAge;
//	int speciesAge;

	// how the neural network should be constructed: -1 is normal based on called text, 0 = L-system creation
//	bool networkConstructionType;
	string g_objectSensors; 

	bool g_useSensors;
	bool g_useTactileSensors;
	
	bool g_useVestibularSystem;
	bool g_useProprioception;
	vector<int> g_inputNeuronToInputSense;


	// temp
//	vector<int> geneSeperator;

	// --------------------------------------------------------
	// Variable Values: not stored in .csv files but updated during simulation. 
	// --------------------------------------------------------

	// the five variable values
	
	vector<vector<float>> g_inputNeuronWeights;
	vector<vector<float>> g_interNeuronWeights;
	vector<vector<float>> g_outputNeuronWeights;
	vector<vector<float>> g_patternGenerationTime;
	vector<vector<float>> g_patternGenerationWeights;
	
	// - BP, Hebbian etc.
	//vector<int> appliedLearningStrategy;

	// create connections --------------------------------
	// load connection presets
	// create three vectors defining the connections
	// vector < > ;

	int g_objectAmount;
	vector<vector<float>> g_objectOrientation; // store initial Robot values. 
	vector<vector<float>> g_objectPosition;
	vector<vector<float>> g_objectSizes;
	vector<float> g_objectMass;
	vector<int> g_objectParent;
	vector<int> g_objectType;

	int g_jointAmount;
	vector<vector<float>> g_jointOrientation; // store initial Robot values. 
	vector<vector<float>> g_jointPosition;
	vector<vector<float>> g_jointSizes;
	vector<float> g_jointMass;
	vector<int> g_jointParent;
	vector<int> g_jointType;
	vector<vector<float>> g_jointMaxMinAngles;
	vector<float> g_jointMaxForce;
	vector<float> g_jointMaxVelocity;
	//
	vector<int> g_tactileObjects;


	float g_mutationRate;
	vector<float> g_senseInput;

}; */

class I_TEMP {
public:
	bool i_stored = false;

	int i_amountNeurons;
	int i_amountSenseParts;

	vector<float> i_acquiredActivationLevels;

	int i_amountInputNeurons;
	vector<int> i_inputNeuronConnectionAmount;
	vector<vector<int>> i_inputNeuronConnections;
	vector<vector<float>> i_inputNeuronWeights;
	vector<float> i_inputActivationThreshold;
	vector<int> i_inputNeuronToInputSense;

	int i_amountInterNeurons;
	vector<int> i_interNeuronConnectionAmount;
	vector<vector<int>> i_interNeuronConnections;
	vector<vector<float>> i_interNeuronWeights;
	vector<float> i_interNeuronActivationThresholds;

	int i_amountOutputNeurons;
	vector<int> i_outputNeuronConnectionAmount;
	vector<vector<int>> i_outputNeuronConnections;
	vector<vector<float>> i_outputNeuronWeights;
	vector<float> i_outputActivationThreshold;

	int baseGenome = 0;

	int i_amountPatternGenerators;
	vector<int> i_patternGeneratorConnectionAmount;
	vector<vector<int>> i_patternGeneratorConnections;
	vector<vector<float>> i_patternGenerationWeights;
	vector<vector<float>> i_patternGenerationTime;
	vector<int> i_patternGenerationType;
	// end neural network pars
	bool i_useSensors;
	bool i_useVestibularSystem;
	bool i_useProprioception;
	bool i_useTactileSensors;
	vector<int> i_tactileHandles;

	int i_individualAge;
	float i_mutationRate;

	// initial
	vector<vector<float>> i_initialInputNeuronWeights;
	vector<vector<float>> i_initialInterNeuronWeights;
	vector<vector<float>> i_initialOutputNeuronWeights;
	vector<vector<float>> i_initialPatternGenerationTime;
	vector<vector<float>> i_initialPatternGenerationWeights;

};


class CENN
{
public:
	CENN();
	virtual ~CENN();

	simulationSetup simSet = RECALLROTATOR;
	int customIndividual = 36783;

	int amountIncrements = 6;

	int sceneNum = -1;
	void setScene();

	int eNNMethod = 2;
	void checkMemoryLeak(); 
	void resetDefaultPlantNN();
	void defaultPlantNN(); 
	void defaultModuleNN();
	void resetDefaultModuleNN();

	void useDefaultPlantPars();

	void mutateLProperties(float); 
	void mutateModularLProperties(float);
	void extremeMutateLProperties(float);

	void nameObject(string, int);
	void lGrowth(int, int, int, int);
	void modularLGrowth(int, int, int, int);

	int nNHandlingType = 20;
	int evaluationMethod = 1;
	void growStem(int, int);

	void checkLError();

	bool _initialize = true;

	void useDefaultGenome();

	string mainHandleName;

	void resetParameters();

	bool s_stored = false;

	void lSystem();
	void modularLSystem();

	void createMorphologyWithLeafs();
	int generation = 0; 

	// Neural network Parameters:
	void storeNNPars(DefaultGenome, int);
	void callNNPars(DefaultGenome);

//	void storeNNPars(CIndividualGenome, int);
//	void callNNPars(CIndividualGenome);
	
	// sensors start
	void tactileSensors(); // 1 object per sensor
	void resetTheSun();
	void rotateTheSun(); 

	// sensors end
	void loadCreature(int);
	void createCreature();
	void createCreature2();
	void initializeCreaturePars();
	void initializePlantPars();
	void objectCreator(int);

	void createCube(float, float, float, float, float, float, float, float, float, float, int, int);
	int createVisionSensor(int);
	int createArtificialLeaf(float, float, float, float, float, float, float, float, float, float, int, int);
	void createArtificialLeafWithJoint(float, float, float, float, float, float, float, float, float, float, int, int);
	void createJointAndCube(float, float, float, float, float, float, float, float, float, float, int, int, int);
	void createJointAndCubeST(float, float, float, float, float, float, float, float, float, float, int, int, int);
	void createJointAndCubeNew(float, float, float, float, float, float, float, float, float, float, int, int, int);
	void createJointAndCubeInLeafHandles(float, float, float, float, float, float, float, float, float, float, int, int, int);
	void createCubeModule(float, float, float, float, float, float, float, float, float, float, int, int, int);
	vector<vector<int>> moduleHandles;

	vector<int> visionHandles; 
	vector <int> _visionHandles;

	// light detection using rays
	int lightHandle;
	bool createRay(int, int);
	void testCreateRay(int, int);
	void leafToSunRay();
	void checkRayCollision(int);
	void removeRay(int);
	vector<int> rayHandles;
	void initialize();
	

	void updateAcquiredActivationLevels();			// updates the neural network
	void updateJointForces(int);						// updates all the forces of the joints
	void updateAcquiredActivationLevels2();			// updates the neural network
	void updateJointForces2(int);						// updates all the forces of the jointsf
	vector<int> i_jointHandles;
	vector<float> i_jointActivity;

	vector<int> objectHandles; 

	int bufferStatePropertiesSize;

	vector<unique_ptr<DefaultGenome>> storedIndividualGenomes;
	DefaultGenome currentGenome;
	ObjectCreator vcreate;
	ObjectRemover vremove;
	SetVREPInfo vset;
	RetrieveVREPInfo vret;

	typedef unique_ptr<Population> PopulationPointer;
	vector<PopulationPointer> populations;

	Environment environment;
	Agent agent;

//	vector<CIndividualGenome> storedIndividualGenomes;
//	CIndividualGenome currentGenome;

	string nNTemplate = "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\interfaceFiles\\NNTemplate.csv";
	bool useTemplate = false;
	void initializeNN(string);
	int newGenerations; 

	int i_objectAmount;
	vector<int> i_objectHandles;
	vector<vector<float>> i_objectOrientation; // store initial Robot values. 
	vector<vector<float>> i_objectPosition;
	vector<vector<float>> i_objectSizes;
	vector<float> i_objectMass;
	vector<int> i_objectParent;
	vector<int> i_objectType;

	int i_jointAmount;
	vector<vector<float>> i_jointOrientation; // store initial Robot values. 
	vector<vector<float>> i_jointPosition;
	vector<vector<float>> i_jointSizes;
	vector<float> i_jointMass;
	vector<int> i_jointParent;
	vector<int> i_jointType;
	vector<vector<float>> i_jointMaxMinAngles;
	vector<float> i_jointMaxForce;
	vector<float> i_jointMaxVelocity;
	//
	vector<int> i_tactileObjects;

	void storeObjectPars(int, float[3], float[3], float[3], float, int, int);
	void storeJointPars(int, float[3], float[3], float[3], float, int, int, float[2], float, float);


	//--------------- VS
	float catHeadOrientation[3];		// orientation of the head, used by the vestibular system 
	float headOrientation[3];			// "													"
	float previousHeadPosition[3];
	float previousHeadRotation[3];
	float headPosition[3];
	float errorHeadPosition[3];
	float headRotAcceleration[3];
	float headRotation[3];
	float errorHeadRotation[3];
	float headAcceleration[3];
	float headAccelerationActivator[3];
	float headOrientationActivator[3];
	float normalizedHeadOrientation[6];
	//---------------

//	void identifyMainHandle(int);
	void identifyParameters();

	void initializeEnvironment();
	void initializeAgents();
	void initializeAgents2();
	void identifyRobotMorphology();
	void initializePopulation(int);
	void initializePopulation2(int);
	int indCounter = 0;
	int currentInd = 0;
	bool sexualReproduction = false;

	void useDefaultPars();

	int speciesLimit;

	float simulationTime = 0;
	float maxSimulationTime = 0.1;
	int simulationResetter = 2;

	void addConnection();
	void removeConnection();
	void mutateWeight();
	void mutateConnection();
	void mutateCPGTime();

	int mutationType;

	vector<int> mainHandle;
	void startENN(int);
	void startOfSimulation();
	float growTimeKeeper;

	int createWalls(float density);

	// NN handling functions
	vector<float> i_senseInput;
	void proprioception();
	vector<float> proprioceptionInput();
	void vestibularSystem();
	vector<float> vestibularSystemInput;
	void tactileSensing();
	vector<float> tactileSenseInput;

	void proprioception2();
	void vestibularSystem2();
	void tactileSensing2();


	int i_tactileSensingStartsAt;
	int i_proprioceptionStartsAt;
	int i_vestibularSystemStartsAt;

	void patternGeneration();
//	void updateAcquiredActivationLevels();
	void updateJointOutput();

	void mutation();
	void mutation2();
	void checkMutation(); 
	void crossover();
	void initializer(int);

	void addNeuron(int);
	void removeNeuron(int); // removes a random neuron of type ...
	void addConnection(int);
	void deleteConnection(int);

	void addNeuron2(int);
	void removeNeuron2(int); // removes a random neuron of type ...
	void addConnection2();
	void removeConnection2();

	void handleInitializer();
	void genomeCaller();

	void printMorphologyParameters(DefaultGenome); 
	void printLStateProperties(DefaultGenome);
	//	void printMorphologyParameters(CIndividualGenome); 
	//	void printLStateProperties(CIndividualGenome);

	void genomeSaver();

	void genomeSaver2();
	void resetParameters2();

	float leakyIntegrationFactor = 0.8; 

	void initializeIndividual(); 
	void initializeENNPars();
	int individualCounter;
	int speciesCounter;

	float individualFitness;
	vector<float> populationFitness; 
	int maxAmountConnections = 10; 
	
	int floorHandle;

	void incrementConnections(int);
	void decrementConnections(int);

	// Robot Properties -----------------------------------------------------------
//	int s_amountJoints;
	// parameters not yet defined in the interface
	bool useLoadedModels = true;
	bool jointCalculator; 
	int s_totalInput;

	/*
	bool s_useSpringDamper = false;
	bool s_useMaxVelocity = true;
	bool s_useMaxForce = true;
	bool s_useDesiredAngle = true;
	bool s_usePID = false;


	// stored values that's why "s_" is in front of the names---------------------------------------------------------
	// the values are called from the ENNUI .csv file located in the interface folder

	// robot properties
	int s_amountJoints;						// #0
	string s_jointControl;					// #1
	

	// EA properties
	int s_initialPopulationSize;			// #2
	bool s_islandPopulations;				// #3
	bool s_useSensors;						// #4
	bool s_useCPGs;							// #5
	bool s_useProprioception;				// #6
	bool s_useVestibularSystem;				// #7
	bool s_useTactileSensing;				// #8
	string s_tactileSensingType;			// #9
	int s_amountSenseParts;					// #10
	string s_specifiedPartNames;			// #11
	string s_reproductionType;				// #12
	string s_genomeType;					// #13
	float s_evolvableMutationRate;			// #14
	float s_initialMutationRate;			// #15
	int s_amountInterNeurons;				// #16
	int s_amountCPGs;						// #17
	int s_amountInputNeurons;				// #18
	int s_amountOutputNeurons;				// #19
	bool s_useCrossover;					// #10
	int s_amountCrossover;					// #21
	bool s_evolvableCrossover;				// #22
	bool s_variableGenomeSize;				// #23
	bool s_geneDuplication;					// #24
	bool s_evolvingMorphology;				// #25
	bool s_evolvingDimensions;				// #26
	bool s_deleteRandomParts;				// #27
	bool s_addRandomParts;					// #28
	bool s_lEvolution;						// #29

	*/
	// new saved .csv
	//Robot Properties--------------------------
	int s_amountJoints;
	int s_jointControl;
	bool s_useSensors;
	bool s_useProprioception;
	bool s_useVestibularSystem;
	bool s_useTactileSensing;
	bool s_tactileSensingType;
	int s_amountSenseParts;
	string s_specifiedPartNames;
	bool s_identifyControl;
	bool s_identifyMaxForceAndVelocity;
	bool s_usePID;
	float s_upperVelLimit;
	float s_pValue;
	float s_iValue;
	float s_dValue;
	bool s_useSpringDamper;
	float s_kValue;
	float s_cValue;
	bool s_useMaxVelocity;
	float s_maxVelocity;
	bool s_useMaxForce;
	float s_maxForce;
	bool s_useDesiredAngle;

	//	Evolution Parameters--------------------------
	float s_simulationTime;

	int s_initialPopulationSize;
	bool s_variablePopulationSize;
	bool s_islandPopulations;
	int s_maxAmountIslandPopulations;
	float s_defaultIslandInvasionRate;
	bool s_evolvableInvasionRate;
	float s_initialMutationRate;	// in percentage of genome
	bool s_evolvableMutationRate;
	bool s_incrementalEvolution;
	int s_incrementInterval;
	bool s_realTimeEvolution;
	float s_realTimeEvolutionUpdateTime;
	float s_realTimeEvolutionTimeLimit;
	bool s_evolvingMorphology;
	bool s_evolvingDimensions;		// dimensions of objects are evolved by percentage
	bool s_deleteRandomParts;
	bool s_addRandomParts;
	bool s_lEvolution;

	int s_genomeType;
	int s_ploidity;
	bool s_dominanceGenes;
	int s_reproductionType;
	int s_randSeed;
	bool s_neuroEvolution = true;
	bool s_variableGenomeSize;
	bool s_geneDuplication;
	bool s_useCrossover;
	int s_maxAmountCrossover;
	bool s_evolvableCrossover;
	bool s_historicalMarkers;
	bool s_geneScopeFitness;

	
		//	ANN Parameters--------------------------
	bool s_useCustomNN;
	string s_customNNname;
	bool s_calculateAmountInputNeurons;
	bool s_jointCalculator;
	bool s_calculateAmountOutputneurons;
	int s_amountInterNeurons;
	bool s_usePatternGenerators;
	int s_patternGeneratorTypes;
	int s_amountPatternGenerators;
	bool s_variableAmountNeurons;
	bool s_variableAmountInputNeurons;
	bool s_variableAmountInterNeurons;
	bool s_variableAmountOutputNeurons;
	bool s_variableAmountPatternGenerators;
	int s_maxAmountNeurons;
	int s_maxAmountInputNeurons;
	int s_maxAmountInterNeurons;
	int s_maxAmountOutputNeurons;
	int s_maxAmountPatternGenerators;
	
	//	Evaluation Properties--------------------------
	int s_evaluationType;
	//	Summary--------------------------
	int s_amountInputNeurons;
	int s_amountOutputNeurons;


	//-----------------------------------------------------------



	void checkLoadedPars();
	void displayPars();

	void handleSimulation();
	void endOfSimulation();

	void saveCreatureMorphology();

	void evaluateIndividual();
	// handle of simulation properties

	void genomeChecker(int); 


	/*


	// ENN classes:
	void updateAcquiredActivationLevels();	// updates the neural network
	void updateJointForces();				// updates all the forces of the joints
	void mutation();						// mutates
	void checkMutation();
	void crossover();
	void initializer();						// initializes a new individual
	void activityVisualizer();				// idea was to use bitmaps to track the activity of neurons, altough this was too confusing to implement and not a priority
	//	void handleInitializer();			// this class identifies the joints and limbs of the robot in V-REP, because this class is called fiirst, it is usefull to entail 
	//										functions that require to be called right before or after the robot is identified.  
	void catContinuerInitialization();		// If the simulation crashes, this class calls back the last known population and the simulation can continue
	void incrementAndEvaluate();			// This class is responsible for evaluating the robot at the end of the simulation
	void onlyEvaluate();
	void networkCaller();					// this class is responsible for recalling a specific neural network	
	void networkCustomCaller();
	void checkSavedNN();					// this class checks if the saved NN is the same as the simulated NN
	void catSaver();						// Responsible for storing the neural networks. 
	//	void reflexes();						// reflexes are turned off (in progress)
	//	void gaitAnalysis();					// not used
	void serialPrinter();
	void fitnessUpdater();					// Add additional fitness functions
	void mutationClass0();
	void mutationClass1();
	void mutationClass2();
	void parameterResetter();
	void positionSaver();
	void positionResetter();
	void catReset();

	int dialogHandle;
	// sensors
	void feetSensors();						// regulating on/off sensors
	void vestibularSystem();				// biomimetic balance system
	void proprioceptors();					// reception of the joint positions
	void cPGs();							// central pattern generators

	float _simulationTime = 0;				// tracks simulation time

	// Sensors..............................................................
	int floorHandle; // foot handle is required for detecting the collision with feet
	//	Feet sensors represented by the booleans below represent colissions of the feet, if feet collide these values will be set to true, otherwise, they are set to false
	bool inpThresholdLF;
	bool inpThresholdRF;
	bool inpThresholdLB;
	bool inpThresholdRB;
	// the following are used to analyze the gait (By saving ground contact times of the feet). Everytime inpThreshold parameters change, it is stored in footContactTimes[8][100]
	// note that the gait is not analyzed in the thesis!
	float footContactTimer;
	int footContacter[4];
	int footContactCounter[8];
	float footContactTimes[8][100];

	// Joint angle initialization
	//float jointPos[28]; // stores positions of the joints

	std::vector<float> jointPos;

	// joint reflex input // not used // under construction
	//	float reflexInput[30];
	//	float jointAngleCPG[4][3];
	//	bool cpgSwitch[4];
	//	float reflexPosition[30];
	//	float gaitReflex[8][2];
	//	int reflexMutPos[10];


	// The vestibular system
	float catHeadOrientation[3];		// orientation of the head, used by the vestibular system 
	float headOrientation[3];			// "													"
	float previousHeadPosition[3];
	float previousHeadRotation[3];
	float headPosition[3];
	float errorHeadPosition[3];
	float headRotAcceleration[3];
	float headRotation[3];
	float errorHeadRotation[3];
	float headAcceleration[3];
	float headAccelerationActivator[3];
	float headOrientationActivator[3];
	float normalizedHeadOrientation[6];


	//
	int amountSelectedObjects;
	vector<int> robotHandles;
	int uiHandle;
	vector<string> jointNames;
	vector<int> jointHandles;
	vector<int> objectHandles;
	vector<int> jointAngleLimits;
	vector<int> jointOffset;
	vector<int> jointMaxForce;
	vector<int> jointMaxVel;

	// original positions
	vector<float> savedJointPositions;
	vector<vector<float>> savedObjectPositions;
	vector<vector<float>> savedObjectOrientations;
	string objectName[1000];
	int amountObjects = 0;
	int maxAmountCrossovers = 9;

	// CPGs
	int amountCPGs = 0;
	int customAmountCPGs = 0;
	std::vector<float> cpgVelocity;				// some outputneurons determine the frequency of the CPGs activity
	std::vector<float> burstLimitVelocity;		// some outputneurons determin the frequency of the bursts of the CPGs (during bursts certain inputneurons are active for a given period) 
	std::vector<bool> cpgInput;
	std::vector<bool> cpgInput2;
	std::vector<float> cpgTime;
	std::vector<float> cpgTimeR;
	//	float cpgTime2[4];					// not used
	//	float cpgTimer2[4];					// not used
	std::vector<float> burstLimit;
	string headObject;


	//double checking values that used to produce errors in previous versions:: used by checkSavedNN()
	std::vector<float> tempBurstLimit;
	std::vector<float> tempCpgTime;

	int maxAmountConnections;
	int amountInputNeurons;
	int amountInterNeurons;
	int amountOutputNeurons;

	int customAmountInputNeurons;
	int customAmountInterNeurons;
	int customAmountOutputNeurons;
	int customMaxCon;

	// CCats's neural network -----------------------------------------------------------------------------------------------------

	vector<int> inputNeuronConnectionAmount;
	vector<int> interNeuronConnectionAmount;
	vector<int> outputNeuronConnectionAmount;
	vector<vector<float>> inputNeuronWeights;
	vector<vector<float>> inputNeuronWeightsBuffer;
	vector<vector<int>> inputNeuronConnections;
	vector<vector<float>> interNeuronWeights;
	vector<vector<float>> interNeuronWeightsBuffer;
	vector<vector<int>> interNeuronConnections;
	vector<float> activationThreshold;

	vector<float> acquiredActivationLevel; // is for both input and output neurons. 
	vector<float> bufferAcquiredActivationLevel;
	vector<float> outputAcquiredActivationLevel;

	// 	float inputNeuronConnectionAmount[150];
	// 	float interNeuronConnectionAmount[150];
	// 	float outputNeuronConnectionAmount[150];

	//	float inputNeuronWeights[150][10];
	//	float inputNeuronWeightsBuffer[150][10];
	//	int inputNeuronConnections[150][10];
	//	float interNeuronWeights[150][10];
	//	float interNeuronWeightsBuffer[150][10];
	//	int interNeuronConnections[150][10];
	//	float activationThreshold[150];

	// output
	//	float outputNeuronActivationThreshold[150];
	// float acquiredActivationLevel[300]; // is for both input and output neurons.
// 	float bufferAcquiredActivationLevel[300]; The acquired activation level is stored in bufferAcquiredActivationLevel in order
// 	to update acquiredActivationLevel once (Otherwise, certain acquiredActivationLevel values are
// 	updated more frequently than others. E.g. if neuron 5 is activated and connected to neurons 2
// 	and 7, acquiredActivationLevel 2 and 7 are updated, however, updating the acquiredActivationLevel array
// 	is done by a for loop iterating through every neuron. In this case the for loop still reaches
// 	neuron 7 and not neuron 2. This is unwanted thus the updated values are temporatily stored in
// 	bufferAcquiredActivationLevel.
	//float outputAcquiredActivationLevel[150];
	// end of neural network -----------------------------------------------------------------------------------------------------------------


//	vector<int> tempInputNeuronConnectionAmount;
//	vector<int> tempInterNeuronConnectionAmount;
//	vector<int> tempOutputNeuronConnectionAmount;
//	vector<vector<float>> tempInputNeuronWeights;
//	vector<vector<int>> tempInputNeuronConnections;
//	vector<vector<float>> tempInterNeuronWeights;
//	vector<vector<int>> tempInterNeuronConnections;
//	vector<float> tempActivationThreshold;
	
	// Actuation ..........................................................
	// PID parameters
	vector<float> kP;
	float kI = 0;
	float kD = 0;
	// joint parameters
	float maxForce = 20.0;				// limiting force // original = 20.0
	float maxVelocity = 8.0;			// limiting velocity // original = 8.0
	// requested joint target position
	vector<float> reqJointPos;				// stores required joint positions
	vector<float> errorJointPos;			// stores the error between required and actual joint position
	vector<float> jointVelocity;			// stores required velocities
	vector<float> requiredJointForce;		// stores required forces

	//which simulation to run;
	int runningSimulation = 1;			// 1 = original, 2 = continuer, 3 = simulate custom ind 
	string runningSim;
	string catScript;						// for recalling a NN for evaluation
	string networkName;
	int creationAmount = 1;				// strangely enough, simulations of the same neural network do not always perform similarly. CreationAmount determines how many times an individual needs to be evaluated
	int creationCounter = 0;			// creationcounter keeps track of the amount of individuals created having the same neural network

	int madeFrom;						// saves the parent // used by catSaver()
	int indMutation;					// determines the mutation class
	//	float originalJointPos[28][3];		// able to store original positions of the joints useful for resetting a creature without using a add-on script

	// fitness parameters
	float headHeight;					// height of the head
	float sidewardTravel;				// Used to store the distance travelled in sideward direction
	bool populationIndividualsAct;		// determining which individuals to load
	bool populationFitnessAct;			// determining fitness
	vector<int> populationIndividuals;		// max number of individuals in the population (actual number of individuals is defined by initialPopualtionSize
	vector<float> populationFitness;		// max number of fitness values to be stored
	vector<float> fitness;					// creationAmount can make more individuals with the same neural network. 
	float bestIndividualFitness;
	int bestIndividual;					// best individual of all individuals simulated
	float currentBestIndividualFitness;
	int currentBestIndividual;			// best individual of the current population
	float worstIndividualFitness;
	int worstIndividual;
	float averageFitness;
	float totalFitness;
	float travelledDistance[3];
	float totalForce;					// stores the total force needed by the simulation (currently based on jointVelocity and isn't accurate or usable)
	bool headCollision;					// fitnessUpdater() tracks headCollision and reduces the fitness when collission occurs
	bool torsoCollision;				// fitnessUpdater() tracks torsoCollision and reduces the fitness when collission occurs
	//	float highestHead;					// for making fitness function to select on jumping movements

	// define serial
	// Serial.h

	//Serial Port
	string serial;
	string portNum;
	float fitness8;

	// recalling catCounter from .csv file
	int getCatCounter = 0;
	int catSimCount = 0;
	int acquiredCatCounter;				// stores the catCounter when called
	// counters
	int generation = 0;					// keeps track of the generation
	int generationCounter = 0;			// keeps track of the number of individuals simulated in a new generation

	int randomSeed;
	string savingDirectory = "saved NNs0";
	string desiredDirection = "x";
	float maxSimulationTime;
	float resetSec;
	// Determine simulation parameters
	bool customization = false;			// when customization is set to true, predefined neural networks are used in individuals of the initial population. Otherwise, randomly generated neural networks are used in the initial population
	string customFileName;
	bool enableFitnessUpdater = false;	// enables fitnessUpdater() class
	bool enableCrossover = false;		// enables crossover() class
	int chanceOfCrossover;				// 1/4 chance to get a crossover when mutating 
	int amountOfCrossovers;				// 1-4 crossover positions per crossover() used
	bool useMutationClass0;
	bool useMutationClass1;
	bool useMutationClass2;

	// following declerations define how much mutations of each mutation class can occur. Each new individual is subjected to one of the three mutation classes
	int amountOfMutations;
	int amountOfMutationsClass0;
	int amountOfMutationsClass1;
	int amountOfMutationsClass2;
	// Enabling sensors: 
	bool useFeetSensors = false;
	bool useVestibularSystem = false;
	bool useProprioception = false;
	bool useCPGs = false;
	int amountFeetSensors = 0;
	int amSNeuronsVestibularSystem = 0;
	int amSNeuronsProprioception = 0;
	//	int amSNeuronsCPGs = 0;
	vector<string> feetNames;
	vector<int> feetHandles;
	vector<bool> inpFeetSensorThreshold;

	int feetSensorsStartsAt = 0;
	int vestibularSystemStartsAt = 0;
	int proprioceptionStartsAt = 0;
	int CPGsStartsAt = 0;

	vector<int> savedAmountInputNeurons;
	vector<int> savedAmountInterNeurons;
	vector<int> savedAmountOutputNeurons;
	vector<int> savedMaxCon;
	vector<vector<int>> savedInputNeuronConnectionAmount;
	vector<vector<int>> savedInterNeuronConnectionAmount;
	//	vector<vector<int>> savedOutputNeuronConnectionAmount;
	vector<vector<vector<float>>> savedInputNeuronWeights;
	vector<vector<vector<int>>> savedInputNeuronConnections;
	vector<vector<vector<float>>> savedInterNeuronWeights;
	vector<vector<vector<int>>> savedInterNeuronConnections;
	vector<vector<float>> savedActivationThreshold;
	vector<int> savedAmountCPGs;
	vector<vector<float>> savedCpgTime;
	vector<vector<float>> savedBurstLimit;
	vector<float> savedFitness;
	*/
/*	//	Recalling neural networks ----------------------------------------------------------------------------------
	// activators used when calling csv files
	//	int indNum;							// not used
	int currentInd;						// used to set catCounter to last called individual when using catContinuer() class
	bool currentIndSaver;				// enables saving currentInd
	bool currentIndParSaver;			// Used to save parent individual of a neural network in a .csv file
	int amInpNeurSaver;					// Determines saving of input neurons
	int amIntNeurSaver;					// Determines saving of interneurons
	int amOutNeurSaver;					// Determines saving of outputNeurons

	// other saving and loading variables 
	bool saverActivator;
	int inpConCounter;
	int inpWeightCounter;
	int inpNeuronWeightsSaver;

	bool amountInpNeurAct;
	bool amountIntNeurAct;
	bool amountOutNeurAct;
	bool maxConAct;
	bool amountInpNeurConAct;
	bool amountIntNeurConAct;
	bool amountOutNeurConAct;
	bool inputNeuronWeightAct;
	bool inputNeuronConnectionsAct;
	bool interNeuronWeightAct;
	bool interNeuronConnectionsAct;
	bool activationThresholdActivator;
	bool cpgAmountAct;
	bool cpgActivator;
	bool catCounterActivator;
	bool burstLimitActivator;
	int amInpArrayCounter;
	int amIntArrayCounter;
	int amOutArrayCounter;
	int inpWeightCounter1;
	int inpWeightCounter2;
	int intWeightCounter1;
	int intWeightCounter2;
	int inpConnectionsCounter1;
	int inpConnectionsCounter2;
	int intConnectionsCounter1;
	int intConnectionsCounter2;
	int thresholdCounter;
	int cpgCounter;
	int blCounter;
	int arrayCounter;
	int arrayCounter2;
	int saveInd;
	*/
	// End of recalling neural network---------------------------------------------------------------------------------------------

	// catCounter
	int catCounter = 0;			// keeps track of the amount of simulated robots	
	int simNum = 0;				// sets random seed and determines names under which the .csv files are saved

	// mutation parameters
	int mutAmount;				// Specifies amount of mutations
	int mutType;				// specifies type of mutation

	// Neural Network Initializer
	vector<float> entirePopulationFitness;
	vector<int> currentPopulation;
	vector<int> entirePopulation;
	int selectedInd;
	int offspring;

	// Neural Network Mutator---------------------------------
	int offspringSelector;
	int amountOfOffspring = 100;

	bool aa;					// used in crossovers
	bool bb;					// used in crossovers

	// original limb positions
	vector<vector<float>> originalLimbPositions;
	vector<vector<float>> originalLimbOrientations;
	
protected:
	
private:
	int lGenome = 1;

};

class CUserInterface
{
public:
	CUserInterface();
	virtual ~CUserInterface();

	void createUserInterface();
private:
};