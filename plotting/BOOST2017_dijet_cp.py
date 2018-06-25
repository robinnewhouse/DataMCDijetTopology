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

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

# CP_ROOT_FILEPATH = "/afs/cern.ch/work/o/omajersk/private/DataMCDijetTopology/dijet.merged.cp.root"
CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180504_syst/dijet.merged.cp.root"
HISTLOADER = DijetLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_Dijet"

DO_SYSTEMATICS_DEFAULT = SYSTEMATICS_MC15C_WEAK

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/control"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

DEF_LINES = ["Multijet Selection", "Trimmed anti-#it{k}_{t} #it{R}=1.0", "#it{p}_{T} > 450 GeV"]
HTT_DEF_LINES = ["Multijet Selection", "Trimmed C/A #it{R}=1.5", "#it{p}_{T} > 450 GeV"]
MASS_PLOT_REBIN = 8

class PlotDataPythiaHerwig(PlotBase):
    def __init__(self, var_name, flip_legend = False, do_systematics = DO_SYSTEMATICS_DEFAULT,
                 wzjets_sf = 50, ttbar_sf = 50, suffix = "", **kwargs):

        super(PlotDataPythiaHerwig, self).__init__(
                lumi_val = "36.7",
                width = 600,
                name = var_name,
                atlas_mod = "Internal",
                legend_loc = [0.20,0.93,0.56,0.66] if flip_legend else [0.53,0.93,0.90,0.66],
                atlas_loc = [0.6,0.9] if flip_legend else None,
                extra_lines_loc = [0.6,0.82] if flip_legend else None,
                tex_size_mod = 0.9,
                tex_spacing_mod = 0.80,
                x_title = get_axis_title(var_name),
                **kwargs)

        print var_name

        self.var_name  = var_name
        self.wzjets_sf = wzjets_sf
        self.ttbar_sf  = ttbar_sf

        # GET PLOTS FROM FILE

        self.h_data = HISTLOADER.get_hist(["data", "nominal"] , var_name)
        set_data_style_simple(self.h_data, marker_size = 0.7)

        self.h_pythia_dijet = HISTLOADER.get_normalized_dijet("pythia_dijet", var_name)
        self.h_herwig_dijet = HISTLOADER.get_normalized_dijet("herwig_dijet", var_name)

        # scale factors, just for the legend
        self.pythia_dijet_SF = self.h_pythia_dijet.Integral() / HISTLOADER.get_hist(["pythia_dijet", "nominal"] , var_name).Integral()
        self.herwig_dijet_SF = self.h_herwig_dijet.Integral() / HISTLOADER.get_hist(["herwig_dijet", "nominal"] , var_name).Integral()

        self.h_wjets = HISTLOADER.get_wjets(var_name)
        self.h_zjets = HISTLOADER.get_zjets(var_name)
        self.h_ttbar = HISTLOADER.get_ttbar(var_name)

        print "###", var_name, "###"
        print "dijet: ", self.h_pythia_dijet.Integral()
        print "W+jets: ", self.h_wjets.Integral()
        print "Z+jets: ", self.h_zjets.Integral()
        print "ttbar: ", self.h_ttbar.Integral()

        for h in [self.h_pythia_dijet, self.h_herwig_dijet]:
           h.Add(self.h_wjets)
           h.Add(self.h_zjets)
           h.Add(self.h_ttbar)


        if (do_systematics):
          pythia_sys_dict = HISTLOADER.get_systematics_dictionary("pythia_dijet", var_name, do_systematics)
        else:
          pythia_sys_dict = {}
        self.hsys_pythia = TH1Sys(self.h_pythia_dijet, pythia_sys_dict)

        if (self.rebin != None):
            self.h_data.Rebin(self.rebin)
            self.h_pythia_dijet.Rebin(self.rebin)
            self.h_herwig_dijet.Rebin(self.rebin)
            self.h_wjets.Rebin(self.rebin)
            self.h_zjets.Rebin(self.rebin)
            self.h_ttbar.Rebin(self.rebin)
            self.hsys_pythia.rebin(self.rebin)

        #TODO: check if this is necessary
        self.hsys_pythia._compute_errors()
        self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs()

        for h in [self.h_data, self.h_pythia_dijet, self.h_herwig_dijet, self.h_pythia_sys, self.h_wjets, self.h_zjets, self.h_ttbar]:
            self.set_x_axis_bounds(h)

        self.determine_y_axis_title(self.h_data)

        for h in [self.h_data, self.h_pythia_dijet, self.h_herwig_dijet, self.h_pythia_sys, self.h_wjets, self.h_zjets, self.h_ttbar]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetLabelSize(0)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pad_empty_space([self.h_data, self.h_pythia_dijet, self.h_herwig_dijet, self.h_pythia_sys])

        # CREATE RATIO PLOTS

        self.h_pythia_dijet_ratio = self.h_pythia_dijet.Clone("h_pythia_dijet_ratio")
        self.h_herwig_dijet_ratio = self.h_herwig_dijet.Clone("h_herwig_dijet_ratio")
        self.h_pythia_sys_ratio   = self.h_pythia_sys.Clone("h_pythia_sys_ratio")

        # in ratio plots, include contribution of 'signal': W/Z+jets and ttbar
        #for h in [ self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio]:
        #    h.Add(self.h_wjets)
        #    h.Add(self.h_zjets)
        #    h.Add(self.h_ttbar)

        self.h_pythia_dijet_ratio.Divide(self.h_data, self.h_pythia_dijet_ratio, 1, 1, "")
        self.h_herwig_dijet_ratio.Divide(self.h_data, self.h_herwig_dijet_ratio, 1, 1, "")
        self.h_pythia_sys_ratio.Divide(self.h_data, self.h_pythia_sys_ratio, 1, 1, "")

        self.h_pythia_stat_ratio = self.h_pythia_dijet_ratio.Clone("h_pythia_stat_ratio")

        # center the stat. and syst. errors (in the ratio plot) around data/mc ratio = 1.0
        for ibin in range(self.h_pythia_sys_ratio.GetSize()):
            self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)
            self.h_pythia_stat_ratio.SetBinContent(ibin, 1.0)

        # ratio_title = "#frac{Data}{MC}"
        ratio_title = "Data/Pred."
        for h_ratio in [self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio, self.h_pythia_stat_ratio]:
            set_style_ratio(h_ratio, y_title = ratio_title, y_min = 0.25, y_max = 1.75, Ndiv = 504)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            #self.set_x_axis_bounds(h_ratio) #TODO
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)
            h_ratio.GetXaxis().SetLabelSize(19)

        self.name = self.var_name + "_data_mc_dijet"
        if self.log_scale: self.name += "_log"

        # TODO: necessary?
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        # magnified ONLY for visibility
        h_wjets_mag = self.h_wjets.Clone("h_wjets_mag")
        h_zjets_mag = self.h_zjets.Clone("h_zjets_mag")
        h_ttbar_mag = self.h_ttbar.Clone("h_ttbar_mag")
        h_wjets_mag.Scale(self.wzjets_sf)
        h_zjets_mag.Scale(self.wzjets_sf)
        h_ttbar_mag.Scale(self.ttbar_sf)

        #set_mc_style_line(h_wjets_mag, WJETS_COLOR, 1, 2)
        #set_mc_style_line(h_zjets_mag, ZJETS_COLOR, 1, 2)
        #set_mc_style_line(h_ttbar_mag, TTBAR_COLOR, 1, 2)

        set_mc_sys_err_style(self.h_pythia_sys, col = kGreen + 2, alpha = 0.7)
        set_mc_sys_err_style(self.h_pythia_sys_ratio, col = kGreen - 8, alpha = 0.7)
        set_mc_sys_err_style(self.h_pythia_stat_ratio, col = kGreen - 5, alpha = 0.7)

        set_mc_style_line(self.h_pythia_dijet_ratio, HISTLOADER.PYTHIA_COLOR, line_width = 3, alpha = 0.9)
        set_mc_style_line(self.h_herwig_dijet_ratio, HISTLOADER.HERWIG_COLOR, line_width = 3, alpha = 0.9)

        #self.h_pythia_sys_ratio.SetMarkerSize(0)
        #self.h_pythia_stat_ratio.SetMarkerSize(0)
        #self.h_pythia_sys_ratio.SetFillColor(kGreen-8)
        #self.h_pythia_stat_ratio.SetFillColor(kGreen-5)

        #self.h_pythia_stat_ratio.SetFillStyle(1001)
        #self.h_pythia_stat_ratio.SetLineWidth(0)

        #self.h_pythia_dijet_ratio.SetFillStyle(0)
        #self.h_pythia_dijet_ratio.SetFillColorAlpha(PYTHIA_COLOR, 0.85)
        #self.h_herwig_dijet_ratio.SetFillStyle(0)
        #self.h_herwig_dijet_ratio.SetFillColorAlpha(HERWIG_COLOR, 0.85)

        # SET UP THE CANVAS
        self.canvas.Divide(1,2)
        self.canvas.Modified()

        self.pad1 = self.canvas.cd(1)
        self.pad1.SetPad(0.0, 0.33, 1., 1.)
        self.pad1.SetTopMargin(0.07)
        self.pad1.SetRightMargin(0.07)
        self.pad1.SetBottomMargin(0.02)
        self.pad1.SetFillColorAlpha(0, 0.)
        self.pad1.SetBorderSize(0)
        if self.log_scale: self.pad1.SetLogy()

        self.pad2 = self.canvas.cd(2)
        self.pad2.SetPad(0.0, 0.0, 1., 0.33)
        self.pad2.SetTopMargin(0.025)
        self.pad2.SetRightMargin(0.07)
        self.pad2.SetBottomMargin(0.3)
        self.pad2.SetFillColorAlpha(0, 0.)
        self.pad2.SetGridy(1) # grid for ratio plot
        self.pad2.SetBorderSize(0)

        self.pad1.cd()

        self.hs_sig = THStack("hs_sig_" + var_name, "hs_sig_" + var_name)
        self.hs_sig.Add(self.h_ttbar)
        self.hs_sig.Add(self.h_zjets)
        self.hs_sig.Add(self.h_wjets)

        self.h_herwig_dijet.Draw("hist,same")
        self.h_pythia_dijet.Draw("hist,same")
        self.hs_sig.Draw("histo,same")
        #h_wjets_mag.Draw("hist,same")
        #h_zjets_mag.Draw("hist,same")
        #h_ttbar_mag.Draw("hist,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data")
        self.leg.AddEntry(self.h_pythia_dijet, "Pythia8 (#times " + '{0:.2f}'.format(self.pythia_dijet_SF) + ")", "f")
        self.leg.AddEntry(self.h_herwig_dijet, "Herwig++ (#times " + '{0:.2f}'.format(self.herwig_dijet_SF) + ")", "f")
        self.leg.AddEntry(h_wjets_mag, "W+jets", "f")
        self.leg.AddEntry(h_zjets_mag, "Z+jets", "f")
        self.leg.AddEntry(h_ttbar_mag, "all-had t#bar{t}", "f")
        self.leg.AddEntry(self.h_pythia_stat_ratio, "Stat. uncert.", "f")
        if (self.hsys_pythia.num_systematics != 0):
            self.leg.AddEntry(self.h_pythia_sys_ratio, "Total uncert.", "f")
        self.leg.Draw()

        self.pad2.cd()

        if (self.hsys_pythia.num_systematics != 0):
            self.h_pythia_sys_ratio.Draw("E2,same")
        self.h_pythia_stat_ratio.Draw("E2,same")
        self.h_herwig_dijet_ratio.Draw("hist,same")
        self.h_pythia_dijet_ratio.Draw("hist,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + suffix + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + suffix + ".eps")
        self.canvas.Clear()


data_mc_plots = []

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 4.5,
        extra_legend_lines = DEF_LINES + ["m^{comb} + #tau_{32} (#epsilon_{sig} = 80%)", "top tag pass"],
        log_scale = True,
        x_min = 50,
        x_max = 350,
        y_min = 1.1e1,
        ttbar_sf = 5,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 4.5,
        extra_legend_lines = DEF_LINES + ["m^{comb} + #tau_{32} (#epsilon_{sig} = 80%)", "top tag pass"],
        log_scale = True,
        x_min = 0,
        x_max = 350,
        y_min = 1.1e1,
        ttbar_sf = 5,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 2.3,
        extra_legend_lines = DEF_LINES + ["m^{comb} + D_{2} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        y_min = 0.01,
        x_min = 50,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 2.3,
        extra_legend_lines = DEF_LINES + ["m^{comb} + D_{2} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        y_min = 0.01,
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 10,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "BDT_Top",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["BDT top (#epsilon_{sig} = 80%)", "top tag pass"],
        x_min = 50,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "BDT_Top",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["BDT top (#epsilon_{sig} = 80%)", "top tag pass"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "BDT_W",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["BDT #it{W} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        x_min = 0,
        x_max = 350,
        flip_legend = True,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        suffix = "_unzoomed"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "BDT_W",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["BDT #it{W} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        x_min = 50,
        x_max = 150,
        flip_legend = True,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "DNN_Top",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["DNN top (#epsilon_{sig} = 80%)", "top tag pass"],
        x_min = 50,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "DNN_Top",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["DNN top (#epsilon_{sig} = 80%)", "top tag pass"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))


data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "DNN_W",
        empty_scale = 2.0,
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["DNN #it{W} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = 4,
        suffix = "_unzoomed"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "DNN_W",
        empty_scale = 2.0,
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["DNN #it{W} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        x_min = 50,
        x_max = 130,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "TopoTag_Top_80_qqb",
        empty_scale = 2.0,
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["TopoDNN (#epsilon_{sig} = 80%)", "top tag pass"],
        x_min = 50,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "TopoTag_Top_80_qqb",
        empty_scale = 2.0,
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["TopoDNN (#epsilon_{sig} = 80%)", "top tag pass"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_m_comb",
        empty_scale = 3.0,
        extra_legend_lines = DEF_LINES,
        x_min = 50,
        x_max = 350,
        y_min = 10e1,
        wzjets_sf = 25,
        ttbar_sf = 25,
        log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_m_comb",
        empty_scale = 3.0,
        extra_legend_lines = DEF_LINES,
        x_min = 0,
        x_max = 350,
        y_min = 10e1,
        wzjets_sf = 25,
        ttbar_sf = 25,
        log_scale = True,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_D2_combMgt50GeV",
            empty_scale = 2.0,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
            x_units = "",
            rebin = 4,
            x_max = 4.0,
            y_min = 0.01,
            ))

for masstag in ["combMgt100GeV"]:
    data_mc_plots.append(PlotDataPythiaHerwig(
            "h_rljet0_Tau32_wta" if not masstag else "h_rljet0_Tau32_wta_" + masstag,
            empty_scale = 2.0,
            flip_legend = True,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            x_units = "",
            x_min = 0.01,
            x_max = 0.9,
            y_min = 0.01,
            rebin = 4
            ))

# data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_atan1312",
#         empty_scale = 1.9,
#         extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
#         ttbar_sf = 25,
#         wzjets_sf = 25,
#         rebin = 5,
#         do_systematics = SYSTEMATICS_MC15C_CAJET,
#         ))
#
# data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m23m123",
#         empty_scale = 2.0,
#         extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
#         ttbar_sf = 25,
#         wzjets_sf = 25,
#         rebin = 8,
#         do_systematics = SYSTEMATICS_MC15C_CAJET,
#         ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m",
        empty_scale = 2.0,
        x_min = 50,
        x_max = 250,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m",
        empty_scale = 2.0,
        x_min = 20,
        x_max = 250,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_m",
        empty_scale = 2.5,
        log_scale = True,
        x_min = 50,
        x_max = 1000,
        extra_legend_lines = HTT_DEF_LINES,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 14,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_m",
        empty_scale = 2.5,
        log_scale = True,
        x_min = 0,
        x_max = 1000,
        extra_legend_lines = HTT_DEF_LINES,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 14,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_m_HTT_CAND",
        empty_scale = 3.5,
        log_scale = True,
        x_min = 50,
        x_max = 1000,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-Tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 16,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_m_HTT_CAND",
        empty_scale = 3.5,
        log_scale = True,
        x_min = 0,
        x_max = 1000,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-Tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 16,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_pt",
        empty_scale = 4,
        extra_legend_lines = ["Multijet Selection", "Trimmed C/A #it{R}=1.5"],
        x_min = 450,
        x_max = 3000,
        log_scale = True,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_pt_HTT_CAND",
        empty_scale = 6,
        extra_legend_lines = ["Multijet Selection", "Trimmed C/A #it{R}=1.5"] + ["HTT-Tagged"],
        x_min = 450,
        x_max = 3000,
        log_scale = True,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        do_systematics = SYSTEMATICS_MC15C_CAJET,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb",
        empty_scale = 6,
        extra_legend_lines = ["Multijet Selection", "Trimmed anti-#it{k_{t}} #it{R}=1.0"],
        x_min = 450,
        x_max = 4000,
        log_scale = True,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_topTag_BDT_qqb_score_mva",
        empty_scale = 3,
        x_units = "",
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        y_min = 1e2,
        ttbar_sf = 15,
        wzjets_sf = 20,
        rebin = 4,
        log_scale = True,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_topTag_DNN_qqb_score_mva",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        x_units = "",
        y_min = 1e2,
        ttbar_sf = 15,
        wzjets_sf = 20,
        log_scale = True,
        rebin = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_wTag_BDT_qq_score_mva",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        x_units = "",
        ttbar_sf = 20,
        wzjets_sf = 10,
        rebin = 4,
        y_min = 1e2,
        log_scale = True,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_wTag_DNN_qq_score_mva",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        x_units = "",
        y_min = 1e2,
        ttbar_sf = 20,
        wzjets_sf = 10,
        log_scale = True,
        rebin = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_topTag_TopoTagger_score_mva",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES,
        x_units = "",
        y_min = 1e2,
        ttbar_sf = 20,
        wzjets_sf = 10,
        log_scale = True,
        rebin = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
        ))


data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16WTag_50eff_MassJSSCut",
        empty_scale = 10,
        extra_legend_lines = ["Multijet Selection", "Trimmed anti-#it{k_{t}} #it{R}=1.0"] + ["m^{comb} + D_{2} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        log_scale = True,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))


data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "BDT_W",
        empty_scale = 10,
        extra_legend_lines = ["Multijet Selection", "Trimmed anti-#it{k_{t}} #it{R}=1.0"] + ["BDT #it{W} (#epsilon_{sig} = 50%)", "#it{W} tag pass"],
        log_scale = True,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))


data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 7,
        extra_legend_lines = ["Multijet Selection", "Trimmed anti-#it{k_{t}} #it{R}=1.0"] + ["m^{comb} + #tau_{32} (#epsilon_{sig} = 80%)", "top tag pass"],
        log_scale = True,
        y_min = 10,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_mu",
       empty_scale = 2.0,
       extra_legend_lines = DEF_LINES,
       do_systematics = False,
       x_units = "",
       x_min = 5,
       x_max = 45,
       rebin = 4,
       ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_ungroomed_ntrk500",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES,
        do_systematics = False,
        x_units = "",
        x_min = 0,
        x_max = 80,
        ttbar_sf = 20,
        wzjets_sf = 20
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_NTrimSubjets",
        empty_scale = 7.0,
        extra_legend_lines = DEF_LINES,
        do_systematics = False,
        log_scale = True,
        x_units = "",
        x_min = 0,
        x_max = 10,
        y_min = 1.0,
        ttbar_sf = 20,
        wzjets_sf = 20
        ))


## SD Workaround Section
# CP_ROOT_FILEPATH = "/afs/cern.ch/work/o/omajersk/private/DataMCDijetTopology/dijet.merged.cp.root"
CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180515_syst/dijet.merged.cp.root"
HISTLOADER = DijetLoader(CP_ROOT_FILEPATH)
# ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_Dijet"

# There are still some systematics missing, this is a hack to let it plot with the ones that still are there
SD_SYSTEMATICS = SYSTEMATICS_MC15C_WEAK_NOINPUTS
MISSING_SYSTEMATICS = [
# "LARGERJET_Weak_JET_Rtrk_Baseline_D2Beta1",
# "LARGERJET_Weak_JET_Rtrk_Baseline_Tau32WTA",
# "LARGERJET_Weak_JET_Rtrk_Modelling_D2Beta1",
# "LARGERJET_Weak_JET_Rtrk_Modelling_Tau32WTA",
# "LARGERJET_Weak_JET_Rtrk_TotalStat_D2Beta1",
# "LARGERJET_Weak_JET_Rtrk_TotalStat_Tau32WTA",
# "LARGERJET_Weak_JET_Rtrk_Tracking_D2Beta1",
# "LARGERJET_Weak_JET_Rtrk_Tracking_Tau32WTA",
# "EG_SCALE_ALL",
# "EG_RESOLUTION_ALL",
]

# OUTPUT_DIR = ROOT_OUTPUT_DIR + "/control"
# make_dir(ROOT_OUTPUT_DIR)
# make_dir(OUTPUT_DIR)


data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "SDt_dcut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["SD (#epsilon_{sig} = 80%)","top tag pass"],
        x_min = 50,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = SD_SYSTEMATICS,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "SDt_dcut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["SD (#epsilon_{sig} = 80%)","top tag pass"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = SD_SYSTEMATICS,
        rebin = MASS_PLOT_REBIN,
        suffix = "_fullrange"
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SD_logchi_t_dcut",
        empty_scale = 2.2,
        do_systematics = SD_SYSTEMATICS,
        x_units = "",
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 60 GeV"],
        x_min = -10,
        x_max = 10,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        ))
