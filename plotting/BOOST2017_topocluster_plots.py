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
from ROOT import TH1F, TText, gPad, TLine

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

# DIJET_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2017/cp.merged.root"
# DIJET_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/studies/topocluster/topocluster_distribution_histograms.root"
# BASE_DIR = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/studies/topocluster/gt150/"
BASE_DIR = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/studies/topocluster/gt450/"
DIJET_ROOT_FILEPATH = BASE_DIR + "output.cp.nominal.root"
ZPRIME_ROOT_FILEPATH = BASE_DIR + "zprime_cluster_study_4000/output.root"
LOADER = DijetLoader(DIJET_ROOT_FILEPATH)
LOADER_SMOOTH = LOADER
ROOT_OUTPUT_DIR = os.path.dirname(DIJET_ROOT_FILEPATH) + "/plots_4000"
DO_SYSTEMATICS_DEFAULT = False
CLUSTER_INDEX = [0,1,2,9]
N_CLUSTERS_MEAN = 20
Y_MAX = 0.16

OUTPUT_DIR = ROOT_OUTPUT_DIR
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

DEF_LINES = [ "#scale[0.85]{Trimmed anti-#it{k}_{t} #it{R}=1.0 jets} ", 
              # "#scale[0.85]{Light Quark Jet Sample :}" , 
              "#scale[0.85]{   #it{p}_{T} > 450 GeV}", 
              # "#scale[0.85]{Top Quark Jet Sample:}" , 
              # "#scale[0.85]{   #it{p}_{T} > 150 GeV}", 
              ]
MASS_PLOT_REBIN = 8

#SYSTEMATICS = [ ]
SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM

class PlotTopoclusters(PlotBase):
    def __init__(self, **kwargs):

        super(PlotTopoclusters, self).__init__(
            atlas_loc = [0.21,0.89],
            hide_lumi = True,
            extra_legend_lines = DEF_LINES,
            extra_lines_loc = [0.61,0.65],
            #               x1   y1   x2   y2
            legend_loc = [0.60,0.850,1.15,0.69],
            atlas_mod = "Simulation Internal",
            tex_size_mod = 0.9,
            height = 600,
            width = 600,
            **kwargs)

        # Load and validate histograms
        sig = False
        bkg = False
        try:
            self.h_topo_pt_bkg = get_background_histograms()
            if self.h_topo_pt_bkg[0]: bkg = True
        except Exception as e:
            pass

        try:
            self.h_topo_pt_sig = get_signal_histograms()
            if self.h_topo_pt_sig[0]: sig = True
        except Exception as e:
            pass

        self.canvas.cd()
        colors = [kGreen - 2, kBlue, kBlue - 5, 1,1,1,1,1,1, kRed +1 ]

        for i in [0,1,2,9]:
            if bkg: h_bkg = self.h_topo_pt_bkg[i]
            if sig: h_sig = self.h_topo_pt_sig[i]
            if bkg: 
                h_bkg.GetXaxis().SetTitle("#it{p}_{T} fraction of cluster")
                h_bkg.GetYaxis().SetTitle("Normalized units")
                h_bkg.GetXaxis().SetTitleSize(21.0) # percent
                h_bkg.GetYaxis().SetTitleSize(21.0)
                h_bkg.GetXaxis().SetLabelSize(20.0)
                h_bkg.GetYaxis().SetLabelSize(20.0)
                h_bkg.GetYaxis().SetTitleOffset(1.6)


            if bkg: set_mc_style_line(h_bkg, colors[i], line_width = 4, line_style = 1, alpha = 1.0)
            if sig: set_mc_style_line(h_sig, colors[i], line_width = 4, line_style = 2, alpha = 0.7)
            if bkg: h_bkg.Rebin(2)
            if sig: h_sig.Rebin(2)
            if bkg: h_bkg.Scale(1/h_bkg.Integral()/2)
            if sig: h_sig.Scale(1/h_sig.Integral()/2)
            if bkg: h_bkg.SetMaximum(Y_MAX)
            if sig: h_sig.SetMaximum(Y_MAX)
            if bkg: h_bkg.Draw("hist,same")
            if sig: h_sig.Draw("hist,same")
            gPad.RedrawAxis()


        self.leg.SetColumnSeparation(0.0)
        self.leg.SetEntrySeparation(0.0) 
        self.leg.SetNColumns(2)
        # self.leg.AddEntry("")
        # self.leg.AddEntry(" Pythia8 multijet" )
        # self.leg.AddEntry(" Pythia8 Z'")

        for i in CLUSTER_INDEX:
            # self.leg.AddEntry("Cluster " +str(i),"","l")
            if bkg: self.leg.AddEntry( self.h_topo_pt_bkg[i], " ","l")
            if sig: self.leg.AddEntry(self.h_topo_pt_sig[i], "#scale[0.85]{ Cluster " +str(i)+"}" ,"l")

        # ugly hack to get rid of horizontal red line of 9th cluster at top of
        # plot -- basically draw a white thick line over it and then redraw
        # pad axes
        l = TLine(0., Y_MAX, 0.0167, Y_MAX)
        l.SetLineColor(0)
        l.SetLineWidth(5)
        l.Draw()
        gPad.RedrawAxis()


        draw_sample_names()

        self.name = "topocluster_plots"
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")

        self.name = "topocluster_plots_log"
        self.canvas.SetLogy()
        self.canvas.Update()
        self.canvas.Modified()
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf") // No one wants to see this
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")

        self.canvas.Clear()

        # TODO igure out how to get both of these to plot

