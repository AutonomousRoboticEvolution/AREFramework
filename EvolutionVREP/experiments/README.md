# Examples of how to use the ARE Framework

## Simple experiment

To define a new experiment you have to :

*  Define the new component of your experiment by defining new classes which inherit from the base abstract classes in the framework
*  Define the factories
*  Define the setting class


### Defining new components

In the folder, simple experiment you can find two classes : [DummyEnv](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/examples/simple_experiment/DummyEnv.h) and [DummyControl](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/examples/simple_experiment/DummyControl.h)

DummyEnv defines an environment for the evaluations and inherit from the [Environment](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/ERFiles/env/Environment.h) asbtract class.
DummyControl defines a controler for the robots and inherit from the [Control](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/ERFiles/control/Control.h) abstract class.

### Defining the factories

You have to define 5 factories :

- the environment factory
- the morphology factory
- the control factory
- the genome factory
- the EA factory

You can find in the source file [factories.cpp](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/examples/simple_experiment/factories.cpp) how it is possible to define them.

For instance to define the EA factory :

```
extern "C" std::shared_ptr<EA> EAFactory(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
    std::shared_ptr<EA> ea;
    if(st->evolutionType == Settings::STEADY_STATE)
        ea.reset(new EA_SteadyState(*st));
    else if (st->evolutionType == Settings::EA_MULTINEAT)
        ea.reset(new EA_MultiNEAT(*st));

    ea->randomNum = rn;
    return ea;
}
```

Each factories have a specific prototype which have to be followed otherwise your code will not work properly. 

### Defining the settings class

### Install the Experiment

First, you have to write a CMakeLists.txt in your experiment folder. You can do either like in the present example which is a subfolder of the ARE Framework or put it in an external folder.
You can follow the example of the [CMakeLists.txt](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/examples/CMakeLists.txt) of the simple experiment.

As subfolder of the framework : 
```
cmake_minimum_required(VERSION 3.0)


add_library(exp_name SHARED source_file1 source_file2 ....)
target_include_directories(simple_experiment PUBLIC
    "../ERFiles"
    "${VREP_FOLDER}/programming/include"
    )
target_link_libraries(exp_name ER_library)
install(TARGETS exp_name DESTINATION lib) #this line is optional, but it is conveniant to install the shared lib of the experiment for accessibility.
```
Then, you have to add at the end of the CMakeLists.txt of the framework :
`
