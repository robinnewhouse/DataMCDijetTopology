#!/bin/bash
lsetup 'rcsetup Top,2.4.38'

git clone ssh://git@gitlab.cern.ch:7999/omajersk/NNTaggingTools.git
# cd NNTaggingTools && git checkout tags/2.1 && cd ..

git clone ssh://git@gitlab.cern.ch:7999/omajersk/BDTWTopTagger.git
# cd BDTWTopTagger && git checkout tags/2.1 && cd ..

## Copied from Oliver's l+jets 20.7 setup script
# ShowerDeconstruction tagger
svn co svn+ssh://svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/trunk ShowerDeconstruction
# for simple cut-based aggers and HepTopTagger
echo '> Clone BoostedJetTaggers'
git clone ssh://git@gitlab.cern.ch:7999/rnewhous/BoostedJetTaggers.git
echo '> Checkout JetAnalysisInterfaces'
svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/Interfaces/JetAnalysisInterfaces/tags/JetAnalysisInterfaces-00-00-03 JetAnalysisInterfaces
# May want to change the following repo or include the changes in the next AnalysisTop build
echo '> Clone TopObjectSelectionTools'
git clone ssh://git@gitlab.cern.ch:7999/rnewhous/TopObjectSelectionTools.git

## Added to make MiscTools run. May be taken out later if tool is not necessary
git clone ssh://git@gitlab.cern.ch:7999/atlas-jetetmiss-substructure/JetMatchingTools.git

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
