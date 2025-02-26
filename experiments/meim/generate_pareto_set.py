#! /usr/bin/python3

import sys
import os
import numpy as np
from sklearn import mixture
from sklearn import cluster
import paretoset as pareto
import pandas as pd
import copy

def load_fitnesses(filename: str, max = -1) -> dict:
    ''' load fitness.csv file from meim, mnipes_v2 experiments
        output: dictionary with id: fitness value '''
    with open(filename) as file:
        counter = 0
        lines = file.readlines()
        fitnesses = dict()
        for line in lines:
            if counter == max:
                break
            line = line.split(",")
            fitnesses[int(line[0])] = float(line[-2])
            counter+=1

        return fitnesses
    
def load_descriptors(filename: str, max = -1) -> dict:
    ''' Load descriptors from morph_descriptor.csv from meim, mnipes_v2 experiments
        output: dictionary with id: numpy.array'''

    with open(filename) as file:
        counter = 0
        lines = file.readlines()
        descriptors = dict()
        for line in lines:
            if counter == max:
                break
            line = line.split(",")
            id = int(line[0])
            dim = int(line[1])
            desc = []
            for i in range(dim*dim*dim):
                desc.append(int(line[i+2]))
            descriptors[id] = np.array(desc)
            counter += 1
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


def organ_position_distance(v : np.array,w : np.array) -> float:
    sum: int = 0
    dim: int = 11
    v_coord = []
    w_coord = []
    for x in range(dim):
        for y in range(dim):
            for z in range(dim):
                i = x + y*dim + z*dim*dim
                if w[i] > 0 and v[i] > 0 :
                    sum += dim if v[i] != w[i] else 0
                    continue
                elif v[i] > 0:
                    v_coord.append((x,y,z))
                elif w[i] > 0:
                    w_coord.append((x,y,z))
    
    if len(v_coord) == 0 and len(w_coord) == 0:
        return sum
    if len(v_coord) == 0:
        for _ in w_coord:
            sum += dim
        return sum
    if len(w_coord) == 0:
        for _ in v_coord:
            sum += dim
        return sum
    
    V = (v,v_coord)
    W = (w,w_coord)
    L = max(V,W,key= lambda a: len(a[1]))
    S = min(W,V,key= lambda a: len(a[1]))
    distances = []
    L1 = lambda a,b : abs(a[0] - b[0]) + abs(a[1] - b[1]) +abs(a[2] - b[2])
    for i in range(len(L[1])):
        dists = []
        for j in range(len(S[1])):
            dists.append([L1(L[1][i],S[1][j]),L[1][i],S[1][j]])
        dists.sort(key=lambda a: a[0])
        distances.append(dists)
    
    for i in range(max(len(v_coord),len(w_coord))):
        dist = distances[i][0]
        j = dist[1][0] + dist[1][1]*dim + dist[1][2]*dim*dim
        k = dist[2][0] + dist[2][1]*dim + dist[2][2]*dim*dim
        sum += dist[0] + (dim if L[0][j] != S[0][k] else 0)

    return sum


def compute_sparsity(descriptors: dict,k: int, distance) -> dict:
    sparsity = dict()
    distances = np.full((len(descriptors),len(descriptors)),-1)
    for (key, desc), i in zip(descriptors.items(),range(len(descriptors))):
        for (key2, desc2), j in zip(descriptors.items(),range(len(descriptors))):
            if((distances[i,j] != -1)):
                continue
            distances[i,j] = distance(desc,desc2)
            distances[j,i] = distances[i,j]

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
    if(len(sys.argv) != 5):
        print("usage:\n - arg 1: folder path\n",
              "- arg 2: pareto file name\n", 
              "- arg 3: fitness threshold\n",
              "- arg 4: distance (L2|organ_dist)")
        exit(0)


    folder_name = sys.argv[1]
    distance = sys.argv[4]
    for folder in os.listdir(folder_name):
        if(folder.split("_")[0] != "meim"):
            continue
        print(folder)
        exp_folder = folder_name + "/" + folder
        if os.path.exists(exp_folder + "/" + sys.argv[2]):
            continue
        descriptors = load_descriptors(exp_folder + "/morph_descriptor.csv",max=5000)
        fitnesses = load_fitnesses(exp_folder + "/fitness.csv",max=5000)
        fitnesses, descriptors = apply_fitness_threshold(fitnesses,descriptors,float(sys.argv[3]))
        print("Number of individuals =",len(fitnesses))
       # print(fitnesses)
        if distance == "L2":
            L2_norm = lambda v,w: np.linalg.norm(v-w)
            sparsity = compute_sparsity(descriptors,15,L2_norm)
        elif distance == "organ_dist":
            sparsity = compute_sparsity(descriptors,15,organ_position_distance)
        else:
            print("Error - distance unkown.\ndistance available: L2 or organ_dist")
            exit(0)
        pareto_set = compute_pareto_front(sparsity,fitnesses)
        pareto_set.to_csv(exp_folder + "/" + sys.argv[2],index=False)
        print(pareto_set)

