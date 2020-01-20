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

# XXX check this
if [ -n "$pending" ]; then
    # the range that we DON'T want because they're being processed
    end=$(cat $infile.offset)
    start=$((end - pending))

    head -n $start $infile > $infile.tmp
    tail -n +$end $infile >> $infile.tmp
else
    mv $infile $infile.tmp
fi

comm -23 <(sort $infile.tmp) <(awk '{print $2}' $infile.done | sort) > $infile
rm $infile.offset

rm -f $infile.lock
