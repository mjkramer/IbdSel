#!/bin/bash

source bash/stage2_vars.inc.sh

tag=$1; shift
configname=$1; shift

stage2_vars $tag $configname

source bash/job_init.inc.sh

sockdir=$(mktemp -d)
export IBDSEL_SOCKDIR=$sockdir
echo "Sockets in $sockdir"

python/zmq_fan.py $infile &
qbPid=$!

while [[ ! -S $sockdir/InputReader.ipc || ! -S $sockdir/DoneLogger.ipc ]]; do
    sleep 5
done

echo "Launching $IBDSEL_NTASKS tasks"

echo "Beginning at $(date +%s) = $(date)"

maybe_srun -n $IBDSEL_NTASKS --cpu-bind=cores -- python/stage2_worker.py $tag $configname

echo "Ending at $(date +%s) = $(date)"

python/zmq_shutdown.py

while [ -d /proc/$qbPid ]; do
    sleep 5
done

rm -rf $sockdir
