<!--TOC-->

# PACKAGE DESCRIPTION

This package can be used to accomplish the following:

* Produce flat ntuples of calibrated objects from JETM8 DxAOD's for data and Monte Carlo, locally or on the grid
* From the flat ntuples, produce sets of raw histograms with optional offline cuts
* From the raw histograms, produce publication quality plots

This package was created primarily for evaluating the performance of jet-substructure-based W/Z/Top tagging in Data/MC.
More details specific to this can be found in the [twiki documentation](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TopBosonTagAnalysis2016#Dijet_Data_Study_Background_Effi) for this effort.
With minimal modification/extension this package can perform the same tasks outlined above for any study centered around large-R jets and jet substructure.

This package is based on AnalysisTop. If you are not familiar, consider these resources:
* Relase Notes: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisTop
* Quick Start Guide: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuide
* TopRun2WorkshopTutorial2016: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopRun2WorkshopTutorial2016

# SETUP

See `setup_script.sh`. Due to variations between lxplus/local servers and different setups, it is not recommended to simply run this script, but to follow it one line at a time, making the necessary changes.

# NTUPLE PRODUCTION

## RUNNING LOCAL
NTuples may be produced locally (primarily for testing purposes) with the following command:

` top-xaod grid/cuts/cuts_XXX.txt input.txt `

where `cuts_XX.txt` the configuration file with basic cuts and AnalysisTop settings.  For more information, see [AnalysisTop documentation](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuide).

The second argument, `input.txt`, is a simple text file with list of paths to locally available DxAOD files to run over.

The resulting file, `output.root`, will contain all variables specified/saved by `DataMCbackgroundEventSaver.cxx/h`

## RUNNING ON THE GRID

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

## Custom EventSaver Configuration Options

In the `grid/cuts/cuts_XXX.txt` files, there exist some configuration options that are local to this package and not part of AnalysisTop:

* **RunHTT**:
* **HTTConfigs**:
* **RunShowerDeconstruction**:
* **SaveTrackAssistedMass**:
* **KeepNLargeRJets**:

# PILEUP REWEIGHTING
Pileup reweighting is done automatically as defined in the configuration file `grid/cuts/cuts_XXX.txt`, using the files in `DataMCdijetTools/data/` , which contain PRW metadata for the MC samples used.

In case you need to rerun pileup reweighting (for instance because of different run number, new pileup profile, MC campaign, etc.), use the script `submit_pileupReweight.py` in grid/ folder.  This script needs a list of MC samples. You can use any derivation, as long it is UNSKIMMED (e.g. not EXOT4, JETM8, etc.). It is often easiest to simply use the AOD.

In order to run the script:

1. setup the proper version of Athena Analysis Base: ex: `asetup AthAnalysisBase,2.4.17,here`
2. setup panda and voms-proxy
3. modify the samples list inside `submit_pileupReweight.py`
4. run the script to submit the PRW job

# DESCRIPTION OF CONTENTS

What follows is a basic overview of the important files in this package. For a more detailed understanding, consult the documentation in the code itself.

**DataMCbackgroundEventSaver.h/cxx**:

This is the heart of what is compiled into the `top-xaod` executable and the place where all the variables stored in the ntuple are computed/defined.

**DataMCbackgroundToolsLoader.h/cxx**:

Loads the custom event level selectors from this package (ex: LARGEJET_N, GAMMAJET). Any new event level selector will have to be loaded here to.

**DataMCbackgroundSelector.h/cxx**:

This is derived from a TSelector and is used by `histogram-factory` to produce the final control plots.
see: https://root.cern.ch/developing-tselector

**InputManager.h/cxx**:

This tool takes the input file given to `histogram-factory` and loads all the branches (nominal + systematics if requested) from all the files into appropriately grouped TChains. These TChains are then passed to DataMCbackgroundSelector.

**WeightTool.h/cxx**:

This tool performs two main tasks:
1. Loads the cross sections, filter efficiencies, and number of events in each MC sample. These are gathered from `DataMCbackgroundTools/data/sample_weights.txt`, a text file which is produced by `grid/get_xsection.py`.
2. Vetoes bugged high-weight events, pulled from the file `DataMCbackgroundTools/data/bugged_events.txt` which lists them in the form `DSID EVENT_NUMBER`

**NLargeJetSelector.h/cxx**:

Checks that there are at least N large jets in the event with a specified minimum pT cut.

**TH1Tagged.h/cxx**:

A utility class that extends a basic TH1 by making it easy to fill combinations of tagged/vetoed/inclusive plots without much boilerplate code.

**HistoPack.h/cxx**:

A class for packaging together all the necessary histograms. This is where all the raw histograms are defined and their properties set: x_min, x_max, n_bins, etc.

**LocalTools.h/cxx**:

A generic dumping ground for functions that are either re-used or don't fit anywhere else.

**util/histogram-factory.cxx**:

Produces raw histograms for control plots via the executable `histogram-factory`. Before using, run `histogram-factory` without arguments or view the util/histogram-factory.cxx source for **important** details regarding command line flags.

The first argument passed to the `histogram-factory` executable should be a list of input ntuples, each with a corresponding label for grouping multiple DSID's together: see data/example-histogram-factory-input.txt

Output is in the form of a single .root file with TDirectories for individual samples (i.e. data, pythia_dijet, herwig_wjets, etc). Inside each of these directories will be separate sub-directories corresponding to each systematic branch processed (if any).

**plotting/plot_base.py**:

A base class that takes care of repeitive plotting commands that show up in all ATLAS plots. All the plots made by this package come
from classes derived from PlotBase. For an examle of a derived class, see plotting/control_plots.py

**plotting/plot_loader.py**:

A utility for conveniently loading nominal/systematic histograms from root files.

**plotting/plot_dmd.py**:

A collection of functions/classes that are entirely specific to this package (DataMCbackground).

**plotting/plot_systematics.py**:

A class that holds a nominal histogram along with a dictionary of systematics histograms. Furthermore it can compute errors in different ways
(symmetric, asymmetric) and produce histograms with the systematic errors included.

**plotting/plot_util.py**:

various re-usable utility functions (ex: histogram styles, set sane default style, etc)

# EXTENDING THIS PACKAGE

### Adding or modifying a dataset or MC sample
Outlined here are the steps that would be taken in order to add/update MC samples. It may not be necessary to perform all of them.

1. Add the relevant set of JETM8/EVNT sample file names to grid/samples.py
2. Update DataMCdijetTools/data/sample_weights.txt to include xsections/filter efficiencies/nevents for the new samples.
3. Add a new submit script in the grid/ folder
4. if necessary, submit new pileup reweighting jobs (if, for example, switching to a new MC campaign)
5. if necessary, create a new AnalysisTop coniguraiton file in grid/cuts. This could be necessary if:
    * the samples are from a new derivation (i.e. not JETM8)
    * a new online cutflow is needed
    * a new MC campaign is being used
    * etc.
6. Run locally on a test file from the new sample before submitting grid jobs

### Creating a new selection/cutflow

#### online (top-xaod)
If the new selection is too complicated to be represented by [built-in AnalysisTop cuts](http://epweb2.ph.bham.ac.uk/user/head/AnalysisTop-2.3.13/testpage.html), you must create a custom EventSelector for AnalysisTop. see `GammaJetSelector.h/cxx` in this package for an example.

#### offline (histogram-factory)
For any additional offline selection, you must create a custom EventSelector function. See the top of DataMCbackgroundSelector for examples. This new selector must be given a name, and then passed to the histogram-factory executable with a command line flag. ex: `-E MY_FANCY_NEW_SELECTOR`

# CREDIT

Primary developer/contact: Zac Meadows (zmeadows@physics.umass.edu)

This package is based upon Davide Melini's framework for RunII JMS/JMR uncertanties study, with many further modifications and advice from Oliver Majersky.

**Additional input/contributions from**:

Misha Lisovyi

Amal Vaidya

Attilio Picazio

Sam Meehan
