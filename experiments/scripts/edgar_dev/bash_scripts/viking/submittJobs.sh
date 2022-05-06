#!/bin/bash
for i in {1..5}
do
   cp ../parameters.csv ../evolutionary_robotics_framework/EvolutionVREP/experiments/morphology/
   sed -i '/\#seed,int,/ s/$/$i/' ../evolutionary_robotics_framework/EvolutionVREP/experiments/morphology/parameters.csv
   sbatch exampleJob.job
   echo "Welcome $i times"
done
