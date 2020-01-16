#!/bin/bash

timeout=$1; shift
infile=$1; shift
outdir=$1; shift

source job_init.inc.sh

sockdir=$(mktemp -d)
echo "Sockets in $sockdir"

./queue_buffer.py -t $timeout $sockdir $infile &
qbPid=$!

while [[ ! -S $sockdir/InputReader.ipc || ! -S $sockdir/DoneLogger.ipc ]]; do
    sleep 5
done

# NB: Looks like 24 might be closer to the ideal -n, based on looking at "top"
# output in the logs
srun -n 48 --cpu-bind=cores ./ibd_worker_knl.py -q $sockdir $infile $outdir

./knl_shutdown.py $sockdir

while [ -d /proc/$qbPid ]; do
    sleep 5
done

rm -rf $sockdir
