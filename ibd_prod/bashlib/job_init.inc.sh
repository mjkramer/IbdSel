# -*- sh-shell: bash -*-

source bashlib/job_env.inc.sh
source bashlib/maybe_srun.inc.sh

echo Running on $(hostname)

# sleep $(( RANDOM % 200 ))       # so they don't all mob the lockfile at once on startup (400 jobs, staggered)
