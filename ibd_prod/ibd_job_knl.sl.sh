#!/bin/bash
#SBATCH -A dayabay -C knl -L project,projecta,SCRATCH -q regular

#SBATCH -t 6:00:00

# Submit me from my directory! Do a "newenv" first!
# Make sure ../selector/stage1_main.cc has already been compiled, to avoid race conditions
# submit with submit.sh

timeout=5                       # hours

./ibd_job_knl.main.sh $timeout $@
