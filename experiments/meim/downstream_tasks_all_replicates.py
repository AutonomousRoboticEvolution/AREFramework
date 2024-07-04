#! /usr/bin/python3

import sys
import os
import launch_downstream_tasks as ldt 

if __name__ == "__main__":

    if(len(sys.argv) < 3):
        print("usage:\n - arg 1: folder path to set of replicates\n",
              "- arg 2: ARE framework path\n",
              "- arg 3: start port")
        exit(0)

    folder_name = sys.argv[1]
    are_framework = sys.argv[2]
    base_port = sys.argv[2]
    for folder in os.listdir(folder_name):
        if folder.split("_")[0] != "meim":
            continue

        exp_folder = folder_name + "/" + folder + "/selected_robots/"
        ldt.launch_downstream_tasks(exp_folder,are_framework,base_port)
        base_port += 10000