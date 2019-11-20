# Examples of how to define a new experiment within the ARE Framework

## Simple experiment

First and foremost, create a new folder in the [experiment](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/restruct_refact/EvolutionVREP/experiments/) folder in which you will put all the sources of your new experiment.

Then, to define a new experiment you have to :

*  Define the new components of your experiment by defining new classes which inherit from the base abstract classes of the framework
*  Define the factories
*  Write a CMakeLists.txt file to install your experiment

### Defining new components

In the folder [hyperneat](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/restruct_refact/EvolutionVREP/experiments/hyperneat) you can find several classes which define the EA algorithm, the genotype, the controller, the morphology, the individual and the environment in use for this experiment.


### Defining the factories

You have to define 2 factories :

- the environment factory
- the EA factory

You can find in the source file [factories.cpp](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/restruct_refact/EvolutionVREP/experiments/hyperneat/factories.cpp) how it is possible to define them.

For instance to define the EA factory :

```
extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;
    ea.reset(new are::EA_HyperNEAT(st));
    ea->set_randomNum(rn);
    return ea;
}
```

Each factories have a specific prototype and name which have to be followed otherwise your code will not work properly. 

### Parameters

The parameters of your experiment have to be set in a .csv file. You can find an example [here](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/restruct_refact/EvolutionVREP/experiments/hyperneat/parameters.csv).

The parameters file is in the following format :
```
#expPluginName,string,/usr/local/lib/libhyperneat.so
#populationSize,int,400
#verbose,bool,1
```

Which correspond to : `#<name>,<type>,<value>`

### Install the Experiment

First, you have to write a CMakeLists.txt in your experiment folder. You can follow the example of the [CMakeLists.txt](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/restruct_refact/EvolutionVREP/experiments/hyperneat/CMakeLists.txt) of the simple experiment.

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
```
cd <the root folder of ARE framework>
mkdir build #if the build folder does not exist yet
cd build
cmake -DCMAKE_INSTALL_PREFIX=where/you/want/to/install ..
make -j4 && make install # add sudo to the make install if the install prefix is /usr/local/
export LD_LIBRARY_PATH=where/you/want/to/install/lib
``` 

### Launch the experiment

```
cd VREP_FOLDER
./vrep.sh -gpath/to/parameters/file
```