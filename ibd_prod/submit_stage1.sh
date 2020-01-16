#!/bin/bash

# brittle hack
if (env | grep LD_LIBRARY_PATH | grep mkramer >/dev/null 2>&1); then
    echo "Your environment looks dirty. Bailing."
    exit 1
fi

# If we're invoked by something in examples/
if [ $# -eq 3 ]; then
    echo "Need two more args: tag and njob"
    exit 1
fi

if [ $# -ne 5 ]; then
    echo "Usage: submit.sh slurmfile walltime timeout tag njob"
    exit 1
fi

slurmfile=$1; shift
walltime=$1; shift
timeout=$1; shift
tag=$1; shift
njob=$1; shift

if [ ! -f $slurmfile ]; then
    echo "Slurm file does not exist. Bailing."
    exit 1
fi

logdir=../../log/ibd_prod/$tag
infile=../../data/prod_input/$tag/input.ibd.txt
outdir=../../data/ibd_fbf/$tag

mkdir -p $logdir

if [ ! -f $infile ]; then
    echo "$infile does not exist. Bailing."
    exit 1
fi

if [ ! -d $outdir ]; then
    echo "$outdir does not exist. Bailing."
    exit 1
fi

if [ ! -f ../selector/stage1_main_cc.so ]; then
    echo "Compile ../selector/stage1_main.cc (in ROOT 6) first"
    exit 1
fi

echo sbatch -t $walltime --array=1-$njob -o $logdir/slurm-%A_%a.out $slurmfile $timeout $infile $outdir
