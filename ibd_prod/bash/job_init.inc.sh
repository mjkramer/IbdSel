source bash/job_env.inc.sh
source bash/maybe_srun.inc.sh

echo Running on $(hostname)

# Try to prevent "error forking child", "resource temporarily unavailable", etc.
# This error was encountered when we tried to run 272 tasks per KNL node. Using
# ulimit did not fix it; however, it is pointless to run so many tasks on a
# single node, given that other bottlenecks make us plateau around 35 tasks.
ulimit -Sn unlimited
ulimit -Sl unlimited
