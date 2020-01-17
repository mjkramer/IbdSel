#!/bin/bash

# protip: use { eval `./clear.sh 'benchmark.*'` } to clear all benchmarks, etc.

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag!"
    exit 1
fi

source bash/set_vars.inc.sh
set_vars $tag

echo rm -rf $trueOutdir $outdir $indir $logdir
