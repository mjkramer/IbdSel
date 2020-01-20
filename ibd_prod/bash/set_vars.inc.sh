# -*- sh-shell: bash -*-

set_vars() {
    tag=$1; shift

    trueOutdir=$SCRATCH/p17b/ibd_fbf/$tag
    outdir=../../data/ibd_fbf/$tag
    indir=../../data/prod_input/$tag
    infile=$indir/input.ibd.txt
    logdir=../../log/ibd_prod/$tag
    logfmt=$logdir/slurm-%A_%a.out
}
