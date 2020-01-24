#!/bin/bash

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

source bash/stage1_vars.inc.sh
# no tag specified -> end up with parent dirs (including trailing slash)
# for getting $firstIndir
stage1_vars

tagbase=benchmark_$sysname.$nfiles
firstFactor=$(printf "%03d" ${factors%% *})
firstIndir=$(mktemp -d ${indir}${tagbase}.XXX.$firstFactor)
uniqueId=$(basename $firstIndir | cut -d. -f3)

for f in $factors; do
    tag=$tagbase.$uniqueId.$(printf "%03d" $f)
    stage1_vars $tag
    scripts/prep_p17b.sh -f "shuf -n $nfiles" $tag

    expt="ALL,IBDSEL_NTASKS=$f"
    echo sbatch -q debug --export=$expt -t 00:30:00 -o $logdir/$logfmt slurm/stage1_${sysname}.sl.sh 0.5 $tag
done
