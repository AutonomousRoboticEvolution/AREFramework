#!/bin/bash
echo To force rebuild use different values for REBUILD_ARE
echo Use BRANCH to set ARE branch, default is 'main'
echo Example of use: ./build.sh --build-arg=\"REBUILD_ARE=0\" --build-arg=\"BRANCH=melai_optimal_hk\" 
sudo docker build --build-arg="N_CPU=20" $1 $2 -t artificialsimon/areframework:melai .
