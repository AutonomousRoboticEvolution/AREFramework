#! /usr/bin/python3

import os
import sys
import csv

def load_morph_desc(gen,filename):
    with open(filename) as file:
        csv_data = csv.reader(file,delimiter=',')
        descriptor = []
        for row in csv_data:
            if(gen == row[0]):
                print(gen,row[0],row[1])
                print(float(row[6]),float(row[7]),float(row[8]))
                descriptor.append([float(row[6])*16,float(row[7])*16,float(row[8])*16])
        return descriptor

def load_fitnesses(filename):
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=',')
        line = []
        for row in csv_data :
            line = row
        fitnesses = []
        for i in range(3,len(line)-1):
            fitnesses.append(line[i])
    return line[0], fitnesses


def load_ctrl_genomes(fitness, filename):
    with open(filename) as file:
        csv_data = csv.reader(file,delimiter=',')
        next_genome = False
        genome = []
        for row in csv_data:
            if(len(row) == 2 and row[0] == fitness):
                next_genome = True
            if(len(row) > 4 and next_genome):
                for i in range(len(row)-1):
                    genome.append(float(row[i]))
                return genome


def nbr_weights_biases(desc,hidden):
    weights = (desc[1]*2+1)*hidden + \
                hidden*hidden + \
                hidden + \
                hidden + \
                hidden*(desc[0] + desc[2]) + \
                hidden + (desc[0] + desc[2])
    biases = desc[1]*2  + hidden*2 + desc[0] + desc[2] + 1
    return weights, biases

def write_ctrl_genome_file(folder,gen,ind,weights,biases,genome):
    with open(folder + "/ctrlGenome_" + str(gen) + '_' + str(ind),'w') as file:
        file.write(str(int(weights)) + '\n')
        file.write(str(int(biases)) + '\n')
        for g in genome:
            file.write(str(g) + '\n')


if __name__ == '__main__':
    foldername = sys.argv[1]
    fitnesses_file = sys.argv[2]
    descriptor_file = sys.argv[3]
    gen, fitnesses = load_fitnesses(fitnesses_file)
    print(fitnesses)
    descriptors = load_morph_desc(gen, descriptor_file)
    for file in os.listdir(foldername):
        list_str = file.split("_")
        if(list_str[0] == "controllers"):
            print(file)
            genome = load_ctrl_genomes(fitnesses[int(list_str[2])],foldername + '/' + file)
            weights, biases = nbr_weights_biases(descriptors[int(list_str[2])],8)
            print(len(genome), weights + biases)
            # assert(len(genome) == weights + biases)
            write_ctrl_genome_file(foldername,int(list_str[1]),int(list_str[2]),weights,biases,genome)



