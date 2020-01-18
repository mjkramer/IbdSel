#!/bin/bash

timeout=$1; shift
tag=$1; shift

source bashlib/set_vars.inc.sh
set_vars $tag

source bashlib/job_init.inc.sh

sockdir=$(mktemp -d)
echo "Sockets in $sockdir"

sleep $(( RANDOM % 120 ))       # prevent lockfile contention (200 jobs)

python/queue_buffer.py -t $timeout $sockdir $infile &
qbPid=$!

while [[ ! -S $sockdir/InputReader.ipc || ! -S $sockdir/DoneLogger.ipc ]]; do
    sleep 5
done

ntasks=${IBDSEL_NTASKS:-48}
echo "Launching $ntasks tasks"

echo "Beginning at $(date +%s) = $(date)"

# NB: Looks like 24 might be closer to the ideal -n, based on looking at "top"
# output in the logs
maybe_srun -n $ntasks --cpu-bind=cores -- python/stage1_worker.py -q $sockdir $infile $outdir

echo "Ending at $(date +%s) = $(date)"

python/stage1_shutdown.py $sockdir

while [ -d /proc/$qbPid ]; do
    sleep 5
done

rm -rf $sockdir
