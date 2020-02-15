#!/bin/bash

export IBDSEL_CHUNKSIZE=YYY

export IBDSEL_WALLTIME=02:00:00
export IBDSEL_CHUNK_MARGIN_SECS=YYY
export IBDSEL_FILE_MARGIN_SECS=YYY
export IBDSEL_STARTUP_SLEEP_SECS=120
export IBDSEL_NTASKS=YYY
export IBDSEL_SLURMFILE=slurm/run_hsw.sl.sh
export IBDSEL_LOGFMT_EXTRA=_hsw

source bash/do_submit_stage2.inc.sh
