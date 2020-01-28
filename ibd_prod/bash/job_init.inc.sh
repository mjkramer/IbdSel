source bash/job_env.inc.sh
source bash/maybe_srun.inc.sh

# This prevents srun from using the same --export option as sbatch. In the case
# of sbatch, we only propagate the IBDSEL_* variables, to avoid any potential
# contamination from the submission environment. Inside the job, however, we
# want our job steps to inherit the job's environment.
export SLURM_EXPORT_ENV=ALL

echo Running on $(hostname)

# Try to prevent "error forking child", "resource temporarily unavailable", etc.
# This error was encountered when we tried to run 272 tasks per KNL node. Using
# ulimit did not fix it; however, it is pointless to run so many tasks on a
# single node, given that other bottlenecks make us plateau around 35 tasks.
ulimit -Sn unlimited
ulimit -Sl unlimited

if [ ${IBDSEL_STARTUP_SLEEP_SECS:-0} -ne 0 ]; then
    export IBDSEL_TRUE_SLEEP_SECS=$(( RANDOM % IBDSEL_STARTUP_SLEEP_SECS ))
    echo "Sleeping for $IBDSEL_TRUE_SLEEP_SECS seconds"
    sleep $IBDSEL_TRUE_SLEEP_SECS
else
    export IBDSEL_TRUE_SLEEP_SECS=0
fi
