#!/bin/bash

for i in {0..10}; do
	mkdir -p "../run$i"
	MAKEFLAGS=-j64 ./mpi_setup_and_run.sh "../V-REP_PRO_EDU_V3_5_0_Linux/vrep.sh" "$i" "../run$i" 2>&1 |tee "../run$i/run$i.log"
	sleep 10
done
