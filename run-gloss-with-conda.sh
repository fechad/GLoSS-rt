#!/bin/bash

### Conda packages to install ###
# conda install -c conda-forge gdal
# conda install -c conda-forge libgdal
# conda install -c conda-forge libgdal-arrow-parquet
###

[ $# -ne 1 ] && \
    echo "You must provide 1 argument to the script: the path of the conda environment with which GDAL was installed." && \
    exit 1

# CONDA_ENV_PATH=""$HOME"/.conda/envs/"$1""
CONDA_ENV_PATH=$1
echo "Using "$CONDA_ENV_PATH" environment"

export PROJ_LIB=""$CONDA_ENV_PATH"/share/proj"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:"$CONDA_ENV_PATH"/lib"

g++ -std=c++17 src/gloss.cpp -g -I"$CONDA_ENV_PATH"/include/ -I"../empirical_pathloss" -L"$CONDA_ENV_PATH"/lib/ -lgdal && \
./a.out antenna_topology/videotron.csv "data/montreal/montreal.tif" "data/montreal/montreal_MNT.tif"
