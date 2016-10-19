#!/bin/zsh

#PBS -W umask=022
#PBS -j oe

# INPUT_FILE=$1
# OUTPUT_FILE=$2
# SAMPLE_TYPE=$3
DATA_TRIGGER=HLT_j420_a10r_L1J100
LUMINOSITY=4.87734
EVENT_SELECTOR=DIJET_LOOSE

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh

cd /home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/
lsetup 'rcsetup -u'
lsetup rcsetup

histogram-factory -i $INPUT_FILE -o $OUTPUT_FILE -t $DATA_TRIGGER -L $LUMINOSITY -E $EVENT_SELECTOR -p $SAMPLE_TYPE
