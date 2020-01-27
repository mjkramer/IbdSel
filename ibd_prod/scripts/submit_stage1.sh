#!/bin/bash

# Calculations assume best-case 1 node-sec per file and 60 seconds per process.
# Using a safety factor of 5.

# Min. lockfile period here is 250 * 1sec / njobs
# So 1Hz for 250 jobs at worst (and we probably need < 250 total 2hr jobs)
# Testing shows $HOME/$CFS can handle at least 4Hz no problem
export IBDSEL_CHUNKSIZE=250

export IBDSEL_WALLTIME=02:00:00
export IBDSEL_CHUNK_MARGIN_SECS=1250 # 250 * 1sec * 5
export IBDSEL_FILE_MARGIN_SECS=300   # 60sec * 5
export IBDSEL_STARTUP_SLEEP_SECS=120
export IBDSEL_NTASKS=54

tag=$1; shift
njob=$1; shift
slurm_args=$@

bash/do_submit_stage1.sh slurm/stage1_knl.sl.sh $tag $njob $slurm_args
