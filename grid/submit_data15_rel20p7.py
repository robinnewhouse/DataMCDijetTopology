#!/usr/bin/env python2
import TopExamples.grid
import samples

config = TopExamples.grid.Config()
config.code          = 'top-xaod'
config.settingsFile  = 'cuts/cuts_boosted_dijet_data15_rel20p7.txt'
config.gridUsername  = 'zmeadows'
config.suffix        = 'data15_20p7_0722_v0'
config.excludedSites = ''
config.noSubmit      = True
config.memory        = '1999' # A lot of sites were short on RAM recently, so we avoid brokerage issues this way
config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'

datasetsName = 'data_2015_25ns_rel20p7'
TopExamples.grid.Add(datasetsName).datasets = samples.samples_JETM8[datasetsName]
TopExamples.grid.submit(config, TopExamples.grid.Samples([datasetsName]))
