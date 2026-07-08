# Autonomous Robotic Evolution Framework
The ARE Framework is an evolutionary robotic framework made within the ARE project. It is structured around an evolutionary algorithm structure allowing to evolve morphologies and controllers. Currently, it is designed as a plugin to the V-REP simulator (Coppelia). The following instruction is related to the master branch. 

After having successfully installed the framework, you can refer to the next tutorial: [How to define an experiment within the ARE Framework](https://github.com/AutonomousRoboticEvolution/AREFramework/wiki/Defining-an-experiment-within-the-ARE-Framework).

## Required software

* Preferably use Ubuntu 22.04
* Polyvox - [link](https://github.com/portaloffreedom/polyvox) 
* libdlibxx - [link](https://github.com/m-renaud/libdlibxx)
* Boost 1.77 - [link](https://www.boost.org/users/history/version_1_77_0.html)
* Eigen 3 (apt package libeigen3-dev)
* TBB - (apt package libtbb-dev)
* libtorch (https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-2.12.1%2Bcpu.zip)
* LIBCMAES - [link](https://github.com/beniz/libcmaes)

## Installation

### Dependencies 


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

* COPPELIASIM_FOLDER: the path Coppelia simulator if using CoppeliaSim 4.1.0
* ONLY_SIMULATION: (0|1) whether to compile only for simulation
* ONLY_PHYSICAL: (0|1) whether to compile only for real experiment
* RASPBERRY: (0|1) compile on a raspberry pi. Will compile only the part needed to control a ARE robot or Thymio
* LIMBO_FODLER: path to limbo headers, if using bayesian optimisation
* WITH_NN2: (0|1) if using the NN2 headers library

In order to install the framework
```
git clone https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework.git
cd evolutionary_robotics_framework
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path -DCMAKE_PREFIX_PATH=</path/to/libtorch>/lib/ -DCOPPELIASIM_FOLDER=/AREFramework/coppeliaSim/ -DWITH_NN2=1 ..
make
make install # add sudo if the install prefix is /usr/local/ (default value)
```

## How to launch an experiment

To launch nipes experiments:
First, open the parameter file of the nipes experiment: AREFramework/experiments/nipes/parameters.csv
And check if the following parameters correspond to where the AREFramework is installed:
```
#expPluginName,string,/usr/local/lib/ARE/experiments/libNIPES.so
#scenePath,string,/home/leni/git/AREFramework/simulation/models/scenes/exp_side_obstacles_hard.ttt
#robotPath,string,/home/leni/git/AREFramework/simulation/models/robots/are_four_legged/model_2.ttm
#modelsPath,string,/home/leni/git/AREFramework/simulation/models/
#repository,string,/home/leni/are-logs/
```
Then, the parameters related to the neural network architecture correspond to the robot you are loading:
```
#NbrInputNeurones,int,12
#NbrOutputNeurones,int,8
#NbrHiddenNeurones,int,6
```
For instance, the are_four_legged robot has 2 sensors with 2 inputs each (proximity sensor and IR detector) and 8 joints. This results in 12 inputs (4 exterioceptives and 8 proprioceptives) and 8 outputs.

Then run the experiments in headless mode and 10 simulators. 
```
python3 AREFramework/simulation/app/run.py --xvfb 0 --headless 2 --params AREFramework/experiments/nipes/parameters.csv --client are-client --port-start 10000 --coppelia AREFramework/coppeliaSim/coppeliaSim 10
```
To have a full description of the options:
```
python3 AREFramework/simulation/app/run.py -h
```
