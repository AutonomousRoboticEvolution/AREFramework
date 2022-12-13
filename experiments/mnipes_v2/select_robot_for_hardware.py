#! /usr/bin/python3

import sys
import os
import csv
import numpy as np

def load_fitness_values(foldername):
    fits = []
    i = 0
    for folder in os.listdir(foldername):
        print(folder)
        with open(foldername + "/" + folder + "/fitnesses.csv") as file :
            csv_data = csv.reader(file,delimiter=',')
            for row in csv_data:
                k = 3
                if(len(row) == 3):
                    k = 1
                if(float(row[k]) == 1.0):
                    fits.append([folder,int(row[0]),0.])
                else:
                    fits.append([folder,int(row[0]),float(row[k])])
        i+=1
    return fits

def load_descriptors(foldername):
    descriptors = []
    i = 0
    for folder in os.listdir(foldername):
        with open(foldername + "/" + folder + "/morph_descriptors.csv") as file :
            csv_data = csv.reader(file,delimiter=',')
            for row in csv_data :
                descriptors.append([folder,int(row[0]),float(row[5]),float(row[6]),float(row[7]),float(row[8])])
        i+=1 
    return descriptors
    

def selection(fitnesses,descriptors,N,P):
    ''' return N ids of selected individuals among the P first individuals in each islands.
        The best individual in performance will be selected the N-1 individuals which are the best but have different organ combination.
    '''
    _set = []

    #Select the P first individuals
    p = 0
    isle_nbr = 0
    data = []
    for f,d in zip(fitnesses,descriptors):
        if(p == 0):
            isle_nbr = f[0]
        if(p < P):
            data.append([f[0],f[1],f[2],d[2],d[3],d[4],d[5]])
            p+=1
        elif(isle_nbr != f[0]):
            p = 0

    #sort the list in descent order of fitness values
    data.sort(reverse=True,key=lambda e: e[2])
    #add the first of the data (the best in term of fitness values)
    _set.append([data[0][0],data[0][1]])
    print(data[0])
    index = 0
    desc_ref = []
    #select the N-1 best individual which are the most morphologically different from each others
    while(len(_set) < N):
        desc_ref.append(data[index][3:])
        distances = []
        for d in data:
            dist = 0
            if [d[0],d[1]] in _set:
                dist = 0.
            else:
                for ref in desc_ref:
                    dist += np.linalg.norm(np.array(d[3:]) - np.array(ref))
            distances.append(dist/len(desc_ref))
        sorted_dist_indexes = (-np.array(distances)).argsort()
        order = [i + sorted_dist_indexes[i] for i in range(len(sorted_dist_indexes))]
        index = sorted_dist_indexes[np.argmin(np.array(order))]
        print(data[index])
        _set.append([data[index][0],data[index][1]])

    return _set


if __name__ == '__main__':
    if(len(sys.argv) < 4):
        print("usage\n - arg 1 path to folder containing simulation runs of mnipes_v2 experiments\n - arg 2 number of robot to select\n - arg 3 the size of the pool of robots to select from per islands")
    else:
        desc = load_descriptors(sys.argv[1])
        print(len(desc))
        fits = load_fitness_values(sys.argv[1])
        print(len(fits))
        _set = selection(fits,desc,int(sys.argv[2]),int(sys.argv[3]))
        print(_set)
    
