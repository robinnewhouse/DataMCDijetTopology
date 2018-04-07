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

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2017/cp.merged.root"
CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180227/merged.cp.root"
LOADER = DijetLoader(CP_ROOT_FILEPATH)
LOADER_SMOOTH = LOADER
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"
DO_SYSTEMATICS_DEFAULT = False

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/BOOST2017"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" , "p_{T} > 450 GeV"]
HTT_DEF_LINES = [ "Trimmed C/A #it{R}=1.5", "Dijet Selection" , "p_{T} > 450 GeV"]
MASS_PLOT_REBIN = 8

#SYSTEMATICS = [ ]
SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM

class PlotDataPythiaHerwig(PlotBase):
    def __init__(self, var_name, flip_legend = False, do_systematics = DO_SYSTEMATICS_DEFAULT,
                 wzjets_sf = 50, ttbar_sf = 50, **kwargs):

        super(PlotDataPythiaHerwig, self).__init__(
                lumi_val = "36.7",
                width = 600,
                name = var_name,
                atlas_mod = "Internal",
                legend_loc = [0.20,0.93,0.56,0.66] if flip_legend else [0.53,0.93,0.90,0.66],
                atlas_loc = [0.6,0.9] if flip_legend else None,
                extra_lines_loc = [0.6,0.82] if flip_legend else None,
                tex_size_mod = 0.9,
                tex_spacing_mod = 0.75,
                x_title = get_axis_title(var_name),
                **kwargs)

        print var_name

        tmp_loader = LOADER_SMOOTH if "smooth" in var_name or "pt_comb" in var_name else LOADER

        self.var_name  = var_name
        self.wzjets_sf = wzjets_sf
        self.ttbar_sf  = ttbar_sf

        # GET PLOTS FROM FILE

        self.h_data = tmp_loader.get_hist(["data", "nominal"] , var_name)
        set_data_style_simple(self.h_data, marker_size = 0.7)

        self.h_pythia_dijet = tmp_loader.get_normalized_dijet("pythia_dijet", var_name)
        self.h_herwig_dijet = tmp_loader.get_normalized_dijet("herwig_dijet", var_name)

        # scale factors, just for the legend
        self.pythia_dijet_SF = self.h_pythia_dijet.Integral() / tmp_loader.get_hist(["pythia_dijet", "nominal"] , var_name).Integral()
        self.herwig_dijet_SF = self.h_herwig_dijet.Integral() / tmp_loader.get_hist(["herwig_dijet", "nominal"] , var_name).Integral()

        self.h_wjets = tmp_loader.get_wjets(var_name)
        self.h_zjets = tmp_loader.get_zjets(var_name)
        self.h_ttbar = tmp_loader.get_ttbar(var_name)

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
          pythia_sys_dict = tmp_loader.get_systematics_dictionary("pythia_dijet", var_name, SYSTEMATICS)
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

        ratio_title = "#frac{Data}{MC}"
        for h_ratio in [self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio, self.h_pythia_stat_ratio]:
            set_style_ratio(h_ratio, y_title = ratio_title)
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

        set_mc_style_line(self.h_pythia_dijet_ratio, tmp_loader.PYTHIA_COLOR, line_width = 3, alpha = 0.9)
        set_mc_style_line(self.h_herwig_dijet_ratio, tmp_loader.HERWIG_COLOR, line_width = 3, alpha = 0.9)

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
        self.pad1.SetBottomMargin(0.00)
        self.pad1.SetFillColorAlpha(0, 0.)
        self.pad1.SetBorderSize(0)
        if self.log_scale: self.pad1.SetLogy()

        self.pad2 = self.canvas.cd(2)
        self.pad2.SetPad(0.0, 0.0, 1., 0.33)
        self.pad2.SetTopMargin(0.05)
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
        self.leg.AddEntry(self.h_pythia_dijet, "Pythia8 dijet (#times " + '{0:.2f}'.format(self.pythia_dijet_SF) + ")", "f")
        self.leg.AddEntry(self.h_herwig_dijet, "Herwig++ dijet (#times " + '{0:.2f}'.format(self.herwig_dijet_SF) + ")", "f")
        self.leg.AddEntry(h_wjets_mag, "W+jets", "f")
        self.leg.AddEntry(h_zjets_mag, "Z+jets", "f")
        self.leg.AddEntry(h_ttbar_mag, "all-had t#bar{t}", "f")
        self.leg.AddEntry(self.h_pythia_stat_ratio, "Stat. uncert.", "f")
        if (self.hsys_pythia.num_systematics != 0):
            self.leg.AddEntry(self.h_pythia_sys_ratio, "Stat. #oplus syst. uncert.", "f")
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

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.canvas.Clear()


data_mc_plots = []

