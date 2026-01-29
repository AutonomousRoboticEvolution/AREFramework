#! /usr/bin/python3

import sys
import os
import copy
import pandas as pd
import subprocess as sp
import numpy as np
import zipfile as zf

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
    # if(len(sys.argv) < 4):
    #     print("usage:\n - arg 1: folder path\n",
    #           "- arg 2: ARE framework path\n", 
    #           "- arg 3: best robots file name\n")
    #     exit(0)

    # folder_name = sys.argv[1]
    best_filename = sys.argv[1]
    repository = sys.argv[2]
    encoding = sys.argv[3]
    zip_archive = sys.argv[4]

    morph_genome_type = 0
    if(encoding == "dual_cppn"):
        morph_genome_type = 3
    elif(encoding == "sq_cppn"):
        morph_genome_type = 1
    # are_framework = sys.argv[2]
    
    best_robots = pd.read_csv(best_filename)
    with zf.ZipFile("/media/leni/DATADRIVE1/gecco_2026/"+zip_archive) as zipfile:
        for replicate, id in zip(best_robots["replicate"],best_robots["robot index"]):
            print(replicate,id)
            folder_name = replicate.split(repository)[1]
            if(encoding == "dual_cppn"):
                try:
                    zipfile.extract(folder_name + "/org_cppn_" + str(id),path=repository)
                except:
                    print("org_cppn not found")
                    continue
                try:
                    zipfile.extract(folder_name + "/skel_cppn_" + str(id),path=repository)
                except:
                    print("skel_cppn not found")
                    continue
            elif(encoding == "sq_cppn"):
                try:
                    zipfile.extract(folder_name + "/quadrics.csv",path=repository)
                except:
                    print("quadrics.csv not found")
                    continue
                try:
                    zipfile.extract(folder_name + "/cppn_" + str(id),path=repository)
                except:
                    print("cppn not found")
                    continue
            else:
                print("algorithm not recognized: dual_cppn or sq_cppn expected")
                exit(0)

            if(len(sys.argv) < 6):
                continue
        
            are_framework = sys.argv[5]

            ori_parameters = read_parameters(are_framework + "/experiments/meim/parameters_visu2.csv")

            parameters = copy.copy(ori_parameters)
            parameters["#folderToLoad"] = replicate
            parameters["#imageRepository"] = replicate + "/robots/"
            parameters["#idToLoad"] = id
            parameters["#modelRepository"] = replicate + "/robots/"
            parameters["#scenePath"] = are_framework + "/simulation/models/scenes/ARE_arena.ttt"
            parameters["#modelsPath"] = are_framework+ "/simulation/models/"
            parameters["#morphGenomeType"] = morph_genome_type
            write_parameters(parameters,replicate,"parameters_visu_" + str(id) + ".csv")

            if(len(sys.argv) == 7):    
                vrep_exec = sys.argv[6]
                #run visualization to create robots images and models

                if os.path.exists(replicate + "/robots/robot_" + str(id) + "_3.png"):
                    print(replicate,id,": robot image and model already generated skip")
                    continue
                param_file = replicate + "/parameters_visu_" + str(id) + ".csv"
                sp.run([vrep_exec, "-h", "-g" + param_file])


