from ROOT import *
import atlas_style

import os
from math import *
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

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/diffuseness_study_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

class PlotDataPythiaHerwigDiffuseness(PlotBase):
    def __init__(self,
            h_data_wide, h_data_narrow,
            h_pythia_wide, h_pythia_narrow,
            **kwargs):
        super(PlotDataPythiaHerwigDiffuseness, self).__init__(**kwargs)

        self.h_data_wide      = h_data_wide
        self.h_data_narrow    = h_data_narrow
        self.h_pythia_wide    = h_pythia_wide
        self.h_pythia_narrow  = h_pythia_narrow

        self.determine_y_axis_title(self.h_data_wide, "#epsilon_{QCD}")

        set_mc_style_marker(self.h_pythia_wide, kRed, shape = 21)
        set_mc_style_marker(self.h_pythia_narrow, kRed, shape = 22)
        set_mc_style_marker(self.h_data_wide, kBlack, shape = 21)
        set_mc_style_marker(self.h_data_narrow, kBlack, shape = 22)

        for h in [ self.h_data_wide, self.h_data_narrow, self.h_pythia_wide, self.h_pythia_narrow ]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

        self.h_pythia_wide.Draw("PE")
        self.h_pythia_narrow.Draw("PE,same")
        self.h_data_wide.Draw("PE,same")
        self.h_data_narrow.Draw("PE,same")

        self.leg.AddEntry(self.h_data_wide, "Data 2016: AWAY TRIGGER")
        self.leg.AddEntry(self.h_data_narrow, "Data 2016: NEAR TRIGGER")
        self.leg.AddEntry(self.h_pythia_wide, "Pythia8: AWAY TRIGGER")
        self.leg.AddEntry(self.h_pythia_narrow, "Pythia8: NEAR TRIGGER")
        self.leg.Draw()

DEF_EXTRA_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" ]

REBIN_PT = 8

def make_pt_efficiency_plot(wide_var_name, narrow_var_name, **kwargs):
    wide_hist_name = "h_rljet0_pt_" + wide_var_name
    narrow_hist_name = "h_rljet0_pt_" + narrow_var_name

    print wide_hist_name, narrow_hist_name

    h_data_total   = RAW.get_hist(["data","nominal"], "h_rljet0_pt")
    h_pythia_total = RAW.get_hist(["pythia_dijet","nominal"], "h_rljet0_pt")
    h_data_wide      = RAW.get_hist(["data","nominal"], wide_hist_name)
    h_pythia_wide    = RAW.get_hist(["pythia_dijet","nominal"], wide_hist_name)
    h_data_narrow    = RAW.get_hist(["data","nominal"], narrow_hist_name)
    h_pythia_narrow  = RAW.get_hist(["pythia_dijet","nominal"], narrow_hist_name)

    h_data_total.Rebin(REBIN_PT)
    h_pythia_total.Rebin(REBIN_PT)
    h_data_wide.Rebin(REBIN_PT)
    h_pythia_wide.Rebin(REBIN_PT)
    h_data_narrow.Rebin(REBIN_PT)
    h_pythia_narrow.Rebin(REBIN_PT)

    h_data_wide.Divide(h_data_total)
    h_data_narrow.Divide(h_data_total)
    h_pythia_wide.Divide(h_pythia_total)
    h_pythia_narrow.Divide(h_pythia_total)

    return PlotDataPythiaHerwigDiffuseness(
            h_data_wide,
            h_data_narrow,
            h_pythia_wide,
            h_pythia_narrow,
            name = wide_hist_name + "_eff",
            lumi_val = "3.5",
            atlas_mod = "Internal",
            legend_loc = [0.75,0.93,0.94,0.79],
            x_title = "Leading Large-R Jet #it{p_{T}}",
            extra_legend_lines = DEF_EXTRA_LINES,
            x_min = 500,
            x_max = 2000,
            width = 600,
            **kwargs)

plots_list = [
        make_pt_efficiency_plot(
            "massD2W50wp_away_trigger",
            "massD2W50wp_near_trigger",
            y_max = 0.03,
            y_min = 0.00001
            ),

        make_pt_efficiency_plot(
            "massTAU3250wp_away_trigger",
            "massTAU3250wp_near_trigger",
            y_max = 0.14,
            y_min = 0.00001
            ),

        make_pt_efficiency_plot(
            "massD2W50wp_width",
            "massD2W50wp_ANTI_width",
            y_max = 0.1,
            y_min = -0.1
            ),

        make_pt_efficiency_plot(
            "massTAU3250wp_width",
            "massTAU3250wp_ANTI_width",
            y_max = 0.14,
            y_min = 0.000001
            ),

        make_pt_efficiency_plot(
            "massTAU3250wp_ntrk",
            "massTAU3250wp_ANTI_ntrk",
            y_max = 0.14,
            y_min = 0.000001
            ),

        make_pt_efficiency_plot(
            "massD2W50wp_ntrimsubjets",
            "massD2W50wp_ANTI_ntrimsubjets",
            y_max = 0.06,
            y_min = 0.000001
            ),

        make_pt_efficiency_plot(
            "massTAU3250wp_ntrimsubjets",
            "massTAU3250wp_ANTI_ntrimsubjets",
            y_max = 0.2,
            y_min = 0.000001
            )

        ]

for i in range(len(plots_list)):
    plots_list[i].print_to_file(OUTPUT_DIR + "/" + plots_list[i].name + ".pdf")
    plots_list[i].print_to_file(OUTPUT_DIR + "/" + "data16_mc15c_efficiency_plots", i, len(plots_list))