#for npvtag in ["NPVLT10", "NPVGT10LT15", "NPVGT15LT20", "NPVGT20"]:
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut" + "_" + npvtag,
#            empty_scale = 2.6,
#            extra_legend_lines = DEF_LINES + ["Top-tagged"],
#            log_scale = True,
#            do_systematics = False,
#            x_min = 0,
#            x_max = 400,
#            y_min = 1600.01,
#            ttbar_sf = 5,
#            wzjets_sf = 12,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut" + "_" + npvtag,
#            empty_scale = 2.6,
#            extra_legend_lines = DEF_LINES + ["Top-tagged"],
#            log_scale = True,
#            x_min = 0,
#            do_systematics = False,
#            x_max = 400,
#            y_min = 10000.01,
#            ttbar_sf = 5,
#            wzjets_sf = 12,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff" + "_" + npvtag,
#            empty_scale = 2.4,
#            extra_legend_lines = DEF_LINES + ["Top-tagged"],
#            do_systematics = False,
#            x_min = 90,
#            x_max = 400,
#            y_min = 0.01,
#            ttbar_sf = 5,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff" + "_" + npvtag,
#            empty_scale = 2.2,
#            extra_legend_lines = DEF_LINES + ["Top-tagged"],
#            do_systematics = False,
#            x_min = 70,
#            x_max = 400,
#            y_min = 0.01,
#            ttbar_sf = 10,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff" + "_" + npvtag,
#            empty_scale = 2.5,
#            extra_legend_lines = DEF_LINES + ["Top-tagged"],
#            do_systematics = False,
#            x_min = 50.01,
#            x_max = 400,
#            y_min = 0.01,
#            ttbar_sf = 5,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff" + "_" + npvtag,
#            empty_scale = 2.0,
#            extra_legend_lines = DEF_LINES + ["Top-tagged"],
#            do_systematics = False,
#            x_min = 50,
#            x_max = 400,
#            y_min = 0.01,
#            ttbar_sf = 10,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut" + "_" + npvtag,
#            empty_scale = 1.4,
#            extra_legend_lines = DEF_LINES + ["W-tagged"],
#            do_systematics = False,
#            x_min = 0,
#    	      y_min = 0.01,
#            x_max = 400,
#            ttbar_sf = 25,
#            wzjets_sf = 10,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut" + "_" + npvtag,
#            empty_scale = 2.2,
#            extra_legend_lines = DEF_LINES + ["W-tagged"],
#            do_systematics = False,
#            x_min = 0,
#            x_max = 400,
#            ttbar_sf = 25,
#            wzjets_sf = 25,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut" + "_" + npvtag,
#            empty_scale = 1.4,
#            extra_legend_lines = DEF_LINES + ["Z-tagged"],
#            do_systematics = False,
#            x_min = 0,
#            x_max = 400,
#            ttbar_sf = 25,
#            wzjets_sf = 12,
#            rebin = MASS_PLOT_REBIN,
#            ))
#
#    data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut" + "_" + npvtag,
#            empty_scale = 2.2,
#            extra_legend_lines = DEF_LINES + ["Z-tagged"],
#            do_systematics = False,
#            x_min = 0,
#            x_max = 400,
#            ttbar_sf = 25,
#            wzjets_sf = 25,
#            rebin = MASS_PLOT_REBIN,
#            ))


