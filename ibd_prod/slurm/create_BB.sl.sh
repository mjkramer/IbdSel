#!/bin/bash
# Note: Must specify -C [haswell or knl] on command line
#SBATCH -q debug -t 00:02:00 -N 1
#BB create_persistent name=dyb_ibdsel capacity=2TB type=scratch access_mode=striped
