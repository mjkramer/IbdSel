#!/bin/bash

# Usage: filter_done.sh [-p PENDING] STEP TAG
# where STEP is e.g. stage1 and TAG is e.g. 2020_01_26
# or where STEP is e.g. stage2 and TAG is e.g. 2020_01_26@yolo.
# If jobs are currently running, PENDING should be NUMJOBS * CHUNKSIZE;
# otherwise you'll get multiple jobs processing the same file.

while getopts "p:" opt; do
    case $opt in
        p)
            pending=$OPTARG
            ;;
        *)
            exit 1
            ;;
    esac
done
shift $((OPTIND-1))

step=$1; shift
if [ -z "$step" ]; then
    echo "Specify a step (e.g. stage1)"
    exit 1
fi

tag=$1; shift
if [ -z "$tag" ]; then
    echo "Specify a tag"
    exit 1
fi

source bash/${step}_vars.inc.sh
${step}_vars $tag

echo "Acquiring lock"
lockfile -5 $infile.lock

cut -d' ' -f2- $infile.done > $infile.omit

if [ -n "$pending" ]; then
    offset=$(cat $infile.offset)

    # In 0-based indexing, take l = lines[ [o-p, o) ]. Count = p.
    # sed uses 1-based indexing and inclusive ranges: l == lines'[ [o-p+1, o] ]
    sed -n "$((offset - pending + 1)),$((offset))p;$((offset+1))q" \
        $infile >> $infile.omit
fi

cp $infile $infile.prev
comm -23 <(sort $infile.prev) <(sort $infile.omit) > $infile
rm $infile.offset

rm -f $infile.lock
