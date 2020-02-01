#!/bin/bash

# protip: use { eval `scripts/clear.sh -r stage1 'benchmark.*'` } to clear all benchmarks, etc.

while getopts "r" opt; do
    case $opt in
        r)
            reckless=1
            ;;
        *)
            exit 1
            ;;
    esac
done
shift $((OPTIND-1))

step=$1; shift
if [ -z $step ]; then
    echo "Specify a step"
    exit 1
fi

source bash/${step}_vars.inc.sh
${step}_vars $@

# Safety check: Don't delete a dir if we made it non-writable
if [ -z "$reckless" ]; then
    for varname in trueOutdir outdir indir logdir; do
        dir=$(eval echo \$$varname)
        if [ ! -w "$dir" ]; then
            >&2 echo "Skipping $dir as it's not writable"
            eval $varname=
        fi
    done
fi

echo rm -rf $trueOutdir $outdir $indir $logdir
