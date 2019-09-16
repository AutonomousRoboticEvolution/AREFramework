# Evolutionary Robotics Framework

This directory contains all the necessary files to run evolution with C++ and V-REP as simulator.

## Installing

In order to install this plu-in please open terminal in the current project and enter the following commands.

```
mkdir build
cd build
cmake ..
make
make install
```
If you would like to use CLion IDE with this project please follow the instructions in the [report](https://www.overleaf.com/8988212588bdkjhpfdtckz).

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
* **Frank Veenstra**

See also the list of [contributors](https://www.york.ac.uk/robot-lab/are/) who participated in this project.
