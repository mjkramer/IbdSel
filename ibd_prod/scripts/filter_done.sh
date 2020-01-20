#!/bin/bash

infile=$1; shift

if [ -z "$infile" ]; then
    echo "Specify an input list file"
    exit 1
fi

comm -23 <(sort $infile.orig) <(awk '{print $2}' $infile.done | sort) > $infile
rm $infile.offset
