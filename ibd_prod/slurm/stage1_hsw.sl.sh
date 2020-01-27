#!/bin/bash
#SBATCH -N 1 -A dayabay -C haswell -L project,projecta,SCRATCH -q regular

# Submit me from ibd_prod directory! Do a "newenv" first! Specify -t!
# Make sure stage1_main.cc has already been compiled, to avoid race conditions
# Submit with scripts/submit_stage1.sh

if [ -z "$IBDSEL_NTASKS" ]; then
   export IBDSEL_NTASKS=32
fi

bash/stage1_job.sh $@
