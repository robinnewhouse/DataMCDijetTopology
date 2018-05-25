import sys
sys.path.append('lib/')

# import atlas_style

import os
import math
import copy
from sys import argv, exit

from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *
from ROOT import TH1F, TText

MERGED_CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180504_syst/dijet.merged.cp.root"
CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180504_syst/mc15c/merged/"
CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180515_syst/mc15c/merged/"



hist_name = "h_rljet0_pt_comb_BDT_Top"

def print_nom_sys_ratio(dsid, tree_name):
	filepath = CP_ROOT_FILEPATH+str(dsid)+".merged.cp.root"
	# LOADER = DijetLoader(filepath)
	# h_dijet_syst = LOADER.get_hist(["pythia_dijet", "LARGERJET_Weak_JET_Rtrk_Baseline_pT__1up"], hist_name)
	# h_dijet_nominal = LOADER.get_hist(["pythia_dijet", "nominal"], hist_name)

	# print h_dijet_syst.GetEntries()
	# print h_dijet_nominal.GetEntries()

	# ratio = h_dijet_syst.GetEntries()/h_dijet_nominal.GetEntries()
	# print "ratio: " + str(ratio) + "    DSID: " + str(dsid)

	tfile = TFile.Open(filepath, "READ")
	tfile.cd(tree_name)
	tfile.ls()


dsid_list=[
	# 361020,
	# 361021,
	# 361022,
	361023,
	361024,
	361025,
	361026,
	361027,
	361028,
	361029,
	361030,
	361031,
	361032,
	]

for i in dsid_list:
	print_nom_sys_ratio(i, "pythia_dijet")

dsid_list = [
# 426040,
# 426041,
426042,
426043,
426044,
426045,
426046,
426047,
426048,
426049,
426050,
426051,
426052,
]

for i in dsid_list:
	print_nom_sys_ratio(i, "herwig_dijet")