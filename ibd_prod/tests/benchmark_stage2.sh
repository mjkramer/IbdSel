#!/bin/bash

nfiles=500
walltime=00:60:00

while getopts "n:w:f:H" opt; do
    case $opt in
        n)
            nfiles=$OPTARG
            ;;
        w)
            walltime=$OPTARG
            ;;
        f)
            factors=$(echo $OPTARG | sed 's/,/ /g')
            ;;
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
    factors=${factors:-"8 16 24 32 48"}
    sysname=hsw
else
    factors=${factors:-"32 48 58 68 78 88"}
    sysname=knl
fi

export IBDSEL_CHUNKSIZE=$nfiles
export IBDSEL_WALLTIME=$walltime

export IBDSEL_CHUNK_MARGIN_SECS=0
export IBDSEL_FILE_MARGIN_SECS=0
export IBDSEL_STARTUP_SLEEP_SECS=0
export IBDSEL_USE_BURSTBUF=1
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
