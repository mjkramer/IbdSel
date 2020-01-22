#!/bin/bash

step=$1; shift
if [ -z $step ]; then
    echo "Specify a step"
    exit 1
fi

tag=$1; shift
if [ -z $tag ]; then
    echo "Specify a tag"
    exit 1
fi

source bash/${step}_vars.inc.sh
${step}_vars $tag

rm $infile.done
cp $infile.orig $infile

rm -rf $outdir/EH1 $outdir/EH2 $outdir/EH3

rm $logdir/*

