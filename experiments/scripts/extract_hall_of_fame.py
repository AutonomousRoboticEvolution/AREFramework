#! /usr/bin/python3

import sys
import os
import zipfile as zf

def load_fitness_values(filename):
    fitnesses = []
    for l in open(filename).readlines():
        split = l.split(',')
        fitnesses.append([int(split[0]),int(split[1]),int(split[2]),float(split[3])]) #id, parents' id 1, parents' id 2, fitness
    return fitnesses

def load_fitness_values_nipes(filename):
    fitnesses = []
    for l in open(filename).readlines():
        split = l.split(',')
        for i in range(0,int(split[1]),2):
            fitnesses.append([int(split[0]),int(i/2),float(split[i+3])]) #gen, id, fitness
    return fitnesses


def extract_files_nipes(folder,archive,id_list):
    zip_file = zf.ZipFile(archive,'r')
    for _id in id_list:
        print("extracting files for robot",_id[0],_id[1])
        zip_file.extract(folder + "ctrlGenome_" + str(_id[0]) + "_" + str(_id[1]),path='.')
        zip_file.extract(folder + "traj_" + str(_id[0]) + "_" + str(_id[1]),path='.') 

def extract_files(folder,archive,id_list):
    zip_file = zf.ZipFile(archive,'r')
    for _id in id_list:
        print("extracting files for robot",_id)
        zip_file.extract(folder + "morph_genome_" + str(_id),path='.')
        zip_file.extract(folder + "ctrl_genome_" + str(_id),path='.')
        zip_file.extract(folder + "traj_" + str(_id) + "_0",path='.')
        zip_file.extract(folder + "model_" + str(_id) + ".ttm",path='.')
        zip_file.extract(folder + "mesh_" + str(_id) + ".stl",path='.')
        zip_file.extract(folder + "blueprint_" + str(_id) + ".csv",path='.')
        zip_file.extract(folder + "controllers_" + str(_id),path='.')

def extract_best_robots_from_folder(folder,archive,nbr_of_robs):
    fitnesses = load_fitness_values(folder + '/fitnesses.csv')[:200]
    fitnesses.sort(reverse=True,key=lambda l:l[3])
    id_list = [f[0] for f in fitnesses[:int(nbr_of_robs)]]
    extract_files(folder,archive,id_list)

def extract_best_robots_from_folder_nipes(folder,archive,nbr_of_robs):
    fitnesses = load_fitness_values_nipes(folder + '/fitnesses.csv')
    fitnesses.sort(reverse=True,key=lambda l:l[2])
    print(fitnesses[:int(nbr_of_robs)])
    id_list = [[f[0],f[1]] for f in fitnesses[:int(nbr_of_robs)]]
    extract_files_nipes(folder,archive,id_list)



if __name__ == '__main__':
    if(len(sys.argv) < 4):
        print("usage:")
        print("\targ1: folder containing the experiments log folders containing fitnesses file")
        print("\targ2: the archive of the experiments logs")
        print("\targ3: the number of robots to extract")
        print("\toptional arg4: nipes if archive is from a nipes experiment")
        exit(1)

    folder = sys.argv[1]
    archive = sys.argv[2]
    nbr_of_robs = sys.argv[3]
    nipes = (len(sys.argv) == 5)

    for f in os.listdir(folder):
        print("Entering folder", f)
        if(nipes == False):
            extract_best_robots_from_folder(folder + f + "/",archive,nbr_of_robs) 
        else:
            extract_best_robots_from_folder_nipes(folder + f + "/",archive,nbr_of_robs) 


