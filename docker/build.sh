#!/bin/bash
echo './build.sh --build-arg="REBUILD_ARE=1" to force rebuild ARE'
sudo docker build --build-arg="N_CPU=20" $1 -t artificialsimon/are_framework:melai .
