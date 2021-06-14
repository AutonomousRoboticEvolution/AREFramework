# Autonomous Robotic Evolution Framework

## Introduction

The ARE Framework is an evolutionary robotic framework made within the ARE project. It is structured around an evolutionary algorithm structure allowing to evolve morphologies and controllers. Currently, it is designed as a plugin to the V-REP simulator (Coppelia). The following instruction is related to the master branch. 

After, having successfully installed the framework, you can refer to the next tutorial: [How to define an experiment within the ARE Framework](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/wiki/Defining%20an%20experiment%20within%20the%20ARE%20Framework).

## Required software

* Preferably use Ubuntu 18.04 as OS. Works also with Ubuntu 20.04
* CoppeliaSim Edu 4.1.0 18.04 - [link](https://www.coppeliarobotics.com/files/CoppeliaSim_Edu_V4_1_0_Ubuntu18_04.tar.xz) (do not use either version 4.0.0 or version 4.2.0). Works also with V-REP 3.6.2.
* MultiNEAT -  [link](https://github.com/ci-group/MultiNEAT)
* Polyvox - [link](https://github.com/portaloffreedom/polyvox) 
* libdlibxx - [link](https://github.com/m-renaud/libdlibxx)
* Boost (apt package libboost-all-dev)
* Eigen 3 (apt package libeigen3-dev)
* TBB - (apt package libtbb-dev)

Dependencies for Morphogenesis and Hyperneat

* Polyvox - [link](https://github.com/portaloffreedom/polyvox) 
* MultiNEAT -  [link](https://github.com/ci-group/MultiNEAT)

Dependencies for learning 

* LIBCMAES - [link](https://github.com/beniz/libcmaes)
* NN2 (please put this code in modules) - [link](https://github.com/LeniLeGoff/nn2)
* Limbo (cmake -DLIMBO_FOLDER=/limbo_folder/) .. - [link](https://github.com/resibots/limbo)

## Installation

### Dependencies 

These instructions are specific to Ubuntu (preferably 18.04).

First, install the libraries available via aptitude :
```
sudo apt install libboost-all-dev libeigen3-dev 
```

Install MutliNEAT :
```
git clone https://github.com/ci-group/MultiNEAT.git
cd MultiNEAT
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path  ..
make 
make install # add sudo before the command if the install prefix is /usr/local (default value)
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

Install Limbo
```
git clone https://github.com/resibots/limbo.git
```
This library has headers only, so there is no need to be compiled or installed. 

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
* ONLY_SIMULATION: (0|1) whether to compile only for simulation
* ONLY_PHYSICAL: (0|1) whether to compile only for real experiment
* RASPBERRY: (0|1) compile on a raspberry pi. Will compile only the part needed to control a ARE robot or Thymio
* LIMBO_FODLER: path to limbo headers, if using bayesian optimisation
* WITH_NN2: (0|1) if using the NN2 headers library

**Note on the options VREP_FOLDER and COPPELIASIM_FOLDER** Only one of these options have to be used. The other one have to be let empty. There are both empty per default but if you want to switch from V-REP to CoppeliaSim (or the opposite) you have to explicitly set one to empty.  

In order to install the framework
```
git clone https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework.git
cd evolutionary_robotics_framework
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/install/path -DCOPPELIASIM_FOLDER=/coppeliasim/folder/path .. #add the other option needed 
make
make install # add sudo if the install prefix is /usr/local/ (default value)
```
If you would like to use CLion IDE with this project please follow the instructions in the [report](https://www.overleaf.com/8988212588bdkjhpfdtckz).

## How to launch an experiment

You can launch experiments in two different ways : in local mode or in remote mode. The local mode is for launching sequential experiments only, it is meant to be a debug mode. The remote mode is for launching several simulator instances and thus launch an experiment in parallel.

### Local Mode

```
cd v_rep_folder
./vrep.sh -h -g/path/to/exp/parameters/file/parameters.csv
```
-h option is for headless, which means without a GUI. The parameters file is a csv file in which all the parameters of your experiment are written. You can find an example [here](https://bitbucket.org/autonomousroboticsevolution/evolutionary_robotics_framework/src/restruct_refact/EvolutionVREP/experiments/hyperneat/parameters.csv).
All the logs of your experiment are stored in a folder named after the name of your experiment (specified in the parameters file) and the date of its launch. This folder is itself in the repository folder you specified in the parameters file.

### Remote Mode

```
cd are_framework_folder/EvolutionVREP/Cluster
python3 run_cluster.py N --vrep vrep_folder/vrep.sh --client are_framework_folder/EvolutionVREP/ERClient/ERClient --params /path/to/exp/parameters/file/parameters.csv --port_start PORT_START --xvbf (0|1)
```
N corresponds to the number of simulator instances you want to launch which should not be greater than the number of core. PORT_START will be the communication port of the first simulator instance, then PORT_START + 1 will the port for the second instance and so on.
the --xvbf option is to be set at 1 if you launch your experiment through ssh. This script will launch N simulator instance and a client process which handles the experiment.

Like in local mode, all the ogs of your experiment are stored in a folder named after the name of your experiment (specified in the parameters file) and the date of its launch. This folder is itself in the repository folder you specified in the parameters file.
As well, you have log files corresponding to the outputs of the client and all the simulator instances stored where you launched your experiment. This files are named client_<date> for the client output and sim_<nb-instance>_<date>.
