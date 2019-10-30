# IbdSel

Daya Bay IBD selection

In order to checkout out the required modules, clone with the following commands:

```sh
git clone git@github.com:maxfl/IbdSel.git --recursive
```

# Requirements

- cmake
- gcc, C++14 at least
- ROOT
- Python3 or Python2
- Python modules: pygraphviz, yaml
- Submodules: SelectorFramework, dagflow

Python requirements may be installed via the following command:
```sh
pip install --user -r requirements.txt
```
# Environment variables

The following environment variables should be set: `ROOTSYS`, `LD_LIBRARY_PATH`, `PYTHONPATH`.

Here is an example `.envrc` file for https://github.com/direnv/direnv:
```sh
export ROOTSYS=/data/work/soft/root-6.18.04_install_p3
PATH_add $ROOTSYS/bin $PWD/macro

PATH_add LD_LIBRARY_PATH $ROOTSYS/lib/root $PWD/build
PATH_add PYTHONPATH $ROOTSYS/lib $ROOTSYS/lib/root $PWD/pylib
```

# Example scripts to run:

Using simple python macro:
```sh
stage1_main.py inputs/dayabay_p19a_test/recon.Neutrino.0072512.Physics.EH2-Merged.P19A-I._0001.root  -o output/test_stage1_py.root -s 2
```

Using python macro:
```sh
analysis-p19a.py -f inputs/dayabay_p19a_test/recon.Neutrino.0072508.Physics.EH1-Merged.P19A-I._0001.root -o output/ stage1 -g output/anap19a_graph.pdf -vv
```

Use `--help` option to print available parameters.
