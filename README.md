<!--TOC-->

# PACKAGE DESCRIPTION

This package can be used to accomplish the following:

* Produce flat ntuples from JETM8 DxAOD's for data and Monte Carlo, locally or on the grid
* From the flat ntuples, produce sets of raw histograms with any combination of cuts
* From the raw histograms, produce publication quality plots

This package was created primarily for evaluating the performance of jet-substructure-based W/Z/Top tagging in Data/MC. More details specific to this can be found in the [twiki documentation](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TopBosonTagAnalysis2016#Dijet_Data_Study_Background_Effi) for this effort.

With minimal modification/extension this package can perform the same tasks outlined above for any study centered around large-R jets and jet substructure.

# SETUP

See `setup_script.sh`. Due to variations between lxplus/local servers and different setups, it is not recommended to simply run this script, but to follow it one line at a time, making the necessary changes.

# NTUPLE PRODUCTION

### RUNNING LOCAL
NTuples may be produced locally (primarily for testing purposes) with the following command:

` top-xaod grid/cuts/cuts_XXX.txt input.txt `

where `cuts_XX.txt` the configuration file with basic cuts and AnalysisTop settings.  For more information, see [AnalysisTop documentation](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuide).

The second argument, `input.txt`, is a simple text file with list of paths to locally available DxAOD files to run over.

The resulting file, `output.root`, will contain all variables specified/saved by `DataMCbackgroundEventSaver.cxx/h`

* * *

### GRID

Everything related to grid job submission/downloading/merging is located in the `grid/` folder

#### Job Submission

Jobs are submitted using python scripts: `submit_XXX.py`, where XXX = data, pythia, herwig, etc.

Inside each script, there are configuration options which **must be specified on an individual basis**, including:

* *config.gridUsername*: specify your lxplus user name
* *config.suffix*: specify a suffix for job (i.e. 'dijets_18Feb'), this will be appended to the output container of the job
* *config.settingsFile*: specifies the AnalysisTop cuts/settings file used.

The list of all samples can be found in the file samples.py. The samples are grouped by entries in the python dictionaries `samples_XXX` in this file, where XXX = JETM8, EVNT, etc.

#### Retrieving Grid Job Output

The finished output from grid jobs may be downloaded with `download_from_grid.py`. As with the submission scripts, this must be configured on an individual basis.

#### Merging Grid Job Output

The downloaded output from the grid can be merged by DSID with `merge_output.py`. Run 'python merge_output.py --help' for usage details.

* * *

### Custom EventSaver Configuration Options

In the `grid/cuts/cuts_XXX.txt` files, there exist some configuration options that are local to this package and not part of AnalysisTop:

* **RunHTT**:
* **HTTConfigs**:
* **RunShowerDeconstruction**:
* **SaveTrackAssistedMass**:
* **KeepNLargeRJets**:

## PILEUP REWEIGHTING
Pileup reweighting is done automatically as defined in the configuration file `grid/cuts/cuts_XXX.txt`, using the files in `DataMCdijetTools/data/` , which contain PRW metadata for the MC samples used.

In case you need to rerun pileup reweighting (for instance because of different run number, new pileup profile, MC campaign, etc.), use the script `submit_pileupReweight.py` in grid/ folder.  This script needs a list of MC samples. You can use any derivation, as long it is UNSKIMMED (e.g. not EXOT4, JETM8, etc.). It is often easiest to simply use the AOD.

In order to run the script:

1. setup the proper version of Athena Analysis Base: ex: `asetup AthAnalysisBase,2.4.17,here`
2. setup panda and voms-proxy
3. modify the samples list inside `submit_pileupReweight.py`
4. run the script to submit the PRW job


## DESCRIPTION OF CONTENTS

What follows is a basic overview of the important files in this package. For a more detailed understanding, consult the documentation in the code itself.

**DataMCbackgroundEventSaver.h/cxx**:

create new variables to be saved in the output TTree. It also saves information for selected pre-recommendation top and W taggers.
Additionally it saves all the variables of the normal "top:NtupleEventSaver" from AnaysisTop.

**DataMCbackgroundToolsLoader.h/cxx**:
Loads the new selections required. They currently include only "LARGEJET_N". Details can be found in the RunI internal note on JMS/JMR in-situ measurement.

**DataMCbackgroundSelector.h/cxx**:
The meat of the package that creates the final control plots. This is derived from a TSelector.

**InputManager.h/cxx**:
This tool takes the input file given to dmd-control-plots and loads all branches into TChains to be processed by DataMCbackgroundSelector

**WeightTool.h/cxx**:
This tool performs two main tasks:
1. Loading the cross sections, filter efficiencies, and number of events in each MC sample
    as gathered by grid/get_xsection.py and listed in DataMCbackgroundTools/data/sample_weights.txt
2. Veto bugged high-weight events (pulled from DataMCbackgroundTools/data/bugged_events.txt)

**NLargeJetSelector.h/cxx**:
Checks that there are at least N large jets with a specified minimum pT cut.

**TH1Tagged.h/cxx**:
A utility class for making it easy to fill combinations of tagged/vetoed/inclusive plots without much boilerplate code.

**HistoPack.h/cxx**:
A class for packaging together all the necessary histograms. This grouping together is done so that it is possible to create the same
exact 'pack' of plots with different event level cuts, if necessary.

**LocalTools.h/cxx**:
Contains several helper functions for control plot histograms and inputs for fitting frameworks production.
Each function is individualy documented (perhaps only in the header file)

**util/histogram-factory.cxx**:
Produces raw histograms for control plots via the executable `histogram-factory`.

The first argument passed to the `histogram-factory` executable should be a list of input ntuples, each with a corresponding label for grouping
multiple DSID's together: see data/example-histogram-factory-input.txt

Output is in the form of a single .root file with TDirectories for individual samples (i.e. data, pythia_dijet, herwig_wjets, etc).
Inside each of these directories will be separate sub-directories corresponding to each systematic branch processed (if any).

Before using, run `histogram-factory` without arguments or view the util/histogram-factory.cxx source for **important** details regarding command line flags.

#### plotting

**plot_base.py**:
A base class that takes care of repeitive plotting commands that show up in all ATLAS plots. All the plots made by this package come
from classes derived from PlotBase. For an examle of a derived class, see plotting/control_plots.py

**plot_loader.py**:
A utility for conveniently loading nominal/systematic histograms from root files.

**plot_dmd.py**:
A collection of functions/classes that are entirely specific to this package (DataMCbackground).

**plot_systematics.py**:
A class that holds a nominal histogram along with a dictionary of systematics histograms. Furthermore it can compute errors in different ways
(symmetric, asymmetric) and produce histograms with the systematic errors included.

**plot_util.py**:
various re-usable utility functions (ex: histogram styles, set sane default style, etc)

* * *

# EXTENDING THIS PACKAGE

### Adding or modifying a dataset or MC sample
Outlined here are the potential steps that would be taken in order to add/update MC samples:

1. add the relevant set of samples to grid/samples.py
2. add the event generator file list to grid/get_xsection.py and update DataMCdijetTools/data/sample_weights.txt
3. add a new submit script in the grid/ folder
4. add the new files to data/samples_XXX.txt
5. Run locally on a test file from the new sample before submitting grid jobs
6. if necessary, submit new pileup reweighting jobs (if, for example, switching to a new MC campaign)
7. if necessary, create a new AnalysisTop cuts/options file. This could be necessary if, for example, the added samples are
    from a new derivation (not JETM8).

### Creating a new selection/cutflow

### Adding a new variable the ntuples

### Producing a new raw histogram

# CREDIT

Primary developer: Zac Meadows (zmeadows@physics.umass.edu)

This package is based upon Davide Melini's framework for RunII JMS/JMR uncertanties study, with many further modifications and advice from Oliver Majersky.

**Additional input/contributions from**:
Misha Lisovyi
Amal Vaidya
Attilio Picazio
Sam Meehan

