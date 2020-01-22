#!/bin/bash

nfiles=500
factors="48 68 136 272"

source bash/stage1_vars.inc.sh

firstFactor=$(printf "%03d" ${factors%% *})
firstIndir=$(mktemp -d ../../data/prod_input/benchmark.$nfiles.XXX.$firstFactor)
uniqueId=$(basename $firstIndir | cut -d. -f3)

for f in $factors; do
    tag=benchmark.$nfiles.$uniqueId.$(printf "%03d" $f)
    stage1_vars $tag
    scripts/prep_p17b.sh -f "shuf -n $nfiles" $tag

    expt="ALL,IBDSEL_NTASKS=$f"
    echo sbatch -q debug --export=$expt -t 00:30:00 -o $logfmt slurm/stage1_knl.sl.sh 0.5 $tag
done
