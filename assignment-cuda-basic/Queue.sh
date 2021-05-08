#!/bin/bash

# This is ugly (cries)
"./Clean.sh"
"./Compile.sh"

if [ -f "./a.out" ]; then
  qsub -d `pwd` -l nodes=1:ppn=1:gpus=1 -q mamba -l walltime=01:00:00 Job.sh
fi
