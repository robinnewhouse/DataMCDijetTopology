#!/bin/zsh
set -e

#DRY_RUN=true

DESCRIPTION="02072017_gridjobs_withQwSplit23Sys"

INPUT_DIR=/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20170207
OUTPUT_DIR_BASE=/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/dijet
HISTOGRAM_FACTORY_JOB_SCRIPT=/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/Batch/histogram_factory_job.sh
SUBMIT_DIR=/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/Batch/log

MY_DATE=$(date "+%d-%m-%Y__%H:%M:%S")
OUTPUT_DIR="$OUTPUT_DIR_BASE/${MY_DATE}__${DESCRIPTION}/"

#copy directory structure from INPUT_DIR to OUTPUT_DIR
mkdir -p $OUTPUT_DIR
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
        elif [[ $1 -ge 426131 && $1 -le 426142 ]] then echo "sherpa_dijet"
        else
            echo "$0: ERROR: unknown DSID ($1) encountered!" >&2 && exit 1
        fi
}


echo "SUBMITTING JOBS..."
echo ""
cd $SUBMIT_DIR
for INPUT_FILE in $(find $INPUT_DIR -type f -name '*root*'); do
        DSID=${INPUT_FILE#*user.zmeadows.}
        DSID=${DSID%%.*}

        OUTPUT_FILE=$OUTPUT_DIR${INPUT_FILE#$INPUT_DIR/}
        OUTPUT_FILE=${OUTPUT_FILE%.root*}.cp.root

        if [[ $INPUT_FILE == *"physics_Main"* ]]
        then
            if [[ $INPUT_FILE == *"period"* ]]
            then
                SAMPLE_TYPE="data16"
            else
                SAMPLE_TYPE="data15"
            fi
        else
            SAMPLE_TYPE=$(get_sample_type DSID)
        fi

        INPUT_FILE=${INPUT_FILE#/titan/*}
        INPUT_FILE="root://eosatlas/$INPUT_FILE"

        echo "INPUT FILE: $INPUT_FILE"
        echo "OUTPUT FILE: $OUTPUT_FILE"
        echo "DSID: $DSID"
        echo "SAMPLE TYPE: $SAMPLE_TYPE"

        job_cmd="INPUT_FILE=$INPUT_FILE OUTPUT_FILE=$OUTPUT_FILE SAMPLE_TYPE=$SAMPLE_TYPE $HISTOGRAM_FACTORY_JOB_SCRIPT"

        if [ $DRY_RUN ]
        then
            echo "COMMAND: $job_cmd"
        else
            eval "bsub -R \"pool>3000\" -q 8nh \"$job_cmd\""
        fi
        echo ""
done

echo ""
echo "DONE SUBMITTING... CROSS YOUR FINGERS!"
