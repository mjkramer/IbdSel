#!/bin/bash

# Assume best-case 3 node-sec per file and 90 seconds per process.
# (Reality is 3-4 minutes per process, 3.6 node-sec per file)
# Using a safety factor of 5.

# Min. lockfile period here is 50 * 3sec / njobs
# So 1Hz for 150 jobs. That's far more jobs than we need. We're good.
export IBDSEL_CHUNKSIZE=${IBDSEL_CHUNKSIZE:-50}

export IBDSEL_WALLTIME=${IBDSEL_WALLTIME:-02:00:00}
# export IBDSEL_CHUNK_MARGIN_SECS=${IBDSEL_CHUNK_MARGIN_SECS:-750} # 50 * 3sec * 5
export IBDSEL_CHUNK_MARGIN_SECS=${IBDSEL_CHUNK_MARGIN_SECS:-1200} # 50 * 3sec * 8
export IBDSEL_FILE_MARGIN_SECS=${IBDSEL_FILE_MARGIN_SECS:-450}  # 90sec * 5
export IBDSEL_STARTUP_SLEEP_SECS=${IBDSEL_STARTUP_SLEEP_SECS:-120}
export IBDSEL_NTASKS=${IBDSEL_NTASKS:-68}
export IBDSEL_SLURMFILE=${IBDSEL_SLURMFILE:-slurm/run_knl.sl.sh}
export IBDSEL_LOGFMT_EXTRA=${IBDSEL_LOGFMT_EXTRA:-_knl}
# export IBDSEL_USE_BURSTBUF=${IBDSEL_USE_BURSTBUF:-1}

source bash/do_submit_stage2.inc.sh
