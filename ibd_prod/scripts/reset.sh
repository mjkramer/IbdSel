#!/bin/bash

step=$1; shift
if [ -z $step ]; then
    echo "Specify a step"
    exit 1
fi

tag=$1; shift
if [ -z $tag ]; then
    echo "Specify a tag"
    exit 1
fi

eval $(scripts/clear.sh $step $tag)
scripts/prep_${step}.sh $tag
