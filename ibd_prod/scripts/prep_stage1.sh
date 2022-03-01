#!/bin/bash

source bash/stage1_vars.inc.sh

# For backward compatibility, accept LISTFILEPATH in lieu of
# IBDSEL_FILELIST_PATH
IBDSEL_FILELIST_PATH=${LISTFILEPATH:-${IBDSEL_FILELIST_PATH}}

while getopts "f:" opt; do
    case $opt in
        f)
            filter_cmd=$OPTARG
            ;;
        *)
            exit 1
            ;;
    esac
done
shift $((OPTIND-1))

tag=$1; shift

if [ -z $tag ]; then
    echo "Specify a tag!"
    exit 1
fi

if [ -z $IBDSEL_FILELIST_PATH ]; then
    echo "Whoops, please set IBDSEL_FILELIST_PATH"
    exit 1
fi

stage1_vars $tag

mkdir -p $indir $logdir $trueOutdir
mkdir -p $(dirname $outdir)
ln -sfn $trueOutdir $outdir

filter_cmd=${filter_cmd:-cat}

if [[ ${LISTFILEPATH##*.} == "gz" ]]; then
    cat_cmd="gunzip -c"
else
    cat_cmd=cat
fi

$cat_cmd $LISTFILEPATH | $filter_cmd | shuf > $infile
