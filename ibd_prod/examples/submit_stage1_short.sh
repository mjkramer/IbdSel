#!/bin/bash

tag=$1; shift
njob=$1; shift

scripts/submit_stage1.sh slurm/stage1_knl.sl.sh 2:00:00 1.5 $tag $njob
