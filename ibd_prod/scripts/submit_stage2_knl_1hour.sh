#!/bin/bash

# Assume best-case 3 node-sec per file and 90 seconds per process.
# (Reality is 3-4 minutes per process, 3.6 node-sec per file)
# Using a safety factor of 5.

# Min. lockfile period here is 25 * 3sec / njobs
# So 1Hz for 75 jobs. That's far more jobs than we need. We're good.
export IBDSEL_CHUNKSIZE=25

export IBDSEL_WALLTIME=01:00:00
# previous chunk margin of 450 (== file margin) left us with some stragglers
export IBDSEL_CHUNK_MARGIN_SECS=600
export IBDSEL_FILE_MARGIN_SECS=450  # 90sec * 5
export IBDSEL_STARTUP_SLEEP_SECS=60
export IBDSEL_NTASKS=68
export IBDSEL_SLURMFILE=slurm/run_knl.sl.sh
export IBDSEL_LOGFMT_EXTRA=_knl
export IBDSEL_USE_BURSTBUF=1

source bash/do_submit_stage2.inc.sh
