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
  print "Usage: python2 {0} <histogram-factory-output-gammajet.root> <output_directory>" . format(argv[0])
  exit(1)

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)

RAW = DMDLoader(argv[1])

ROOT_OUTPUT_DIR = argv[2]

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/control_plots_gammajet"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

LOG_FILE = open(OUTPUT_DIR + "/control_plots_gammajet.log", 'w')

class PlotGammaJet(PlotBase):
    def __init__(self, var_name, is_htt = False, **kwargs):

        super(PlotGammaJet, self).__init__(
                lumi_val = "3.5",
                width = 600,
                name = var_name + "_data_pythia_gammajet",
                atlas_mod = "Internal",
                legend_loc = [0.70,0.93,0.94,0.8],
                x_title = get_axis_title(var_name),
                **kwargs)

        if self.log_scale: self.name += "_log"

        LOG_FILE.write( (4 + len(var_name)) * '#' + '\n' )
        LOG_FILE.write( "# " + var_name + " #"  + '\n')
        LOG_FILE.write( (4 + len(var_name)) * '#'  + '\n')

        if is_htt: var_name += "_def"

        self.h_data = RAW.get_hist(["data", "nominal"] , var_name)
        set_data_style_simple(self.h_data)

        self.h_pythia_gammajet = RAW.get_hist(["gamma_jet", "nominal"] , var_name)

        pythia_gammajet_SF = self.h_data.Integral() / self.h_pythia_gammajet.Integral()
        self.h_pythia_gammajet.Scale(pythia_gammajet_SF)

        LOG_FILE.write("\n")
        LOG_FILE.write("data yield: " + str(self.h_data.Integral()) + "\n")
        LOG_FILE.write("pythia gammajet SF: "    + str(pythia_gammajet_SF) + "\n")
        LOG_FILE.write("\n")

        if (self.rebin != None):
            self.h_data.Rebin(self.rebin)
            self.h_pythia_gammajet.Rebin(self.rebin)

        self.determine_y_axis_title(self.h_data)

        for h in [self.h_data, self.h_pythia_gammajet]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pad_empty_space([self.h_data, self.h_pythia_gammajet])

        # create ratio plots
        self.h_pythia_gammajet_ratio = self.h_data.Clone("h_pythia_gammajet_ratio")
        self.h_pythia_gammajet_ratio.Divide(self.h_data, self.h_pythia_gammajet, 1, 1, "")

        for h_ratio in [self.h_pythia_gammajet_ratio]:
            set_style_ratio(h_ratio)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h_ratio)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)

        set_mc_style_marker(self.h_pythia_gammajet, kRed, shape = 21, line_style = 3)

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

        self.h_pythia_gammajet.Draw("PE")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia_gammajet, "#gamma + jet")
        self.leg.Draw()

        self.pad2.cd()

        self.h_pythia_gammajet_ratio.Draw("PE1,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "#gamma + jet selection" ]

MASS_PLOT_REBIN = 8

data_mc_plots = [
        PlotGammaJet("h_rljet0_m",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES,
            x_max = 400,
            rebin = MASS_PLOT_REBIN,
            y_min = 0.01,
            ),

        PlotGammaJet("h_rljet0_eta",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES,
            x_min = -2,
            x_max = 2
            ),

        PlotGammaJet("h_rljet0_phi",
            empty_scale = 1.7,
            extra_legend_lines = DEF_LINES
            ),

        PlotGammaJet("h_rljet0_pt",
            empty_scale = 2.1,
            extra_legend_lines = DEF_LINES,
            log_scale = True,
            x_min = 200,
            x_max = 1000
            ),

        #PlotGammaJet("h_rljet0_area",
        #    empty_scale = 1.9,
        #    x_units = "",
        #    x_max = 1.5,
        #    ),

        PlotGammaJet("h_rljet0_ECF1",
            empty_scale = 1.9,
            x_units = "",
            x_min = 0.5,
            y_min = 0.01,
            ),

        PlotGammaJet("h_rljet0_ECF2",
            empty_scale = 1.9,
            x_units = "",
            rebin = 2,
            ),

        PlotGammaJet("h_rljet0_ECF3",
            empty_scale = 1.9,
            x_units = "",
            rebin = 2,
            ),

        PlotGammaJet("h_rljet0_NTrimSubjets",
            empty_scale = 1.4,
            x_units = "",
            x_max = 10,
            log_scale = True,
            ),

        PlotGammaJet("h_rljet0_Width",
            empty_scale = 1.9,
            x_units = "",
            x_max = 0.5,
            ),

        PlotGammaJet("h_rljet0_Split12",
            empty_scale = 1.9,
            x_units = "",
            ),

        PlotGammaJet("h_rljet0_Split23",
            empty_scale = 1.9,
            x_units = "",
            ),

        PlotGammaJet("h_rljet0_Split34",
            empty_scale = 1.9,
            x_units = "",
            ),

        PlotGammaJet("h_rljet0_Tau1",
            empty_scale = 1.5,
            x_units = "",
            x_max = 0.9,
            ),

        PlotGammaJet("h_rljet0_Tau2",
            empty_scale = 1.5,
            x_units = "",
            x_max = 0.5,
            ),

        PlotGammaJet("h_rljet0_Tau3",
            empty_scale = 1.5,
            x_units = "",
            x_max = 0.3,
            ),

        #PlotGammaJet("h_rljet0_ungroomed_ntrk500",
        #    empty_scale = 1.9,
        #    x_units = "",
        #    ),

        PlotGammaJet("h_rljet0_D2",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m_{calo} > 50 GeV"],
            y_min = 0.01,
            x_units = "",
            x_max = 4.0
            ),

        PlotGammaJet("h_rljet0_m_w_prerec_50eff_D2cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth D_{2}-tagged (50% wp)"],
            y_min = 0.01,
            x_min = 20.1,
            x_max = 200,
            rebin = 4
            ),

        PlotGammaJet("h_rljet0_m_w_prerec_25eff_D2cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth D_{2}-tagged (25% wp)"],
            y_min = 0.01,
            x_min = 20.1,
            x_max = 200,
            rebin = 2,
            ),

        PlotGammaJet("h_rljet0_Tau32",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m_{calo} > 100 GeV"],
            x_units = "",
            x_min = 0.01,
            y_min = 0.01,
            x_max = 0.9
            ),

        PlotGammaJet("h_rljet0_m_t_prerec_50eff_Tau32cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotGammaJet("h_rljet0_m_t_prerec_80eff_Tau32cut",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotGammaJet("h_rljet0_m_t_prerec_80eff_Tau32cut",
            empty_scale = 3.5,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotGammaJet("h_rljet0_m_t_prerec_50eff_Tau32cut",
            empty_scale = 3.5,
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            y_min = 10,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        PlotGammaJet("h_htt0_m23m123",
            is_htt = True,
            x_min = 0.35,
            x_max = 0.8,
            x_units = "",
            y_min = 0.01,
            extra_legend_lines = ["HTT Top Candidate"],
            empty_scale = 1.5
            ),

        PlotGammaJet("h_htt0_atan1312",
            is_htt = True,
            x_min = 0.2,
            x_max = 1.3,
            rebin = 2,
            x_units = "",
            extra_legend_lines = ["HTT Top Candidate"],
            empty_scale = 1.9
            ),

        PlotGammaJet("h_htt0_m",
            is_htt = True,
            x_min = 140,
            x_max = 210,
            extra_legend_lines = ["HTT Top Candidate"],
            empty_scale = 1.9
            ),

        PlotGammaJet("h_htt0_eta",
            is_htt = True,
            x_min = -2,
            x_max = 2,
            extra_legend_lines = ["HTT-tagged"],
            empty_scale = 1.9
            ),

        PlotGammaJet("h_htt0_phi",
            is_htt = True,
            rebin = 2,
            x_min = -3.14,
            x_max = 3.14,
            extra_legend_lines = ["HTT-tagged"],
            empty_scale = 1.9
            ),

        PlotGammaJet("h_htt0_pt",
            is_htt = True,
            x_min = 500,
            x_max = 1500,
            log_scale = True,
            extra_legend_lines = ["HTT-tagged"],
            empty_scale = 1.9
            ),

        #PlotGammaJet("h_htt_caGroomJet0_m",
        #    is_htt = True,
        #    rebin = 4,
        #    x_max = 500,
        #    empty_scale = 1.9
        #    ),

        #PlotGammaJet("h_htt_caGroomJet0_eta",
        #    is_htt = True,
        #    x_min = -2,
        #    x_max = 2,
        #    x_units = "",
        #    empty_scale = 1.9
        #    ),

        #PlotGammaJet("h_htt_caGroomJet0_phi",
        #    is_htt = True,
        #    rebin = 2,
        #    x_min = -3.14,
        #    x_max = 3.14,
        #    x_units = "",
        #    empty_scale = 1.9
        #    ),

        #PlotGammaJet("h_htt_caGroomJet0_pt",
        #    is_htt = True,
        #    x_min = 0,
        #    x_max = 1500,
        #    empty_scale = 1.9
        #    ),

        PlotGammaJet("h_BDT_TopTag",
            extra_legend_lines = DEF_LINES,
            x_min = -1,
            x_max = 1,
            rebin = 4,
            log_scale = True,
            y_min = 10**3,
            x_units = "",
            empty_scale = 1.3
            ),

        PlotGammaJet("h_BDT_TopTag_mass_gt100",
            extra_legend_lines = DEF_LINES + ["m_{calo} > 100 GeV"],
            x_min = -1,
            x_max = 1,
            rebin = 4,
            log_scale = True,
            y_min = 10**3,
            x_units = "",
            empty_scale = 1.3
            ),

        PlotGammaJet("h_BDT_TopTag",
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
    plot.print_to_file(OUTPUT_DIR + "/" + plot.name + ".pdf")

LOG_FILE.close()
