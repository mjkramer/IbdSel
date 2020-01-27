#!/bin/bash

export IBDSEL_CHUNKSIZE=25
export IBDSEL_WALLTIME=00:10:00
export IBDSEL_CHUNK_MARGIN_SECS=240
export IBDSEL_FILE_MARGIN_SECS=120
export IBDSEL_STARTUP_SLEEP_SECS=10
export IBDSEL_NTASKS=54

tag=$1; shift
njob=1

bash/do_submit_stage1.sh slurm/stage1_knl.sl.sh $tag $njob -q debug
