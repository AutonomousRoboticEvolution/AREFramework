#!/bin/bash
echo "Set number of CPU for building as N_CPUS, default is 16"
echo "Set branch to build as BRANCH"
apptainer build --build-arg N_CPUS=16 --build-arg BRANCH=update_decoding are.sif are.def