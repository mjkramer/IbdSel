source bash/common_vars.inc.sh

stage1_vars() {
    local tag=$1; shift
    if [ -z "$tag" ]; then
        echo "Specify a tag!"
        exit 1
    fi

    trueOutdir=$SCRATCH/p17b/stage1_fbf/$tag

    outdir=$(data_dir_for   stage1_fbf   $tag)
    indir=$( data_dir_for   stage1_input $tag)
    infile=$(input_file_for stage1       $tag)
    logdir=$(log_dir_for    stage1       $tag)
}
