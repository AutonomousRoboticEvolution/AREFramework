#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <fstream> // ifstream
#include <list>
#include <vector>
#include <misc/utilities.h>

/* Dynamixel artefact : To be used in experimental version
//#include "../dynamixel/c++/src/dynamixel_sdk.h"
#define PROTOCOL_VERSION1               1.0                 // See which protocol version is used in the Dynamixel
#define PROTOCOL_VERSION2               2.0
#define DEVICENAME                      "\\\\.\\COM26"      //"/dev/ttyUSB0"      // Check which port is being used on your controller
*/

namespace are {

namespace settings
{

class Type
{
public:
    Type(){}
    virtual ~Type(){}
    typedef std::shared_ptr<Type> Ptr;
    typedef std::shared_ptr<const Type> ConstPtr;
    virtual void fromString(const std::string& str) = 0;
    std::string name;
};

class Boolean : public Type
{
public:
    Boolean(){name = "bool";}
    bool value = false;
    void fromString(const std::string& str){value = std::stoi(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

class Integer : public Type
{
public:
    Integer(){name = "int";}
    int value = 0;
    void fromString(const std::string& str){value = std::stoi(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

class Float : public Type
{
public:
    Float(){name = "float";}
    float value = 0.0;
    void fromString(const std::string& str){value = std::stof(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};


class Double : public Type
{
public:
    Double(){name = "double";}
    double value = 0.0;
    void fromString(const std::string& str){value = std::stod(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

class String : public Type
{
public:
    String(){name = "string";}
    std::string value = "";
    void fromString(const std::string& str){value = str;}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

Type::Ptr buildType(const std::string &name);


template<typename T>
std::shared_ptr<const T> cast(const Type::ConstPtr val)
{
    return std::dynamic_pointer_cast<const T>(val);
}

/// Instance type defines in what mode the "Evolutionary Robotics" plugin runs // TODO: define name
enum InstanceType {
    /// Single thread (local)
    INSTANCE_REGULAR = 0,
    /// Waits for genome signals for using in parallel execution
    INSTANCE_SERVER = 1,
    /// deprecated
    INSTANCE_DEBUGGING = 2
};

typedef std::map<const std::string,const Type::ConstPtr> ParametersMap;
typedef std::shared_ptr<ParametersMap> ParametersMapPtr;

template<typename T>
T getParameter(const ParametersMapPtr &params,const std::string& name)
{
    if(params->find(name) == params->end()){
        std::cerr << "unable to find parameters " << name << std::endl
                  << "you should define it in the parameters file." << std::endl;
        return T();
    }
    return *(cast<T>(params->at(name)));
}

template<typename T>
T getParameter(const ParametersMap &params,const std::string& name)
{
    if(params.find(name) == params.end()){
        std::cerr << "unable to find parameters " << name << std::endl
                  << "you should define it in the parameters file." << std::endl;
        return T();
    }
    return *(cast<T>(params.at(name)));
}

ParametersMap loadParameters(const std::string& file);
void saveParameters(const std::string& file,const ParametersMapPtr &param); //todo

struct Property
{
    typedef std::shared_ptr<Property> Ptr;
    typedef std::shared_ptr<const Property> ConstPtr;

    Property(){}
    Property(const Property& prop) :
        generation(prop.generation)
    {}

    //Properties
    int generation = 0;
    std::vector<int> indNumbers;
    std::vector<double> indFits;
    int indCounter = 0;
    int clientID;
};


} //settings

//class Settings
//{
//public:

//    typedef std::shared_ptr<Settings> Ptr;
//    typedef std::shared_ptr<const Settings> ConstPtr;

//    Settings();
//    Settings(const Settings& st) :
//        startingCondition(st.startingCondition),
//        instanceType(st.instanceType),
//        evolutionType(st.evolutionType),
//        fitnessType(st.fitnessType),
//        environmentType(st.environmentType),
//        moveDirection(st.moveDirection),
//        morphologyType(st.morphologyType),
//        controlType(st.controlType),
//        selectionType(st.selectionType),
//        replacementType(st.replacementType),
//        indCounter(st.indCounter),
//        sceneNum(st.sceneNum),
//        sendGenomeAsSignal(st.sendGenomeAsSignal),
//        mutationRate(st.mutationRate),
//        morphMutRate(st.morphMutRate),
//        generation(st.generation),
//        maxGeneration(st.maxGeneration),
//        xGenerations(st.xGenerations),
//        populationSize(st.populationSize),
//        individualCounter(st.individualCounter),
//        crossover(st.crossover),
//        crossoverRate(st.crossoverRate),
//        ageInds(st.ageInds),
//        maxAge(st.maxAge),
//        minAge(st.minAge),
//        deathProb(st.deathProb),
//        indNumbers(st.indNumbers),
//        indFits(st.indFits),
//        exp_plugin_name(st.exp_plugin_name),
//        load_external_settings(st.load_external_settings),
//        useVarModules(st.useVarModules),
//        maxForce(st.maxForce),
//        maxForceSensor(st.maxForceSensor),
//        maxForce_ForceSensor(st.maxForce_ForceSensor),
//        maxTorque_ForceSensor(st.maxTorque_ForceSensor),
//        consecutiveThresholdViolations(st.consecutiveThresholdViolations),
//        maxNumberModules(st.maxNumberModules),
//        moduleTypes(st.moduleTypes),
//        maxModuleTypes(st.maxModuleTypes),
//        numberOfModules(st.numberOfModules),
//        repository(st.repository),
//        initialModuleType(st.initialModuleType),
//        energyDissipationRate(st.energyDissipationRate),
//        lIncrements(st.lIncrements),
//        bestIndividual(st.bestIndividual),
//        loadInd(st.loadInd),
//        verbose(st.verbose),
//        colorization(st.colorization),
//        killWhenNotConnected(st.killWhenNotConnected),
//        shouldReopenConnections(st.shouldReopenConnections),
//        createPatternNeurons(st.createPatternNeurons),
//        initialInputNeurons(st.initialInputNeurons),
//        initialInterNeurons(st.initialInterNeurons),
//        initialOutputNeurons(st.initialOutputNeurons),
//        initialAmountConnectionsNeurons(st.initialAmountConnectionsNeurons),
//        maxAddedNeurons(st.maxAddedNeurons),
//        savePhenotype(st.savePhenotype),
//        envObjectHandles(st.envObjectHandles),
//        autoDeleteSettings(st.autoDeleteSettings),
//        bOrgansAbovePrintingBed(st.bOrgansAbovePrintingBed),
//        bCollidingOrgans(st.bCollidingOrgans),
//        bNonprintableOrientations(st.bNonprintableOrientations),
//        bAnyOrgansNumber(st.bAnyOrgansNumber),
//        loadFromQueue(st.loadFromQueue),
//        vrep_folder(st.vrep_folder),
//        scene_path(st.scene_path)
//    {}
//    virtual ~Settings();
//    /// Type of fitness function
//    enum FitnessType {
//        /// Movement
//        MOVE = 0,
//        /// Deprecated
//        SOLAR = 1,
//        /// Deprecated
//        SWIM = 2,
//        /// Deprecated
//        MOVE_AND_SOLAR = 3,
//        /// Deprecated
//        FITNESS_JUMP = 4
//    };

//    enum StartingCondition {
//        COND_LOAD_BEST,
//        COND_RUN_EVOLUTION_CLIENT,
//        COND_RUN_EVOLUTION_SERVER,
//        COND_LOAD_SPECIFIC_INDIVIDUAL
//    };

//    /// This enum defines which environment is loaded
//    enum EnvironmentType {
//        /// Default environment
//        DEFAULT_ENV = 0,
//        /// Deprecated
//        SUN_BASIC = 1,
//        /// Deprecated
//        SUN_CONSTRAINED = 2,
//        /// Deprecated
//        SUN_BLOCKED = 7,
//        /// Deprecated
//        SUN_MOVING = 3,
//        /// Deprecated
//        SUN_CONSTRAINED_AND_MOVING = 4,
//        /// Deprecated
//        WATER_ENV = 6,
//        /// Loads an environment with a heightmap
//        ROUGH = 5,
//        /// Deprecated
//        CONSTRAINED_MOVING_SUN = 8,
//        /// Deprecated
//        ENV_FOURSUNS = 9,
//        /// Deprecated
//        ENV_SWITCHOBJECTIVE = 10,
//        /// Used for the phototaxis scenario
//        ENV_PHOTOTAXIS = 11
//    };

//    /// Enum defining the selection method used by the EA
//    enum SelectionType {
//        RANDOM_SELECTION,
//        PROPORTIONATE_SELECTION
//    };

//    enum ReplacementType {
//        RANDOM_REPLACEMENT = 0,
//        REPLACE_AGAINST_WORST = 1,
//        PARETOMORPH_REPLACEMENT = 2
//    };

//    /// Can be used to color the modules
//    enum Colorization {
//        /// This colors the robot based on the L-System genotypes expressed
//        COLOR_LSYSTEM = 0,
//        /// Colors modules based on the outputs of the neural networks
//        COLOR_NEURALNETWORK = 1,
//        /// Deprecated
//        COLOR_NEURALNETWORKANDLSYSTEM = 2
//    };
//    /// Specify which direction the fitness function should look at.
//    enum MoveDirection {
//        DISTANCE_X = 0,
//        DISTANCE_Y = 1,
//        DISTANCE_Z = 2, // jump
//        DISTANCE_XY = 3,
//        DISTANCE_XZ = 4,
//        DISTANCE_YZ = 5,
//        DISTANCE_XYZ = 6,
//        FORWARD_X = 7,
//        FORWARD_Y = 8,
//        FORWARD_XY = 9,
//        FORWARD_NEGX = 10,
//        FORWARD_NEGY = 11,
//        FORWARD_NEGXY = 12
//    };
//    /// Used to define which morphology to evolve
//    enum MorphologyType
//    {
//        CAT_MORPHOLOGY = 0,
//        MODULAR_LSYSTEM = 1,
//        NO_MORPHOLOGY = 2, // I don't know why you would want this though
//        MODULAR_CPPN = 3,
//        MODULAR_DIRECT = 4,
//        CUSTOM_MORPHOLOGY = 5,
//        VOXEL_MORPHOLOGY = 6,
//        MODULAR_PHENOTYPE = 11, /// Used to load a modular robot from a phenotype file (phenotype<individualNumber>.csv)
//        CPPN_NEAT_MORPH = 12,
//        INTEGRATION = 22
//    };

//    enum ControlType
//    {
//        /// Neural network that changes the topology of the network as well
//        ANN_DEFAULT = 0,
//        /// Neural network that doesn't change it's topology
//        ANN_CUSTOM = 1,
//        /// Compositional pattern producing network as interpreted by Frank; not original implementation from stanley (2007)
//        ANN_CPPN = 2,
//        //	ANN_NEAT = 4,
//        /// Used to define the directionality of communication between neural networks of neighbouring modules
//        ANN_DISTRIBUTED_UP = 3,
//        /// Used to define the directionality of communication between neural networks of neighbouring modules
//        ANN_DISTRIBUTED_DOWN = 4,
//        /// Used to define the directionality of communication between neural networks of neighbouring modules
//        ANN_DISTRIBUTED_BOTH = 5,
//        /// Should be used for calling the ANN used in NEAT
//        ANN_NEAT = 6,
//        /// Can be used to specify leaky integrators in the ANN
//        LEAKY_ANN_CONTROL = 7,  // TODO: adjustable leakiness parameter
//        /// Fixed structure ANN
//        ANN_FIXED_STRUCTURE = 8
//    };

//    enum EvolutionType {
//        /// deprecated
//        RANDOM_SEARCH = 0,
//        STEADY_STATE = 1,
//        GENERATIONAL = 2,
//        /// deprecated
//        //		EA_NEAT = 3,
//        EMBODIED_EVOLUTION = 4,
//        EA_MULTINEAT = 5,
//    };



//    /// Can be used when there are differences between operating systems. Not encountered yet.
//    enum OS {
//        WINDOWS,
//        LINUX
//    };


//    StartingCondition startingCondition = COND_RUN_EVOLUTION_CLIENT;
//    InstanceType instanceType;		// Whether the code runs in client or server mode
//    EvolutionType evolutionType = STEADY_STATE;			// Type of evolutionary algorithm used
//    FitnessType fitnessType = MOVE;						// Fitness type
//    EnvironmentType environmentType = DEFAULT_ENV;		// Environment type
//    MoveDirection moveDirection = DISTANCE_XY;			// This is used to modify the fitness function by specifying the direction. By default
//    // it is set to DISTANCE_XY which is the horizontal distance moved
//    MorphologyType morphologyType = CAT_MORPHOLOGY;		// Type of morphology used
//    ControlType controlType = ANN_DEFAULT;				// Specifies the type of controller used
//    SelectionType selectionType = RANDOM_SELECTION;		// What is the selection operator (TODO: Not used yet but adviced)
//    ReplacementType replacementType = RANDOM_REPLACEMENT;// What is the replacement operator (TODO: Not used yet but adviced)

//    int indCounter;					// record the number of individuals
//    int sceneNum;					// Experiment ID
//    bool sendGenomeAsSignal = false;// When false, genome is saved as csv and loaded in server. true opens a port and passes the genome as a stringstream from client to server
//    bool client;					// Stating whether the instance is in client mode. DEPRECATED
//    float mutationRate = 0.1; 		// mutation rate...
//    float morphMutRate = 0.1;		// morphology mutation rate...
//    int generation = 0;				// Tracks the generation. Used in single thread.
//    int maxGeneration = 1000;		// maximum number of generations
//    int xGenerations = 4;			// for debugging, this specifies after how many generations the simulator should quit and reopen
//    int populationSize = 3; 		// Population size
//    int individualCounter = 0;		// This variable counts the total number of individuals that have been evaluated during one evolutionary runs
//    int crossover = 0;				// Using crossover (int value for potentially using different crossover strategies)
//    float crossoverRate = 0;		// Rate at which crossover happens. Variable can be used like mutation rate

//    int ageInds = 0;				// Whether age should be used (deprecated) (TODO: DELETE : DEPRECATED)
//    int maxAge = 0;					// An absolute maximum age could be set. (TODO: DELETE : DEPRECATED)
//    int minAge = 0;					// Minimum age, death cannot occur before this (TODO: DELETE : DEPRECATED)
//    float deathProb = 0;			// Probability of individuals being removed at random from the population (TODO: DELETE : DEPRECATED)

//    std::vector<int> indNumbers;			// Vector storing the IDs of the individuals in the current population. This vector can be used to load all individuals from a specific generation
//    std::vector<float> indFits;			// Vector storing the fitness values of the individuals in the current population
//    int seed = 0;					// Random seed passed to the random number generator (TODO: random number generator names are slightly different)


//    std::string exp_plugin_name; //name of the shared library corresponding to your experiment.
//    bool load_external_settings = false;
//    std::string vrep_folder;
//    std::string scene_path;

//    /*enum LSystemType{
//    DEFAULT_LSYSTEM = 0,
//    CUBE_LSYSTEM = 1,
//    JOINT_AND_CUBE_LSYSTEM = 2,
//    MODULAR_LSYSTEM = 3,
//    LIGHT_LSYSTEM = 4,
//    };*/

//    /// modular parameters
//    int useVarModules = 0;			// There was an option to mutate the number of modules in the L-System (TODO: DELETE : DEPRECATED)
//    float maxForce = 1.5;			// Maximum force to be set on all joints
//    float maxForceSensor = 80;		// Maximum force set on all force sensors (N*m)
//    float maxForce_ForceSensor = 0.01;// Maximum force set on all force sensors (by default) TODO: should replace maxForceSensor
//    float maxTorque_ForceSensor = 1000;    // Maximum torque set on all force sensors (by default)

//    //	EvolutionType evolutionType = GENERATIONAL; // not implemented yet

//    int consecutiveThresholdViolations = 10; // Determining how many consecutive force sensor threshold violations will lead to the module breaking
//    int maxNumberModules = 20;		// Maximum number of modules allowed in phenotype
//    std::vector<int> moduleTypes;		// Vector storing the module types
//    std::vector<std::vector<int> > maxModuleTypes; /* Vector storing how many of each module type can be expressed
//                                         NOTE: this takes into account how many times the module type is expressed in total! So if you choose to append module
//                                         type 4 to be appended twice in module Types, both of them will be counted when checking for the maximum module types */

//    int numberOfModules = 5;		// The number of module types used by the L-System (note: not maximum number of modules!!!)


//    std::string repository = "files";	/* The repository seen from the working directory to look for the settings.csv and the genomes.
//                                    NOTE: the morphologies subfolders are not automatically created! */
//    int initialModuleType = 1;		// initial module for direct encoding, similar to axiom of L-System. This is the cube module by default

//    float energyDissipationRate = 0.00;// Can be used to simulate how much energy is consumed by specific modules (TODO: DELETE : DEPRECATED)
//    int lIncrements = 3;			// Specifies how many times the L-System is iterated over or specifies
//    int bestIndividual = 0;			// Tracks the best individual of every generation
//    int loadInd = 0;				// Specifies if an individual should be loaded (used in client-server mode)
//    bool verbose = false;			// Specifies whether to output print statement.
//    Colorization colorization = COLOR_NEURALNETWORK; // To specify how to color the robot morphology. This is useful for debugging.

//    bool killWhenNotConnected = true;// Whether to send a message to shutdown V-REP servers if the connection with the client is lost
//    bool shouldReopenConnections = false;// Sometimes, connections between client and servers are lost, this specifies whether the connections should be reopened.
//    bool createPatternNeurons = false;// Pattern neurons are basically sinusoidal wave functions now



//    // neural network parameters
//    int initialInputNeurons = 3;	// Initial number of input neurons
//    int initialInterNeurons = 1;	// Initial number of interneurons
//    int initialOutputNeurons = 3;	// Initial number of output neurons
//    int initialAmountConnectionsNeurons = 3; // Initial number of connections for each neuron
//    int	maxAddedNeurons = 4;		// The maximum number of neurons that can be added in one call of the mutation operator
//    bool savePhenotype = true;		// Whether to save the phenotype or not
//    std::vector<int> envObjectHandles;	// Object handles of the environment objects (TODO: DELETE : DEPRECATED)

//    void readSettings();			// Reading a .csv settings file
//    void readSettings(const std::string& filename);
//    void saveSettings();			// Saving a .csv settings file
//    bool autoDeleteSettings = false;

//    /// Viability variables
//    // Allow organs above printing bed
//    bool bOrgansAbovePrintingBed = false;
//    // Allow colliding organs
//    bool bCollidingOrgans = false;
//    // Allow any orientation for organs
//    bool bNonprintableOrientations = false;
//    // Any number of organs
//    bool bAnyOrgansNumber =false;

//    bool loadFromQueue = false;

//    /**
//        @brief Set the repository for saving data
//    */
//    void setRepository(const std::string &repository);

//};

}//are

#endif //SETTINGS_H