#!/bin/bash
EXPERIMENT="nipes"
PCNAME="are"
echo "$EXPERIMENT"
echo "Link libraries"
export QT_QPA_PLATFORM='offscreen'; export LD_LIBRARY_PATH=/home/are/vrep:/home/$PCNAME/vrep/lib
echo "Changing to vrep"
cd /home/$PCNAME/evolutionary_robotics_framework/simulation/Cluster
echo "Running vrep"
python3 run_cluster.py 32 --vrep /home/$PCNAME/vrep/vrep.sh --client /home/$PCNAME/evolutionary_robotics_framework/build/simulation/ERClient/are-client --params /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/parameters.csv --port-start 20000 --xvfb 0 &
