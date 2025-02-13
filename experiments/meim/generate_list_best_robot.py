#! /usr/bin/python3

import sys
import os
import numpy as np
from sklearn import mixture
from sklearn import cluster
import paretoset as pareto
import pandas as pd
import copy

def load_fitnesses(filename: str) -> dict:
    ''' load fitness.csv file from meim, mnipes_v2 experiments
        output: dictionary with id: fitness value '''
    
    with open(filename) as file:
        lines = file.readlines()
        fitnesses = dict()
        for line in lines:
            line = line.split(",")
            fitnesses[int(line[0])] = float(line[-2])
        return fitnesses
    
 
def apply_fitness_threshold(fitnesses: dict, fit_thresh: int):
    fit_copy = copy.copy(fitnesses)
    for key, fit in fitnesses.items():
        if(fit < fit_thresh):
            del fit_copy[key]
    return fit_copy





if __name__ == "__main__":
    if(len(sys.argv) != 4):
        print("usage:\n - arg 1: folder path\n",
              "- arg 2: file name\n", 
              "- arg 3: fitness threshold\n")
        exit(0)


    folder_name = sys.argv[1]
    for folder in os.listdir(folder_name):
        if(folder.split("_")[0] != "meim"):
            continue
        print(folder)
        exp_folder = folder_name + "/" + folder
        if os.path.exists(exp_folder + "/" + sys.argv[2]):
            continue
        fitnesses = load_fitnesses(exp_folder + "/fitness.csv")
        fitnesses = apply_fitness_threshold(fitnesses,float(sys.argv[3]))
        print("Number of individuals =",len(fitnesses))
        fits = pd.DataFrame(list(fitnesses.items()),columns=["id","fitness"])
        print(fits)
        fits.to_csv(exp_folder + "/" + sys.argv[2],index=False)
        

