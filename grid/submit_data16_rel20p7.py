#!/usr/bin/env python2
import TopExamples.grid
from samples import *

config = TopExamples.grid.Config()

config.code          = 'top-xaod'
config.settingsFile  = 'cuts/cuts_boosted_dijet_data16_rel20p7.txt'
config.gridUsername  = 'zmeadows'
config.suffix        = 'data16_dijet_0907_v0'
config.excludedSites = ''
config.noSubmit      = False
config.memory        = '1999' # A lot of sites were short on RAM recently, so we avoid brokerage issues this way
config.mergeType     = 'Default' #'None', 'Default' or 'xAOD'
config.destSE        = '' #This is the default (anywhere), or try e.g. 'UKI-SOUTHGRID-BHAM-HEP_LOCALGROUPDISK'

datasetsName = 'data_2016_25ns'
TopExamples.grid.Add(datasetsName).datasets = samples_JETM8[datasetsName]
TopExamples.grid.submit(config, TopExamples.grid.Samples([datasetsName]))
