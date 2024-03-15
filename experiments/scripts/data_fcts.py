import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import math
import numpy as np
import numpy.linalg as la
from PIL import Image

def perc_high(a):
    return np.percentile(a,q=90)

def perc_low(a):
    return np.percentile(a,q=10)

def find_index(_id,ids):
    for i in range(len(ids)):
        if(ids[i] == _id):
            return i


def load_fitness(filename):
    ids = []
    fits = []
    parents = []
    evals = []
    deltas = []
    with open(filename) as file :
        lines = file.read().splitlines()
        for line in lines:
            line = line.split(",")
            ids.append(int(line[0]))
            parents.append([int(line[1]),int(line[2])])
            fits.append(float(line[3]))
            if(len(line) == 6):
                evals.append(int(line[4]))
                deltas.append(float(line[5]))
    return ids, parents, fits, evals, deltas

def load_parent_pool(filename):
    parent_ids = []
    with open(filename) as file:
        lines = file.readlines()
        for line in lines:
            line = line.split(',')
            ids = []
            for l in line:
                ids.append(int(l))
            parent_ids.append(ids)
    return parent_ids

def filter_to_parent_pool(data,parent_ids):
    filtered = []
    data = {d[0]: d[1:] for d in data}
    iter = 0
    for ids in parent_ids:
        if(len(ids) != len(parent_ids[0])):
            continue
        for _id in ids:
            filtered.append([iter,_id] + data[_id])
        iter+=1

    return filtered

def load_comp_time(filename):
    eval_times = []
    with open(filename) as file:
        lines = file.read().splitlines()
        start_0 = float(lines[0].split(",")[1])
        for line in lines:
            line = line.split(",")
            start = float(line[1])
            end = float(line[2])
            eval_times.append([end - start,end-start_0]) #eval duration, date
        return eval_times


def add_comp_time(fitnesses,eval_times):
    position = 0
    for fit in fitnesses:
        comp_time = 0
        for i in range(int(fit[4])):
            comp_time += eval_times[i + position][0]
        T = max(eval_times[position:position+int(fit[4])][1])
        fit += [comp_time,T,int(T/(60*10e6))]
        position += int(fit[4])
    return fitnesses
        

def read_parameters(filename):
    '''
    Read a parameter and return a dictionnary
    '''
    parameters = dict()
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=',')
        for row in csv_data:
            parameters[row[0]] = row[2]
    return parameters


def compute_gen_rec(par_id,ids,parents):
    if(par_id == -1):
        return 0
    for i, id in zip(range(len(ids)),ids):
        if(par_id == id):
            return compute_gen_rec(parents[i],ids,parents) + 1

def compute_generation(ids,parents):
    generation = [0 for _ in range(len(ids))]
    for i in range(len(ids)):
        generation[i] = compute_gen_rec(parents[i],ids,parents)
    return generation

def load_descriptor(filename) :
    '''
    Load a list of descriptor from a file
    Trait descriptor: <width,depth,height,voxels,wheels,sensor,joint,caster>
    '''
    with open(filename) as file :
        descriptors = []
        for row in file.readlines():
            row = row.split(',')
            desc = [int(row[0])] 
            for r in row[1:-1] :
                desc.append(float(r))
            descriptors.append(desc + [la.norm(desc[1:])])
        return descriptors
    
def load_controller_archive(filename):
    lines = []
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=',')
        coord = [0]*3
        state = 0
        nbr_param = 0
        i=0
        for row in csv_data:
            if(len(row) == 3):
                coord[0] = int(row[0])
                coord[1] = int(row[1])
                coord[2] = int(row[2])
                state = 1
                i=0
            elif(len(row[0].split(" "))==4):
                continue
            elif(state == 1):
                nbr_param = int(row[0])
                state = 2
            elif(state == 2):
                nbr_param += int(row[0])
                state = 3
            elif(state == 3 and i < nbr_param):
                i+=1
            elif(state == 3 and i >= nbr_param):
                if(coord[1] == 0):
                    lines.append([coord[0],coord[2],float(row[0])])
    return pd.DataFrame(data=lines,columns=["number of wheels","number of sensors","fitness"])

def compute_lineage_rec(ids,par_id,parents):
    if(par_id == -1):
        return []
    for i, _id in zip(range(len(ids)),ids):
        if(par_id == _id):
            return [_id] + compute_lineage_rec(ids,parents[i],parents)
def compute_lineage(ids,parents):
    lineages = dict()
    for i, _id in zip(range(len(ids)),ids):
        lineages[_id] = compute_lineage_rec(ids,parents[i],parents)
    return lineages

def load_trajectory(filename):
    traj = []
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=';')
        t = 0
        for row in csv_data:
            position = row[0].split(',')
            pos = [float(elt) for elt in position]
            traj.append([t] + pos)
            t+=1
    return traj

def compute_exploration_fitness(desc):
    _sum = 0
    for d in desc:
        _sum += d[1]

    max_sum = 0
    for i in range(8):
        for j in range(8):
            max_sum += math.floor(math.sqrt((i - 3)*(i - 3) + (j - 3)*(j - 3)))
    return float(_sum)/float(max_sum)

def load_grid_descriptor(filename):
    with open(filename) as file:
        state = 0
        descs = dict()    
        _id = 0
        i = 0
        for line in file.readlines():
            if(state == 0):
                _id = int(line)
                state = 1
                descs[_id] = []
                i = 0
            elif(state == 1):
                if(int(line) > 0):
                    descs[_id].append((i,int(line)))
                i+=1
                if(i == 64):
                    state = 0
        return descs
    
def compute_runtime(filename,nbr_eval):
    with open(filename) as file :
        csv_data = csv.reader(file,delimiter=',')
        i = 0
        start_time = 0
        end_time = 0
        for row in csv_data:
            if(i==0):
                start_time = float(row[1])
            if(row[0] == "overhead"):
                continue
            if(i==nbr_eval):
                end_time = float(row[2])
            i+=1
    return end_time - start_time
