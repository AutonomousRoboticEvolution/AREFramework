#!/bin/bash
EXPERIMENT="nipes"
PCNAME="are"
echo "$EXPERIMENT"
echo "$PCNAME"
echo "Link libraries"
export QT_QPA_PLATFORM='offscreen'; export LD_LIBRARY_PATH=/home/are/vrep:/home/$PCNAME/vrep/lib
echo "Chaning to vrep"
cd /home/$PCNAME/vrep/
echo "Running vrep"
./vrep.sh -h -g/home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/parameters.csv &
