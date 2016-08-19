This README is a supplement to the [twiki documentation] (https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TopBosonTagAnalysis2016#Dijet_Data_Study_Background_Effi)

## SETUP/COMPILATION

See 'setup_script.sh'. Due to variations between lxplus/local servers and different setups,
it is not recommended to simply run this script, but follow it one line at a time, making the necessary changes.

## GENERAL NOTES

For clarity, any files/executables/classes with a name beginning with DMD/dmd ("Data Mc Dijets") are specific to this package.

## RUNNING LOCALLY
NTuples may be produced locally with the following command:
top-xaod grid/cuts/cuts_boosted_dijet_XXX.txt input.txt

The first argument is the configuration file with basic cuts -- common for both data and MC
(see AnalysisTop documentation for more info, for instance here: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuide).

The second argument is a file with list of paths to locally available dxAOD's.

## GRID

#### SUBMITTING JOBS

Everything needed for running jobs on the grid is in the grid/ folder.
Jobs are submitted using python scripts:

submit_XXX.py

where XXX = data, pythia, herwig, etc

Inside each script, there are configuration options which *must be specified on an individual basis*, including:
config.gridUsername -- specify your lxplus user name
config.suffix       -- specify a suffix for job (i.e. 'dijets_18Feb'), this will be appended to the output
                       container of the job
config.settingsFile -- specifies the AnalysisTop cuts/settings file used.

The list of all samples can be found in the file samples.py.
The samples are grouped by entries in the python dictionary 'samples' inside this file.

#### DOWNLOAD GRID OUTPUT

The finished output from grid jobs may be downloaded with grid/download_from_grid.py.
Like the submit scripts, this must be configured on an individual basis.

#### MERGING GRID OUTPUT

The downloaded output from the grid can be merged by DSID with grid/merge_output.py
run 'python merge_output.py --help' for usage details

## PILEUP REWEIGHTING
Pileup reweighting is done automatically as defined in the configuration file grid/cuts/cuts_boosted_dijet_XXX.txt,
using the files in DataMCdijetTools/data/

These files contain PRW metadata for the MC samples used.

In case you need to rerun pileup reweighting (for instance because of different run number, new pileup profile, MC campaign, etc.),
use the script 'submit_pileupReweight.py' in grid/ folder.

This script needs a list of MC samples. You can use any derivation, as long it is UNSKIMMED (e.g. not EXOT4, JETM8, etc.).
It is probably easiest to simply use the AOD.

In order to run the script:
1. setup Athena Analysis Base: asetup AthAnalysisBase,2.4.17,here
2. setup panda and voms-proxy
3. run the script 'submit_pileupReweight.py'

## ADDING/UPDATING A SAMPLE
Outlined here are the potential steps that would be taken in order to add/update MC samples:

1. add the relevant set of samples to grid/samples.py
2. add the event generator file list to grid/get_xsection.py and update DataMCdijetTools/data/sample_weights.txt
3. add a new submit script in the grid/ folder
4. add the new files to data/samples_XXX.txt
5. Run locally on a test file from the new sample before submitting grid jobs
6. if necessary, submit new pileup reweighting jobs (if, for example, switching to a new MC campaign)
7. if necessary, create a new AnalysisTop cuts/options file. This could be necessary if, for example, the added samples are
    from a new derivation (not JETM8).

## DESCRIPTION OF CONTENTS

#### DataMCdijetsTools/Root

DataMCljetsEventSaver.cxx:
create new variables to be saved in the output TTree. It also saves information for selected pre-recommendation top and W taggers.
Additionally it saves all the variables of the normal "top:NtupleEventSaver" from AnaysisTop.

DataMCljetsToolsLoader.cxx
Loads the new selections required. They currently include only "LARGEJET_N". Details can be found in the RunI internal note on JMS/JMR in-situ measurement.

NLargeJetSelector.cxx:
Checks that there are at least N large jets with a specified minimum pT cut.

LocalTools.cxx:
Contains several helper functions for control plot histograms and inputs for fitting frameworks production.
Each function is individualy documented (perhaps only in the header file)

DMDHistoPack.cxx:
A class for packaging together all the necessary histograms. This grouping together is done so that it is possible to create the same
exact 'pack' of plots with different event level cuts, if necessary.

InputManager.cxx:
This tool takes the input file given to dmd-control-plots and loads all branches into TChains to be processed by DMDSelector

DMDHistoTagged.cxx:
A utility class for making it easy to fill combinations of tagged/vetoed/inclusive plots without much boilerplate code.

DMDSelector.cxx:
The meat of the package that creates the final control plots. This is derived from a TSelector.

WeightTool.cxx:
This tool performs two main tasks:
1. Loading the cross sections, filter efficiencies, and number of events in each MC sample
    as gathered by grid/get_xsection.py and listed in DataMCdijetsTools/data/sample_weights.txt
2. Veto bugged high-weight events (pulled from DataMCdijetsTools/data/bugged_events.txt)

#### DataMCdijetsTools/util

dmd-control-plots.cxx:
Produces raw histograms for control plots.
The first argument to the 'dmd-control-plots' executable is a list of files with a corresponding label for each file for grouping
multiple DSID's together (example: pythia/herwig dijet JZXW slices): see data/example_dmd-control-plots_input.txt
Output is in the form of a single .root file with TDirectories for individual samples (i.e. data, pythia_dijet, herwig_wjets, etc).
Inside each of these directories will be separate sub-directories corresponding to each systematic branch processed (if any).
Run dmd-control-plots without arguments or view the util/dmd-control-plots.cxx source for more details regarding command line flags.

#### plotting

What follows are only basic descriptions, consult the documentation in each file individually for more information.

plot_base.py:
A base class that takes care of repeitive plotting commands that show up in all ATLAS plots. All the plots made by this package come
from classes derived from PlotBase. For an examle of a derived class, see plotting/control_plots.py

plot_loader.py:
A utility for conveniently loading nominal/systematic histograms from root files.

plot_dmd.py:
A collection of functions/classes that are entirely specific to this package (DataMCdijets).

plot_systematics.py:
A class that holds a nominal histogram along with a dictionary of systematics histograms. Furthermore it can compute errors in different ways
(symmetric, asymmetric) and produce histograms with the systematic errors included.

plot_util.py:
various re-usable utility functions (ex: histogram styles, set sane default style, etc)

