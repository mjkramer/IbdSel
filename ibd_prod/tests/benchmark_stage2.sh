#!/bin/bash

export IBDSEL_CHUNKSIZE=25
export IBDSEL_WALLTIME=00:30:00
export IBDSEL_CHUNK_MARGIN_SECS=0
export IBDSEL_FILE_MARGIN_SECS=0
export IBDSEL_STARTUP_SLEEP_SECS=0
export IBDSEL_USE_BURSTBUF=1

nfiles=100

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

tag=$1; shift
if [ -z "$tag" ]; then
    echo "Specify a tag"
    exit 1
fi

if [ -n "$use_haswell" ]; then
    factors="8 16 24 32 48"
    sysname=hsw
else
    factors="16 32 48 58 68 78 88"
    sysname=knl
fi

export IBDSEL_SLURMFILE=slurm/run_${sysname}.sl.sh

source bash/stage2_vars.inc.sh
stage2_vars DUMMY DUMMY
inroot=$(dirname $indir)

basebenchname=bm_$sysname.$nfiles
firstFactor=$(printf "%03d" ${factors%% *})
firstIndir=$(mktemp -d ${inroot}/${tag}@${basebenchname}.XXX.$firstFactor)
uniqueId=$(basename $firstIndir | cut -d @ -f 2 | cut -d . -f 3)

oldconfigdir=$IBDSEL_CONFIGDIR

export IBDSEL_CONFIGDIR=$(mktemp -d)

for f in $factors; do
    configname=$basebenchname.$uniqueId.$(printf "%03d" $f)
    cp $oldconfigdir/config.nominal.txt $IBDSEL_CONFIGDIR/config.$configname.txt

    stage2_vars $tag $configname
    scripts/prep_stage2.sh -f "shuf -n $nfiles" $tag $configname

    export IBDSEL_NTASKS=$f
    source bash/do_submit_stage2.inc.sh $tag $configname 1 -q regular
done