class PlotTopoclustersMean(PlotBase):
    def __init__(self, **kwargs):

        super(PlotTopoclustersMean, self).__init__(
            atlas_loc = [0.25,0.88],
            hide_lumi = True,
            extra_legend_lines = DEF_LINES + ["#scale[0.9]{Errors represent RMS}","#scale[0.9]{       of #it{p}_{T} fraction dist.}"],
            extra_lines_loc = [0.61,0.80],
            #               x1   y1   x2   y2
            legend_loc = [0.60,0.915,0.83,0.83],
            atlas_mod = "Simulation Internal",
            tex_size_mod    = 0.9, # FIXME temporary, because Preliminary was too long to fit in without overlaps
            tex_spacing_mod    = 0.9,
            height = 600,
            width = 600,
            **kwargs)

        tmp_loader = LOADER

        # Load and validate histograms
        sig = False
        bkg = False
        try:
            self.h_topo_pt_bkg = get_background_histograms()
            if self.h_topo_pt_bkg[0]: bkg = True
        except Exception as e:
            pass

        try:
            self.h_topo_pt_sig = get_signal_histograms()
            if self.h_topo_pt_sig[0]: sig = True
        except Exception as e:
            pass

   
        # h_mean = TH1F("topocluster_mean_pt","Topocluster Mean pT", 10,0,10)
        h_bkg_mean = TH1F("topocluster_bkg_mean_pt","Topocluster Background Mean pT", N_CLUSTERS_MEAN,-0.50,20.5)
        h_sig_mean = TH1F("topocluster_sig_mean_pt","Topocluster Signal Mean pT", N_CLUSTERS_MEAN,-0.50,20.5)

        # self.x_min = -.5
        # self.x_max = 10 - .5
        for h_mean in [h_bkg_mean, h_sig_mean]:
            self.set_x_axis_bounds(h_mean)  
            h_mean.SetMaximum(.8)
            h_mean.SetMinimum(0)
            h_mean.GetYaxis().SetLimits(-0.4, 1.2)
            h_mean.GetXaxis().SetLimits(-0.5, N_CLUSTERS_MEAN - 0.5)
        # Offset the signal marks slightly
        h_sig_mean.GetXaxis().SetLimits(-0.3, N_CLUSTERS_MEAN - 0.3)

        # h_mean.GetXaxis().SetRange(1,30)

        for i in range(len(self.h_topo_pt_bkg)):
            h_bkg_mean.SetBinContent(i+1, self.h_topo_pt_bkg[i].GetMean())
            h_bkg_mean.SetBinError(i+1, self.h_topo_pt_bkg[i].GetStdDev())
            h_sig_mean.SetBinContent(i+1, self.h_topo_pt_sig[i].GetMean())
            h_sig_mean.SetBinError(i+1, self.h_topo_pt_sig[i].GetStdDev())


        self.canvas.cd()
        self.title = "Mean fractional pT of First "+str(N_CLUSTERS_MEAN)+" Topoclusters"
        ### Legend
        self.leg.AddEntry(h_bkg_mean, "Pythia8 multijet")
        self.leg.AddEntry(h_sig_mean, "Pythia8 Z'")

        # set_mc_style_line(h_mean, kGreen-2, line_width = 4)
        set_mc_style_line(h_bkg_mean, kViolet -7, line_width = 4, alpha = 1.0)
        set_mc_style_line(h_sig_mean, kGreen -2, line_width = 4, alpha = 0.8)

        self.canvas.Clear()
        
        for h_mean in [h_bkg_mean, h_sig_mean]:
            h_mean.GetYaxis().SetTitle("#LT#it{p}_{T}#GT fraction of cluster")
            h_mean.GetXaxis().SetTitle("Leading trimmed large-#it{R} jet cluster index (#it{p}_{T} sorted)")
            h_mean.GetXaxis().SetTitleSize(21.0) # percent
            h_mean.GetYaxis().SetTitleSize(21.0) 
            h_mean.GetXaxis().SetLabelSize(20.0)
            h_mean.GetYaxis().SetLabelSize(20.0)
            h_mean.GetYaxis().SetTitleOffset(1.4)
            h_mean.GetXaxis().SetNdivisions(10, True)

            h_mean.Draw("e1,same")
        

        self.name = "topocluster_mean_pt"
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")

        self.name = "topocluster_mean_log_pt"
        for h_mean in [h_bkg_mean, h_sig_mean]:
            h_mean.SetMaximum(1.25)
            # h_mean.SetMinimum(3E-3)
            h_mean.SetMinimum(1E-3)
        self.canvas.SetLogy()
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".png")
        self.canvas.Clear()



