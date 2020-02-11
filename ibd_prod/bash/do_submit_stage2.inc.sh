#!/bin/bash

source bash/submit_utils.inc.sh
source bash/stage2_vars.inc.sh

if [ $# -lt 3 ]; then
    echo "Usage: $0 tag configname njob [extra sbatch args..]"
    exit 1
fi

tag=$1; shift
configname=$1; shift
njob=$1; shift
sbatch_extra=$@

stage2_vars $tag $configname
check_paths
check_compiled ../selector/stage2/stage2_main.cc

$(sbatch_cmd $njob $sbatch_extra) bash/stage2_job.sh $tag $configname
