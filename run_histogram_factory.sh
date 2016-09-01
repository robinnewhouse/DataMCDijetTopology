#!/bin/bash

INPUT_FILEPATH=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/inputs/08012016_dijet_ttbar_vjets_pythia_only.txt
OUTPUT_FILEPATH=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/dijet/08012016_pythia_nominal_only_fitting_input_test.root
DATA_TRIGGER=HLT_j420_a10r_L1J100
EVENT_SELECTOR=NO_SELECTION
LUMINOSITY=3.49756
PROCESS_SYSTEMATICS=false

if [ $PROCESS_SYSTEMATICS ]; then
    histogram-factory $INPUT_FILEPATH -o $OUTPUT_FILEPATH -t $DATA_TRIGGER -L $LUMINOSITY -E $EVENT_SELECTOR -S
else
    histogram-factory $INPUT_FILEPATH -o $OUTPUT_FILEPATH -t $DATA_TRIGGER -L $LUMINOSITY -E $EVENT_SELECTOR
fi
