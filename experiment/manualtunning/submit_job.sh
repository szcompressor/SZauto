#!/bin/bash

mkdir -p run
cd run
sbatch ../hurricane.sh
sbatch ../miranda.sh
sbatch ../noaa.sh
sbatch ../nyx.sh
sbatch ../qmcpack.sh
sbatch ../qmcpack8h.sh
sbatch ../scale.sh
sbatch ../snp.sh
