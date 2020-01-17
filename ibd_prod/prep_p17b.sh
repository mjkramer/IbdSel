#!/bin/bash

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag!"
    exit 1
fi

source bash/set_vars.inc.sh
set_vars $tag

mkdir -p $trueOutdir
mkdir -p ../../data/ibd_fbf
ln -s $trueOutdir $outdir

mkdir -p $indir
ln -s ../orig/paths.physics.good.p17b.v3.sync.txt $infile.orig
cp $infile.orig $infile
chmod +w $infile

mkdir -p $logdir
