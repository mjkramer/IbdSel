source bash/common_vars.inc.sh

stage2_vars() {
    local arr=(${1//@/ }); shift # split $1 at the @ character
    local tag=${arr[0]}
    local configname=${arr[1]}

    if [ -z "$tag" ]; then
        echo "Specify a tag!"
        exit 1
    fi

    if [ -z "$configname" ]; then
        echo "Specify a config!"
        exit 1
    fi

    export IBDSEL_CONFIGDIR=${IBDSEL_CONFIGDIR:-../static/configs}

    trueOutdir=$SCRATCH/IbdSelData/stage2_dbd/$tag@$configname

    outdir=$(data_dir_for   stage2_dbd   $tag@$configname)
    indir=$( data_dir_for   stage2_input $tag@$configname)
    infile=$(input_file_for stage2       $tag@$configname)
    logdir=$(log_dir_for    stage2       $tag@$configname)
}
