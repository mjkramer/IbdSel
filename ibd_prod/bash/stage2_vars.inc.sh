source bash/common_vars.inc.sh

stage2_vars() {
    local tag=$1; shift
    if [ -z "$tag" ]; then
        echo "Specify a tag!"
        exit 1
    fi

    local configname=$1; shift
    if [ -z "$configname" ]; then
        echo "Specify a config!"
        exit 1
    fi

    export IBDSEL_CONFIGDIR=${IBDSEL_CONFIGDIR:-../static/configs}

    trueOutdir=$SCRATCH/p17b/stage2_dbd/$tag@$configname

    outdir=$(data_dir_for   stage2_dbd   $tag@$configname)
    indir=$( data_dir_for   stage2_input $tag@$configname)
    infile=$(input_file_for stage2       $tag@$configname)
    logdir=$(log_dir_for    stage2       $tag@$configname)
}
