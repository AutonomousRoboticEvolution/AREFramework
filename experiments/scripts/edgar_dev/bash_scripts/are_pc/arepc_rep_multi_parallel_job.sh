#!/bin/bash
EXPERIMENT="nipes"
PCNAME="are"
echo "$EXPERIMENT"
echo "Link libraries"
export QT_QPA_PLATFORM='offscreen'; export LD_LIBRARY_PATH=/home/are/vrep:/home/$PCNAME/vrep/lib
echo "Changing to vrep"
cd /home/$PCNAME/evolutionary_robotics_framework/simulation/Cluster
parameterValues=(30 60 90 120)
for t in ${parameterValues[@]}; do
	echo "$t"
	for i in {1..20}; do
		echo "$i"
		cp /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/parameters.csv /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/arepcpam.csv
		sed -i '/\#maxEvalTime,float,/ s/$/'$t'/' /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/arepcpam.csv
		sed -i '/\#seed,int,/ s/$/'$i'/' /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/arepcpam.csv
		python3 run_cluster.py 20 --vrep /home/$PCNAME/vrep/vrep.sh --client /home/$PCNAME/evolutionary_robotics_framework/build/simulation/ERClient/are-client --params /home/$PCNAME/evolutionary_robotics_framework/experiments/$EXPERIMENT/arepcpam.csv --port-start 20000 --xvfb 0
		killall -9 vrep; killall -9 are-client;
	done
	echo "Done!"
done
echo "Done!!!"
