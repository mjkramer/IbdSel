#!/bin/bash

source bash/stage1_vars.inc.sh

P17B_LIST=../static/filelist/paths.physics.good.p17b.v3.sync.txt.gz

LISTFILEPATH=${LISTFILEPATH:-${P17B_LIST}}

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

stage1_vars $tag

mkdir -p $indir $logdir $trueOutdir
mkdir -p $(dirname $outdir)
ln -s $trueOutdir $outdir

filter_cmd=${filter_cmd:-cat}

if [[ ${LISTFILEPATH##*.} == "gz" ]]; then
    cat_cmd="gunzip -c"
else
    cat_cmd=cat
fi

$cat_cmd $LISTFILEPATH | $filter_cmd | shuf > $infile
