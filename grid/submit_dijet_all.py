#!/usr/bin/env python2
import TopExamples.grid
from samples import *

mc_config = TopExamples.grid.Config()
mc_config.settingsFile  = 'cuts/cuts_boosted_dijet_mc15c.txt'
mc_config.suffix        = 'mc15_10022016_v0'

data_config = TopExamples.grid.Config()
data_config.settingsFile  = 'cuts/cuts_boosted_dijet_data16_rel20p7.txt'
data_config.suffix        = 'data16_10022016_v0'

for config in [mc_config, data_config]:
    config.code          = 'top-xaod'
    config.gridUsername  = 'zmeadows'
    config.excludedSites = ''
    config.noSubmit      = False
    config.memory        = '1999' # A lot of sites were short on RAM recently, so we avoid brokerage issues this way
    config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
    config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'

mc_sample_names = [
        'pythia_dijet_mc15c',
        'herwig_dijet_mc15c',
        'ttbar_allhad_mc15c',
        'V+jets_mc15c'
        ]

data16_sample_names = [ 'data_2016_25ns' ]

for sample_name in mc_sample_names + data16_sample_names:
    TopExamples.grid.Add(sample_name).datasets = samples_JETM8[sample_name]

TopExamples.grid.submit(mc_config, TopExamples.grid.Samples(mc_sample_names))
TopExamples.grid.submit(data_config, TopExamples.grid.Samples(data_sample_names))
