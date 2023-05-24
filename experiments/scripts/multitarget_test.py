#! /usr/bin/python3

import os
import sys
import subprocess

def write_parameter_file(parameters,filename):
	with open(filename,"w") as file:
		for param in parameters:
			if len(param) == 3:
				file.write(param[0] + "," + param[1] + "," + param[2])



if(len(sys.argv) != 5):
	print("usage:")
	print("\targ 1 : path to experiment logs folder")
	print("\targ 2 : path to base parameter file")
	print("\targ 3 : ARE framework experiments folder, e.g. evolutionary_robots_framework/experiments")
	print("\targ 4 : start port")
	exit(1)

archive_folder = sys.argv[1]
parameter_file = sys.argv[2]
experiments_folder = sys.argv[3]
start_port = int(sys.argv[4])

folder_list = [f for f in os.listdir(archive_folder)]
parameters = [x.split(',') for x in open(parameter_file).readlines()]

start_ports = [start_port + i*100 for i in range(len(folder))]

for folder, sp in zip(folder_list,start_ports):
	for i in range(len(parameters)):
		if(parameters[i][0] == "#folderToLoad"):
			parameters[i] = ["#folderToLoad","string",archive_folder + "/" + folder + "\n"]
		exp_folder = experiments_folder + "/multitarget_test_" + folder.split("/")[-1]
		if not os.path.exists(exp_folder):
			os.makedirs(exp_folder)
		write_parameter_file(parameters,exp_folder + "/parameters.csv")
		subprocess.run(["sbatch","--cpus-per-task=10","--job-name=mutate","../are-parallel.job","nipes_four_legged",str(sp),"10"])
