#!/bin/sh

max=$(($2+$3*100))
for (( port=$2; port<$max; port+=100 ))
do
	python3 -u /share/llegoff/evolutionary_robotics_framework2/simulation/Cluster/run_cluster.py --xvfb 1 --params /share/llegoff/evolutionary_robotics_framework2/experiments/$1/parameters.csv --client /share/llegoff/library2/bin/are-client --vrep /share/llegoff/CoppeliaSim_Edu_V4_3_0_rev12_Ubuntu18_04/coppeliaSim.sh --port-start $port $3
done

