#!/bin/bash
EXPERIMENT="mnipes_v2_protomatrix"
PCNAME="ebb505/ARE_Project"
echo "$EXPERIMENT"
echo "$PCNAME"
echo "Link libraries"
export QT_QPA_PLATFORM='offscreen'; export LD_LIBRARY_PATH=/home/$PCNAME/vrep:/home/$PCNAME/vrep/lib
echo "Chaning to vrep"
cd /home/$PCNAME/evolutionary_robotics_framework/simulation/Cluster
echo "Running vrep"
python3 run_cluster.py 4 --vrep /home/$PCNAME/vrep/vrep.sh --client /home/$PCNAME/evolutionary_robotics_framework/cmake-build-debug/simulation/ERClient/are-client --params /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/parameters.csv --port-start 20000 --xvfb 0
