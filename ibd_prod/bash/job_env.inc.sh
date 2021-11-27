# See /global/cfs/cdirs/dayabay/scratch/mkramer/build/NOTES.org for notes on how
# ROOT and conda were set up on Cori.

module purge
module load gcc/10.1.0
module load cmake/3.21.3
export CC=gcc CXX=g++ LD=ld

myconda=/global/cfs/projectdirs/dayabay/scratch/mkramer/apps/miniconda3
export PATH=$myconda/bin:$PATH
source activate $myconda/envs/ibdsel2 # /doc/conda/ibdsel2.txt
unset myconda

# CONDA_PREFIX is $myconda/envs/ibdsel2
source $CONDA_PREFIX/bin/thisroot.sh

export CFLAGS="-I$CONDA_PREFIX/include $CFLAGS"
export LIBRARY_PATH=$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$CONDA_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH

export IBDSEL_HOME=$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)

export PYTHONPATH=$IBDSEL_HOME/ibd_prod/python:$PYTHONPATH
export LD_LIBRARY_PATH=$IBDSEL_HOME/selector/_build:$LD_LIBRARY_PATH

export NO_JUPYROOT=1            # for jupyter console; see NOTES.org
