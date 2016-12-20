#!/bin/zsh

#PBS -W umask=022
#PBS -j oe

LUMINOSITY=36.470
EVENT_SELECTOR=DIJET_LOOSE
BUILD_DIR=/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology

DATA_TRIGGER=None
if [[ $SAMPLE_TYPE = "data16" ]]
then
    DATA_TRIGGER=HLT_j420_a10r_L1J100
    SAMPLE_TYPE="data"
elif [[ $SAMPLE_TYPE = "data15" ]]
then
    DATA_TRIGGER=HLT_j360_a10r_L1J100
    SAMPLE_TYPE="data"
fi

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

cd /afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/
lsetup 'rcsetup -u'
lsetup rcsetup

job_cmd="histogram-factory -i $INPUT_FILE -o $OUTPUT_FILE -t $DATA_TRIGGER -L $LUMINOSITY -E $EVENT_SELECTOR -p $SAMPLE_TYPE -S"

echo $job_cmd
#eval $job_cmd


