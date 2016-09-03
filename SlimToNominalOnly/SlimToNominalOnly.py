#########################################################
# Author : Samuel Meehan <samuel.meehan@cern.ch>
#
# Description : For use to skim Zac's mega ntuples accessing them from eos and storing them back on eos.
#               The idea is that they are big so you never want to copy them locally.
#
# Setup and Running :
# Get eos mounted so that you can access it like a mounted file system without xrootd (just eases the usage)
# $ eosmount eos
#
# Look at the options for running the code
# $ python SlimToNominalOnly.py --help
#
# Example run to slim the mc and data
# $ python SlimToNominalOnly.py --mcdata mc   --eosdir eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20160801 --filelist filelist_mc
# $ python SlimToNominalOnly.py --mcdata data --eosdir eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20160801
#
#########################################################

from ROOT import *
import os
import argparse

parser = argparse.ArgumentParser(description='For picking out only the nominal' , add_help = True)
parser.add_argument('--eosdir',   type=str, default="./",             help='The base path to where the files are stored on eos.  Something like \"eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20160801\"')
parser.add_argument('--mcdata',   type=str, default="def",            help='Flag to say mc or data.  It should be either \"mc\" or \"data\" or the program will not execute')
parser.add_argument('--filelist', type=str, default="filelist_mc",    help='The input file list.  This is only relevant for when running mc.')
args = parser.parse_args()

print "\nARGUMENTS: ================="
print "eosdir   :  ",args.eosdir
print "mcdata   :  ",args.mcdata
print "filelist :  ",args.filelist
print "============================\n"

eosdir   = args.eosdir
mcdata   = args.mcdata
filelist = args.filelist

#check if you are looking at mc or data
#exit if its not one of these
if mcdata!="mc" and mcdata!="data":
    exit()
    
#format the name of the input directory
inputdir = ""
if mcdata=="mc":
    inputdir = "mc15c"
elif mcdata=="data":
    inputdir = "data16"

#format the name of the output directory
outputdir = inputdir+"_nominal/"
inputdir  = inputdir+"/"

#create that directory on eos
os.system("mkdir "+eosdir+"/"+outputdir)

#make the lists of files for skimming
files_to_skim = []
if mcdata=="data":
    files_to_skim.append("data.merged.root")
elif mcdata=="mc":    
    f_files_in = open(filelist,"r+")
    for line in f_files_in.readlines():
    
        file_to_add = line.strip()
        
        print "Adding mc : ",file_to_add

        files_to_skim.append(file_to_add)

#loop over the files and make copies of the relevant trees from the input file to the output file
for file in files_to_skim:

    print file

    fin  = TFile(eosdir+"/"+inputdir+file)
    fin.ls()

    tree_nominal = fin.Get("nominal")
    tree_sumWeights = fin.Get("sumWeights")

    fout = TFile(eosdir+"/"+outputdir+file,"RECREATE")
    fout.cd()
    
    tree_nominal.CloneTree().Write()
    tree_sumWeights.CloneTree().Write()

    fin.Close()
    fout.Close()
    
    
