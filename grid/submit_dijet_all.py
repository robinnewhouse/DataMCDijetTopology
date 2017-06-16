#!/usr/bin/env python2
import TopExamples.grid
from samples import samples_DXAOD

mc_config = TopExamples.grid.Config()
mc_config.settingsFile  = 'cuts/cuts_boosted_dijet_mc15c.txt'
mc_config.suffix        = 'dijet_mc15_100617_v3'

data_config = TopExamples.grid.Config()
data_config.settingsFile  = 'cuts/cuts_boosted_dijet_data16.txt'
data_config.suffix        = 'dijet_data_100617_v3'

for config in [mc_config, data_config]:
    config.code          = 'top-xaod'
    config.gridUsername  = 'zmeadows'
    config.excludedSites = 'US,ANALY_BNL_LONG'
    config.noSubmit      = False
    config.memory        = '1999' # A lot of sites are short on RAM, so we avoid brokerage issues this way
    config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
    config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'


data_sample_names = [ 'data_2015_25ns', 'data_2016_25ns' ]
mc_sample_names = [
        'pythia_dijet_mc15c',
        'herwig_dijet_mc15c',
        #'sherpa_dijet_mc15c',
        'V+jets_mc15c',
        'ttbar_allhad_mc15c'
        ]

for sample_name in mc_sample_names + data_sample_names:
    TopExamples.grid.Add(sample_name).datasets = samples_DXAOD[sample_name]

TopExamples.grid.submit(data_config, TopExamples.grid.Samples(data_sample_names))
TopExamples.grid.submit(mc_config, TopExamples.grid.Samples(mc_sample_names))
