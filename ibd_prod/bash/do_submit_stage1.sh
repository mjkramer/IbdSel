#!/bin/bash

# brittle hack
if (env | grep LD_LIBRARY_PATH | grep `whoami` >/dev/null 2>&1); then
    echo "Your environment looks dirty. Bailing."
    exit 1
fi

# If we're invoked by e.g. submit_stage1.sh
if [ $# -eq 1 ]; then
    echo "Need two more args: tag and njob"
    exit 1
fi

if [ $# -lt 3 ]; then
    echo "Need args: slurmfile tag njob"
    exit 1
fi

slurmfile=$1; shift
tag=$1; shift
njob=$1; shift
slurm_args=$@

source bash/stage1_vars.inc.sh
stage1_vars $tag

if [ ! -f $slurmfile ]; then
    echo "Slurm file does not exist. Bailing."
    exit 1
fi

if [ ! -f $infile ]; then
    echo "$infile does not exist. Bailing."
    exit 1
fi

if [ ! -d $outdir ]; then
    echo "$outdir does not exist. Bailing."
    exit 1
fi

if [ ! -d $logdir ]; then
    echo "$logdir does not exist. Bailing."
    exit 1
fi

if [ ! -f ../selector/stage1_main_cc.so ]; then
    echo "Compile ../selector/stage1_main.cc (in ROOT 6) first"
    exit 1
fi

( set -x
  sbatch $slurm_args -t $IBDSEL_WALLTIME --array=1-$njob -o $logdir/$logfmt $slurmfile $tag
)
