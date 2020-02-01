#!/bin/bash

step=$1; shift
if [ -z "$step" ]; then
    echo "Specify a step"
    exit 1
fi

eval $(scripts/clear.sh $step $@)
scripts/prep_${step}.sh $@
