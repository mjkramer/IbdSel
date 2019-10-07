#!/bin/bash

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag"
    exit 1
fi

inp=../../data/prod_input/$tag

rm $inp/input.ibd.done.txt
cp $inp/input.ibd.orig.txt $inp/input.ibd.txt

outp=../../data/ibd_fbf/$tag

rm -rf $outp/EH1 $outp/EH2 $outp/EH3
