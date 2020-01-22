maybe_srun() {
    local arg srun_args cmd_str

    while [ $# -ne 0 ]; do
        arg=$1; shift
        if [ $arg == "--" ]; then
            break
        fi
        srun_args="$srun_args $arg"
    done

    while [ $# -ne 0 ]; do
        arg=$1; shift
        cmd_str="$cmd_str $arg"
    done

    if [ -n "$SLURM_JOB_ID" ]; then
        srun $srun_args $cmd_str
    else
        $cmd_str
    fi
}
