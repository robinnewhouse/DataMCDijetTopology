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

CP_ROOT_FILEPATH = "/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/gammajet/02-05-2017__10:41:22__30042017_gammajet_test_v2/cp.merged.root"
RAW = DMDLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/pubplots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

SYSTEMATICS = [ ]
#SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM

class PlotDataMcGammaJet(PlotBase):
    def __init__(self, var_name, flip_legend = False, **kwargs):

        super(PlotDataMcGammaJet, self).__init__(
                lumi_val = "36.5",
                width = 600,
                name = var_name,
                atlas_mod = "Preliminary",
                legend_loc = [0.20,0.93,0.56,0.55] if flip_legend else [0.60,0.93,0.90,0.65],
                atlas_loc = [0.6,0.9] if flip_legend else None,
                extra_lines_loc = [0.6,0.8] if flip_legend else None,
                x_title = get_axis_title(var_name),
                **kwargs)

        self.var_name  = var_name

        # print var_name, "\n"

        self.h_data    = RAW.get_hist(["data", "nominal"] , var_name)
        self.h_gamma   = RAW.get_hist(["sherpa_gammajet", "nominal"] , var_name)
        self.h_dijet   = RAW.get_hist(["pythia_dijet", "nominal"] , var_name)
        self.h_wzgamma = RAW.get_hist(["sherpa_wz_gamma", "nominal"] , var_name)
        self.h_ttbar   = RAW.get_hist(["ttbar_gamma", "nominal"] , var_name)

        # gamma_sys_dict = {}
        # self.hsys_gamma = TH1Sys(self.h_gamma, gamma_sys_dict)

        # self.h_dijet.Smooth(1)

        all_mc_histos = [
            self.h_gamma,
            self.h_dijet,
            self.h_wzgamma,
            self.h_ttbar
            ]

        all_histos = all_mc_histos + [ self.h_data ]

        if (self.rebin != None):
          for h in all_histos:
            h.Rebin(self.rebin)

        total_mc_integral = self.h_gamma.Integral() +  self.h_wzgamma.Integral() + self.h_ttbar.Integral() #+ self.h_dijet.Integral()
        print self.h_data.Integral() / total_mc_integral
        for h in all_mc_histos:
          h.Scale(self.h_data.Integral() / total_mc_integral)

        for h in all_histos: self.set_x_axis_bounds(h)
        self.determine_y_axis_title(self.h_data)

        for h in all_histos:
            h.GetYaxis().SetTitle("Events / " + self.x_units if self.x_units else "Events")
            h.GetXaxis().SetLabelSize(0)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pad_empty_space(all_histos)

        self.hs = THStack("hs_gammajet", "hs_gammajet")
        self.hs.Add(self.h_ttbar)
        self.hs.Add(self.h_wzgamma)
        #self.hs.Add(self.h_dijet)
        self.hs.Add(self.h_gamma)

        self.h_stack = self.h_ttbar.Clone("h_stack")
        self.h_stack.Add(self.h_wzgamma)
        #self.h_stack.Add(self.h_dijet)
        self.h_stack.Add(self.h_gamma)

        self.h_stack_err = self.h_stack.Clone("h_stack_err")
        set_mc_sys_err_style(self.h_stack_err)
        self.h_stack_err.SetFillStyle(3144)
        self.h_stack_err.SetFillColorAlpha(1,0.3)

        # print "dijet %: ", 100 * self.h_dijet.Integral() / total_mc_integral
        # print "gamma %: ", 100 * self.h_gamma.Integral() / total_mc_integral
        # print "wzgamma %: ", 100 * self.h_wzgamma.Integral() / total_mc_integral
        # print "ttbar %: ", 100 * self.h_ttbar.Integral() / total_mc_integral
        # print ""

        # self.hsys_gamma._compute_errors()
        # self.h_gamma_sys = self.hsys_gamma.get_histo_with_systematic_errs()

        # CREATE RATIO PLOTS

        self.h_ratio = self.h_stack.Clone("h_ratio")
        self.h_ratio.Divide(self.h_data, self.h_ratio, 1, 1, "")
        self.h_stat_ratio = self.h_ratio.Clone("h_ratio")

        # center the stat. and syst. errors around data/mc ratio = 1.0
        for ibin in range(self.h_stat_ratio.GetSize()):
            self.h_stat_ratio.SetBinContent(ibin, 1.0)

        ratio_title = "#frac{Data}{MC}"
        for h in [self.h_ratio, self.h_stat_ratio]:
            set_style_ratio(h, y_title = ratio_title, y_min = 0.0, y_max = 2.0)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetXaxis().SetTitleOffset(4.0)
            h.GetYaxis().SetTitleOffset(2.0)
            h.GetXaxis().SetLabelSize(19)

        self.name = self.var_name + "_data_mc_gammajet"
        if self.log_scale: self.name += "_log"

        # TODO: necessary?
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        set_data_style_simple(self.h_data)
        set_data_style_simple(self.h_ratio)
        self.h_gamma.SetFillColor(kRed)
        self.h_dijet.SetFillColor(kBlue)
        self.h_wzgamma.SetFillColor(kGreen)
        self.h_ttbar.SetFillColor(kViolet)

        self.h_gamma.SetMarkerSize(0)
        self.h_dijet.SetMarkerSize(0)
        self.h_wzgamma.SetMarkerSize(0)
        self.h_ttbar.SetMarkerSize(0)

        self.h_gamma.SetLineWidth(0)
        self.h_dijet.SetLineWidth(0)
        self.h_wzgamma.SetLineWidth(0)
        self.h_ttbar.SetLineWidth(0)

        set_mc_sys_err_style(self.h_stat_ratio)
        self.h_stat_ratio.SetFillStyle(3144)
        self.h_stat_ratio.SetFillColorAlpha(1,0.3)

        #self.h_gamma_ratio.SetFillStyle(0)
        #self.h_gamma_ratio.SetFillColorAlpha(kRed, 0.85)

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

        #self.h_gamma.Draw("PE")
        #self.h_wzgamma.Draw("hist,same")
        #self.h_dijet.Draw("hist,same")
        self.h_data.Draw("PE,same")
        self.hs.Draw("histo,same")
        self.h_stack_err.Draw("E2,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2015 + 2016")
        self.leg.AddEntry(self.h_gamma, "Sherpa #gamma + jet")
        self.leg.AddEntry(self.h_dijet, "Pythia8 dijet")
        self.leg.AddEntry(self.h_wzgamma, "Sherpa W/Z + #gamma")
        self.leg.AddEntry(self.h_ttbar, "MG t#bar{t} + #gamma")
        self.leg.AddEntry(self.h_stat_ratio, "Stat. uncert.")
        self.leg.Draw()

        self.pad2.cd()

        #self.h_herwig_sys_ratio.Draw("E2,same")
        self.h_stat_ratio.Draw("E2,same")
        self.h_ratio.Draw("PE,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.canvas.Clear()

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "#gamma + jet selection" ]
MASS_PLOT_REBIN = 8

