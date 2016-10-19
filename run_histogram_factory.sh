#!/bin/bash

INPUT_FILEPATH=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/inputs/09072016_all_inputs.txt
OUTPUT_FILEPATH_ROOT=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/dijet
DATA_TRIGGER=HLT_j420_a10r_L1J100
LUMINOSITY=4.87734

declare -a OUTPUT_FILEPATHS=('pT_GT450GeV' 'pT_GT500GeV')
declare -a EVENT_SELECTORS=('DIJET_LOOSE' 'DIJET_TIGHT')

for ((i=0; i < ${#OUTPUT_FILEPATHS[@]}; i++))
do
    histogram-factory $INPUT_FILEPATH -o $OUTPUT_FILEPATH_ROOT/09072016_${OUTPUT_FILEPATHS[$i]}.nominal.root -t $DATA_TRIGGER -L $LUMINOSITY -E ${EVENT_SELECTORS[$i]}
done

