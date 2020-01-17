#!/bin/bash

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag"
    exit 1
fi

source bash/set_vars.inc.sh
set_vars $tag

rm $indir/input.ibd.done.txt
cp $infile.orig $infile

rm -rf $outdir/EH1 $outdir/EH2 $outdir/EH3

rm $logdir/*

