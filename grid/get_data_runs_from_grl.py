#!/usr/bin/env python2
"""
This script queries AMI to obtain datasets corresponding to data runs in a provided goodrunlist.
It expects path to the GRL xml file, derivation name (e.g. JETM6), production tag (e.g. p2813)
and finally path to output text file, where the list of the datasets will be stored.
*ATTENTION* Since this script uses ami, make sure to setup pyami and valid voms proxy!

This allows the user to avoid manually checking which datasets to use, and also negates the need
to use periodX containers, which contain a lot of redundandt runs, throught which we don't even run.

Additionally, the scripts outputs to console, which datasets are missing on AMI or are available multiple times,
e.g. due to several reconstruction tags available.

Internally, it uses the built-in XML parser xml.etree.ElementTree.
"""

import xml.etree.ElementTree as ET
import sys
import subprocess

if len(sys.argv) != 5:
    print "Usage: {0} <path to goodrunlist.xml> <derivation> <p-tag> <path to output.txt>".format(sys.argv[0])
    print "Example derivation: JETM6"
    print "Example p-tag: p2813"
    sys.exit(1)

with open(sys.argv[4], 'w') as f:
    path_to_xml = sys.argv[1]
    derivation = sys.argv[2]
    ptag = sys.argv[3]

    root = ET.parse(path_to_xml).getroot() # root of the XML file is LumiRangeCollection
    subroot = root.find('NamedLumiRange') # LumiRangeCollection has one element NamedLumiRange

    runs = []

    for LBcollection in subroot.findall('LumiBlockCollection'):
        run = LBcollection.find("Run")
        runs.append(run.text)

    runList = '{'
    for run in runs[:-1]:
        runList += '00' + run + ','
    runList += '00' + runs[-1]
    runList += '}'

    dataCampaign = ''
    if 'data16_13TeV' in path_to_xml:
        dataCampaign = 'data16_13TeV'
    elif 'data15_13TeV' in path_to_xml:
        dataCampaign = 'data15_13TeV'
    else:
        print "ERROR unrecognized data campaign. Expect GRL to contain 'data15_13TeV' or 'data16_13TeV'."
        sys.exit(1)

    amiQuery = dataCampaign + '.' + runList + '.physics_Main%DAOD_' + derivation + '%' + ptag
    print "Searching ami with following query:"
    print amiQuery

    output = subprocess.check_output("ami list datasets " + amiQuery, shell=True)
    f.write(output)
    datasets = output.split('\n')
    for run in runs:
        nOccurences = 0
        for dataset in datasets:
            if run in dataset:
                nOccurences += 1
        if nOccurences == 0:
            print "Run {0} not found among datasets!".format(run)
        elif nOccurences > 1:
            print "Run {0} found multiple times!".format(run)
    f.close()
