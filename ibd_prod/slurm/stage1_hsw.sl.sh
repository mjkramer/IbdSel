#!/bin/bash
#SBATCH -N 1 -A dayabay -C haswell -L project,projecta,community,SCRATCH -q regular

bash/stage1_job.sh $@
