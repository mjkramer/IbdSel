#!/bin/bash
#SBATCH -N 1 -A dayabay -C knl -L project,projecta,SCRATCH -q regular

# Submit me from ibd_prod directory! Do a "newenv" first! Specify -t!
# Make sure stage1_main.cc has already been compiled, to avoid race conditions
# Submit with scripts/submit_stage1.sh

bashlib/stage1_job.sh $@
