# See /global/cfs/cdirs/dayabay/scratch/mkramer/build/NOTES.org for notes on how
# python, ROOT, and the ibdsel3 virtualenv were set up on Cori.

module purge
module load gcc/10.1.0
module load cmake/3.21.3
export CC=gcc CXX=g++ LD=ld

openssldir=/global/cfs/cdirs/dune/users/mkramer/apps/openssl-1.1.1l
pydir=/global/cfs/cdirs/dune/users/mkramer/apps/mypy310
rootdir=/global/cfs/cdirs/dayabay/scratch/mkramer/apps/root-6.24.06-mypy310
venvdir=/global/cfs/cdirs/dayabay/scratch/mkramer/venvs/ibdsel3

export PATH=$openssldir/bin:$PATH
export PKG_CONFIG_PATH=$openssldir/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$openssldir/lib:$LD_LIBRARY_PATH

export PATH=$pydir/bin:$PATH
export PKG_CONFIG_PATH=$pydir/lib/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$pydir/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$pydir/lib/python3.10/site-packages:$pydir/lib/python3.10:$PYTHONPATH

source $rootdir/bin/thisroot.sh

source $venvdir/bin/activate
export PYTHONPATH=$venvdir/lib/python3.10/site-packages:$PYTHONPATH

export IBDSEL_HOME=$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)

export PYTHONPATH=$IBDSEL_HOME/ibd_prod/python:$PYTHONPATH
# export LD_LIBRARY_PATH=$IBDSEL_HOME/selector/_build:$LD_LIBRARY_PATH

export LIBRARY_PATH=$LD_LIBRARY_PATH
export NO_JUPYROOT=1            # for jupyter console; see NOTES.org

unset openssldir pydir rootdir venvdir
