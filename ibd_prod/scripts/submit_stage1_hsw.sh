#!/bin/bash

# Calculations assume best-case 0.3 node-sec per file and 10 seconds per process.
# Using a safety factor of 5.

# Min. lockfile period here is 250 * 0.3sec / njobs
# So 1.33Hz for 100 jobs at worst (and we probably need < 60 total 2hr jobs)
# Testing shows $HOME/$CFS can handle at least 4Hz no problem
export IBDSEL_CHUNKSIZE=${IBDSEL_CHUNKSIZE:-250}

export IBDSEL_WALLTIME=${IBDSEL_WALLTIME:-02:00:00}
export IBDSEL_CHUNK_MARGIN_SECS=${IBDSEL_CHUNK_MARGIN_SECS:-375}  # 250 * 0.3sec * 5
export IBDSEL_FILE_MARGIN_SECS=${IBDSEL_FILE_MARGIN_SECS:-50}    # 10sec * 5
export IBDSEL_STARTUP_SLEEP_SECS=${IBDSEL_STARTUP_SLEEP_SECS:-120}
export IBDSEL_NTASKS=${IBDSEL_NTASKS:-40}
export IBDSEL_SLURMFILE=${IBDSEL_SLURMFILE:-slurm/run_hsw.sl.sh}
export IBDSEL_LOGFMT_EXTRA=${IBDSEL_LOGFMT_EXTRA:-_hsw}

source bash/do_submit_stage1.inc.sh
