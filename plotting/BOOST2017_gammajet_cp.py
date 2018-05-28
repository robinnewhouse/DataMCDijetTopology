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

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/gammajet_20180504_syst/gammajet.merged.cp.root"
HISTLOADER = GammaJetLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_GammaJet"
DO_SYSTEMATICS_DEFAULT = SYSTEMATICS_MC15C_WEAK_GAMMAJET

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/control"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)


class PlotDataMcGammaJet(PlotBase):
    def __init__(self, var_name, flip_legend = False, do_systematics = DO_SYSTEMATICS_DEFAULT, suffix = "", **kwargs):
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
        self.h_data = HISTLOADER.get_hist(["data"            , "nominal"] , var_name)

        self.hs_sherpa    = HISTLOADER.get_stack_plot(var_name, generator = "sherpa_gammajet")
        self.hs_pythia    = HISTLOADER.get_stack_plot(var_name, generator = "pythia_gammajet")
        self.h_sum_sherpa = HISTLOADER.get_sum_plot(var_name, "nominal", generator = "sherpa_gammajet")
        self.h_sum_pythia = HISTLOADER.get_sum_plot(var_name, "nominal", generator = "pythia_gammajet")
        self.h_gamma_sherpa = HISTLOADER.get_normalized_gamma("sherpa_gammajet", var_name, "nominal")
        self.h_gamma_pythia = HISTLOADER.get_normalized_gamma("pythia_gammajet", var_name, "nominal")
        self.h_sum_sherpa_stat = self.h_sum_sherpa.Clone(self.h_sum_sherpa.GetName() + "_stat")
        self.h_sum_pythia_stat = self.h_sum_pythia.Clone(self.h_sum_pythia.GetName() + "_stat")

        if (do_systematics):
            gammajet_sysdict = HISTLOADER.get_systematics_dictionary("sherpa_gammajet", var_name, do_systematics, True)
        else:
            gammajet_sysdict = {}

        self.hsys_sum = TH1Sys(self.h_gamma_sherpa, gammajet_sysdict)
        self.hsys_sum._compute_errors()
        self.h_sum_sherpa_sys = self.hsys_sum.get_histo_with_systematic_errs()
        self.h_sum_sherpa_sys.SetName(self.h_sum_sherpa.GetName() + "_sys")
        self.h_sum_sherpa_sys.Add(HISTLOADER.get_wzgamma(var_name))
        self.h_sum_sherpa_sys.Add(HISTLOADER.get_ttbar(var_name))
        # scale factors, just for the legend
        self.gamma_sherpa_SF = self.h_gamma_sherpa.Integral() / HISTLOADER.get_hist(["sherpa_gammajet", "nominal"] , var_name).Integral()
        self.gamma_pythia_SF = self.h_gamma_pythia.Integral() / HISTLOADER.get_hist(["pythia_gammajet", "nominal"] , var_name).Integral()


        all_hists = [self.h_data, self.h_sum_sherpa, self.h_sum_sherpa_stat, self.h_sum_sherpa_sys, self.h_sum_pythia]

        for h in self.hs_sherpa.GetHists():
          all_hists.append(h)
        for h in self.hs_pythia.GetHists():
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

        self.h_sherpa_ratio = self.h_sum_sherpa.Clone("h_sherpa_ratio")
        self.h_pythia_ratio = self.h_sum_pythia.Clone("h_pythia_ratio")

        self.h_sherpa_ratio.Divide(self.h_data, self.h_sherpa_ratio, 1, 1, "")
        self.h_pythia_ratio.Divide(self.h_data, self.h_pythia_ratio, 1, 1, "")

        self.h_sys_ratio = self.h_sum_sherpa_sys.Clone("h_sherpa_ratio_sys")
        self.h_sys_ratio.Divide(self.h_data, self.h_sys_ratio, 1, 1, "")
        self.h_stat_ratio = self.h_sherpa_ratio.Clone("h_sherpa_ratio_stat")


        # center the stat. and syst. errors around data/mc ratio = 1.0
        for ibin in range(self.h_stat_ratio.GetSize()):
            self.h_stat_ratio.SetBinContent(ibin, 1.0)
            self.h_sys_ratio.SetBinContent(ibin, 1.0)

        # ratio_title = "#frac{Data}{MC}"
        ratio_title = "Data/Pred"
        for h in [self.h_sherpa_ratio, self.h_pythia_ratio, self.h_stat_ratio, self.h_sys_ratio]:
            set_style_ratio(h, y_title = ratio_title, y_min = 0.5, y_max = 1.5)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetXaxis().SetTitleOffset(4.0)
            h.GetYaxis().SetTitleOffset(2.0)
            h.GetXaxis().SetLabelSize(19)

        self.name = var_name + "_data_mc_gammajet"
        if self.log_scale: self.name += "_log"

        # TODO: necessary?
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        set_mc_sys_err_style(self.h_sum_sherpa_sys, col = kGreen+2)
        set_mc_sys_err_style(self.h_sys_ratio, col = kGreen - 8)
        set_mc_sys_err_style(self.h_stat_ratio, col = kGreen - 5)

        set_mc_style_line(self.h_sherpa_ratio, HISTLOADER.SHERPA_COLOR, line_width = 3, alpha = 0.9)
        set_mc_style_line(self.h_pythia_ratio, HISTLOADER.PYTHIA_COLOR, line_width = 3, alpha = 0.9)

        self.h_stat_ratio.SetFillColor(kGreen-5)
        self.h_stat_ratio.SetFillStyle(1001)

        set_data_style_simple(self.h_data)
        # set_data_style_simple(self.h_sherpa_ratio)
        # set_data_style_simple(self.h_pythia_ratio)


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
        self.hs_sherpa.Draw("histo,same")
        self.hs_pythia.Draw("histo,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data       , "Data")
        self.leg.AddEntry(self.h_gamma_sherpa, "Sherpa #gamma + jet (#times " + '{0:.2f}'.format(self.gamma_sherpa_SF) + ")", "f")
        self.leg.AddEntry(self.h_gamma_pythia, "Pythia8 #gamma + jet (#times " + '{0:.2f}'.format(self.gamma_pythia_SF) + ")", "f")

        xss = self.hs_sherpa.GetHists()
        xss.reverse()
        for h in xss:
          if ("gamma" in h.GetName()):
            # self.leg.AddEntry(h      , "Sherpa #gamma + jet"  , "f")
            pass
          elif ("wz" in h.GetName()):
            self.leg.AddEntry(h    , "Sherpa W/Z + #gamma"  , "f")
          elif ("ttbar" in h.GetName()):
            self.leg.AddEntry(h      , "MG t#bar{t} + #gamma" , "f")
        self.leg.AddEntry(self.h_stat_ratio , "Stat. uncert.", "f")
        if (do_systematics):
            self.leg.AddEntry(self.h_sys_ratio , "Total uncert.", "f")
        self.leg.Draw()
        xss.reverse()

        self.pad2.cd()

        #self.h_herwig_sys_ratio.Draw("E2,same")
        if (do_systematics):
            self.h_sys_ratio.Draw("E2,same")
        self.h_stat_ratio.Draw("E2,same")
        self.h_sherpa_ratio.Draw("hist,same")
        self.h_pythia_ratio.Draw("hist,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + suffix + ".pdf")
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.canvas.Clear()

SEL_LINE = [ "#gamma + jet selection" ]
W_PT_LINE = ["p_{T} > 200 GeV"]
TOP_PT_LINE = ["p_{T} > 350 GeV"]
TOPOTAGGER_PT_LINE = ["p_{T} > 450 GeV"]
DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0" ] + SEL_LINE
HTT_LINES = [ "Trimmed C/A #it{R}=1.5" ] + TOP_PT_LINE + SEL_LINE
MASS_PLOT_REBIN = 5

data_mc_plots = []

# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
#         empty_scale = 2.1,
#         extra_legend_lines = DEF_LINES + ["Top-tagged"],
#         x_min = 0,
#         x_max = 350,
#         y_min = 1,
#         log_scale = True,
#         rebin = MASS_PLOT_REBIN + 4,
#         ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 4,
        extra_legend_lines = DEF_LINES + TOP_PT_LINE + ["Top-tagged"],
        x_min = 50,
        x_max = 350,
        y_min = 1,
        log_scale = True,
        rebin = MASS_PLOT_REBIN + 4,
        ))

# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff",
#         empty_scale = 2.4,
#         extra_legend_lines = DEF_LINES + ["Top-tagged"],
#         x_min = 0,
#         x_max = 350,
#         y_min = 0.01,
#         rebin = MASS_PLOT_REBIN+4,
#         ))
#
# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff",
#         empty_scale = 2.0,
#         extra_legend_lines = DEF_LINES + ["Top-tagged"],
#         x_min = 0,
#         x_max = 350,
#         y_min = 0.01,
#         rebin = MASS_PLOT_REBIN+4,
#         ))

# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff",
#         empty_scale = 2.45,
#         extra_legend_lines = DEF_LINES + ["Top-tagged"],
#         x_min = 0,
#         x_max = 350,
#         y_min = 0.01,
#         rebin = MASS_PLOT_REBIN + 4,
#         ))
#
# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff",
#         empty_scale = 1.8,
#         extra_legend_lines = DEF_LINES + ["Top-tagged"],
#         x_min = 0,
#         x_max = 350,
#         y_min = 0.01,
#         rebin = MASS_PLOT_REBIN + 4,
#         ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + W_PT_LINE + ["W-tagged"],
        y_min = 0.01,
        x_min = 50,
        x_max = 350,
        #log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut",
#         empty_scale = 1.75,
#         extra_legend_lines = DEF_LINES + ["W-tagged"],
# 	      y_min = 0.01,
#         x_min = 0,
#         x_max = 350,
#         #log_scale = True,
#         rebin = MASS_PLOT_REBIN,
#         ))

# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut",
#         empty_scale = 2.0,
#         extra_legend_lines = DEF_LINES + ["Z-tagged"],
# 	      y_min = 0.01,
#         x_min = 0,
#         x_max = 350,
#         #y_max = 20e3,
#         #log_scale = True,
#         rebin = MASS_PLOT_REBIN,
#         ))
#
# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut",
#         empty_scale = 1.75,
#         extra_legend_lines = DEF_LINES + ["Z-tagged"],
# 	      y_min = 0.01,
#         x_min = 0,
#         x_max = 350,
#         #log_scale = True,
#         rebin = MASS_PLOT_REBIN,
#         ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "BDT_Top",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + TOP_PT_LINE + ["BDT Top-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        y_min = 0.01,
        x_min = 0,
        x_max = 350,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "BDT_W",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + W_PT_LINE + ["BDT W-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        suffix = "_unzoomed"
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "BDT_W",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + W_PT_LINE + ["BDT W-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        x_min = 50,
        x_max = 150,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "DNN_Top",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + TOP_PT_LINE + ["DNN Top-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        x_min = 0,
        y_min = 0.01,
        x_max = 350,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "DNN_W",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + W_PT_LINE + ["DNN W-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        x_min = 0,
        x_max = 350,
        y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        suffix = "_unzoomed"
        ))


data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "DNN_W",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + W_PT_LINE + ["DNN W-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        x_min = 50,
        x_max = 150,
          y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))


data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "TopoTag_Top_80_qqb",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + TOPOTAGGER_PT_LINE + ["Topo Top-tagged"],
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        x_min = 0,
        x_max = 350,
          y_min = 0.01,
        rebin = MASS_PLOT_REBIN,
        ))

# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "TopoTag_Top_20",
#         empty_scale = 1.75,
#         extra_legend_lines = DEF_LINES + ["Topocluster Top-tagged 20wp"],
#         do_systematics = False,
#         x_min = 0,
#         x_max = 350,
#           y_min = 0.01,
#         rebin = MASS_PLOT_REBIN,
#         ))


# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "TopoTag_Top_50",
#         empty_scale = 1.75,
#         extra_legend_lines = DEF_LINES + ["Topocluster Top-tagged 50wp"],
#         do_systematics = False,
#         x_min = 0,
#         x_max = 350,
#           y_min = 0.01,
#         rebin = MASS_PLOT_REBIN,
#         ))


# data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_m_comb_" + "TopoTag_Top_80",
#         empty_scale = 1.75,
#         extra_legend_lines = DEF_LINES + ["Topocluster Top-tagged 80wp"],
#         do_systematics = False,
#         x_min = 0,
#         x_max = 350,
#           y_min = 0.01,
#         rebin = MASS_PLOT_REBIN,
#         ))


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
data_mc_plots.append(PlotDataMcGammaJet( "h_mu",
       empty_scale = 2.0,
       extra_legend_lines = DEF_LINES,
       do_systematics = False,
       x_units = "",
       x_min = 5,
       x_max = 45,
       rebin = 4,
       ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_m_comb",
        empty_scale = 2.5,
        extra_legend_lines = DEF_LINES,
        x_min = 50,
        x_max = 350,
        #y_min = 6e3 + 0.1,
        log_scale = True,
        rebin = MASS_PLOT_REBIN,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_pt_comb",
        empty_scale = 3.0,
        extra_legend_lines = DEF_LINES,
        x_max = 2000,
        x_min = 200,
        log_scale = True,
        rebin = 4,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_pt_comb_smooth16WTag_50eff_JSSCut",
        empty_scale = 2.5,
        extra_legend_lines = DEF_LINES + W_PT_LINE + ["W-tagged"],
        x_max = 600,
        x_min = 300,
        log_scale = True,
        ))

# data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_Split23_combMgt100GeV",
    #     empty_scale = 5,
    #     extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
    #     log_scale = True,
    #     x_max = 140,
    #     ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_D2_combMgt50GeV",
        empty_scale = 1.9,
        extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
        x_units = "",
        rebin = 4,
        x_max = 4.0,
        y_min = 0.01,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_topTag_BDT_qqb_score_mva",
        empty_scale = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        extra_legend_lines = DEF_LINES + TOP_PT_LINE,
        x_units = "",
        rebin = 3,
        y_min = 0.1,
        log_scale = True,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_wTag_BDT_qq_score_mva",
        empty_scale = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        extra_legend_lines = DEF_LINES + W_PT_LINE,
        x_units = "",
        rebin = 3,
        y_min = 0.1,
        log_scale = True,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_topTag_DNN_qqb_score_mva",
        empty_scale = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        extra_legend_lines = DEF_LINES + TOP_PT_LINE,
        x_units = "",
        rebin = 3,
        y_min = 0.1,
        log_scale = True,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_wTag_DNN_qq_score_mva",
        empty_scale = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        extra_legend_lines = DEF_LINES + W_PT_LINE,
        x_units = "",
        rebin = 3,
        y_min = 0.1,
        log_scale = True,
        ))

data_mc_plots.append(PlotDataMcGammaJet("h_rljet0_topTag_TopoTagger_score_mva",
        empty_scale = 4,
        do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET,
        extra_legend_lines = DEF_LINES + TOPOTAGGER_PT_LINE,
        x_units = "",
        rebin = 3,
        y_min = 0.1,
        log_scale = True,
        ))


# data_mc_plots.appendPlotDataMcGammaJet("h_rljet0_Qw_combMgt100GeV",
    #     extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
    #     log_scale = True,
    #     x_max = 200,
    #     rebin = 2,
    #     empty_scale = 5,
    #     ))

