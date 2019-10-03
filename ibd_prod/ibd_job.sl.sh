#!/bin/bash

#SBATCH -A dayabay -C haswell -L project,projecta,SCRATCH -q shared -t 6:00:00

# Submit me from my directory! Do a "newenv" first!
# Make sure ../selector/stage1_main.cc has already been compiled, to avoid race conditions

# submit with submit.sh

module purge
module load gcc/8.2.0
module load python/2.7-anaconda-2019.07
module load gsl/2.5
source /global/project/projectdirs/dayabay/scratch/mkramer/apps/root-6.18.00-py27/bin/thisroot.sh

echo Running on $(hostname)

sleep $(( RANDOM % 200 ))       # so they don't all mob the lockfile at once on startup (400 jobs, staggered)

export PYTHONPATH=$(pwd)/../common:$PYTHONPATH

./ibd_worker.py -t 4 $@
