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

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/gammajet_20180219/merged.cp.root"
RAW = GammaJetLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"
DO_SYSTEMATICS_DEFAULT = False

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/pubplots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

#SYSTEMATICS = [ ]
PHOTON_SYSTEMATICS = [
    "photonSF_ID"
    "photonSF_effTrkIso"
    ]
SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM + PHOTON_SYSTEMATICS

class PlotDataMcGammaJet(PlotBase):
    def __init__(self, var_name, flip_legend = False, do_systematics = DO_SYSTEMATICS_DEFAULT, **kwargs):
        super(PlotDataMcGammaJet, self).__init__(
                lumi_val = "36.1",
                width = 600,
                name = var_name,
                atlas_mod = "Internal",
                legend_loc = [0.20,0.93,0.56,0.70] if flip_legend else [0.59,0.93,0.89,0.70],
                atlas_loc = [0.6,0.9] if flip_legend else None,
                extra_lines_loc = [0.6,0.82] if flip_legend else None,
                tex_size_mod = 0.95,
                tex_spacing_mod = 0.75,
                x_title = get_axis_title(var_name),
                **kwargs)

        self.h_data = RAW.get_hist(["data"            , "nominal"] , var_name)

        self.hs    = RAW.get_stack_plot(var_name)
        self.h_sum = RAW.get_sum_plot(var_name, "nominal")
        self.h_gamma = RAW.get_normalized_gamma(var_name, "sherpa_gammajet", "nominal")
        self.h_sum_stat = self.h_sum.Clone(self.h_sum.GetName() + "_stat")

        if (do_systematics):
            gammajet_sysdict = RAW.get_systematics_dictionary(var_name, SYSTEMATICS, "sherpa_gammajet", True)
        else:
            gammajet_sysdict = {}

        self.hsys_sum = TH1Sys(self.h_gamma, gammajet_sysdict)
        self.hsys_sum._compute_errors()
        self.h_sum_sys = self.hsys_sum.get_histo_with_systematic_errs()
        self.h_sum_sys.SetName(self.h_sum.GetName() + "_sys")
        self.h_sum_sys.Add(RAW.get_wzgamma(var_name))
        self.h_sum_sys.Add(RAW.get_ttbar(var_name))

        all_hists = [self.h_data, self.h_sum, self.h_sum_stat, self.h_sum_sys]

        for h in self.hs.GetHists():
          all_hists.append(h)

        for h in all_hists:
          if (self.rebin): h.Rebin(self.rebin)
          self.set_x_axis_bounds(h)

        self.determine_y_axis_title(self.h_data)

        for h in all_hists:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetLabelSize(0)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pad_empty_space(all_hists)


        # CREATE RATIO PLOTS

        self.h_ratio = self.h_sum.Clone("h_ratio")
        self.h_ratio.Divide(self.h_data, self.h_ratio, 1, 1, "")
        self.h_stat_ratio = self.h_ratio.Clone("h_ratio")
        self.h_sys_ratio = self.h_sum_sys.Clone("h_ratio_sys")
        self.h_sys_ratio.Divide(self.h_data, self.h_sys_ratio, 1, 1, "")

        # center the stat. and syst. errors around data/mc ratio = 1.0
        for ibin in range(self.h_stat_ratio.GetSize()):
            self.h_stat_ratio.SetBinContent(ibin, 1.0)
            self.h_sys_ratio.SetBinContent(ibin, 1.0)

        ratio_title = "#frac{Data}{MC}"
        for h in [self.h_ratio, self.h_stat_ratio, self.h_sys_ratio]:
            set_style_ratio(h, y_title = ratio_title, y_min = 0.5, y_max = 1.5)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetXaxis().SetTitleOffset(4.0)
            h.GetYaxis().SetTitleOffset(2.0)
            h.GetXaxis().SetLabelSize(19)

        set_mc_sys_err_style(self.h_sum_sys, col = kGreen+2)
        set_mc_sys_err_style(self.h_sys_ratio, col = kGreen - 8)
        set_mc_sys_err_style(self.h_stat_ratio, col = kGreen - 5)

        self.h_stat_ratio.SetFillColor(kGreen-5)
        self.h_stat_ratio.SetFillStyle(1001)

        self.name = var_name + "_data_mc_gammajet"
        if self.log_scale: self.name += "_log"

        # TODO: necessary?
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        set_data_style_simple(self.h_data)
        set_data_style_simple(self.h_ratio)

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

        self.h_data.Draw("PE,same")
        self.hs.Draw("histo,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data       , "Data")
        xss = self.hs.GetHists()
        xss.reverse()
        for h in xss:
          if ("gamma" in h.GetName()):
            self.leg.AddEntry(h      , "Sherpa #gamma + jet"  , "f")
          elif ("wz" in h.GetName()):
            self.leg.AddEntry(h    , "Sherpa W/Z + #gamma"  , "f")
          elif ("ttbar" in h.GetName()):
            self.leg.AddEntry(h      , "MG t#bar{t} + #gamma" , "f")
        self.leg.AddEntry(self.h_stat_ratio , "Stat. uncert.", "f")
        if (do_systematics):
            self.leg.AddEntry(self.h_sys_ratio , "Sys. #oplus Stat. uncert.", "f")
        self.leg.Draw()
        xss.reverse()

        self.pad2.cd()

        #self.h_herwig_sys_ratio.Draw("E2,same")
        if (do_systematics):
            self.h_sys_ratio.Draw("E2,same")
        self.h_stat_ratio.Draw("E2,same")
        self.h_ratio.Draw("PE,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.canvas.Clear()

SEL_LINE = [ "#gamma + jet selection" ]
DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0" ] + SEL_LINE
HTT_LINES = [ "Trimmed C/A #it{R}=1.5" ] + SEL_LINE
MASS_PLOT_REBIN = 5

data_mc_plots = []

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
        empty_scale = 2.1,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 1,
        log_scale = True,
        rebin = MASS_PLOT_REBIN + 4,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 4,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 1,
        log_scale = True,
        rebin = MASS_PLOT_REBIN + 4,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff",
        empty_scale = 2.4,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN+4,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN+4,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff",
        empty_scale = 2.45,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN + 4,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff",
        empty_scale = 1.8,
        extra_legend_lines = DEF_LINES + ["Top-tagged"],
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN + 4,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
	      y_min = 0.01,
        x_min = 0,
        x_max = 350,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["W-tagged"],
	      y_min = 0.01,
        x_min = 0,
        x_max = 350,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
	      y_min = 0.01,
        x_min = 0,
        x_max = 350,
        #y_max = 20e3,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["Z-tagged"],
	      y_min = 0.01,
        x_min = 0,
        x_max = 350,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "BDT_Top",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["BDT Top-tagged"],
        do_systematics = False,
	      y_min = 0.01,
        x_min = 0,
        x_max = 350,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "BDT_W",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["BDT W-tagged"],
        do_systematics = False,
        x_min = 0,
        x_max = 350,
	      y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "DNN_Top",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["DNN Top-tagged"],
        do_systematics = False,
        x_min = 0,
	      y_min = 0.01,
        x_max = 350,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "DNN_W",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["DNN W-tagged"],
        do_systematics = False,
        x_min = 0,
        x_max = 350,
	      y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

#data_mc_plots.append(PlotDataMcGammaJet( "h_mu",
#        empty_scale = 2.0,
#        extra_legend_lines = DEF_LINES,
#        do_systematics = False,
#        x_units = "",
#        x_min = 5,
#        x_max = 45,
#        rebin = 4,
#        ))
#
#data_mc_plots.append(PlotDataMcGammaJet( "h_mu_corrSF",
#        empty_scale = 2.0,
#        extra_legend_lines = DEF_LINES,
#        do_systematics = False,
#        x_units = "",
#        x_min = 5,
#        x_max = 45,
#        rebin = 4,
#        ))

data_mc_plots += [
        PlotDataMcGammaJet("h_rljet0_m_comb",
            empty_scale = 2.5,
            extra_legend_lines = DEF_LINES,
            x_min = 50,
            x_max = 350,
            #y_min = 6e3 + 0.1,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotDataMcGammaJet("h_rljet0_pt_comb",
            empty_scale = 3.0,
            extra_legend_lines = DEF_LINES,
            x_max = 2000,
            x_min = 200,
            log_scale = True,
            rebin = 4,
            ),

        PlotDataMcGammaJet("h_rljet0_pt_comb_smooth16WTag_50eff_JSSCut",
            empty_scale = 2.5,
            extra_legend_lines = DEF_LINES,
            x_max = 600,
            x_min = 300,
            log_scale = True,
            ),

        PlotDataMcGammaJet("h_rljet0_Split23__combMgt100GeV",
            empty_scale = 5,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 140,
            ),

        PlotDataMcGammaJet("h_rljet0_D2__combMgt50GeV",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
            x_units = "",
            rebin = 4,
            x_max = 4.0,
            y_min = 0.01,
            ),

        PlotDataMcGammaJet("h_rljet0_BDT_score_top",
            empty_scale = 4,
            do_systematics = False,
            x_units = "",
            rebin = 3,
            y_min = 0.1,
            log_scale = True,
            ),

        PlotDataMcGammaJet("h_rljet0_BDT_score_w",
            empty_scale = 4,
            do_systematics = False,
            x_units = "",
            rebin = 3,
            y_min = 0.1,
            log_scale = True,
            ),

        PlotDataMcGammaJet("h_rljet0_DNN_score_top",
            empty_scale = 4,
            do_systematics = False,
            x_units = "",
            rebin = 3,
            y_min = 0.1,
            log_scale = True,
            ),

        PlotDataMcGammaJet("h_rljet0_DNN_score_w",
            empty_scale = 4,
            do_systematics = False,
            x_units = "",
            rebin = 3,
            y_min = 0.1,
            log_scale = True,
            ),

        PlotDataMcGammaJet( "h_htt0_atan1312",
               empty_scale = 1.9,
               extra_legend_lines = ["HTT-tagged"] + HTT_LINES,
               rebin = 5,
               ),

        PlotDataMcGammaJet( "h_htt0_m23m123",
               empty_scale = 2.0,
               extra_legend_lines = ["HTT-tagged"] + HTT_LINES,
               rebin = 8,
               ),

        PlotDataMcGammaJet( "h_htt0_m",
               empty_scale = 2.35,
               x_min = 20,
               x_max = 250,
               extra_legend_lines = ["HTT-tagged"] + HTT_LINES,
               rebin = 4,
               ),

        PlotDataMcGammaJet( "h_htt_caGroomJet0_m",
                empty_scale = 2.15,
                log_scale = True,
                x_min = 0,
                x_max = 350,
                extra_legend_lines = HTT_LINES,
                rebin = 10,
                ),

        PlotDataMcGammaJet( "h_htt_caGroomJet0_m_HTT_CAND",
                empty_scale = 2.15,
                log_scale = True,
                x_min = 0,
                x_max = 350,
                extra_legend_lines = HTT_LINES,
                rebin = 10,
                ),

        PlotDataMcGammaJet( "h_htt_caGroomJet0_pt",
                empty_scale = 2.15,
                log_scale = True,
                x_min = 200,
                x_max = 3000,
                extra_legend_lines = HTT_LINES,
                rebin = 4,
                ),

        PlotDataMcGammaJet( "h_htt_caGroomJet0_pt_HTT_CAND",
                empty_scale = 2.15,
                log_scale = True,
                x_min = 200,
                x_max = 2500,
                extra_legend_lines = HTT_LINES,
                rebin = 4,
                ),

        PlotDataMcGammaJet("h_rljet0_Qw__combMgt100GeV",
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200,
            rebin = 2,
            empty_scale = 5,
            )
        ]

for masstag in ["", "combMgt100GeV", "combMlt100GeV", "combMgt180GeV", "combMgt100lt150GeV", "combMgt150lt180GeV"]:
  PlotDataMcGammaJet(
              "h_rljet0_Tau32_wta" if not masstag else "h_rljet0_Tau32_wta__" + masstag,
              empty_scale = 2.0,
              flip_legend = True,
              extra_legend_lines = DEF_LINES,
              x_units = "",
              x_min = 0.01,
              x_max = 0.9,
              y_min = 0.01,
              rebin = 4
              )
