#!/bin/bash
lsetup 'rcsetup Top,2.4.42'

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

# Checkout fork of JetUncertainties package with support for pT resolution uncertainty for fat jets
# at some point this will be added to the official SVN repo
git clone ssh://git@gitlab.cern.ch:7999/omajersk/JetUncertainties.git

# checkout TopCPTools where we have to add support for these uncertainties
rc checkout_pkg `rc version | grep TopCPTools`
patch -p0 -i ./data/initialSetup/TopCPTools_addFatJetJER.patch


rc find_packages
rc compile
