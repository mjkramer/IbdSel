#!/bin/bash

source bash/submit_utils.inc.sh
source bash/stage1_vars.inc.sh

if [ $# -lt 2 ]; then
    echo "Usage: $0 tag njob [extra sbatch args...]"
    exit 1
fi

tag=$1; shift
njob=$1; shift
sbatch_extra=$@

stage1_vars $tag
check_paths
check_compiled ../selector/stage1/stage1_main.cc

$(sbatch_cmd $njob $sbatch_extra) bash/stage1_job.sh $tag