# for masstag in ["", "combMgt100GeV", "combMlt100GeV", "combMgt180GeV", "combMgt100lt150GeV", "combMgt150lt180GeV"]:
for masstag in ["", "combMgt100GeV"]:
    data_mc_plots.append(PlotDataMcGammaJet(
              "h_rljet0_Tau32_wta" if not masstag else "h_rljet0_Tau32_wta_" + masstag,
              empty_scale = 2.0,
              flip_legend = True,
              extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"] + ["p_{T} > 450 GeV"] ,
              x_units = "",
              x_min = 0.01,
              x_max = 0.9,
              y_min = 0.01,
              rebin = 4
              ))


## SD Workaround Section

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/gammajet_20180520_syst/gammajet.merged.cp.root"
HISTLOADER = GammaJetLoader(CP_ROOT_FILEPATH)
# ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_GammaJet"

# OUTPUT_DIR = ROOT_OUTPUT_DIR + "/control"
# make_dir(ROOT_OUTPUT_DIR)
# make_dir(OUTPUT_DIR)

# There are still some systematics missing, this is a hack to let it plot with the ones that still are there
SD_SYSTEMATICS = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET
MISSING_SYSTEMATICS = [
"LARGERJET_Weak_JET_Rtrk_Baseline_D2Beta1",
"LARGERJET_Weak_JET_Rtrk_Baseline_Tau32WTA",
"LARGERJET_Weak_JET_Rtrk_Modelling_D2Beta1",
"LARGERJET_Weak_JET_Rtrk_Modelling_Tau32WTA",
"LARGERJET_Weak_JET_Rtrk_TotalStat_D2Beta1",
"LARGERJET_Weak_JET_Rtrk_TotalStat_Tau32WTA",
"LARGERJET_Weak_JET_Rtrk_Tracking_D2Beta1",
"LARGERJET_Weak_JET_Rtrk_Tracking_Tau32WTA",
]

for systematic in MISSING_SYSTEMATICS:
    while systematic in SD_SYSTEMATICS: SD_SYSTEMATICS.remove(systematic)
    while "RES_"+systematic in SD_SYSTEMATICS: SD_SYSTEMATICS.remove("RES_"+systematic)

data_mc_plots.append(PlotDataMcGammaJet( "h_rljet0_SD_logchi_t_dcut",
        empty_scale = 4,
        do_systematics = SD_SYSTEMATICS,
        x_units = "",
        rebin = 3,
        y_min = 0.1,
        x_min = -8,
        x_max = 10,
        log_scale = True,
        ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt0_atan1312",
            empty_scale = 1.9,
            extra_legend_lines = ["HTT-tagged"] + HTT_LINES,
            x_min = 50,
            x_max = 250,
            rebin = 5,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt0_m23m123",
            empty_scale = 2.0,
            extra_legend_lines = ["HTT-tagged"] + HTT_LINES,
            x_min = 50,
            x_max = 250,
            rebin = 8,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt0_m",
            empty_scale = 2.35,
            x_min = 50,
            x_max = 250,
            extra_legend_lines = ["HTT-tagged"] + HTT_LINES,
            rebin = 4,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt_caGroomJet0_m",
            empty_scale = 2.15,
            log_scale = True,
            x_min = 0,
            x_max = 350,
            extra_legend_lines = HTT_LINES,
            rebin = 10,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt_caGroomJet0_m_HTT_CAND",
            empty_scale = 2.15,
            log_scale = True,
            x_min = 0,
            x_max = 350,
            extra_legend_lines = HTT_LINES,
            rebin = 10,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt_caGroomJet0_pt",
            empty_scale = 2.15,
            log_scale = True,
            x_min = 200,
            x_max = 3000,
            extra_legend_lines = HTT_LINES,
            rebin = 4,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))

data_mc_plots.append(PlotDataMcGammaJet( "h_htt_caGroomJet0_pt_HTT_CAND",
            empty_scale = 2.15,
            log_scale = True,
            x_min = 200,
            x_max = 2500,
            extra_legend_lines = HTT_LINES,
            rebin = 4,
            do_systematics=SYSTEMATICS_MC15C_CAJET_GAMMAJET,
            ))
