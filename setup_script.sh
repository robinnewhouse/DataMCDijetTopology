#!/bin/bash
rcSetup Top,2.4.19

# for simple cut-based taggers and HEPTopTagger
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/BoostedJetTaggers/tags/BoostedJetTaggers-00-00-25 BoostedJetTaggers

# not all of the necessary dijet background study samples are included in TopDataPreparation xsection lists
# this is only needed for letting AnalysisTop compute the b-tag scale factors, since we use xsections/filter efficiencies/nEvents
# from our own custom WeightTool.cxx/h
rc checkout_pkg $(rc version | grep "TopDataPreparation")
cat data/xsections.txt >> TopDataPreparation/data/XSection-MC15-13TeV.data

# some of the DxAOD's used to not have 'DFCommon' branch(es), but all have plain 'Tight'/'Loose'
# rc checkout_pkg $(rc version | grep "TopObjectSelectionTools")
# sed -i -- 's/DFCommonPhotonsIsEMLoose/Loose/g' TopObjectSelectionTools/Root/PhotonMC15.cxx
# sed -i -- 's/DFCommonPhotonsIsEMTight/Tight/g' TopObjectSelectionTools/Root/PhotonMC15.cxx

# Copy the JetCalibTools to your local area
rc checkout_pkg $(rc version | grep "JetCalibTools")

mkdir -p JetCalibTools/share/JetCalibTools
cd JetCalibTools/share/JetCalibTools
# Copy CalibArea tag you are using
cp -r /afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/JetCalibTools/CalibArea-00-04-65 .
# Go back to the working dir:
cd ../../../
# Put the configuration files at the right place
cp data/SubjetCalibration/CamKt2LCTopoAreaCalib_361000_weighted_EtaJES_consts.config JetCalibTools/share/JetCalibTools/CalibArea-00-04-65/CalibrationFactors/
cp data/SubjetCalibration/CamKt2LCTopoAreaCalib_JES_HTTmodified.config JetCalibTools/share/JetCalibTools/CalibArea-00-04-65/CalibrationConfigs/

# ShowerDeconstruction tagger
svn co svn+ssh://svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/trunk ShowerDeconstruction

git clone ssh://git@gitlab.cern.ch:7999/omajersk/JetSubstructureTools.git
cd JetSubstructureTools
git checkout rootcore-ntuples
mv NNTaggingTools ../
cd ..
rm -rf JetSubstructureTools

https://:@gitlab.cern.ch:8443/tnitta/BosonTopTaggingBDT.git

rc find_packages
rc compile
