#!/bin/bash

# Assume best-case 1 node-sec per file and 30 seconds per process.
# (Reality is 30-60 sec per process, 0.83 node-sec per file)
# Using a safety factor of 5.

# Min. lockfile period here is 50 * 1sec / njobs
# So 1Hz for 50 jobs. That's far more jobs than we need. We're good.
export IBDSEL_CHUNKSIZE=50

export IBDSEL_WALLTIME=02:00:00
export IBDSEL_CHUNK_MARGIN_SECS=250 # 50 * 1sec * 5
export IBDSEL_FILE_MARGIN_SECS=150  # 30sec * 5
export IBDSEL_STARTUP_SLEEP_SECS=120
export IBDSEL_NTASKS=32
export IBDSEL_SLURMFILE=slurm/run_hsw.sl.sh
export IBDSEL_LOGFMT_EXTRA=_hsw

source bash/do_submit_stage2.inc.sh
