#!/bin/bash

INPUT_FILEPATH=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/all_inputs_rel20p7_data16_mc15c.txt
OUTPUT_FILEPATH=raw_histograms.root
DATA_TRIGGER=HLT_j420_a10r_L1J100
EVENT_SELECTOR=NO_SELECTION
LUMINOSITY=3.20905
PROCESS_SYSTEMATICS=true

if [ $PROCESS_SYSTEMATICS ]; then
    histogram-factory $INPUT_FILEPATH -o $OUTPUT_FILEPATH -t $DATA_TRIGGER -L $LUMINOSITY -E $EVENT_SELECTOR -S
else
    histogram-factory $INPUT_FILEPATH -o $OUTPUT_FILEPATH -t $DATA_TRIGGER -L $LUMINOSITY -E $EVENT_SELECTOR
fi
