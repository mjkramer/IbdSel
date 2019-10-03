#!/bin/bash

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag!"
    exit 1
fi

outdir=$SCRATCH/p17b/ibd_fbf/$tag
mkdir -p $outdir
mkdir -p ../../data/ibd_fbf
ln -s $outdir ../../data/ibd_fbf/$tag

indir=../../data/prod_input/$tag
mkdir -p $indir
cp ../../data/prod_input/orig/paths.physics.good.p17b.v3.sync.txt $indir/input.ibd.txt
chmod +w $indir/input.ibd.txt
