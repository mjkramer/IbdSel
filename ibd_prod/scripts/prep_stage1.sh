#!/bin/bash

source bash/stage1_vars.inc.sh

LISTFILEPATH=../static/filelist/paths.physics.good.p17b.v3.sync.txt.gz

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

stage1_vars $tag

mkdir -p $indir $logdir $trueOutdir
mkdir -p $(dirname $outdir)
ln -s $trueOutdir $outdir

filter_cmd=${filter_cmd:-cat}

gunzip -c $LISTFILEPATH | $filter_cmd > $infile.orig
cp $infile.orig $infile
