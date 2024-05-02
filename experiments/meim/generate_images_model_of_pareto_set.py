#! /usr/bin/python3

import sys
import os
import copy
import pandas as pd
import subprocess as sp
import numpy as np

def read_parameters(filename: str) -> dict:
    parameters = dict()
    with open(filename) as file:
        lines = file.readlines()
        for line in lines:
            line = line.split(",")
            if(len(line) < 3):
                continue
            if(line[1] == "int"):
                parameters[line[0]] = int(line[2])
            elif(line[1] == "float"):
                parameters[line[0]] = np.float32(line[2])
            elif(line[1] == "double"):
                parameters[line[0]] = float(line[2])
            elif(line[1] == "bool"):
                parameters[line[0]] = bool(int(line[2]))
            elif(line[1] == "string"):
                parameters[line[0]] = str(line[2])
            elif(line[1] == "sequence_string"):
                parameters[line[0]] = [v for v in line[2].split(";")]
            elif(line[1] == "sequence_int"):
                parameters[line[0]] = [int(v) for v in line[2].split(";")]
            elif(line[1] == "sequence_float" or line[1] == "sequence_double"):
                parameters[line[0]] = [float(v) for v in line[2].split(";")]
            else:
                print("unknown parameter type")
                continue
        return parameters
            

def type_to_string(value):
    if(type(value) == int):
        return "int"
    elif(type(value) == bool):
        return "bool"
    elif(type(value) == np.float32):
        return "float"
    elif(type(value) == float):
        return "double"
    elif(type(value) == str):
        return "string"
    elif(type(value) == list):
        if(type(value[0]) == str):
            return "sequence_string"
        elif(type(value[0]) == np.float32):
            return "sequence_float"
        elif(type(value[0]) == float):
            return "sequence_double"
        elif(type(value[0]) == int):
            return "sequence_int"
        else:
            print("sequence type not handled in parameters")
            return ""
    else:
        print("type not handled in parameters")
        return ""
    
def write_parameters(parameters: dict, folder: str, filename: str):
    content = ""
    for name, value in parameters.items():
        if(type(value) == bool):
            content += name + "," + type_to_string(value) + "," + str(int(value)) + "\n"
        elif(type(value) == list):
            str_value = str(value[0])
            for v in value[1:]:
                str_value +=  ";" + str(v)
            content += name + "," + type_to_string(value) + "," + str_value + "\n"
        else:
            content += name + "," + type_to_string(value) + "," + str(value) + "\n"
    mode = "x"
    if(os.path.exists(folder + "/" + filename)):
        mode = "w"
    with open(folder + "/" + filename,mode) as file:
        file.write(content)

if __name__ == "__main__":
    folder_name = sys.argv[1]
    are_framework = sys.argv[2]
    for folder in os.listdir(folder_name):
        if(folder.split("_")[0] != "meim"):
            continue
        # Create parameter files
        robot_repo = folder_name + "/" + folder + "/selected_robots"
        if not os.path.exists(robot_repo):
            os.makedirs(robot_repo)

        pareto_set = pd.read_csv(folder_name + "/" + folder + "/pareto_set.csv")
        ori_parameters = read_parameters(are_framework + "/experiments/meim/parameters_visu.csv")

        list_ids = list(pareto_set["id"])
        for id in list_ids:
            parameters = copy.copy(ori_parameters)
            parameters["#folderToLoad"] = folder_name + "/" + folder
            parameters["#imageRepository"] = robot_repo
            parameters["#idToLoad"] = id
            parameters["#modelRepository"] = robot_repo
            parameters["#scenePath"] = are_framework + "/simulation/models/scenes/ARE_arena.ttt"
            parameters["#modelsPath"] = are_framework+ "/simulation/models/"
            write_parameters(parameters,robot_repo,"parameters_" + str(id) + ".csv")

        if(len(sys.argv) == 4):    
            vrep_exec = sys.argv[3]
            #run visulaization to create robots images and models
            for filename in os.listdir(robot_repo):
                if(filename.split("_")[0] != "parameters"):
                    continue
                robot_id = filename.split("_")[1].split(".")[0]
                if os.path.exists(robot_repo + "/robot_" + robot_id + "_3.png"):
                    continue
                param_file = robot_repo + "/" + filename
                sp.run([vrep_exec, "-h", "-g" + param_file])


