module purge
module load gcc/8.2.0
module load python/3.7-anaconda-2019.07
module load gsl/2.5

source /global/cfs/projectdirs/dayabay/scratch/mkramer/apps/root-6.18.00-py37/bin/thisroot.sh

eval "$(conda shell.bash hook)"
# /doc/conda/ibdsel1.txt
conda activate /global/cfs/projectdirs/dayabay/scratch/mkramer/apps/miniconda3/envs/ibdsel1

export IBDSEL_HOME=$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)

export PYTHONPATH=$IBDSEL_HOME/ibd_prod/python:$PYTHONPATH
export LD_LIBRARY_PATH=$IBDSEL_HOME/selector/_build:$LD_LIBRARY_PATH

export NO_JUPYROOT=1            # for jupyter console; see NOTES.org
