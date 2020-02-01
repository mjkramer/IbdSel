#!/bin/bash

source bash/merge1_vars.inc.sh

while getopts "f:" opt; do
    case $opt in
        f)
            filter_cmd=$OPTARG
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

merge1_vars $tag

mkdir -p $indir $logdir $trueOutdir
mkdir -p $(dirname $outdir)
ln -s $trueOutdir $outdir

filter_cmd=${filter_cmd:-cat}

python/dump_days.py | $filter_cmd > $infile
