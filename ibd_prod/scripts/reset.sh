#!/bin/bash

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag"
    exit 1
fi

source bashlib/set_vars.inc.sh
set_vars $tag

rm $infile.done
cp $infile.orig $infile

rm -rf $outdir/EH1 $outdir/EH2 $outdir/EH3

rm $logdir/*

