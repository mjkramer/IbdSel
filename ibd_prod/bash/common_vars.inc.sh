# relative to ibd_prod
data_base=../../data
log_base=../../log

logfmt=slurm-%A_%a.out

data_dir_for() {
    local name=$1; shift
    local tag=$1; shift
    echo $data_base/$name/$tag
}

log_dir_for() {
    local name=$1; shift
    local tag=$1; shift
    echo $log_base/$name/$tag
}

input_file_for() {
    local step=$1; shift
    local tag=$1; shift
    echo $(data_dir_for ${step}_input $tag)/input.$step.txt
}
