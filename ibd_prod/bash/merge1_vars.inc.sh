source bash/common_vars.inc.sh

merge1_vars() {
    local tag=$1; shift

    trueOutdir=$SCRATCH/IbdSelData/stage1_dbd/$tag

    outdir=$(data_dir_for   stage1_dbd   $tag)
    indir=$( data_dir_for   merge1_input $tag)
    infile=$(input_file_for merge1       $tag)
    logdir=$(log_dir_for    merge1       $tag)
}
