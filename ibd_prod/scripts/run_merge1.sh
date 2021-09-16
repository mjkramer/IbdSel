#!/bin/bash

source bash/merge1_vars.inc.sh

if [ $# -ne 2 ]; then
    echo "Usage: $0 tag nproc"
    exit 1
fi

tag=$1; shift
nproc=$1; shift

merge1_vars $tag

for i in $(seq $nproc); do
    logfile=$(mktemp $logdir/merge1.$tag.$(hostname).XXX.out)
    nohup python/merge1_worker.py $tag &>$logfile </dev/null &
done
