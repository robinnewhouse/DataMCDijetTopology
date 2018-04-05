#!/usr/bin/env python2
import TopExamples.grid
from samples import samples_DXAOD

suffix = 'dijet_20180405_syst'

mc_config = TopExamples.grid.Config()
mc_config.settingsFile  = 'cuts/cuts_boosted_dijet_mc15c.txt'
mc_config.suffix        = 'mc15_'+suffix

data_config = TopExamples.grid.Config()
data_config.settingsFile  = 'cuts/cuts_boosted_dijet_data16.txt'
data_config.suffix         = 'data_'+suffix

for config in [mc_config, data_config]:
    config.code          = 'top-xaod'
    config.gridUsername  = 'rnewhous'
    config.excludedSites = 'US,ANALY_BNL_LONG'
    config.noSubmit      = True
    config.memory        = '1999' # A lot of sites are short on RAM, so we avoid brokerage issues this way
    config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
    config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'


data_sample_names = [ 'data_2015', 'data_2016' ]
mc_sample_names = [
        'pythia_dijet',
        'herwig_dijet',
        'sherpa_dijet',
        'pythia_wjets',
        'pythia_zjets',
        'sherpa_wjets',
        'sherpa_zjets',
        'ttbar_allhad'
        ]

for sample_name in mc_sample_names + data_sample_names:
    TopExamples.grid.Add(sample_name).datasets = samples_DXAOD[sample_name]

TopExamples.grid.submit(mc_config, TopExamples.grid.Samples(mc_sample_names))
TopExamples.grid.submit(data_config, TopExamples.grid.Samples(data_sample_names))
