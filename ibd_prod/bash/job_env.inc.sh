# -*- sh-shell: bash -*-

module purge
module load gcc/8.2.0
module load python/3.7-anaconda-2019.07
module load gsl/2.5
source /global/project/projectdirs/dayabay/scratch/mkramer/apps/root-6.18.00-py37/bin/thisroot.sh

export PYTHONPATH=$PWD/python:$PYTHONPATH