def draw_sample_names():
    sample_label = TText();
    sample_label.SetNDC();
    sample_label.SetTextFont(42);
    sample_label.SetTextAngle(0);
    sample_label.SetTextSize(0.03);
    sample_label.SetTextColor(1);
    y = 0.86
    x = 0.61

    sample_label.DrawText(x,y+.033, "Pythia 8");
    sample_label.DrawText(x+.125,y+.033, "Pythia 8");
    sample_label.DrawText(x,y, "Multijet");
    sample_label.DrawText(x+.125,y, "Z'");



def get_background_histograms():
    tmp_loader = LOADER
    var_base_name = "h_rljet0_fractional_pt_"
    h_topo_pt_bkg = []

    for i in range(0,N_CLUSTERS_MEAN):
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
    for i in range(0,N_CLUSTERS_MEAN):
        h_topo_pt_signal.append(TH1F("h_rljet_fractional_pt_"+str(i),"Topocluster fractional pT "+str(i), 100, 0.0, 1.0))

    # Open output ntuples and read values
    ntuples_file = TFile.Open( ZPRIME_ROOT_FILEPATH , "READ" )
    nominal = ntuples_file.Get("nominal")
    for j in range(nominal.GetEntries()):
        nominal.GetEntry(j)

        for i in range(0,N_CLUSTERS_MEAN):
            h_topo_pt_signal[i].Fill(nominal.GetLeaf("rljet_fractional_pt_"+str(i)).GetValue())
    
    # Normalize to Unity
    for i in range(0,N_CLUSTERS_MEAN):
        h_topo_pt_signal[i].Scale(1/h_topo_pt_signal[i].Integral())


    return copy.deepcopy(h_topo_pt_signal)
# print nominal.Show(1)


# var_base_name = "h_rljet0_fractional_pt_"
# h_topo_pt = []



# for i in range(0,10):
topocluster_plots = []


topocluster_plots.append(PlotTopoclusters())
topocluster_plots.append(PlotTopoclustersMean())