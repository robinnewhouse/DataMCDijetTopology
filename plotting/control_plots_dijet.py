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

if len(argv) != 3:
  print "Usage: python2 {0} <control_plots.root> <output_directory>" . format(argv[0])
  exit(1)

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)

RAW = DMDLoader(argv[1])

ROOT_OUTPUT_DIR = argv[2]

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/control_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

LOG_FILE = open(OUTPUT_DIR + "/control_plots.log", 'w')

#SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM_SUBSTRUCTURE + SYSTEMATICS_MC15C_MEDIUM_KINEMATIC
SYSTEMATICS = [ ]

class PlotDataPythiaHerwig(PlotBase):
    def __init__(self, var_name, is_htt = False, plot_systematics = True, **kwargs):

        if (not SYSTEMATICS):
            plot_systematics = False

        super(PlotDataPythiaHerwig, self).__init__(
                lumi_val = "3.5",
                width = 600,
                name = var_name,
                atlas_mod = "Internal",
                legend_loc = [0.63,0.93,0.94,0.72] if plot_systematics else [0.70,0.93,0.94,0.8],
                x_title = get_axis_title(var_name),
                **kwargs)

        self.var_name = var_name

        LOG_FILE.write( (4 + len(var_name)) * '#' + '\n' )
        LOG_FILE.write( "# " + var_name + " #"  + '\n')
        LOG_FILE.write( (4 + len(var_name)) * '#'  + '\n')

        if is_htt:
            var_name += "_def"

        self.h_data = RAW.get_hist(["data", "nominal"] , var_name)
        set_data_style_simple(self.h_data)

        self.h_pythia_dijet = RAW.get_hist(["pythia_dijet", "nominal"] , var_name)
        self.h_herwig_dijet = RAW.get_hist(["herwig_dijet", "nominal"] , var_name)

        self.h_wjets = RAW.get_hist(["pythia_wjets", "nominal"] , var_name)
        self.h_wjets.Scale(PYTHIA_SHERPA_WJETS_SF)

        self.h_zjets = RAW.get_hist(["pythia_zjets", "nominal"] , var_name)
        self.h_zjets.Scale(PYTHIA_SHERPA_ZJETS_SF)

        self.h_ttbar = RAW.get_hist(["ttbar_allhad", "nominal"] , var_name)

        self.h_data_sigsub = self.h_data.Clone(self.h_data.GetName() + "_sigsub")

        self.h_data_sigsub.Add(self.h_wjets, -1.0)
        self.h_data_sigsub.Add(self.h_zjets, -1.0)
        self.h_data_sigsub.Add(self.h_ttbar, -1.0)

        pythia_dijet_SF = self.h_data_sigsub.Integral() / self.h_pythia_dijet.Integral()
        herwig_dijet_SF = self.h_data_sigsub.Integral() / self.h_herwig_dijet.Integral()

        pythia_sys_dict = {}
        herwig_sys_dict = {}

        if is_htt:
            pythia_sys_dict = RAW.get_htt_systematics_dict(var_name[:-4], "pythia_dijet", "sjcalib0955", "sjcalib1045")
            herwig_sys_dict = RAW.get_htt_systematics_dict(var_name[:-4], "herwig_dijet", "sjcalib0955", "sjcalib1045")
        elif (plot_systematics):
            pythia_sys_dict = RAW.get_systematics_dict(var_name, SYSTEMATICS, [ "pythia_dijet" ])
            herwig_sys_dict = RAW.get_systematics_dict(var_name, SYSTEMATICS, [ "herwig_dijet" ])

        self.hsys_pythia = TH1Sys(self.h_pythia_dijet, pythia_sys_dict)
        self.hsys_herwig = TH1Sys(self.h_herwig_dijet, herwig_sys_dict)

        self.hsys_pythia.scale(pythia_dijet_SF)
        self.hsys_herwig.scale(herwig_dijet_SF)

        self.h_pythia_dijet.Scale(pythia_dijet_SF)
        self.h_herwig_dijet.Scale(herwig_dijet_SF)

        root_file = TFile(OUTPUT_DIR + "/" + var_name + ".root", "RECREATE")
        root_file.cd()

        default_histo = TH1F()

        hplain_dijet = self.h_pythia_dijet.Clone("dijet")
        hplain_wjets = self.h_wjets.Clone("wjets")
        hplain_zjets = self.h_zjets.Clone("zjets")
        hplain_data  = self.h_data.Clone("data")

        for h in [hplain_dijet, hplain_wjets, hplain_zjets, hplain_data]:
            h.SetMarkerStyle(default_histo.GetMarkerStyle())
            h.SetMarkerColor(default_histo.GetMarkerColor())
            h.SetFillStyle(default_histo.GetFillStyle())
            h.SetFillColor(default_histo.GetFillColor())
            h.SetMarkerStyle(default_histo.GetMarkerStyle())
            h.SetMarkerSize(default_histo.GetMarkerSize())
            h.Write(h.GetName())

        root_file.Close()

        LOG_FILE.write("\n")
        LOG_FILE.write("data yield (before signal subtraction): " + str(self.h_data.Integral()) + "\n")
        LOG_FILE.write("data yield (after signal subtraction): "  + str(self.h_data_sigsub.Integral()) + "\n")
        LOG_FILE.write("\n")
        LOG_FILE.write("wjets yield: " + str(self.h_wjets.Integral()) + "\n")
        LOG_FILE.write("zjets yield: " + str(self.h_zjets.Integral()) + "\n")
        LOG_FILE.write("ttbar yield: " + str(self.h_ttbar.Integral()) + "\n")
        LOG_FILE.write("\n")
        LOG_FILE.write("pythia dijet SF: "    + str(pythia_dijet_SF) + "\n")
        LOG_FILE.write("pythia dijet yield: " + str(self.h_pythia_dijet.Integral()) + "\n")
        LOG_FILE.write("herwig dijet SF: "    + str(herwig_dijet_SF) + "\n")
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
            self.hsys_herwig.rebin(self.rebin)

        self.hsys_pythia._compute_errors()
        self.hsys_herwig._compute_errors()

        self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs()
        self.h_herwig_sys = self.hsys_herwig.get_histo_with_systematic_errs()

        self.determine_y_axis_title(self.h_data)

        for h in [self.h_data, self.h_pythia_dijet, self.h_herwig_dijet,
                self.h_pythia_sys, self.h_herwig_sys]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pad_empty_space([self.h_data, self.h_pythia_dijet, self.h_herwig_dijet,
                                        self.h_pythia_sys, self.h_herwig_sys])

        # create ratio plots
        self.h_pythia_dijet_ratio = self.h_data.Clone("h_pythia_dijet_ratio")
        self.h_pythia_dijet_ratio.Divide(self.h_data, self.h_pythia_dijet, 1, 1, "")

        self.h_pythia_stat_ratio = self.h_pythia_dijet_ratio.Clone("h_pythia_stat_ratio")

        self.h_pythia_sys_ratio = self.h_data.Clone("h_pythia_sys_ratio")
        self.h_pythia_sys_ratio.Divide(self.h_data, self.h_pythia_sys, 1, 1, "")

        self.h_herwig_dijet_ratio = self.h_data.Clone("h_herwig_dijet_ratio")
        self.h_herwig_dijet_ratio.Divide(self.h_data, self.h_herwig_dijet, 1, 1, "")

        self.h_herwig_sys_ratio = self.h_data.Clone("h_herwig_sys_ratio")
        self.h_herwig_sys_ratio.Divide(self.h_data, self.h_herwig_sys, 1, 1, "")

        for ibin in range(self.h_pythia_sys_ratio.GetSize()):
            self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)
            self.h_pythia_stat_ratio.SetBinContent(ibin, 1.0)
            self.h_herwig_sys_ratio.SetBinContent(ibin, 1.0)

        for h_ratio in [self.h_pythia_dijet_ratio, self.h_herwig_dijet_ratio,
                        self.h_pythia_sys_ratio, self.h_herwig_sys_ratio]:
            set_style_ratio(h_ratio)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h_ratio)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)

    def make_data_pythia_herwig_plot(self, output_dir):
        self.name = self.var_name + "_data_pythia_herwig"
        if self.log_scale: self.name += "_log"
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        set_mc_style_marker(self.h_pythia_dijet, kRed, shape = 21, line_style = 3)
        set_mc_style_marker(self.h_herwig_dijet, kBlue, shape = 22, line_style = 4)

        set_mc_sys_err_style(self.h_pythia_sys)
        set_mc_sys_err_style(self.h_herwig_sys)

        self.h_pythia_sys.SetFillColorAlpha(kRed, 0.3)
        self.h_herwig_sys.SetFillColorAlpha(kBlue, 0.3)

        set_mc_style_marker(self.h_pythia_dijet_ratio, kRed, shape = 21, line_style = 3)
        self.h_pythia_stat_ratio.SetMarkerSize(0)
        set_mc_sys_err_style(self.h_pythia_sys_ratio)
        set_mc_style_marker(self.h_herwig_dijet_ratio, kBlue, shape = 22, line_style = 4)
        set_mc_sys_err_style(self.h_herwig_sys_ratio)
        self.h_pythia_sys_ratio.SetMarkerSize(0)
        self.h_pythia_sys_ratio.SetFillColor(kGreen-8)

        self.h_pythia_stat_ratio.SetFillColor(kGreen-5)
        self.h_pythia_stat_ratio.SetMarkerSize(0)
        self.h_pythia_stat_ratio.SetFillStyle(1001)
        self.h_pythia_stat_ratio.SetLineWidth(0)

        self.h_pythia_dijet_ratio.SetFillStyle(0)
        self.h_herwig_dijet_ratio.SetFillStyle(0)

        # SET UP THE CANVAS
        self.canvas.Divide(1,2)
        self.canvas.Modified()

        self.pad1 = self.canvas.cd(1)
        self.pad1.SetPad(0.0, 0.33, 1., 1.)
        self.pad1.SetTopMargin(0.07)
        self.pad1.SetBottomMargin(0.00)
        self.pad1.SetFillColorAlpha(0, 0.)
        if self.log_scale: self.pad1.SetLogy()

        self.pad2 = self.canvas.cd(2)
        self.pad2.SetPad(0.0, 0.0, 1., 0.33)
        self.pad2.SetTopMargin(0.05)
        self.pad2.SetBottomMargin(0.3)
        self.pad2.SetFillColorAlpha(0, 0.)
        self.pad2.SetGridy(1) # grid for ratio plot

        self.pad1.cd()

        self.h_herwig_dijet.Draw("PE,same")
        self.h_pythia_dijet.Draw("PE,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia_dijet, "Pythia8 dijet")
        self.leg.AddEntry(self.h_herwig_dijet, "Herwig++ dijet")
        if (self.hsys_pythia.num_systematics != 0):
            self.leg.AddEntry(self.h_pythia_sys_ratio, "Stat. #oplus syst. uncert.")
            self.leg.AddEntry(self.h_pythia_stat_ratio, "Stat. uncert.")
        self.leg.Draw()

        self.pad2.cd()

        #self.h_herwig_sys_ratio.Draw("E2,same")
        if (self.hsys_pythia.num_systematics != 0):
            self.h_pythia_sys_ratio.Draw("E2,same")
            self.h_pythia_stat_ratio.Draw("E2,same")
        self.h_pythia_dijet_ratio.Draw("PE1,same")
        self.h_herwig_dijet_ratio.Draw("PE1,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(output_dir + "/" + self.name + ".pdf")
        self.canvas.Clear()

    def make_data_pythia_stack_plot(self, output_dir):
        self.name = self.var_name + "_data_pythia_stack"
        if self.log_scale: self.name += "_log"
        self.canvas.Clear()
        self.canvas = None
        self.legend_loc = [0.67,0.93,0.94,0.72]
        self._make_canvas()
        self._make_decorations()

        stack_name = self.name + "_pythia_stack"
        self.hs_pythia = THStack(stack_name, stack_name)

        self.h_pythia_dijet.SetFillColor(kRed)
        self.h_pythia_dijet.SetFillStyle(1001)
        self.h_pythia_dijet.SetMarkerSize(0)
        self.h_pythia_dijet.SetLineWidth(0)
        self.hs_pythia.Add(self.h_pythia_dijet.Clone())

        self.h_zjets.SetFillColor(kGreen)
        self.h_zjets.SetMarkerSize(0)
        self.h_zjets.SetLineWidth(0)
        self.hs_pythia.Add(self.h_zjets.Clone())

        self.h_wjets.SetFillColor(kBlue)
        self.h_wjets.SetMarkerSize(0)
        self.h_wjets.SetLineWidth(0)
        self.hs_pythia.Add(self.h_wjets.Clone())

        self.h_ttbar.SetFillColor(6)
        self.h_ttbar.SetMarkerSize(0)
        self.h_ttbar.SetLineWidth(0)
        self.hs_pythia.Add(self.h_ttbar.Clone())

        self.h_pythia_net = self.h_pythia_dijet.Clone(
                self.h_pythia_dijet.GetName() + "_net")
        self.h_pythia_net.Add(self.h_wjets)
        self.h_pythia_net.Add(self.h_zjets)
        self.h_pythia_net.Add(self.h_ttbar)


        self.h_pythia_net.SetFillStyle(3005)
        self.h_pythia_net.SetFillColor(1)

        self.h_pythia_net_ratio = self.h_data.Clone("h_pythia_net_ratio")
        self.h_pythia_net_ratio.Divide(self.h_data, self.h_pythia_net, 1, 1, "")
        set_data_style_simple(self.h_pythia_net_ratio)

        set_style_ratio(self.h_pythia_net_ratio, y_min = 0.5, y_max = 1.5)
        self.h_pythia_net_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
        self.set_x_axis_bounds(self.h_pythia_net_ratio)
        self.h_pythia_net_ratio.GetXaxis().SetTitleOffset(4.0)
        self.h_pythia_net_ratio.GetYaxis().SetTitleOffset(2.0)

        # SET UP THE CANVAS
        self.canvas.Divide(1,2)
        self.canvas.Modified()

        self.pad1 = self.canvas.cd(1)
        self.pad1.SetPad(0.0, 0.33, 1., 1.)
        self.pad1.SetTopMargin(0.07)
        self.pad1.SetBottomMargin(0.00)
        self.pad1.SetFillColorAlpha(0, 0.)
        if self.log_scale: self.pad1.SetLogy()

        self.pad2 = self.canvas.cd(2)
        self.pad2.SetPad(0.0, 0.0, 1., 0.33)
        self.pad2.SetTopMargin(0.05)
        self.pad2.SetBottomMargin(0.3)
        self.pad2.SetFillColorAlpha(0, 0.)
        self.pad2.SetGridy(1) # grid for ratio plot

        self.pad1.cd()

        self.h_data.Draw("PE,same")
        self.hs_pythia.Draw("histo,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia_dijet, "Pythia8 dijet")
        self.leg.AddEntry(self.h_wjets, "W+jets")
        self.leg.AddEntry(self.h_zjets, "Z+jets")
        self.leg.AddEntry(self.h_ttbar, "ttbar")
        self.leg.Draw()

        self.pad2.cd()

        self.h_pythia_net_ratio.Draw("PE")

        self.pad2.RedrawAxis("g")
        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(output_dir + "/" + self.name + ".pdf")
        self.canvas.Clear()

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" ]

MASS_PLOT_REBIN = 4

data_mc_plots = [
        PlotDataPythiaHerwig("h_rljet0_m",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES,
            x_max = 400,
            rebin = MASS_PLOT_REBIN,
            y_min = 0.01,
            ),

        PlotDataPythiaHerwig("h_rljet0_eta",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES,
            x_min = -2,
            x_max = 2
            ),

        PlotDataPythiaHerwig("h_rljet0_phi",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES
            ),

        PlotDataPythiaHerwig("h_rljet0_pt",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES,
            log_scale = True,
            x_min = 500,
            x_max = 3000
            ),

        #PlotDataPythiaHerwig("h_rljet0_area",
        #    empty_scale = 1.9,
        #    x_units = "",
        #    x_max = 1.5,
        #    plot_systematics = False
        #    ),

        PlotDataPythiaHerwig("h_rljet0_ecf1",
            empty_scale = 1.9,
            x_units = "",
            x_min = 0.5,
            y_min = 0.01,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_ecf2",
            empty_scale = 1.9,
            x_units = "",
            rebin = 2,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_ecf3",
            empty_scale = 1.9,
            x_units = "",
            rebin = 2,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_ntrimsubjets",
            empty_scale = 1.4,
            x_units = "",
            x_max = 10,
            log_scale = True,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_width",
            empty_scale = 1.9,
            x_units = "",
            x_max = 0.5,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_split12",
            empty_scale = 1.9,
            x_units = "",
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_split23",
            empty_scale = 1.9,
            x_units = "",
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_split34",
            empty_scale = 1.9,
            x_units = "",
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_tau1",
            empty_scale = 1.5,
            x_units = "",
            x_max = 0.9,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_tau2",
            empty_scale = 1.5,
            x_units = "",
            x_max = 0.5,
            plot_systematics = False
            ),

        PlotDataPythiaHerwig("h_rljet0_tau3",
            empty_scale = 1.5,
            x_units = "",
            x_max = 0.3,
            plot_systematics = False
            ),

        #PlotDataPythiaHerwig("h_rljet0_ungroomed_ntrk500",
        #    empty_scale = 1.9,
        #    x_units = "",
        #    plot_systematics = False
        #    ),

        PlotDataPythiaHerwig("h_rljet0_d2",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m_{calo} > 50 GeV"],
            y_min = 0.01,
            x_units = "",
            x_max = 4.0
            ),

        PlotDataPythiaHerwig("h_rljet0_m_w_prerec_50eff_d2cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth D_{2}-tagged (50% wp)"],
            y_min = 0.01,
            x_min = 20.1,
            x_max = 200,
            rebin = 2,
            ),

        PlotDataPythiaHerwig("h_rljet0_m_w_prerec_25eff_d2cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth D_{2}-tagged (25% wp)"],
            y_min = 0.01,
            x_min = 20.1,
            x_max = 200,
            rebin = 2,
            ),

        PlotDataPythiaHerwig("h_rljet0_tau32",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m_{calo} > 100 GeV"],
            x_units = "",
            x_min = 0.01,
            y_min = 0.01,
            x_max = 0.9
            ),

        PlotDataPythiaHerwig("h_rljet0_m_t_prerec_50eff_tau32cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotDataPythiaHerwig("h_rljet0_m_t_prerec_80eff_tau32cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotDataPythiaHerwig("h_rljet0_m_t_prerec_80eff_tau32cut",
            empty_scale = 3.5,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotDataPythiaHerwig("h_rljet0_m_t_prerec_50eff_tau32cut",
            empty_scale = 3.5,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotDataPythiaHerwig("h_htt0_m23m123",
            is_htt = True,
            x_min = 0.35,
            x_max = 0.8,
            x_units = "",
            y_min = 0.01,
            extra_legend_lines = ["HTT Top Candidate"],
            empty_scale = 1.5
            ),

        PlotDataPythiaHerwig("h_htt0_atan1312",
            is_htt = True,
            x_min = 0.2,
            x_max = 1.3,
            rebin = 2,
            x_units = "",
            extra_legend_lines = ["HTT Top Candidate"],
            empty_scale = 1.9
            ),

        PlotDataPythiaHerwig("h_htt0_m",
            is_htt = True,
            x_min = 140,
            x_max = 210,
            extra_legend_lines = ["HTT Top Candidate"],
            empty_scale = 1.9
            ),

        PlotDataPythiaHerwig("h_htt0_eta",
            is_htt = True,
            x_min = -2,
            x_max = 2,
            extra_legend_lines = ["HTT-tagged"],
            empty_scale = 1.9
            ),

        PlotDataPythiaHerwig("h_htt0_phi",
            is_htt = True,
            rebin = 2,
            x_min = -3.14,
            x_max = 3.14,
            extra_legend_lines = ["HTT-tagged"],
            empty_scale = 1.9
            ),

        PlotDataPythiaHerwig("h_htt0_pt",
            is_htt = True,
            x_min = 500,
            x_max = 1500,
            log_scale = True,
            extra_legend_lines = ["HTT-tagged"],
            empty_scale = 1.9
            ),

        #PlotDataPythiaHerwig("h_htt_caGroomJet0_m",
        #    is_htt = True,
        #    rebin = 4,
        #    x_max = 500,
        #    empty_scale = 1.9
        #    ),

        #PlotDataPythiaHerwig("h_htt_caGroomJet0_eta",
        #    is_htt = True,
        #    x_min = -2,
        #    x_max = 2,
        #    x_units = "",
        #    empty_scale = 1.9
        #    ),

        #PlotDataPythiaHerwig("h_htt_caGroomJet0_phi",
        #    is_htt = True,
        #    rebin = 2,
        #    x_min = -3.14,
        #    x_max = 3.14,
        #    x_units = "",
        #    empty_scale = 1.9
        #    ),

        #PlotDataPythiaHerwig("h_htt_caGroomJet0_pt",
        #    is_htt = True,
        #    x_min = 0,
        #    x_max = 1500,
        #    empty_scale = 1.9
        #    ),

        PlotDataPythiaHerwig("h_BDT_TopTag",
            plot_systematics = False,
            extra_legend_lines = DEF_LINES,
            x_min = -1,
            x_max = 1,
            rebin = 4,
            log_scale = True,
            y_min = 10**3,
            x_units = "",
            empty_scale = 1.3
            ),

        PlotDataPythiaHerwig("h_BDT_TopTag_mass_gt100",
            plot_systematics = False,
            extra_legend_lines = DEF_LINES + ["m_{calo} > 100 GeV"],
            x_min = -1,
            x_max = 1,
            rebin = 4,
            log_scale = True,
            y_min = 10**3,
            x_units = "",
            empty_scale = 1.3
            ),

        PlotDataPythiaHerwig("h_BDT_TopTag",
            plot_systematics = False,
            extra_legend_lines = DEF_LINES,
            x_min = -1,
            x_max = 1,
            rebin = 4,
            y_min = 0.01,
            x_units = "",
            empty_scale = 1.3
            )
        ]

for plot in data_mc_plots:
    plot.make_data_pythia_herwig_plot(OUTPUT_DIR)
    plot.make_data_pythia_stack_plot(OUTPUT_DIR)

LOG_FILE.close()