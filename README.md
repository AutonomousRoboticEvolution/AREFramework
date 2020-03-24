# Evolutionary Robotics Framework

This project contains all the files necessary to run experiments for the Autonomous Robotics Evolution project.

## Required libraries

- MultiNEAT
- PolyVox

## Installing

In order to install this plu-in please open terminal in the current project and enter the following commands.

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=where/you/want/to/install ..
make
make install # add sudo if the install prefix is /usr/local/
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


## Overall structure

* *Ecosystem Manager* contains all the files necessary to the Ecosystem Manager
* *RobotFab* contains all the files necessary to run the Robot Fabricator
* *EvolutionVREP* contains all the files necessary to run evolution in simulation with V-REP as simulator
* *RobotController* contains all the files necessary to compile the controllers to physical robots.

## Authors

* **Matt Hale**
* **Edgar Buchanan**
* **Wei Li**
* **Matteo de Carlo**
* **Robert Woolley**
* **Léni K. Le Goff**

See also the list of [contributors](https://www.york.ac.uk/robot-lab/are/) who participated in this project.
