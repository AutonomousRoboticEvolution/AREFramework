# Examples of how to define a new experiment within the ARE Framework

## Simple experiment

First and foremost, create a new folder in the [experiment](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/) folder in which you will put all the sources of your new experiment.

Then, to define a new experiment you have to :

*  Define the new components of your experiment by defining new classes which inherit from the base abstract classes of the framework
*  Define the factories
*  Define the setting class
*  Write a CMakeLists.txt file to install your experiment

### Defining new components

In the folder, simple experiment you can find two classes : [DummyEnv](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/simple_experiment/DummyEnv.h) and [DummyControl](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/simple_experiment/DummyControl.h)

DummyEnv defines an environment for the evaluations and inherit from the [Environment](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/ERFiles/env/Environment.h) asbtract class.
DummyControl defines a controler for the robots and inherit from the [Control](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/ERFiles/control/Control.h) abstract class.

### Defining the factories

You have to define 5 factories :

- the environment factory
- the morphology factory
- the control factory
- the genome factory
- the EA factory

You can find in the source file [factories.cpp](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/simple_experiment/factories.cpp) how it is possible to define them.

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

Each factories have a specific prototype and name which have to be followed otherwise your code will not work properly. 

### Defining the settings class

A base class [Settings](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/ERFiles/Settings.h) is available in the framework. This class is used to load the settings written in the file [settings0.csv](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/simple_experiment/settings0.csv) at the beginning of the experiment. The two options, #extPluginName and #extSettingsClass, have to be define. The first gives the path of your compiled experiment (shared library) and the second indicates if there is additional settings.

So, if you need to define new settings not available in the framework, you have to put this options at 1 and define a new settings class which inherit the Settings class of the framework. You can follow the example of the [ExpSettings](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/simple_experiment/settings.cpp) in the simple experiment folder.

### Install the Experiment

First, you have to write a CMakeLists.txt in your experiment folder. You can follow the example of the [CMakeLists.txt](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/minimal_framework/EvolutionVREP/experiments/simple_experiments/CMakeLists.txt) of the simple experiment.

The CMakeLists.txt have to look like this :
```
cmake_minimum_required(VERSION 3.0)


add_library(exp_name SHARED source_file1 source_file2 ....)
target_include_directories(exp_name PUBLIC
    "../../ERFiles"
    "${VREP_FOLDER}/programming/include"
    )
target_link_libraries(exp_name ER_library)
install(TARGETS exp_name DESTINATION lib) #this line is optional, but it is conveniant to install the shared lib of the experiment for accessibility purpose.
```
Then, you have to add in the CMakeLists.txt of the experiment folder this line :
`add_subfolder(exp_name)`

Finaly, you just have to run the following commands :
'''
cd <the root folder of ARE framework>
mkdir build #if the build folder does not exist yet
cd build
cmake -DCMAKE_INSTALL_PREFIX=where/you/want/to/install ..
make -j4 && make install # add sudo to the make install if the install prefix is /usr/local/
cp experiment_folder/settings0.csv repository_folder/
''' 

### Launch the experiment

'''
cd VREP_FOLDER
./vrep.sh -g0 -g2 -grepo/folder
'''