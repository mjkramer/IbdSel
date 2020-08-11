#!/bin/bash

source bash/stage2_vars.inc.sh

while getopts "f:cb:" opt; do
    case $opt in
        f)
            filter_cmd=$OPTARG
            ;;
        c)
            create_config=1
            ;;
        b)
            based_on=$OPTARG
            ;;
        *)
            exit 1
            ;;
    esac
done
shift $((OPTIND-1))

tag=$1; shift
if [ -z $tag ]; then
    echo "Specify a tag!"
    exit 1
fi

configname=$1; shift
if [ -z $configname ]; then
    echo "Specify a config!"
    exit 1
fi

stage2_vars $tag@$configname

mkdir -p $indir $logdir $trueOutdir
mkdir -p $(dirname $outdir)

# if $outdir already exists, we'll end up making a symlink *inside* it, bad!
if [ ! -e $outdir ]; then
    ln -s $trueOutdir $outdir
fi

conffile=$indir/config.$configname.txt

if [ -n "$based_on" ]; then
    srcdir=$indir/../$tag@$based_on
    src_infile=$srcdir/$(basename $infile)
    src_conffile=$srcdir/config.${based_on}.txt
    cp $src_infile $infile
    cp $src_conffile $conffile
else
    if [ -n "$create_config" ]; then
        cp $IBDSEL_CONFIGDIR/config.nominal.txt $conffile
    else
        cp $IBDSEL_CONFIGDIR/config.$configname.txt $indir
    fi

    filter_cmd=${filter_cmd:-cat}
    python/dump_days.py | $filter_cmd > $infile
fi

cp $conffile $outdir
