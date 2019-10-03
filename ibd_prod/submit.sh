#!/bin/bash

# brittle hack
if (env | grep LD_LIBRARY_PATH | grep mkramer >/dev/null 2>&1); then
    echo "Your environment looks dirty. Bailing."
    exit 1
fi

tag=$1; shift
njob=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag"
    exit 1
fi

if [ -z $njob ]; then
    echo "Specify # of jobs"
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

echo sbatch --array=1-$njob -o $logdir/slurm-%A_%a.out ibd_job.sl.sh $infile $outdir
