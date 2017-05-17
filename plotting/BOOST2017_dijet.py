from ROOT import *
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

RAW = DMDLoader("./raw/dijet/17-02-2017__15:37:30__02072017_gridjobs_withQwSplit23Sys/cp.merged.root")
ROOT_OUTPUT_DIR = "./raw/dijet/17-02-2017__15:37:30__02072017_gridjobs_withQwSplit23Sys/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/BOOST2017"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

MAKE_ROOT_FILES = False
LOG_FILE = open(OUTPUT_DIR + "/BOOST2017.log", 'w')

#SYSTEMATICS = [ ]
SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM

PYTHIA_COLOR = kRed - 3
HERWIG_COLOR = kBlue - 3
WJETS_COLOR = kOrange+7
ZJETS_COLOR = kPink
TTBAR_COLOR = kViolet

class PlotDataPythiaHerwig(PlotBase):
    def __init__(self, var_name, flip_legend = False,
                 wzjets_sf = 50, ttbar_sf = 50, **kwargs):

        super(PlotDataPythiaHerwig, self).__init__(
                lumi_val = "36.5",
                width = 600,
                name = var_name,
                atlas_mod = "Preliminary",
                legend_loc = [0.20,0.93,0.56,0.55] if flip_legend else [0.53,0.93,0.90,0.55],
                atlas_loc = [0.6,0.9] if flip_legend else None,
                extra_lines_loc = [0.6,0.8] if flip_legend else None,
                x_title = get_axis_title(var_name),
                **kwargs)

        self.var_name  = var_name
        self.wzjets_sf = wzjets_sf
        self.ttbar_sf  = ttbar_sf

        LOG_FILE.write( (4 + len(var_name)) * '#' + '\n' )
        LOG_FILE.write( "# " + var_name + " #"  + '\n')
        LOG_FILE.write( (4 + len(var_name)) * '#'  + '\n')

        # GET PLOTS FROM FILE

        self.h_data = RAW.get_hist(["data", "nominal"] , var_name)

        self.h_pythia_dijet = RAW.get_hist(["pythia_dijet", "nominal"] , var_name)
        self.h_herwig_dijet = RAW.get_hist(["herwig_dijet", "nominal"] , var_name)

        self.h_wjets = RAW.get_hist(["pythia_wjets", "nominal"] , var_name)
        self.h_zjets = RAW.get_hist(["pythia_zjets", "nominal"] , var_name)
        self.h_ttbar = RAW.get_hist(["ttbar_allhad", "nominal"] , var_name)

        # NORMALIZE PLOTS

        self.h_wjets.Scale(PYTHIA_SHERPA_WJETS_SF)
        self.h_zjets.Scale(PYTHIA_SHERPA_ZJETS_SF)

        h_data_sigsub = self.h_data.Clone(self.h_data.GetName() + "_sigsub")
        h_data_sigsub.Add(self.h_wjets, -1.0)
        h_data_sigsub.Add(self.h_zjets, -1.0)
        h_data_sigsub.Add(self.h_ttbar, -1.0)

        self.pythia_dijet_SF = h_data_sigsub.Integral() / self.h_pythia_dijet.Integral()
        self.herwig_dijet_SF = h_data_sigsub.Integral() / self.h_herwig_dijet.Integral()

        # LOAD SYSTEMATICS FOR PYTHIA DIJET ONLY

        if ("htt" in var_name):
           pythia_sys_dict = RAW.get_htt_systematics_dict(var_name, "pythia_dijet", "sjcalib1030", "sjcalib0970")
        elif ("SD" in var_name):
           pythia_sys_dict = {}
        else:
           pythia_sys_dict = RAW.get_systematics_dict(var_name, SYSTEMATICS, [ "pythia_dijet" ])

        self.hsys_pythia = TH1Sys(self.h_pythia_dijet, pythia_sys_dict)
        self.hsys_pythia.scale(self.pythia_dijet_SF)
        self.h_pythia_dijet.Scale(self.pythia_dijet_SF)
        self.h_herwig_dijet.Scale(self.herwig_dijet_SF)

        # LOG THE SCALE FACTORS FOR POSTERITY

        LOG_FILE.write("\n")
        LOG_FILE.write("data yield (before signal subtraction): " + str(self.h_data.Integral()) + "\n")
        LOG_FILE.write("data yield (after signal subtraction): "  + str(h_data_sigsub.Integral()) + "\n")
        LOG_FILE.write("\n")
        LOG_FILE.write("wjets yield: " + str(self.h_wjets.Integral()) + "\n")
        LOG_FILE.write("zjets yield: " + str(self.h_zjets.Integral()) + "\n")
        LOG_FILE.write("ttbar yield: " + str(self.h_ttbar.Integral()) + "\n")
        LOG_FILE.write("\n")
        LOG_FILE.write("pythia dijet SF: "    + str(self.pythia_dijet_SF) + "\n")
        LOG_FILE.write("pythia dijet yield: " + str(self.h_pythia_dijet.Integral()) + "\n")
        LOG_FILE.write("herwig dijet SF: "    + str(self.herwig_dijet_SF) + "\n")
        LOG_FILE.write("herwig dijet yield: " + str(self.h_herwig_dijet.Integral()) + "\n")
        LOG_FILE.write("\n")

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

        # in ratio plots, include contribution of W/Z+jets and ttbar
        for h in [ self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio]:
            h.Add(self.h_wjets)
            h.Add(self.h_zjets)
            h.Add(self.h_ttbar)

        self.h_pythia_dijet_ratio.Divide(self.h_data, self.h_pythia_dijet_ratio, 1, 1, "")
        self.h_herwig_dijet_ratio.Divide(self.h_data, self.h_herwig_dijet_ratio, 1, 1, "")
        self.h_pythia_sys_ratio.Divide(self.h_data, self.h_pythia_sys_ratio, 1, 1, "")

        self.h_pythia_stat_ratio = self.h_pythia_dijet_ratio.Clone("h_pythia_stat_ratio")

        # center the stat. and syst. errors around data/mc ratio = 1.0
        for ibin in range(self.h_pythia_sys_ratio.GetSize()):
            self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)
            self.h_pythia_stat_ratio.SetBinContent(ibin, 1.0)

        ratio_title = "#frac{Data}{MC(sig. + bkg.)}"
        for h_ratio in [self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio, self.h_pythia_sys_ratio, self.h_pythia_stat_ratio]:
            set_style_ratio(h_ratio, y_title = ratio_title)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            #self.set_x_axis_bounds(h_ratio)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)
            h_ratio.GetXaxis().SetLabelSize(19)

    def make_data_pythia_herwig_plot(self, output_dir):

        self.name = self.var_name + "_data_mc_dijet"
        if self.log_scale: self.name += "_log"

        # TODO: necessary?
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        set_data_style_simple(self.h_data)
        set_mc_style_marker(self.h_pythia_dijet, PYTHIA_COLOR, shape = 21, line_style = 3)
        set_mc_style_marker(self.h_herwig_dijet, HERWIG_COLOR, shape = 22, line_style = 4)

        h_wjets_mag = self.h_wjets.Clone("h_wjets_mag")
        h_zjets_mag = self.h_zjets.Clone("h_zjets_mag")
        h_ttbar_mag = self.h_ttbar.Clone("h_ttbar_mag")
        h_wjets_mag.Scale(self.wzjets_sf)
        h_zjets_mag.Scale(self.wzjets_sf)
        h_ttbar_mag.Scale(self.ttbar_sf)
        set_mc_style_line(h_wjets_mag, WJETS_COLOR, 1, 2)
        set_mc_style_line(h_zjets_mag, ZJETS_COLOR, 1, 2)
        set_mc_style_line(h_ttbar_mag, TTBAR_COLOR, 1, 2)

        set_mc_sys_err_style(self.h_pythia_sys)

        self.h_pythia_sys.SetFillColorAlpha(PYTHIA_COLOR, 0.3)

        set_mc_style_line(self.h_pythia_dijet_ratio, PYTHIA_COLOR, line_width = 3)
        set_mc_sys_err_style(self.h_pythia_sys_ratio)
        set_mc_style_line(self.h_herwig_dijet_ratio, HERWIG_COLOR, line_width = 3)

        self.h_pythia_sys_ratio.SetMarkerSize(0)
        self.h_pythia_stat_ratio.SetMarkerSize(0)
        self.h_pythia_sys_ratio.SetFillColor(kGreen-8)
        self.h_pythia_stat_ratio.SetFillColor(kGreen-5)

        self.h_pythia_stat_ratio.SetFillStyle(1001)
        self.h_pythia_stat_ratio.SetLineWidth(0)

        self.h_pythia_dijet_ratio.SetFillStyle(0)
        self.h_pythia_dijet_ratio.SetFillColorAlpha(PYTHIA_COLOR, 0.85)
        self.h_herwig_dijet_ratio.SetFillStyle(0)
        self.h_herwig_dijet_ratio.SetFillColorAlpha(HERWIG_COLOR, 0.85)

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

        self.h_herwig_dijet.Draw("PE,same")
        self.h_pythia_dijet.Draw("PE,same")
        h_wjets_mag.Draw("hist,same")
        h_zjets_mag.Draw("hist,same")
        h_ttbar_mag.Draw("hist,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2015+2016")
        self.leg.AddEntry(self.h_pythia_dijet, "Pythia8 dijet (#times " + '{0:.2f}'.format(self.pythia_dijet_SF) + ")" )
        self.leg.AddEntry(self.h_herwig_dijet, "Herwig++ dijet (#times " + '{0:.2f}'.format(self.herwig_dijet_SF) + ")" )
        self.leg.AddEntry(h_wjets_mag, "W+jets (#times " + '{0:.0f}'.format(self.wzjets_sf) + ")"  )
        self.leg.AddEntry(h_zjets_mag, "Z+jets (#times " + '{0:.0f}'.format(self.wzjets_sf) + ")" )
        self.leg.AddEntry(h_ttbar_mag, "all-had t#bar{t} (#times " + '{0:.0f}'.format(self.ttbar_sf) + ")" )
        self.leg.AddEntry(self.h_pythia_stat_ratio, "Stat. uncert.")
        if (self.hsys_pythia.num_systematics != 0):
            self.leg.AddEntry(self.h_pythia_sys_ratio, "Stat. #oplus syst. uncert.")
        self.leg.Draw()

        self.pad2.cd()

        #self.h_herwig_sys_ratio.Draw("E2,same")
        if (self.hsys_pythia.num_systematics != 0):
            self.h_pythia_sys_ratio.Draw("E2,same")
        self.h_pythia_stat_ratio.Draw("E2,same")
        self.h_herwig_dijet_ratio.Draw("hist,same")
        self.h_pythia_dijet_ratio.Draw("hist,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(output_dir + "/" + self.name + ".pdf")
        self.canvas.Clear()

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" , "p_{T} > 450 GeV"]
MASS_PLOT_REBIN = 8

data_mc_plots = []

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
        empty_scale = 2.6,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        log_scale = True,
        x_min = 0,
        x_max = 400,
        y_min = 1600.01,
        ttbar_sf = 5,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 2.6,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        log_scale = True,
        x_min = 0,
        x_max = 400,
        y_min = 10000.01,
        ttbar_sf = 5,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff",
        empty_scale = 2.4,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 90,
        x_max = 400,
        y_min = 0.01,
        ttbar_sf = 5,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff",
        empty_scale = 2.2,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 70,
        x_max = 400,
        y_min = 0.01,
        ttbar_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff",
        empty_scale = 2.5,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 50.01,
        x_max = 400,
        y_min = 0.01,
        ttbar_sf = 5,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 50,
        x_max = 400,
        y_min = 0.01,
        ttbar_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 1.4,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        x_min = 0,
	      y_min = 0.01,
        x_max = 400,
        ttbar_sf = 25,
        wzjets_sf = 10,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut",
        empty_scale = 2.2,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        x_min = 0,
        x_max = 400,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut",
        empty_scale = 1.4,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
        x_min = 0,
        x_max = 400,
        ttbar_sf = 25,
        wzjets_sf = 12,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut",
        empty_scale = 2.2,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
        x_min = 0,
        x_max = 400,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_m_comb",
            empty_scale = 2.5,
            extra_legend_lines = DEF_LINES,
            x_max = 400,
            y_min = 6e3 + 0.1,
            wzjets_sf = 25,
            ttbar_sf = 25,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_Split23__combMgt100GeV",
            empty_scale = 6,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200,
            x_axis_scale = 0.001,
	          ttbar_sf = 25
            ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_D2__combMgt50GeV",
            empty_scale = 2.0,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
            x_units = "",
            rebin = 4,
            x_max = 4.0,
            y_min = 0.01,
            ))

