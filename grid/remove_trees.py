import os
import sys
import glob
import argparse
from ROOT import *

input_dir = sys.argv[1]

if input_dir[-1] != '/':
    input_dir += '/'

folders = glob.glob(input_dir + '*user.*')
folders.sort()

for i in range(len(folders)):
    if folders[i][-1] != '/':
        folders[i] += '/'

def delete_trees(mc_dir):
    input_files = glob.glob(mc_dir + '*.root*')
    for f in input_files:
        print f
        tfile = TFile(f, "update")
        tfile.cd()
        gDirectory.Delete("JET_NP*;*")
        gDirectory.Delete("JET_JER*;*")
        gDirectory.Close()

for fff in folders:
    delete_trees(fff)

