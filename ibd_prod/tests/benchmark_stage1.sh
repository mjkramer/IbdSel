#!/bin/bash

nfiles=300
factors="48 68 136 272"

source bash/set_vars.inc.sh

# nfiles=50
# factors="48"

firstFactor=$(printf "%03d" ${factors%% *})
firstIndir=$(mktemp -d ../../data/prod_input/benchmark.$nfiles.XXX.$firstFactor)
uniqueId=$(basename $firstIndir | cut -d. -f3)

for f in $factors; do
    tag=benchmark.$nfiles.$uniqueId.$(printf "%03d" $f)
    set_vars $tag
    ./prep_p17b.sh $tag

    mv $infile.orig $infile.full
    shuf -n $nfiles $infile.full > $infile.orig
    cp $infile.orig $infile
    rm $infile.full

    expt="ALL,IBDSEL_NTASKS=$f"
    echo sbatch --export=$expt -t 00:30:00 -o $logfmt slurm/stage1_knl.sl.sh 0.5 $tag
done
