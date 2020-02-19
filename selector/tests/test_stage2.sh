#!/bin/bash

site=$1; shift
phase=$1; shift

script=../ibd_prod/cling/run_stage2.C
conffile=../static/configs/config.nominal.txt
infile=tests/out_stage1.root
outfile=tests/out_stage2.root
seq=0

root -l -b -q "$script(\"$conffile\", \"$infile\", \"$outfile\", $site, $phase, $seq)"
