#!/bin/bash

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

infile=$1; shift

if [ -z "$infile" ]; then
    echo "Specify an input list file"
    exit 1
fi

echo "Acquiring lock"
lockfile -5 $infile.lock

awk '{print $2}' $infile.done > $infile.tmp.omit

if [ -n "$pending" ]; then
    offset=$(cat $infile.offset)

    # In 0-based indexing, take l = lines[ [o-p, o) ]. Count = p.
    # sed uses 1-based indexing and inclusive ranges: l == lines'[ [o-p+1, o] ]
    sed -n "$((offset - pending + 1)),$((offset))p;$((offset+1))q" \
        $infile >> $infile.tmp.omit
fi

comm -23 <(sort $infile.orig) <(sort $infile.tmp.omit) > $infile
rm $infile.offset

rm -f $infile.lock $infile.tmp.omit
