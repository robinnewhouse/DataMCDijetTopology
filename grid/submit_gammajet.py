#!/usr/bin/env python2
import TopExamples.grid
from samples import samples_JETM6

mc_config = TopExamples.grid.Config()
mc_config.settingsFile  = 'cuts/cuts_gamma_jet_mc15c.txt'
mc_config.suffix        = 'gammajet_mc15_v3.1'

data_config = TopExamples.grid.Config()
data_config.settingsFile  = 'cuts/cuts_gamma_jet_data16.txt'
data_config.suffix        = 'gammajet_data_v3'

for config in [mc_config, data_config]:
    config.code          = 'top-xaod'
    config.gridUsername  = 'zmeadows'
    config.excludedSites = 'US,ANALY_BNL_LONG'
    config.noSubmit      = False
    config.memory        = '1999' # A lot of sites are short on RAM, so we avoid brokerage issues this way
    config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
    config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'

data_sample_names = [ 'data_2016_25ns_p2950', 'data_2015_25ns_p2950' ]

mc_sample_names = [
    # 'pythia_gammajet_mc15c',
    # 'sherpa_gammajet_mc15c',
    # 'ttbar_gamma_mc15c',
    # 'wz_gamma_mc15c',
    'pythia_dijet_mc15c_p2949'
    ]

for sample_name in mc_sample_names + data_sample_names:
    TopExamples.grid.Add(sample_name).datasets = samples_JETM6[sample_name]

#TopExamples.grid.submit(data_config, TopExamples.grid.Samples(data_sample_names))
TopExamples.grid.submit(mc_config, TopExamples.grid.Samples(mc_sample_names))