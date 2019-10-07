#!/bin/bash

timeout=$1; shift

source job_init.inc.sh

srun -n 48 --cpu-bind=cores ./ibd_worker.py -t $timeout -c 25 $@
