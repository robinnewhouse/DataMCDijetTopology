#!/usr/bin/env python2
import TopExamples.grid
import samples

config = TopExamples.grid.Config()

config.code          = 'top-xaod'
config.settingsFile  = 'cuts/cuts_boosted_dijet_mc15c.txt'
config.gridUsername  = 'zmeadows'
config.suffix        = 'wz_plus_jets_0907_v0'
config.excludedSites = ''
config.noSubmit      = False
config.memory        = '1999' # A lot of sites were short on RAM recently, so we avoid brokerage issues this way
config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'

datasetsName = 'V+jets_mc15c'
TopExamples.grid.Add(datasetsName).datasets = samples.samples_JETM8[datasetsName]
TopExamples.grid.submit(config, TopExamples.grid.Samples([datasetsName]))
