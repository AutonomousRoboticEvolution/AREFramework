#! /usr/bin/python3

import csv
import os
import sys
from argparse import ArgumentParser

def find_best_fit(filename):
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=',')
        best_fitness = 0
        gen = 0
        ind_id = 0
        for row in csv_data :
            for i in range(3,len(row[:-1]),int(row[2])) :
                if(float(row[i]) > best_fitness) :
                    best_fitness = float(row[i])
                    ind_id = int(i/int(row[2]) - 1)
                    gen = int(row[0])
                
    return gen,ind_id,best_fitness

def load_nbr_wjs(filename,morph_id,max_nbr_organs):
    with open(filename) as file:
        csv_data = csv.reader(file,delimiter=',')
        line = []
        i = 0
        for row in csv_data:
            i+=1
            line = row
            if(i > int(morph_id)):
                break
        return int(float(line[5])*max_nbr_organs),int(float(line[7])*max_nbr_organs),int(float(line[6])*max_nbr_organs)

def load_data(foldername,morph_foldername,max_nbr_organs):
    controllers = []
    for morph_folder in os.listdir(foldername):
        if(morph_folder.split('_')[0] != 'morph'):
            continue
        print("open",morph_folder)

        w,j,s = load_nbr_wjs(morph_foldername + '/MD_Cart_WDH.csv',morph_folder.split('_')[-1],max_nbr_organs)
        gen,iid,fit = find_best_fit(foldername + '/' + morph_folder + '/fitnesses.csv')
        filename = foldername + '/' + morph_folder + '/genome_' + str(gen) + '_' + str(iid)
    
        with open(filename) as file:
            csv_data = csv.reader(file,delimiter=',')
            
            lines = []
            for row in csv_data:
                lines.append(row[0])   
            nbr_weights = int(lines[0])
            nbr_biases = int(lines[1])
            weights = []
            biases = []
            for i in range(nbr_weights):
                weights.append(float(lines[i+2]))    
            for k in range(nbr_biases):
                biases.append(float(lines[k+2+nbr_weights]))
            controllers.append((w,j,s,nbr_weights,nbr_biases,weights,biases,fit))
            print(w,j,s,nbr_weights,nbr_biases,fit)
    return controllers

def create_ctrl_archive(max_nbr_organs,ctlrs):
    ctrl_arch = [[[(0,0,[],[],0) for _ in range(max_nbr_organs)] for _ in range(max_nbr_organs)] for _ in range(max_nbr_organs)]
    for elt in ctlrs:
        if(ctrl_arch[elt[0]][elt[1]][elt[2]][0] == 0):
            ctrl_arch[elt[0]][elt[1]][elt[2]] = (elt[3],elt[4],elt[5],elt[6],elt[7])
        else:
            if(elt[7] > ctrl_arch[elt[0]][elt[1]][elt[2]][4]):
                ctrl_arch[elt[0]][elt[1]][elt[2]] = (elt[3],elt[4],elt[5],elt[6],elt[7])
    return ctrl_arch

def write_ctrl_arch(filename,ctrl_arch):
    with open(filename,'w') as file:
        for w in range(len(ctrl_arch)):
            for j in range(len(ctrl_arch)):
                for s in range(len(ctrl_arch)):
                    file.write(str(w) + ',' + str(j) + ',' + str(s) + '\n')
                    file.write(str(ctrl_arch[w][j][s][0]) + '\n')
                    file.write(str(ctrl_arch[w][j][s][1]) + '\n')
                    for weight in ctrl_arch[w][j][s][2]:
                        file.write(str(weight) + '\n')
                    for biases in ctrl_arch[w][j][s][3]:
                        file.write(str(biases) + '\n')
                    file.write(str(ctrl_arch[w][j][s][4]) + '\n')

if __name__ == '__main__':

    parser = ArgumentParser()
    parser.add_argument("exp_folder",type=str,help="experiment folder path")
    parser.add_argument("morph_folder",type=str,help="morphologies folder path")
    parser.add_argument("-m","--max-nbr-organs",dest="max_nbr_organs",type=int,help="maximum number of organs",default=16)
    parser.add_argument("-o","--output-file",dest="output_file",type=str,help="output file name",default="controller_archive")
    
    args = parser.parse_args()

    controllers = load_data(args.exp_folder,args.morph_folder,args.max_nbr_organs)
    ctrl_arch = create_ctrl_archive(args.max_nbr_organs,controllers)
    write_ctrl_arch(args.output_file,ctrl_arch)                     