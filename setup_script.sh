#!/bin/bash
lsetup 'rcsetup Top,2.4.23'

# for simple and MVA taggers
git clone https://:@gitlab.cern.ch:8443/JSSTools/BoostedJetTaggers.git

# ShowerDeconstruction tagger
svn co svn+ssh://svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/trunk ShowerDeconstruction

# for truth matching
git clone https://:@gitlab.cern.ch:8443/JetSubstructure/JetMatchingTools.git

# not all of the necessary dijet background study samples are included in TopDataPreparation xsection lists
rc checkout_pkg $(rc version | grep "TopDataPreparation")
cat data/xsections.txt >> TopDataPreparation/data/XSection-MC15-13TeV.data

# some of the DxAOD's used to not have 'DFCommon' branch(es), but all have plain 'Tight'/'Loose'
# rc checkout_pkg $(rc version | grep "TopObjectSelectionTools")
# sed -i -- 's/DFCommonPhotonsIsEMLoose/Loose/g' TopObjectSelectionTools/Root/PhotonMC15.cxx
# sed -i -- 's/DFCommonPhotonsIsEMTight/Tight/g' TopObjectSelectionTools/Root/PhotonMC15.cxx

# Copy the JetCalibTools to your local area
rc checkout_pkg atlasoff/Reconstruction/Jet/JetCalibTools/tags/JetCalibTools-00-04-74

mkdir -p JetCalibTools/share/JetCalibTools
cd JetCalibTools/share/JetCalibTools
# Copy CalibArea tag you are using
cp -rL /afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/JetCalibTools/CalibArea-00-04-74 .
# Go back to the working dir:
cd ../../../
# Put the configuration files at the right place
cp data/SubjetCalibration/CamKt2LCTopoAreaCalib_361000_weighted_EtaJES_consts.config JetCalibTools/share/JetCalibTools/CalibArea-00-04-74/CalibrationFactors/
cp data/SubjetCalibration/CamKt2LCTopoAreaCalib_JES_HTTmodified.config JetCalibTools/share/JetCalibTools/CalibArea-00-04-74/CalibrationConfigs/


# Checkout TopCPTools and modify calibration config for large-R jets (combined calo-TA mass)
rc checkout_pkg `rc version | grep "TopCPTools"`
patch -p0 -i ./data/comb_mass_config_TopCPTools.patch

rc find_packages
rc compile
