#!/bin/bash

tag=$1; shift

source bash/stage1_vars.inc.sh
stage1_vars $tag

source bash/job_init.inc.sh

sockdir=$(mktemp -d)
echo "Sockets in $sockdir"

python/zmq_fan.py $sockdir $infile &
qbPid=$!

while [[ ! -S $sockdir/InputReader.ipc || ! -S $sockdir/DoneLogger.ipc ]]; do
    sleep 5
done

echo "Launching $IBDSEL_NTASKS tasks"

echo "Beginning at $(date +%s) = $(date)"

maybe_srun -n $IBDSEL_NTASKS --cpu-bind=cores -- python/stage1_worker.py $sockdir $tag

echo "Ending at $(date +%s) = $(date)"

python/stage1_shutdown.py $sockdir

while [ -d /proc/$qbPid ]; do
    sleep 5
done

rm -rf $sockdir
