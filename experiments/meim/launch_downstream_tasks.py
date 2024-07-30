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
            elif(line[1] == "sequence_float"):
                parameters[line[0]] = [np.float32(v) for v in line[2].split(";")]
            elif(line[1] == "sequence_double"):
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

def tasks_parameters(are_folder,nbr_inputs: int, nbr_outputs: int) -> list:
    loco_corr = {  "#experimentName":"nipes_loco_corridor", 
                   "#envType":int(6),
                   "#maxEvalTime":np.float32(240),
                   "#nbrWaypoints":int(720),
                   "#initPosition":[5.5,0.0,0.2],
                   "#NbrInputNeurones":int(nbr_inputs),
                   "#NbrOutputNeurones":int(nbr_outputs),
                   "#scenePath":are_folder + "/simulation/models/scenes/long_arena.ttt"}
    
    loco_rough = copy.copy(loco_corr)
    loco_rough["#experimentName"] = "nipes_loco_rough"
    loco_rough["#scenePath"] = are_folder + "/simulation/models/scenes/rough_terrain.ttt"

    push_object = copy.copy(loco_corr)
    push_object["#experimentName"] = "nipes_push_object" 
    push_object["#envType"] = int(8)
    push_object["#targetPosition"] = [5.0,0.0,0.05]

    hill_climbing = {"#experimentName":"nipes_hill_climb",
                   "#envType":int(7),
                   "#maxEvalTime":np.float32(120),
                   "#nbrWaypoints":int(360),
                   "#initPosition":[1.4,1.4,0.2],
                   "#NbrInputNeurones":int(nbr_inputs),
                   "#NbrOutputNeurones":int(nbr_outputs),
                   "#scenePath":are_folder + "/simulation/models/scenes/hill.ttt"}

    mt_arena = {"#experimentName":"nipes_multitargets",
                "#envType":int(2),
                "#envNumber":int(3),
                "#maxEvalTime":np.float32(60),
                "#nbrWaypoints":int(180),
                "#initPosition":[-0.6,0.6,0.2],
                "#targets":[0.6,0.6,0.12,0.6,-0.6,0.12,-0.6,-0.6,0.12],
                "#NbrInputNeurones":int(nbr_inputs),
                "#NbrOutputNeurones":int(nbr_outputs),
                "#scenePath": are_folder + "/simulation/models/scenes/ARE_arena.ttt"}

    return [("lc",loco_corr),
            ("lr",loco_rough),
            ("hc",hill_climbing),
            ("po",push_object),
            ("mta",mt_arena)]



def get_io_nn_from_blueprint(filename: str):
    with open(filename) as file:
        lines = file.readlines()
        nbr_sensors = 0
        nbr_joints = 0
        nbr_wheels = 0
        for line in lines:
            organ_type = int(line.split(",")[1])
            if(organ_type == 1):
                nbr_wheels += 1
            elif(organ_type == 2):
                nbr_sensors += 1
            elif(organ_type == 3):
                nbr_joints += 2
        #return inputs, outputs
        return nbr_sensors*2 + nbr_wheels + nbr_joints, nbr_wheels + nbr_joints

def get_id_list(foldername: str) -> list:
    ids = []
    for filename in os.listdir(foldername):
        if filename.split("_")[0] == "model":
            ids.append(int(filename.split("_")[1].split(".")[0]))
    return ids

def launch_downstream_tasks(folder_name,are_framework,base_port):
    id_list = get_id_list(folder_name)

    for id in id_list:
        ori_parameters = read_parameters(are_framework + "/experiments/meim/parameters_nipes.csv")
        nbr_inputs, nbr_outputs = get_io_nn_from_blueprint(folder_name + "/blueprint_" + str(id) + ".csv")
        tasks_params = tasks_parameters(are_framework,nbr_inputs,nbr_outputs)
        for task in tasks_params:
            parameters = copy.copy(ori_parameters)
            parameters = parameters | task[1]
            parameters["#robotPath"] = folder_name + "/model_" + str(id) + ".ttm"
            write_parameters(parameters,folder_name,"parameters_nipes_" + task[0] + "_" + str(id) + ".csv")

            if base_port > 0:
                sp.run(["sbatch","--cpus-per-task=64","--job-name=" + task[0] + str(id),
                    are_framework + "/experiments/meim/downstream_task.job",
                    folder_name + "/parameters_nipes_" + task[0] + "_" + str(id) + ".csv",
                    str(base_port),"64"])
                base_port += 1000   

if __name__ == "__main__":
    if(len(sys.argv) < 3):
        print("usage:\n - arg 1: folder path to set of robots\n",
              "- arg 2: ARE framework path\n",
              "- arg 3: start port (optional put if you want to launch slurm jobs) ")
        exit(0)

    folder_name = sys.argv[1]
    are_framework = sys.argv[2]

    base_port = -1
    if len(sys.argv) == 4:
        base_port = int(sys.argv[3])

    launch_downstream_tasks(folder_name,are_framework,base_port)     
