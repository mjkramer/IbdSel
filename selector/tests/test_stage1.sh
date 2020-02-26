#!/bin/bash

runno=$1; shift
fileno=$1; shift
site=$1; shift
phase=$1; shift

infile=$(p17b_find $runno $fileno)
outfile=tests/out_stage1.root

$IBDSEL_HOME/selector/_build/stage1.exe \
    $infile $outfile $site $phase