data_mc_plots = []

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
        empty_scale = 2.1,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 20,
        x_max = 400,
        y_min = 1,
        log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 20,
        x_max = 400,
        y_min = 1,
        log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff",
        empty_scale = 2.4,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 90,
        x_max = 400,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 70,
        x_max = 400,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff",
        empty_scale = 2.45,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 50.01,
        x_max = 400,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff",
        empty_scale = 1.8,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 50,
        x_max = 400,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 1.6,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        x_min = 20,
	      y_min = 0.01,
        x_max = 300,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
        x_min = 20,
	      y_min = 0.01,
        x_max = 300,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut",
        empty_scale = 1.6,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
        x_min = 20,
	      y_min = 0.01,
        x_max = 300,
        #y_max = 20e3,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
        x_min = 20,
	      y_min = 0.01,
        x_max = 300,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots += [
        PlotDataMcGammaJet("h_rljet0_m_comb",
            empty_scale = 2.5,
            extra_legend_lines = DEF_LINES,
            x_max = 400,
            #y_min = 6e3 + 0.1,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotDataMcGammaJet("h_rljet0_pt_comb",
            empty_scale = 2.5,
            extra_legend_lines = DEF_LINES,
            x_max = 2000,
            x_min = 200,
            #y_min = 6e3 + 0.1,
            log_scale = True,
            rebin = 4,
            ),

        PlotDataMcGammaJet("h_rljet0_Split23__combMgt100GeV",
            empty_scale = 6,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200e3,
            x_units = "MeV",
            #x_max = 200,
            #x_axis_scale = 0.001,
            ),

        PlotDataMcGammaJet("h_rljet0_D2__combMgt50GeV",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
            x_units = "",
            rebin = 4,
            x_max = 4.0,
            y_min = 0.01,
            ),

        PlotDataMcGammaJet("h_rljet0_Tau32_wta__combMgt100GeV",
            empty_scale = 1.9,
            flip_legend = True,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            x_units = "",
            x_min = 0.01,
            x_max = 0.9,
            y_min = 0.01,
            rebin = 4
            ),

        PlotDataMcGammaJet("h_rljet0_Qw__combMgt100GeV",
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200e3,
            x_units = "MeV",
            rebin = 2,
	          #y_min = 10e0 + 0.1,
            # x_axis_scale = 0.001,
            empty_scale = 5,
            )
        ]
