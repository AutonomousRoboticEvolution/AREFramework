#!/bin/bash -l
#SBATCH -q debug    
#SBATCH -N 1      
#SBATCH -t 00:02:00 
#SBATCH -J my_job   

echo Hello

srun -n 16 ./tester     # an extra -c 2 flag is optional for fully packed pure MPI

echo Done
