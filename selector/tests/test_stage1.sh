#!/bin/bash

runno=$1; shift
fileno=$1; shift
site=$1; shift
phase=$1; shift

script=../ibd_prod/cling/run_stage1.C
infile=$(p17b_find $runno $fileno)
outfile=tests/out_stage1.root

root -l -b -q "$script(\"$infile\", \"$outfile\", $site, $phase)"
