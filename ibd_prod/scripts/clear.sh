#!/bin/bash

# protip: use { eval `./clear.sh stage1 'benchmark.*'` } to clear all benchmarks, etc.

step=$1; shift
if [ -z $step ]; then
    echo "Specify a step"
    exit 1
fi

tag=$1; shift
if [ -z $tag ]; then
    echo "Specify a tag!"
    exit 1
fi

source bash/${step}_vars.inc.sh
${step}_vars $tag

echo rm -rf $trueOutdir $outdir $indir $logdir
