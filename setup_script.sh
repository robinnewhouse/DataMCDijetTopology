#!/bin/bash
lsetup 'rcsetup Top,2.4.32'

git clone https://zmeadows@gitlab.cern.ch/omajersk/NNTaggingTools.git
cd NNTaggingTools && git checkout tags/2.1

git clone https://zmeadows@gitlab.cern.ch/omajersk/BDTWTopTagger.git
cd BDTWTopTagger && git checkout tags/2.1

# ShowerDeconstruction tagger
svn co svn+ssh://svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/trunk ShowerDeconstruction

# not all of the necessary dijet background study samples are included in TopDataPreparation xsection lists
rc checkout_pkg $(rc version | grep "TopDataPreparation")
cat data/xsections.txt >> TopDataPreparation/data/XSection-MC15-13TeV.data

# JetEtMiss recs
rc checkout_pkg atlasoff/Reconstruction/Jet/JetCalibTools/tags/JetCalibTools-00-04-76
rc checkout_pkg atlasoff/Reconstruction/Jet/JetUncertainties/tags/JetUncertainties-00-09-63

mkdir -p JetCalibTools/share/JetCalibTools
cd JetCalibTools/share/JetCalibTools
# Copy CalibArea tag you are using
cp -rL /afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/JetCalibTools/CalibArea-00-04-76 .
# Go back to the working dir:
cd ../../../
# Put the configuration files at the right place
cp data/SubjetCalibration/CamKt2LCTopoAreaCalib_361000_weighted_EtaJES_consts.config JetCalibTools/share/JetCalibTools/CalibArea-00-04-76/CalibrationFactors/
cp data/SubjetCalibration/CamKt2LCTopoAreaCalib_JES_HTTmodified.config JetCalibTools/share/JetCalibTools/CalibArea-00-04-76/CalibrationConfigs/

rc find_packages
rc compile
