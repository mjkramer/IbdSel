#!/bin/bash

listfile=../../data/prod_input/orig/paths.physics.good.p17b.v3.sync.txt

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

source bash/set_vars.inc.sh
set_vars $tag

mkdir -p $trueOutdir
mkdir -p $(dirname $outdir)
ln -s $trueOutdir $outdir

mkdir -p $logdir


mkdir -p $indir

if [ -n "$filter_cmd" ]; then
    eval $filter_cmd $listfile > $infile.orig
else
    ln -rs $listfile $infile.orig
fi

cp $infile.orig $infile
chmod +w $infile
