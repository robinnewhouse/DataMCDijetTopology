import sys
sys.path.append('lib/')

import atlas_style

import os
import math
import copy
from sys import argv, exit

from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *
from ROOT import TH1F

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2017/cp.merged.root"
CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/studies/topocluster/topocluster_distribution_histograms.root"
SIGNAL_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/studies/topocluster/zprime_output.root"
LOADER = DijetLoader(CP_ROOT_FILEPATH)
LOADER_SMOOTH = LOADER
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"
DO_SYSTEMATICS_DEFAULT = False

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/BOOST2018"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" , "p_{T} > 450 GeV"]
HTT_DEF_LINES = [ "Trimmed C/A #it{R}=1.5", "Dijet Selection" , "p_{T} > 450 GeV"]
MASS_PLOT_REBIN = 8

#SYSTEMATICS = [ ]
SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM

class PlotTopoclusters(PlotBase):
    def __init__(self, **kwargs):

        super(PlotTopoclusters, self).__init__(
            atlas_mod = "Simulation Internal",
            **kwargs)

        self.h_topo_pt_bkg = get_background_histograms()
        self.h_topo_pt_sig = get_signal_histograms()

        self.canvas.cd()
        colors = [kGreen - 2, kBlue, kBlue - 5, 1,1,1,1,1,1, kRed +1 ]

        for i in [0,1,2,9]:
            h_bkg = self.h_topo_pt_bkg[i]
            h_sig = self.h_topo_pt_sig[i]
            h_bkg.GetYaxis().SetTitle("Normalized units")
            h_sig.GetXaxis().SetTitle("pT fraction of cluster")

            self.leg.AddEntry(self.h_topo_pt_bkg[i], "Cluster " +str(i) + "Dijet")
            self.leg.AddEntry(self.h_topo_pt_bkg[i], "Cluster " +str(i) + "Z'")
            h_bkg.SetMaximum(0.16)
            h_sig.SetMaximum(0.16)
            set_mc_style_line(h_bkg, colors[i], line_width = 4, line_style = 1)
            set_mc_style_line(h_sig, colors[i], line_width = 4, line_style = 2)
            h_bkg.Draw("hist,same")
            h_sig.Draw("hist,same")

        self.name = "topocluster_plots"
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")

        self.name = "topocluster_plots_log"
        self.canvas.SetLogy()
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")

        self.canvas.Clear()

        # TODO igure out how to get both of these to plot

class PlotTopoclustersMean(PlotBase):
    def __init__(self, **kwargs):

        super(PlotTopoclustersMean, self).__init__(
            atlas_loc = [0.2,0.25],
            hide_lumi = True,
            extra_legend_lines = DEF_LINES,
            extra_lines_loc = [0.61,0.80],
            #               x1   y1   x2   y2
            legend_loc = [0.60,0.92,0.83,0.82],
            atlas_mod = "Simulation Internal",
            **kwargs)

        tmp_loader = LOADER
        var_base_name = "h_rljet0_fractional_pt_"
        self.h_topo_pt_bkg = []

        for i in range(0,10):
             self.h_topo_pt_bkg.append(tmp_loader.get_normalized_dijet("pythia_dijet", var_base_name+str(i), normalize_to_unity = True))

   
        # h_mean = TH1F("topocluster_mean_pt","Topocluster Mean pT", 10,0,10)
        h_mean = TH1F("topocluster_mean_pt","Topocluster Mean pT", 10,-0.50,9.5)

        self.x_min = -.5
        self.x_max = 9.5
        self.set_x_axis_bounds(h_mean)   
        h_mean.SetMaximum(.8)
        h_mean.SetMinimum(0)
        # h_mean.GetXaxis().SetRange(1,30)

        for i in range(len(self.h_topo_pt_bkg)):
            h = self.h_topo_pt_bkg[i]
            # print(h.GetMean())
            h_mean.SetBinContent(i+1, h.GetMean())
            h_mean.SetBinError(i+1, h.GetStdDev())


        self.canvas.cd()
        self.title = "Mean Fractional pT of First 10 Topoclusters"
        ### Legend
        self.leg.AddEntry(h_mean, "Pythia8 dijet")

        # set_mc_style_line(h_mean, kGreen-2, line_width = 4)
        set_mc_style_line(h_mean, kViolet -7, line_width = 4)
        h_mean.GetYaxis().SetTitle("<p_{T}> fraction of cluster")
        h_mean.GetXaxis().SetTitle("Leading trimmed large-#it{R} jet cluster index (p_{T} sorted)")
        self.canvas.Clear()

        h_mean.Draw("hist,same")
        h_mean.Draw("e1,same")
        self.name = "topocluster_mean_pt"
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")

        self.name = "topocluster_mean_log_pt"
        h_mean.SetMaximum(1.05)
        h_mean.SetMinimum(3E-3)
        self.canvas.SetLogy()
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")
        self.canvas.Clear()





def get_background_histograms():
    tmp_loader = LOADER
    var_base_name = "h_rljet0_fractional_pt_"
    h_topo_pt_bkg = []

    for i in range(0,10):
         h_topo_pt_bkg.append(tmp_loader.get_normalized_dijet("pythia_dijet", var_base_name+str(i), normalize_to_unity = True))
    return h_topo_pt_bkg

def get_signal_histograms():
    # NOTE: This is incredibly fast and hacky.
    #   The framework for background samples has a quick way to make histograms from ntuples
    #       but I was unable to find a similar event loop for the signal channel framework
    #   I made this simple python looper in order to create the fractional pT histograms on the fly.
    #       There are several better ways to do this, but this is what I was able to make as I learn 
    #       how to use root and pyroot

    # Create histograms
    h_topo_pt_signal = []
    for i in range(0,10):
        h_topo_pt_signal.append(TH1F("h_rljet_fractional_pt_"+str(i),"Topocluster Fractional pT "+str(i), 100, 0.0, 1.0))

    # Open output ntuples and read values
    ntuples_file = TFile.Open( SIGNAL_ROOT_FILEPATH , "READ" )
    nominal = ntuples_file.Get("nominal;2")
    for j in range(nominal.GetEntries()):
        nominal.GetEntry(j)

        for i in range(0,10):
            h_topo_pt_signal[i].Fill(nominal.GetLeaf("rljet_fractional_pt_"+str(i)).GetValue())
    
    for i in range(0,10):
        print h_topo_pt_signal[i].Integral()
        h_topo_pt_signal[i].Scale(1/h_topo_pt_signal[i].Integral())
        print h_topo_pt_signal[i].Integral()

    # Normalize to Unity

    return copy.deepcopy(h_topo_pt_signal)
# print nominal.Show(1)


# var_base_name = "h_rljet0_fractional_pt_"
# h_topo_pt = []



# for i in range(0,10):
topocluster_plots = []


topocluster_plots.append(PlotTopoclusters())
topocluster_plots.append(PlotTopoclustersMean())