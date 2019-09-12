#!/bin/bash

set -e

VREP_EXE=''
SEED=0
REPOSITORY=''
MPI_NODES=16 # population = MPI_NODES-1

function usage {
    echo """
This program will compile and run the entire project in MPI mode.
    Program USAGE: $0 [vrep_exe] [seed] [repository]
"""
}

if [ -z ${1+x} ]; then 
    echo "VREP_EXE is unset"
    usage
    exit 1
else
    VREP_EXE=$(realpath $1)
    echo "VREP_EXE is set to '${VREP_EXE}'"
fi

SEED=$2
if [ -z ${2+x} ]; then 
    echo "SEED is unset"
    usage
    exit 1
else
    echo "SEED is set to '${SEED}'"
fi

if [ -z ${3+x} ]; then 
    echo "REPOSITORY is unset"
    usage
    exit 1
else
    REPOSITORY=$(realpath $3)
    echo "REPOSITORY is set to '${REPOSITORY}'"
fi

mkdir -p build
cd build

# if on FEDORA cmake fails, run the command 
#$ module load mpi/openmpi-x86_64
cmake ..
make

# mkdir -p "${REPOSITORY}/morphologies${SEED}"
mkdir -p "${REPOSITORY}/morphologies0"

echo "copying plugin into vrep folder"
VREP_FOLDER=$(dirname ${VREP_EXE})
cp "../WP4_Ecosytem_Manager/Cplusplus_Evolution/VREP_Files/Servo_Module.ttm" "${VREP_FOLDER}/models"
cp "WP4_Ecosytem_Manager/Cplusplus_Evolution/libv_repExtER.so" "${VREP_FOLDER}/"

echo "STARTING MPI PROGRAM"
echo mpirun -c ${MPI_NODES} vrepPluggin/Evolution/MPI/mpi_er ${VREP_EXE} ${SEED} ${REPOSITORY}
exec mpirun -c ${MPI_NODES} vrepPluggin/Evolution/MPI/mpi_er ${VREP_EXE} ${SEED} ${REPOSITORY}
