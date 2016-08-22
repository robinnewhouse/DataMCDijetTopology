#!/bin/bash

RAW_FILEPATH=$1
BASE_NAME=$(basename $RAW_FILEPATH .root)
OUTPUT_ROOT_DIR=$2/$(date "+%d%m%Y")

echo $OUTPUT_ROOT_DIR

python plotting/control_plots.py $RAW_FILEPATH $OUTPUT_ROOT_DIR
python plotting/systematics_breakdown_plots.py $RAW_FILEPATH $OUTPUT_ROOT_DIR
python plotting/tagger_pt_efficiency_plots.py $RAW_FILEPATH $OUTPUT_ROOT_DIR
