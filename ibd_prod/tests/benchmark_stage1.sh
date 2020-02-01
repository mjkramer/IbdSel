#!/bin/bash

export IBDSEL_CHUNKSIZE=250
export IBDSEL_WALLTIME=00:30:00
export IBDSEL_CHUNK_MARGIN_SECS=0
export IBDSEL_FILE_MARGIN_SECS=0
export IBDSEL_STARTUP_SLEEP_SECS=0

nfiles=500

while getopts "H" opt; do
    case $opt in
        H)
            use_haswell=1
            ;;
        *)
            exit 1
            ;;
    esac
done
shift $((OPTIND-1))

if [ -n "$use_haswell" ]; then
    factors="4 8 16 24 32"
    sysname=hsw
else
    factors="8 16 32 48 68"
    sysname=knl
fi

export IBDSEL_SLURMFILE=slurm/run_${sysname}.sl.sh

# HACK: Call stage1_vars with no tag arg, end up with parent dirs in $indir etc
# (including trailing slash). We need these parent dirs to compute $firstIndir.
source bash/stage1_vars.inc.sh
stage1_vars

tagbase=benchmark_$sysname.$nfiles
firstFactor=$(printf "%03d" ${factors%% *})
firstIndir=$(mktemp -d ${indir}${tagbase}.XXX.$firstFactor)
uniqueId=$(basename $firstIndir | cut -d. -f3)

for f in $factors; do
    tag=$tagbase.$uniqueId.$(printf "%03d" $f)
    stage1_vars $tag
    scripts/prep_stage1.sh -f "shuf -n $nfiles" $tag

    export IBDSEL_NTASKS=$f
    source bash/do_submit_stage1.inc.sh $tag 1 -q debug
done
