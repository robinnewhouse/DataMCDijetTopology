import sys
sys.path.append('lib/')

import atlas_style

import os
import math
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
CP_ROOT_FILEPATH = "/home/newhouse/public/Analysis/TopBosonTagging/DataMCDijetTopology/run/cluster_test_5/output.cp.root"
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

        super(PlotTopoclusters, self).__init__(**kwargs)

        tmp_loader = LOADER
        var_base_name = "h_rljet0_fractional_pt_"
        self.h_topo_pt = []

        for i in range(0,10):
             self.h_topo_pt.append(tmp_loader.get_normalized_dijet("pythia_dijet", var_base_name+str(i), normalize_to_unity = True))

        # tmp_loader = LOADER_SMOOTH if "smooth" in var_name or "pt_comb" in var_name else LOADER

        # self.var_name  = var_name
        # self.wzjets_sf = wzjets_sf
        # self.ttbar_sf  = ttbar_sf

        # # GET PLOTS FROM FILE

        # self.h_data = tmp_loader.get_hist(["data", "nominal"] , var_name)
        # set_data_style_simple(self.h_data, marker_size = 0.7)

        # self.h_pythia_dijet = tmp_loader.get_normalized_dijet("pythia_dijet", var_name)
        # self.h_herwig_dijet = tmp_loader.get_normalized_dijet("herwig_dijet", var_name)

        # # scale factors, just for the legend
        # self.pythia_dijet_SF = self.h_pythia_dijet.Integral() / tmp_loader.get_hist(["pythia_dijet", "nominal"] , var_name).Integral()
        # self.herwig_dijet_SF = self.h_herwig_dijet.Integral() / tmp_loader.get_hist(["herwig_dijet", "nominal"] , var_name).Integral()

        # self.h_wjets = tmp_loader.get_wjets(var_name)
        # self.h_zjets = tmp_loader.get_zjets(var_name)
        # self.h_ttbar = tmp_loader.get_ttbar(var_name)

        # print "###", var_name, "###"
        # print "dijet: ", self.h_pythia_dijet.Integral()
        # print "W+jets: ", self.h_wjets.Integral()
        # print "Z+jets: ", self.h_zjets.Integral()
        # print "ttbar: ", self.h_ttbar.Integral()

        # for h in [self.h_pythia_dijet, self.h_herwig_dijet]:
        #    h.Add(self.h_wjets)
        #    h.Add(self.h_zjets)
        #    h.Add(self.h_ttbar)


        # if (do_systematics):
        #   pythia_sys_dict = tmp_loader.get_systematics_dictionary("pythia_dijet", var_name, SYSTEMATICS)
        # else:
        #   pythia_sys_dict = {}
        # self.hsys_pythia = TH1Sys(self.h_pythia_dijet, pythia_sys_dict)

        # if (self.rebin != None):
        #     self.h_data.Rebin(self.rebin)
        #     self.h_pythia_dijet.Rebin(self.rebin)
        #     self.h_herwig_dijet.Rebin(self.rebin)
        #     self.h_wjets.Rebin(self.rebin)
        #     self.h_zjets.Rebin(self.rebin)
        #     self.h_ttbar.Rebin(self.rebin)
        #     self.hsys_pythia.rebin(self.rebin)

        # #TODO: check if this is necessary
        # self.hsys_pythia._compute_errors()
        # self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs()




        # self.determine_y_axis_title(self.h_data)

        # for h in [self.h_data, self.h_pythia_dijet, self.h_herwig_dijet, self.h_pythia_sys, self.h_wjets, self.h_zjets, self.h_ttbar]:
        #     h.GetYaxis().SetTitle(self.y_title)
        #     h.GetXaxis().SetLabelSize(0)
        #     h.GetYaxis().SetTitleOffset(2.0)
        #     self.set_y_min(h)
        #     self.set_y_max(h)

        # self.pad_empty_space([self.h_data, self.h_pythia_dijet, self.h_herwig_dijet, self.h_pythia_sys])

        # # CREATE RATIO PLOTS

        # self.h_pythia_dijet_ratio = self.h_pythia_dijet.Clone("h_pythia_dijet_ratio")
        # self.h_herwig_dijet_ratio = self.h_herwig_dijet.Clone("h_herwig_dijet_ratio")
        # self.h_pythia_sys_ratio   = self.h_pythia_sys.Clone("h_pythia_sys_ratio")

        # # in ratio plots, include contribution of 'signal': W/Z+jets and ttbar
        # #for h in [ self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio]:
        # #    h.Add(self.h_wjets)
        # #    h.Add(self.h_zjets)
        # #    h.Add(self.h_ttbar)

        # self.h_pythia_dijet_ratio.Divide(self.h_data, self.h_pythia_dijet_ratio, 1, 1, "")
        # self.h_herwig_dijet_ratio.Divide(self.h_data, self.h_herwig_dijet_ratio, 1, 1, "")
        # self.h_pythia_sys_ratio.Divide(self.h_data, self.h_pythia_sys_ratio, 1, 1, "")

        # self.h_pythia_stat_ratio = self.h_pythia_dijet_ratio.Clone("h_pythia_stat_ratio")

        # # center the stat. and syst. errors (in the ratio plot) around data/mc ratio = 1.0
        # for ibin in range(self.h_pythia_sys_ratio.GetSize()):
        #     self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)
        #     self.h_pythia_stat_ratio.SetBinContent(ibin, 1.0)

        # ratio_title = "#frac{Data}{MC}"
        # for h_ratio in [self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio, self.h_pythia_stat_ratio]:
        #     set_style_ratio(h_ratio, y_title = ratio_title)
        #     h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
        #     #self.set_x_axis_bounds(h_ratio) #TODO
        #     h_ratio.GetXaxis().SetTitleOffset(4.0)
        #     h_ratio.GetYaxis().SetTitleOffset(2.0)
        #     h_ratio.GetXaxis().SetLabelSize(19)

        # self.name = self.var_name + "_data_mc_dijet"
        # if self.log_scale: self.name += "_log"

        # # TODO: necessary?
        # self.canvas.Clear()
        # self.canvas = None
        # self._make_canvas()
        # self._make_decorations()

        # # magnified ONLY for visibility
        # h_wjets_mag = self.h_wjets.Clone("h_wjets_mag")
        # h_zjets_mag = self.h_zjets.Clone("h_zjets_mag")
        # h_ttbar_mag = self.h_ttbar.Clone("h_ttbar_mag")
        # h_wjets_mag.Scale(self.wzjets_sf)
        # h_zjets_mag.Scale(self.wzjets_sf)
        # h_ttbar_mag.Scale(self.ttbar_sf)

        # #set_mc_style_line(h_wjets_mag, WJETS_COLOR, 1, 2)
        # #set_mc_style_line(h_zjets_mag, ZJETS_COLOR, 1, 2)
        # #set_mc_style_line(h_ttbar_mag, TTBAR_COLOR, 1, 2)

        # set_mc_sys_err_style(self.h_pythia_sys, col = kGreen + 2, alpha = 0.7)
        # set_mc_sys_err_style(self.h_pythia_sys_ratio, col = kGreen - 8, alpha = 0.7)
        # set_mc_sys_err_style(self.h_pythia_stat_ratio, col = kGreen - 5, alpha = 0.7)

        # set_mc_style_line(self.h_pythia_dijet_ratio, tmp_loader.PYTHIA_COLOR, line_width = 3, alpha = 0.9)
        # set_mc_style_line(self.h_herwig_dijet_ratio, tmp_loader.HERWIG_COLOR, line_width = 3, alpha = 0.9)

        # #self.h_pythia_sys_ratio.SetMarkerSize(0)
        # #self.h_pythia_stat_ratio.SetMarkerSize(0)
        # #self.h_pythia_sys_ratio.SetFillColor(kGreen-8)
        # #self.h_pythia_stat_ratio.SetFillColor(kGreen-5)

        # #self.h_pythia_stat_ratio.SetFillStyle(1001)
        # #self.h_pythia_stat_ratio.SetLineWidth(0)

        # #self.h_pythia_dijet_ratio.SetFillStyle(0)
        # #self.h_pythia_dijet_ratio.SetFillColorAlpha(PYTHIA_COLOR, 0.85)
        # #self.h_herwig_dijet_ratio.SetFillStyle(0)

        # SET UP THE CANVAS
        # self.canvas.Divide(1,2)
        # self.canvas.Modified()

        # self.pad1 = self.canvas.cd(1)
        # self.pad1.SetTopMargin(0.07)
        # self.pad1.SetRightMargin(0.07)
        # self.pad1.SetBottomMargin(0.00)
        # self.pad1.SetFillColorAlpha(0, 0.)
        # self.pad1.SetBorderSize(0)
        # self.pad2.SetTopMargin(0.05)
        # self.pad2.SetRightMargin(0.07)
        # self.pad2.SetBottomMargin(0.3)
        # self.pad2.SetBorderSize(0)

        # self.pad1.cd()

        # self.hs_sig = THStack("hs_sig_" + var_name, "hs_sig_" + var_name)
        # self.hs_sig.Add(self.h_ttbar)
        # self.hs_sig.Add(self.h_zjets)
        # self.hs_sig.Add(self.h_wjets)

        # self.h_herwig_dijet.Draw("hist,same")
        # self.h_pythia_dijet.Draw("hist,same")
        # self.hs_sig.Draw("histo,same")
        # #h_wjets_mag.Draw("hist,same")
        # #h_zjets_mag.Draw("hist,same")
        # #h_ttbar_mag.Draw("hist,same")
        # self.h_data.Draw("PE,same")

        # self.pad1.RedrawAxis()

        # self.canvas.cd()
        # colors = [kGreen - 2, kBlue, kBlue - 5, 1,1,1,1,1,1, kRed +1 ]
        # print(kWhite)
        # for i in [0,1,2,9]:
        #     h = self.h_topo_pt[i]
        #     h.GetYaxis().SetTitle("Normalized Units")
        #     h.GetXaxis().SetTitle("pT Fraction of Cluster")

        #     self.leg.AddEntry(self.h_topo_pt[i], "Cluster " +str(i))
        #     h.SetMaximum(0.16)
        #     set_mc_style_line(h, colors[i], line_width = 4)
        #     h.Draw("hist,same")
        # # self.leg.AddEntry(self.h_pythia_dijet, "Pythia8 dijet (#times " + '{0:.2f}'.format(self.pythia_dijet_SF) + ")", "f")
        # # self.leg.AddEntry(self.h_herwig_dijet, "Herwig++ dijet (#times " + '{0:.2f}'.format(self.herwig_dijet_SF) + ")", "f")
        # # self.leg.AddEntry(h_wjets_mag, "W+jets", "f")
        # # self.leg.AddEntry(h_zjets_mag, "Z+jets", "f")
        # # self.leg.AddEntry(h_ttbar_mag, "all-had t#bar{t}", "f")
        # # self.leg.AddEntry(self.h_pythia_stat_ratio, "Stat. uncert.", "f")
        # # if (self.hsys_pythia.num_systematics != 0):
        # #     self.leg.AddEntry(self.h_pythia_sys_ratio, "Stat. #oplus syst. uncert.", "f")
        # # self.leg.Draw()

        # # self.pad2.cd()

        # # if (self.hsys_pythia.num_systematics != 0):
        # #     self.h_pythia_sys_ratio.Draw("E2,same")
        # # self.h_pythia_stat_ratio.Draw("E2,same")
        # # self.h_herwig_dijet_ratio.Draw("hist,same")
        # # self.pad2.RedrawAxis("g")

        # # self.pad1.RedrawAxis()
        # self.name = "topocluster_plots"
        # self.canvas.Update()
        # self.canvas.Modified()
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")

        # self.name = "topocluster_plots_log"
        # self.canvas.SetLogy()

        # self.canvas.Update()
        # self.canvas.Modified()
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")

        # self.canvas.Clear()


        h_mean = TH1F("topocluster_mean_pt","Topocluster Mean pT", 10,0,10)
        self.x_min = 0
        self.x_max = 10
        self.set_x_axis_bounds(h_mean)   
        h_mean.SetMaximum(1)
        h_mean.SetMinimum(0)
        # h_mean.GetXaxis().SetRange(1,30)

        for i in range(len(self.h_topo_pt)):
            h = self.h_topo_pt[i]
            # print(h.GetMean())
            h_mean.Fill(i, h.GetMean())


        self.canvas.cd()
        self.title = "Mean Fractional pT of First 10 Topoclusters"
        self.leg.AddEntry(h_mean, "Mean pT ")
        set_mc_style_line(h_mean, kGreen-2, line_width = 4)
        h_mean.GetYaxis().SetTitle("Mean pT Fraction of Cluster")
        h_mean.GetXaxis().SetTitle("Cluster Number")



        h_mean.Draw("hist,same")
        self.name = "topocluster_mean_pt"
        self.canvas.Update()
        self.canvas.Modified()
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")



topocluster_plots = []


topocluster_plots.append(PlotTopoclusters())

# data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_NTrimSubjets",
#         empty_scale = 7.0,
#         extra_legend_lines = DEF_LINES,
#         do_systematics = False,
#         log_scale = True,
#         x_units = "",
#         x_min = 0,
#         x_max = 10,
#         y_min = 1.0,
#         ttbar_sf = 20,
#         wzjets_sf = 20
#         ))
