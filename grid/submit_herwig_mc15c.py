#!/usr/bin/env python2
import TopExamples.grid
import samples

config = TopExamples.grid.Config()

config.code          = 'top-xaod'
config.settingsFile  = 'cuts/cuts_boosted_dijet_mc15c.txt'
config.gridUsername  = 'zmeadows'
config.suffix        = 'herw_dijet_0801_v1'
config.excludedSites = ''
config.noSubmit      = False
config.memory        = '1999' # A lot of sites were short on RAM recently, so we avoid brokerage issues this way
config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'

datasetsName = 'herwig_dijet_mc15c_JETM8'
TopExamples.grid.Add(datasetsName).datasets = samples.samples[datasetsName]
TopExamples.grid.submit(config, TopExamples.grid.Samples([datasetsName]))
