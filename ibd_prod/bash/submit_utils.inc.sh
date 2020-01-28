function check_paths() {
    local to_check=("$IBDSEL_SLURMFILE" "$infile" "$outdir" "$logdir")

    for path in "${to_check[@]}"; do
        if [ ! -e "$path" ]; then
            echo "$path does not exist. Bailing."
            exit 1
        fi
    done
}

function check_compiled() {
    local srcfile=$1; shift

    local base="${srcfile%.*}"          # strip extension
    local ext="${srcfile##*.}"          # get extension

    local libfile="${base}_${ext}.so"

    if [ ! -f "$libfile" -o "$srcfile" -nt "$libfile" ]; then
        echo "Compile $srcfile (in ROOT 6) first"
        exit
    fi
}

function sbatch_cmd() {
    local njob=$1; shift
    local extra=$@

    local array="--array=1-$njob"
    local walltime="-t $IBDSEL_WALLTIME"
    local exports="--export="$(echo ${!IBDSEL_*} | tr ' ' ,)
    local logdest="-o $logdir/$logfmt"

    echo "do_sbatch $extra $array $walltime $exports $logdest $IBDSEL_SLURMFILE"
}

function do_sbatch() {
    local cmd="sbatch $@"

    for var in ${!IBDSEL_*}; do
        local value=$(eval echo \$$var)
        echo "$var=$value"
    done

    if [ "$IBDSEL_DRYRUN" == "1" ]; then
        echo $cmd
    else
        ( set -x; $cmd )
    fi
}