data_mc_plots.append(PlotDataPythiaHerwig("h_rljet0_Tau32_wta__combMgt100GeV",
            empty_scale = 2.0,
            flip_legend = True,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
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
	          y_min = 10e2 + 0.1,
            x_axis_scale = 0.001,
            rebin = 5,
            empty_scale = 5,
	          ttbar_sf = 25
            ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_atan1312",
        empty_scale = 1.9,
        extra_legend_lines = DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 3,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m23m123",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 3,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt0_m",
        empty_scale = 2.15,
        x_min = 20,
        x_max = 250,
        extra_legend_lines = DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 3,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_htt_caGroomJet0_m",
        empty_scale = 2.15,
        x_min = 0,
        x_max = 1000,
        extra_legend_lines = DEF_LINES + ["HTT-tagged"],
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 10,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SDw_win20_btag0_logchi",
        empty_scale = 1.9,
        extra_legend_lines = DEF_LINES,
        flip_legend = True,
        x_min = -6,
        x_max = 6,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 3,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_SDt_win50_btag0_logchi",
        empty_scale = 1.9,
        extra_legend_lines = DEF_LINES,
        flip_legend = True,
        x_min = -10,
        x_max = 10,
        ttbar_sf = 25,
        wzjets_sf = 25,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_SDw_win20_btag0",
        empty_scale = 1.9,
        flip_legend = True,
        extra_legend_lines = DEF_LINES,
        x_min = 0,
        x_max = 150,
        ttbar_sf = 25,
        wzjets_sf = 5,
        rebin = MASS_PLOT_REBIN/2,
        ))

data_mc_plots.append(PlotDataPythiaHerwig( "h_rljet0_m_comb_SDt_win50_btag0",
        empty_scale = 1.9,
        flip_legend = True,
        extra_legend_lines = DEF_LINES,
        x_min = 0,
        x_max = 300,
        ttbar_sf = 5,
        wzjets_sf = 25,
        rebin = MASS_PLOT_REBIN,
        ))

for plot in data_mc_plots:
    plot.make_data_pythia_herwig_plot(OUTPUT_DIR)

LOG_FILE.close()
