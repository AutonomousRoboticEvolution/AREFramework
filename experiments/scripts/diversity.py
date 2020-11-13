#! /usr/bin/python3

import scipy.stats as spstat
import scipy.sparse as spsp
import numpy as np
import numpy.linalg as nplinalg
import sys
import csv

traits_desc_size = 8
sym_desc_size = 40
spatial_desc_size = 1728

def load_descriptor(filename,desc_size) :
    '''
    Load a list of descriptor from a file
    '''
    with open(filename) as file :
        print(filename)
        csv_data = csv.reader(file,delimiter=',')
        descriptors = []
        for row in csv_data :
            desc = []
            for i in range(desc_size) :
                desc.append(float(row[i+1]))
            descriptors.append(desc)
        return np.array(descriptors)


def compute_desc_hist(descriptors,bin) :
    '''
    compute an histogram based on a list of descriptors
    '''
    grid_size = bin**len(descriptors[0])
    print(grid_size)
    hist = dict()
    for desc in descriptors :
        coord = 0
        for val,i in zip(desc,range(len(desc))) :
            coord = coord + int(val*bin**i)
        if coord in hist :
            hist[coord] = hist[coord] + 1
        else :
            hist[coord] = 1
    return hist, grid_size



def JSD(P,Q) :
    '''
    Return the Jensen-Shanon divergence between the distributions P and Q
    '''
    P = P / nplinalg.norm(P,ord=1)
    Q = Q / nplinalg.norm(Q,ord=1)
    M = (P + Q)*.5
    return (spstat.entropy(P,M) + spstat.entropy(Q,M))*.5

def uniformity_scores(hist,grid_size) :
    '''
    Compute the coverage of the population in the descriptor space and its uniformity of its distribution.
    return coverage, uniformity
    '''

    max_key = max(hist.keys())
    min_key = min(hist.keys())
    pop_size = 0
    pop_dist = []
    for i in range(min_key,max_key) :
        if i in hist :
            pop_dist.append(hist[i])
            pop_size += hist[i]
        else : 
            pop_dist.append(0)


    uni_ref_val = float(pop_size)/float(len(hist))
    uni_dist = np.full(len(pop_dist),uni_ref_val)
    print(len(hist),grid_size)
    return  1 - JSD(uni_dist,pop_dist)



def write_hist(filename,hist) :
    '''
    To save in a file the histogram of the population
    '''
    with open(filename,'w') as f :
        for line in hist :
            for elt in line :
                f.write(str(elt)+" ")
            f.write("\n")

def read_hist(filename) :
    '''
    to read a file containing the histogram of a population
    '''
    hist = []
    with open(filename) as f :
        for line in f :
            templist = []
            for n in line.split(' ') :
                if(n == '\n') :
                    continue
                templist.append(int(n))
            hist.append(templist)
    return hist



def main() :
    descriptors = load_descriptor(sys.argv[1],traits_desc_size)
    print("population size :",len(descriptors))
    hist, grid_size = compute_desc_hist(descriptors,int(sys.argv[2]))
    print("Coverage :",float(len(hist))/float(grid_size))
    print("Uniformity ... (it can take a while because of the curse of dimensionality)")
    uni = uniformity_scores(hist,grid_size)
    print(uni)

if __name__ == '__main__':
    main()