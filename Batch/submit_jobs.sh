#!/bin/zsh

set -e

DESCRIPTION="INIT_TEST_NO_SYSTEMATICS_v0"

INPUT_DIR=/titan/atlas/common/zmeadows/TopBosonTagging/dijet/09072016
OUTPUT_DIR_BASE=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/dijet

MY_DATE=$(date "+%d-%m-%Y__%H:%M:%S")
OUTPUT_DIR="$OUTPUT_DIR_BASE/${MY_DATE}_${DESCRIPTION}/"
mkdir -p $OUTPUT_DIR

#copy directory structure from INPUT_DIR to OUTPUT_DIR
cd $INPUT_DIR
find . -type d -exec mkdir -p -- $OUTPUT_DIR{} \;

function get_sample_type {
        if [[ $1 -ge 303722 && $1 -le 303726 ]] then echo "ttbar_allhad"
        elif [[ $1 -ge 304307 && $1 -le 304309 ]] then echo "sherpa_wjets"
        elif [[ $1 -ge 304623 && $1 -le 304627 ]] then echo "pythia_wjets"
        elif [[ $1 -ge 304628 && $1 -le 304632 ]] then echo "pythia_zjets"
        elif [[ $1 -ge 304673 && $1 -le 304677 ]] then echo "herwig_wjets"
        elif [[ $1 -ge 304678 && $1 -le 304682 ]] then echo "herwig_zjets"
        elif [[ $1 -ge 304707 && $1 -le 304709 ]] then echo "sherpa_zjets"
        elif [[ $1 -ge 361020 && $1 -le 361032 ]] then echo "pythia_dijet"
        elif [[ $1 -ge 426040 && $1 -le 426052 ]] then echo "herwig_dijet"
        #elif [[ $1 -ge 304307 && $1 -le 304309 ]] then echo "sherpa_dijet"
        else
            echo "$0: ERROR: unknown DSID ($1) encountered!" >&2 && exit 1
        fi
}

HISTOGRAM_FACTORY_JOB_SCRIPT=/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/Batch/histogram_factory_job.sh

cd /home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/Batch/run
for INPUT_FILE in $(find $INPUT_DIR -type f -name '*root*'); do
        DSID=${$(basename $INPUT_FILE)%.merged*}

        OUTPUT_FILE=$OUTPUT_DIR${INPUT_FILE#$INPUT_DIR/}
        OUTPUT_FILE=${OUTPUT_FILE%.root*}.cp.root

        if [[ $DSID == *"data"* ]]
        then SAMPLE_TYPE="data"
        else SAMPLE_TYPE=$(get_sample_type DSID)
        fi

        INPUT_FILE=${INPUT_FILE#/titan/*}
        INPUT_FILE="root://titan.physics.umass.edu//$INPUT_FILE"

        echo "INPUT: $INPUT_FILE"
        echo "OUTPUT_FILE: $OUTPUT_FILE"

        qsub -l cput=08:00:00 -v INPUT_FILE=$INPUT_FILE,OUTPUT_FILE=$OUTPUT_FILE,SAMPLE_TYPE=$SAMPLE_TYPE $HISTOGRAM_FACTORY_JOB_SCRIPT
done
