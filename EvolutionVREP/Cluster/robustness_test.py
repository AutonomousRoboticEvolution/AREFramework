#! /usr/bin/python

import os
import sys
import argparse
import csv
import subprocess as subp
from run_cluster import *

def main():
    for folder in os.listdir(args.folder_name) :
        if(not os.path.isdir(args.folder_name + folder) or folder.split("_")[0] != args.exp_name) :
            continue

        print(folder)

        gen = 0
        with open(args.folder_name + folder + "/fitnesses.csv" ) as  fit_file:
            csv_data = csv.reader(fit_file,delimiter=',')
            for row in csv_data:
                pass
            gen = int(row[0])

        param_file = open("../experiments/robustness_test/parameters.csv",newline='')
        new_param_file = open("../experiments/robustness_test/parameters_tmp.csv",'w',newline='')
        
        csv_reader = csv.reader(param_file,delimiter=',')
        csv_writer = csv.writer(new_param_file,delimiter=',',lineterminator='\n')
        for row in csv_reader :
            if(len(row)==0):
                continue
            if(row[0] == "#folderToLoad") :
                row[2] = args.folder_name + folder + "/"
            if(row[0] == "#genToLoad") :
                row[2] = gen
            csv_writer.writerow(row)
        
        param_file.close()
        new_param_file.close()
        
        args.params = args.are_framework + "/EvolutionVREP/experiments/robustness_test/parameters_tmp.csv"

        servers = []
        client = None
        try:
            import time
            servers = run_servers(args,args.n_vrep)
            client = run_client(args)
            
            time.sleep(1)

        except:
            import traceback
            traceback.print_exc()
            print(f'Exception occurred: killing processes', file=sys.stderr)
            kill(servers, client)

        finally:
            try:
                wait(servers, client)
            except KeyboardInterrupt:
                kill(servers, client)
                wait(servers, client)



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('exp_name', type=str,
                        help='experiment name')

    parser.add_argument('folder_name', type=str,
                        help='folder name')

    parser.add_argument('are_framework',type=str,help='are framework path')

    parser.add_argument('n_vrep', metavar='N', type=int,
                        help='Number of VREP instances')
    
    parser.add_argument('--xvfb',type=int,default=0,help='run with xvfb')

    parser.add_argument('--params', type=str,
                        default=0,
                        help='path to parameters file')

    parser.add_argument('--client', type=str,
                        default='ERClient',
                        help='client executable')

    parser.add_argument('--port-start', type=int,
                        default=10400,
                        help='client executable')

    parser.add_argument('--vrep', type=str,
                        default='vrep.sh',
                        help='path to the vrep starting script')

    args = parser.parse_args()
    main()
