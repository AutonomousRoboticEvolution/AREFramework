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
    
def load_desciptors(filename: str) -> dict:
    ''' Load descriptors from morph_descriptor.csv from meim, mnipes_v2 experiments
        output: dictionary with id: numpy.array'''

    with open(filename) as file:
        lines = file.readlines()
        descriptors = dict()
        for line in lines:
            line = line.split(",")
            id = int(line[0])
            dim = int(line[1])
            desc = []
            for i in range(dim*dim*dim):
                desc.append(int(line[i+2]))
            descriptors[id] = np.array(desc)
        return descriptors

# def sparsity_fct(descriptors: dict, key: int,k: int) -> float:
#     desc = descriptors[key]
#     distances = []
#     for key2, desc2 in descriptors.items():
#             distances[i,j] = (np.linalg.norm(desc - desc2))
#             distances[j,i] = (np.linalg.norm(desc - desc2))

#         dist = list(distances[i])
#         dist.sort()
#         dist = np.array(dist[:k])
#         return dist.mean()
        
def apply_fitness_threshold(fitnesses: dict, descriptors: dict, fit_thresh: int):
    fit_copy = copy.copy(fitnesses)
    for key, fit in fitnesses.items():
        if(fit < fit_thresh):
            del fit_copy[key]
            del descriptors[key]
    return fit_copy, descriptors


def compute_sparsity(descriptors: dict,k: int) -> dict:
    sparsity = dict()
    distances = np.full((len(descriptors),len(descriptors)),-1)
    for (key, desc), i in zip(descriptors.items(),range(len(descriptors))):
        for (key2, desc2), j in zip(descriptors.items(),range(len(descriptors))):
            if((distances[i,j] != -1)):
                continue
            distances[i,j] = (np.linalg.norm(desc - desc2))
            distances[j,i] = (np.linalg.norm(desc - desc2))

        dist = list(distances[i])
        dist.sort()
        dist = np.array(dist[:k])
        sparsity[key] = dist.mean()
        if(sparsity[key] < 0):
            print(dist)
    return sparsity


def compute_pareto_front(sparsity: dict, fitnesses: dict) -> list:
    lines = []
    for key, fit in fitnesses.items():
        lines.append([fit,sparsity[key]])
    data = pd.DataFrame(lines,columns=["fitness","sparsity"])
    mask = pareto.paretoset(data,sense=["max","max"])
    data["id"] = list(fitnesses.keys())
    pareto_set = data[mask]
    return pareto_set

def density_estimation(descriptors: dict,fitnesses: dict, n: int) -> dict:
    descs = np.array(list(descriptors.values()))
    fits = np.array(list(fitnesses.values()))
    gmm = mixture.GaussianMixture(n_components=n, covariance_type="full")
    gmm.fit(descs,fits)
    return gmm


if __name__ == "__main__":
    folder_name = sys.argv[1]
    for folder in os.listdir(folder_name):
        if(folder.split("_")[0] != "meim"):
            continue
        exp_folder = folder_name + "/" + folder
        descriptors = load_desciptors(exp_folder + "/morph_descriptor.csv")
        fitnesses = load_fitnesses(exp_folder + "/fitness.csv")
        fitnesses, descriptors = apply_fitness_threshold(fitnesses,descriptors,float(sys.argv[3]))
        sparsity = compute_sparsity(descriptors,15)
        pareto_set = compute_pareto_front(sparsity,fitnesses)
        pareto_set.to_csv(exp_folder + "/" + sys.argv[2],index=False)
        print(pareto_set)

