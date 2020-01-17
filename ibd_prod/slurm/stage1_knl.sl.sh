#!/bin/bash
#SBATCH -N 1 -A dayabay -C knl -L project,projecta,SCRATCH -q regular

# Submit me from ibd_prod directory! Do a "newenv" first! Specify -t!
# Make sure stage1_main.cc has already been compiled, to avoid race conditions
# Submit with submit_stage1.sh

timeout=$1; shift
tag=$1; shift

infile=../../data/prod_input/$tag/input.ibd.txt
outdir=../../data/ibd_fbf/$tag

source bash/job_init.inc.sh

sockdir=$(mktemp -d)
echo "Sockets in $sockdir"

./queue_buffer.py -t $timeout $sockdir $infile &
qbPid=$!

while [[ ! -S $sockdir/InputReader.ipc || ! -S $sockdir/DoneLogger.ipc ]]; do
    sleep 5
done

ntasks=${IBDSEL_NTASKS:-48}
echo "Launching $ntasks tasks"

echo "Beginning at $(date +%s) = $(date)"

# NB: Looks like 24 might be closer to the ideal -n, based on looking at "top"
# output in the logs
srun -n $ntasks --cpu-bind=cores ./ibd_worker_knl.py -q $sockdir $infile $outdir

echo "Ending at $(date +%s) = $(date)"

./knl_shutdown.py $sockdir

while [ -d /proc/$qbPid ]; do
    sleep 5
done

rm -rf $sockdir
