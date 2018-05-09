#!/bin/env python2
from sys import argv, exit
from ROOT import TFile, TDirectory, gDirectory, gROOT

# This script is used to make "fake" down-shift uncertainties for one-sided
# uncertainties. Because of the symmetrization approach in the analysis plotting
# code, we can just set the down-shift to nominal (i.e. zero uncertainty). This
# is a dirty hack overcoming limitation of the plotting framework, and *should
# be fixed in the long-term!*


def getMCdirs(inputPath):
    inputFile = TFile(inputPath, 'read')
    MClist = []
    keys = inputFile.GetListOfKeys()
    for key in [keys.At(i) for i in range(keys.GetEntries())]:
        if gROOT.GetClass(key.GetClassName()).InheritsFrom("TDirectory"):
            if not 'data' in key.GetName():
                MClist.append(key.GetName())
    inputFile.Close()
    return MClist

def getDirsToFix(inputPath, MCdir):
    inputFile = TFile(inputPath, 'read')
    tdir = inputFile.Get(MCdir)
    keys = tdir.GetListOfKeys()
    dirsToFix = []
    for key in [keys.At(i) for i in range(keys.GetEntries())]:
        if gROOT.GetClass(key.GetClassName()).InheritsFrom("TDirectory"):
            if key.GetName().upper()[-2:] == "UP":
                if "__1up" in key.GetName():
                    downShiftDirName = key.GetName()[:-5] + '__1down'
                elif "_UP" in key.GetName():
                    downShiftDirName = key.GetName()[:-3] + '_DOWN'
                else:
                    downShiftDirName = ""
                if not tdir.Get(downShiftDirName):
                    print "TDirectory {0} does not have down shift partner".format(key.GetName())
                    dirsToFix.append(downShiftDirName)
    inputFile.Close()
    return dirsToFix


def addZeroDownShiftUncert(inputPath, MCdir, systFolderName):
    inputFile = TFile(inputPath, 'update')

    if not inputFile:
        print 'Error: Cannot open input file! Exiting.'
        exit(1)

    # copy over each TDirectory from the weight-systematic root file
    # into the output destination
    print "Copying MC TDirectory {0}/nominal".format(MCdir)
    print "Into subdir \'{0}\' of MC TDirectory {0}".format(systFolderName, MCdir)
    tdir = inputFile.Get(MCdir+'/nominal')
    if not tdir:
        print "Error: Cannot find nominal directory inside {0}. Exiting.".format(MCdir)
        exit(1)
    tdir.ReadAll()

    if inputFile.Get(dirName+'/'+systFolderName) != None:
        print "WARNING: TDirectory {0} already exists. Skipping.".format(dirName+'/'+systFolderName)
        return
    topDir = inputFile.mkdir(MCdir+'/'+systFolderName)
    if not topDir:
        print "Error: Could not create TDirectory {0}/{1} in file {2}. Exiting."\
            .format(MCdir, systFolderName, inputPath)
        exit(1)
    inputFile.cd(MCdir+'/'+systFolderName)
    tdir.GetList().Write()

    inputFile.Close()


if __name__ == "__main__":
    if len(argv) != 2:
        print 'Usage: {0} <path to histogram .root file>'
        print 'This script creates zero-magnitude down-shift uncertainty directories for\
plotting script. This should be considered a work-around for plotting framework.'
        print 'It will copy nominal MC directories from nominal to newly created\
down-shift uncertainty directories.'
        exit(1)

    inputPath=argv[1]

    MClist = getMCdirs(inputPath)

    print "Going to check following TDirectories:"
    print MClist

    for MCdir in MClist:
        print ""
        print "Getting list of down-shift systematic TDirs to add for {0}".format(MCdir)
        dirNames = getDirsToFix(inputPath, MCdir)
        print "Going to add down-shift for the directories above."
        print dirNames
        for dirName in dirNames:
            addZeroDownShiftUncert(inputPath, MCdir, dirName)
