#!/usr/bin/env python2

"""
This script submits pileup-reweighting job for a list of datasets imported.
"""

import os,sys,inspect
from subprocess import call

currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0,parentdir)

from samples import samples_AOD

jobPrefix = 'user.zmeadows.prw'
date = '19Oct2016'
cmdbase = r'pathena PileupReweighting/generatePRW_jobOptions.py --forceStaged --inDS='
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
