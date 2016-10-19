#!/bin/bash

RAW_FILEPATH=$1
BASE_NAME=$(basename $RAW_FILEPATH .root)
OUTPUT_ROOT_DIR=$2/$(date "+%d%m%Y")

echo $OUTPUT_ROOT_DIR

python control_plots_dijet.py $RAW_FILEPATH $OUTPUT_ROOT_DIR
python systematics_breakdown_plots.py $RAW_FILEPATH $OUTPUT_ROOT_DIR
python tagger_pt_efficiency_plots.py $RAW_FILEPATH $OUTPUT_ROOT_DIR