data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
        empty_scale = 4.5,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        log_scale = True,
        x_min = 0,
        x_max = 350,
        y_min = 1.1e1,
        ttbar_sf = 5,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 4.5,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        log_scale = True,
        x_min = 0,
        x_max = 350,
        y_min = 1.1e1,
        ttbar_sf = 5,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff",
        empty_scale = 2.6,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        ttbar_sf = 5,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff",
        empty_scale = 2.2,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        ttbar_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff",
        empty_scale = 2.5,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        ttbar_sf = 5,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        ttbar_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 2.3,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
	      y_min = 0.01,
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut",
        empty_scale = 2.2,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut",
        empty_scale = 2.3,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut",
        empty_scale = 2.2,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "BDT_Top",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["BDT Top-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = False,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "BDT_W",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["BDT W-tagged"],
        x_min = 0,
        x_max = 350,
        flip_legend = True,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = False,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "DNN_Top",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["DNN Top-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = False,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "DNN_W",
        empty_scale = 2.0,
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["DNN W-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = False,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "SDt_dcut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["ShD Top-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 25,
        do_systematics = False,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "SDw_dcut",
        empty_scale = 2.0,
        flip_legend = True,
        extra_legend_lines = DEF_LINES + ["ShD W-tagged"],
        x_min = 0,
        x_max = 350,
        ttbar_sf = 25,
        wzjets_sf = 5,
        do_systematics = False,
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
            ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_Split23__combMgt100GeV",
            empty_scale = 4,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200,
	          y_min = 1e1 + 0.1,
	          ttbar_sf = 25,
            rebin = 4,
            ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_D2__combMgt50GeV",
            empty_scale = 2.0,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
            x_units = "",
            rebin = 4,
            x_max = 4.0,
            y_min = 0.01,
            ))

for masstag in ["", "combMgt100GeV", "combMlt100GeV", "combMgt180GeV", "combMgt100lt150GeV", "combMgt150lt180GeV"]:
  data_mc_plots.append(PlotDataPythiaHerwig(
              "h_rljet0_Tau32_wta" if not masstag else "h_rljet0_Tau32_wta__" + masstag,
              empty_scale = 2.0,
              flip_legend = True,
              extra_legend_lines = DEF_LINES,
              x_units = "",
              x_min = 0.01,
              x_max = 0.9,
              y_min = 0.01,
              rebin = 4
              ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_Qw__combMgt100GeV",
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200,
	          y_min = 1e1 + 0.1,
            rebin = 2,
            empty_scale = 4,
	          ttbar_sf = 25
            ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_atan1312",
        empty_scale = 1.9,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 5,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m23m123",
        empty_scale = 2.0,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 8,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m",
        empty_scale = 2.0,
        x_min = 20,
        x_max = 250,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
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
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_pt",
        empty_scale = 4,
        extra_legend_lines = HTT_DEF_LINES,
        x_min = 450,
        x_max = 3000,
        log_scale = True,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_pt_HTT_CAND",
        empty_scale = 6,
        extra_legend_lines = HTT_DEF_LINES + ["HTT-Tagged"],
        x_min = 450,
        x_max = 3000,
        log_scale = True,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb",
        empty_scale = 6,
        extra_legend_lines = DEF_LINES,
        x_min = 450,
        x_max = 4000,
        log_scale = True,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_BDT_score_top",
        empty_scale = 3,
        x_units = "",
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        y_min = 1e2,
        ttbar_sf = 15,
        wzjets_sf = 20,
        rebin = 4,
        log_scale = True,
        do_systematics = False
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_DNN_score_top",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        x_units = "",
        y_min = 1e2,
        ttbar_sf = 15,
        wzjets_sf = 20,
        log_scale = True,
        rebin = 4,
        do_systematics = False
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_BDT_score_w",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        x_units = "",
        ttbar_sf = 20,
        wzjets_sf = 10,
        rebin = 4,
        y_min = 1e2,
        log_scale = True,
        do_systematics = False
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_DNN_score_w",
        empty_scale = 3,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 40 GeV"],
        x_units = "",
        y_min = 1e2,
        ttbar_sf = 20,
        wzjets_sf = 10,
        log_scale = True,
        rebin = 4,
        do_systematics = False
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SD_logchi_t_calib",
        empty_scale = 2.2,
        flip_legend = True,
        do_systematics = False,
        x_units = "",
        extra_legend_lines = DEF_LINES,
        x_min = -10,
        x_max = 10,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SD_logchi_t_uncalib",
        empty_scale = 2.2,
        do_systematics = False,
        flip_legend = True,
        x_units = "",
        extra_legend_lines = DEF_LINES,
        x_min = -10,
        x_max = 10,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SD_logchi_t_combined",
        empty_scale = 2.2,
        do_systematics = False,
        x_units = "",
        flip_legend = True,
        extra_legend_lines = DEF_LINES,
        x_min = -10,
        x_max = 10,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SD_logchi_t_dcut",
        empty_scale = 2.2,
        do_systematics = False,
        x_units = "",
        flip_legend = True,
        extra_legend_lines = DEF_LINES,
        x_min = -10,
        x_max = 10,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16WTag_50eff_MassJSSCut",
        empty_scale = 10,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        log_scale = True,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16ZTag_50eff_MassJSSCut",
        empty_scale = 10,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        log_scale = True,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16WTag_80eff_JSSCut",
        empty_scale = 10,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        log_scale = True,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
        empty_scale = 7,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        log_scale = True,
        y_min = 10,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_pt_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 7,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        log_scale = True,
        y_min = 10,
        x_min = 450,
        x_max = 4000,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = 8,
        ))

#data_mc_plots.append(PlotDataPythiaHerwig( "h_NPV",
#        empty_scale = 2.0,
#        extra_legend_lines = DEF_LINES,
#        do_systematics = False,
#        x_units = "",
#        x_min = 0,
#        x_max = 40,
#        ttbar_sf = 25,
#        wzjets_sf = 12
#        ))
#
#data_mc_plots.append(PlotDataPythiaHerwig( "h_mu",
#        empty_scale = 2.0,
#        extra_legend_lines = DEF_LINES,
#        do_systematics = False,
#        x_units = "",
#        x_min = 5,
#        x_max = 45,
#        ttbar_sf = 25,
#        wzjets_sf = 12
#        ))
#
#data_mc_plots.append(PlotDataPythiaHerwig( "h_mu_NPV_sf",
#        empty_scale = 2.0,
#        extra_legend_lines = DEF_LINES,
#        do_systematics = False,
#        x_units = "",
#        x_min = 5,
#        x_max = 45,
#        ttbar_sf = 25,
#        wzjets_sf = 12
#        ))
#
#data_mc_plots.append(PlotDataPythiaHerwig( "h_mu_lumi_NPV_sf",
#        empty_scale = 2.0,
#        extra_legend_lines = DEF_LINES,
#        do_systematics = False,
#        x_units = "",
#        x_min = 5,
#        x_max = 45,
#        ttbar_sf = 25,
#        wzjets_sf = 12
#        ))

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
