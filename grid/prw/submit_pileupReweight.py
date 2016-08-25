#!/usr/bin/env python2

"""
This script submits pileup-reweighting job for a list of datasets imported.
"""

import os
from subprocess import call
from samples import samples_AOD

jobPrefix = 'user.zmeadows.prw'
date = '25Aug2016'
cmdbase = r'pathena generatePRW_jobOptions.py --forceStaged --inDS='
sampleListName = 'TopBosonTag_mc15c'

samples = ''
for sample_name, sample_files in samples_AOD.iteritems():
    for f in sample_files:
        samples += f + r'/,'
samples = samples[:-1] # removes the last comma

cmd = cmdbase + r'"' + samples + r'/"' + r' --outDS="' + jobPrefix + \
    '.' + sampleListName + '.' + date + r'/"' + r' --memory="1999"'

print cmd

call(cmd, shell=True)
