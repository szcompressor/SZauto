#!/bin/bash

mkdir -p run
cd run
sbatch ../hurricane.sh
sbatch ../miranda.sh
sbatch ../noaa.sh
sbatch ../nyx.sh
sbatch ../qmc.sh
sbatch ../scale.sh
sbatch ../snp.sh
