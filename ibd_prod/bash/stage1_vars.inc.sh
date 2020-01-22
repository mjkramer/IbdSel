# -*- sh-shell: bash -*-

stage1_vars() {
    tag=$1; shift

    trueOutdir=$SCRATCH/p17b/stage1_fbf/$tag
    outdir=../../data/stage1_fbf/$tag
    indir=../../data/stage1_input/$tag
    infile=$indir/input.ibd.txt
    logdir=../../log/ibd_prod/$tag
    logfmt=$logdir/slurm-%A_%a.out
}
