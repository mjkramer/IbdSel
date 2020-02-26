#!/bin/bash

site=$1; shift
phase=$1; shift

conffile=../static/configs/config.nominal.txt
infile=tests/out_stage1.root
outfile=tests/out_stage2.root
seq=0

$IBDSEL_HOME/selector/_build/stage2.exe \
    $conffile $infile $outfile $site $phase $seq
