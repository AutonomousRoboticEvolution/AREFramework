#! /usr/bin/python3

import numpy as np
import os
import sys


def load_fitness_values(filepath):
    with open(filepath) as file:
        lines = file.readlines()
        fits = []
        for i in range(10):
            fits.append(float(lines[i].split(",")[1]))
        return fits

def load_fitness_medians(folderpath):
    medians = dict()
    for folder in os.listdir(folderpath):
        if(folder.split("_")[0] == "morph" and os.path.isdir(folderpath + "/" + folder)):
            fitnesses = load_fitness_values(folderpath + "/" + folder + "/best_LRS_reevaluated/fitnesses")
            medians[int(folder.split("_")[1])] = np.median(np.array(fitnesses))
    return medians       

def load_evolvability(filepath):
    with open(filepath) as file:
        lines = file.readlines()
        _id = 1
        evolvability = dict()
        for line in lines:
            if(line.split("_")[0] == "morph"):
                _id = int(line.split("_")[-1].split(",")[0])
            elif(line.split(",")[0] == "evolvability"):
                evolvability[_id] = float(line.split(",")[-1])
        return evolvability

def dominance_count(values):
    dominance = dict()
    for val1 in values:
        dominance[val1[0]] = 0
        for val2 in values:
            if(val1[0] == val2[0]):
                continue
            if(val1[1] <= val2[1] and val1[2] <= val2[2]):
                    dominance[val1[0]]+=1
    return dominance

if __name__ == '__main__':
    medians = load_fitness_medians(sys.argv[1])
    print(dict(sorted(medians.items(),key=lambda item:item[1])))
    evols = load_evolvability(sys.argv[1] + "/morph_genome_info.csv")
    print(dict(sorted(evols.items(),key=lambda item:item[1])))
    values = []
    for k,v in medians.items():
        values.append([k,v,evols[k]])
    dc = dominance_count(values)
    dc = sorted(dc.items(),key=lambda item:item[1])
    print(dc)
    order = []
    i=0
    while(i <= len(dc)):
        if(i == len(dc)-1):
            order.append(dc[i][0])
            i+=1
            break
        if(dc[i][1] == dc[i+1][1]):
            if(medians[dc[i][0]] > medians[dc[i+1][0]]):
                order.append(dc[i][0])
                order.append(dc[i+1][0])
            else:
                order.append(dc[i+1][0])
                order.append(dc[i][0])
            i+=2
        else:
            order.append(dc[i][0])
            i+=1

    print(order)
                  