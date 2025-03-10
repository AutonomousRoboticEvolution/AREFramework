# Autonomous Robotic Evolution Framework

Version of the ARE Framework used for the paper Efficient and Diverse Generative Robot Designs using Evolution and Intrinsic Motivation presented at ICRA 2025 in Atlanta. Link to the _paper_.

The ARE Framework is an evolutionary robotic framework made within the ARE project to generate robotic designs and controllers. Currently, it is designed as a plugin to the V-REP simulator (Coppelia).

You can install the framework or use the docker image

# Docker

# Without Docker

## Required software

* Preferably use Ubuntu 20.04 or 22.04
* Polyvox - [link](https://github.com/portaloffreedom/polyvox) 
* libdlibxx - [link](https://github.com/m-renaud/libdlibxx)
* Boost 1.77 - [link](https://www.boost.org/users/history/version_1_77_0.html)
* Eigen 3 (apt package libeigen3-dev)
* TBB - (apt package libtbb-dev)
* LIBCMAES - [link](https://github.com/beniz/libcmaes)


## Installation

### Dependencies 

These instructions are specific to Ubuntu (preferably 20.04 or 22.04).

First, install the libraries available via aptitude :
```
sudo apt install libboost-all-dev libeigen3-dev 
```

Install Polyvox :
```
git clone https://github.com/portaloffreedom/polyvox.git
cd polyvox
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path  ..
make 
make install # add sudo before the command if the install prefix is /usr/local (default value)
```

Install libdlibxx :
```
git clone https://github.com/m-renaud/libdlibxx.git
cd libdlibxx
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path  ..
make 
make install # add sudo before the command if the install prefix is /usr/local (default value)
```

Install LIBCMAES
```
git clone https://github.com/beniz/libcmaes.git
cd libcmaes
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local  .. # you can install somewhere else if you'd like, but this is default
make
sudo make install
```

Install NN2
```
git clone https://github.com/LeniLeGoff/nn2
```
This library has headers only, so there is no need to be compiled or installed. 
And this is a module of the framework

### The framework

The framework has compilation options you can pass to cmake:

* VREP_FOLDER: the path to V-REP simulator if using V-REP 3.6.2
* COPPELIASIM_FOLDER: the path Coppelia simulator if using CoppeliaSim 4.1.0
* WITH_NN2: (0|1) if using the NN2 headers library

**Note on the options VREP_FOLDER and COPPELIASIM_FOLDER** Only one of these options have to be used. The other one have to be let empty. There are both empty per default but if you want to switch from V-REP to CoppeliaSim (or the opposite) you have to explicitly set one to empty.  

In order to install the framework
```
git clone https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework.git
cd evolutionary_robotics_framework
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path -DVREP_FOLDER=/<are framework path>/vrep-3.6.2/ -DWITH_NN .. #add the other option needed 
make
make install # add sudo if the install prefix is /usr/local/ (default value)
```

## How to launch an experiment

### Morpho-Evolution with Homeokinesis (MEHK) and with Fixed-Controller (MEFC)

First, you need to edit the following files:
```
/<are framework folder>/experiments/meim/parameters_mehk.csv
/<are framework folder>/experiments/meim/parameters_mefc.csv
```
And replace the following lines with your file path:
```
#expPluginName,string,/<installation folder>/libmeim.so
#scenePath,string,/<are framework folder>/simulation/models/scenes/exp_side_obstacles_hard.ttt
#modelsPath,string,/<are framework folder>/simulation/models/
#robotPath,string,/<are framework folder>/simulation/models/robots/are_two_legged/model_0.ttm
#repository,string,/<repository folder>/
```

To launch the experiment with MEHK run the following commands:
```
cd simulation/Cluster
python3 run_cluster.py N --vrep /<are framework folder>/vrep-3.6.2/vrep.sh --client /<installation folder>/bin/are-client --params /<are framework folder>/experiments/meim/parameters_mehk.csv --port_start PORT-START --xvfb 0
```

To launch the experiment with MEFC run the following commands:
```
cd simulation/Cluster
python3 run_cluster.py N --vrep /<are framework folder>/vrep-3.6.2/vrep.sh --client /<installation folder>/bin/are-client --params /<are framework folder>/experiments/meim/parameters_mefc.csv --port_start PORT-START --xvfb 0
```

if it runs without problems, you should get one file called _client\_XXX.out_ and N files called _sim\_XXX.out_ where you launched the experiments. And log files in the repository folder you specified in the parameters file. The log files will be stored in folder called _meim\_XXX_.

### Robot Selection

To perform the robot selection, first generate a pareto set and then generate the models of the selected robot.
To generate the pareto set:
```
python3 generate_pareto_set.py <path to repository folder> <pareto file name> <fitness threshold> L2
```
This will output a file with the ids of the robot selected along with their fitnesses and morphological sparsity score.
Then to generate the models:
```
python3 generate_images_model_of_pareto_set.py <path to repository folder> <path to the ARE framework> L2 no <path to ARE framework>/vrep-3.6.2/vrep.sh
```
This will generate the models of 3 selected robot in folder called _selected\_robots_ in the log folder _meim\_XXX_. The models are in the following format _model\_ID.ttm_.

### Learning the downstream tasks  

To train a robot on a downstream task, first edit the file _parameters\_downstream\_task.csv_ to replace following line with your local paths:
```
#expPluginName,string,/<installation folder>/libNIPES.so
#robotPath,string,/<path to the robot model>/
#modelsPath,string,/<are framework folder>/simulation/models/
#repository,string,/<repository folder>/
#vrepFolder,string,/<are framework folder>/vrep-3.6.2/
```
_path to the robot model_ should correspond to the robot you want to train. 

And to select the task.
For hill climbing task:
```
#scenePath,string,/<are framework folder>/simulation/models/scenes/hill.ttt
#envType,int,7
```
For locomotion:
```
#scenePath,string,/<are framework folder>/simulation/models/scenes/long_arena.ttt or rough_terrain.ttt
#envType,int,0
```
For object manipulation:
```
#scenePath,string,/<are framework folder>/simulation/models/scenes/long_arena.ttt
#envType,int,8
```

Finally, to launch the experiment:
```
cd simulation/Cluster
python3 run_cluster.py N --vrep /<are framework folder>/vrep-3.6.2/vrep.sh --client /<installation folder>/bin/are-client --params /<are framework folder>/experiments/nipes/parameters_downstream_task.csv --port_start PORT-START --xvfb 0
```


## Visualize robots behaviours

### Homeokinesis

TO DO

### Learned controller

TO DO