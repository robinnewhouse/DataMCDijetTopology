#!/usr/bin/env python2

"""
This script submits pileup-reweighting job for a list of datasets imported.
"""

import os
from subprocess import call

pythia_dijet_mc15c_AOD = [ # for pileup reweighting
        "mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.merge.AOD.e3569_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.AOD.e3569_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.AOD.e3569_s2608_s2183_r7725_r7676",
        "mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.merge.AOD.e3668_s2608_s2183_r7725_r7676",
        "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.AOD.e3569_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.AOD.e3569_s2576_s2132_r7772_r7676",
        "mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.merge.AOD.e3569_s2576_s2132_r7772_r7676",
        "mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.merge.AOD.e3569_s2576_s2132_r7772_r7676",
        "mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.merge.AOD.e3569_s2608_s2183_r7772_r7676",
        "mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.merge.AOD.e3668_s2608_s2183_r7772_r7676"
        ]

jobPrefix = 'user.zmeadows.prw_file'
date = '18June'
cmdbase = r'pathena generatePRW_jobOptions.py --forceStaged --inDS='
sampleListName = 'pythia_dijet_mc15c_AOD'

samples = ''
for sample in pythia_dijet_mc15c_AOD:
    samples += sample + r'/,'
samples = samples[:-1] # removes the last comma
cmd = cmdbase + r'"' + samples + r'/"' + r' --outDS="' + jobPrefix + \
    '.' + sampleListName + '.' + date + r'/"' + r' --memory="1999"'
print cmd
call(cmd, shell=True)
