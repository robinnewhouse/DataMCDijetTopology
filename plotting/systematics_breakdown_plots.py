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

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/systematics_breakdown_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

class PlotPythiaSystematicsBreakdown(PlotBase):
    def __init__(self, hsys_pythia, **kwargs):

        super(PlotPythiaSystematicsBreakdown, self).__init__(**kwargs)

        self.hsys_pythia = hsys_pythia
        if (self.rebin != None): self.hsys_pythia.rebin(self.rebin)
        self.hsys_pythia._compute_errors()

        h_nominal = self.hsys_pythia.h_nominal.Clone()

        self.breakdown_histograms = []

        for sys_name in self.hsys_pythia.systematics_dictionary.iterkeys():
            # clone the nominal histogram so we have the right bin spacing/bounds
            h_sys = h_nominal.Clone(h_nominal.GetName() + "_" + sys_name)

            for ibin in range(hsys_pythia.num_bins):
                if (h_nominal.GetBinContent(ibin) != 0):
                    sys_err_perc = sqrt(hsys_pythia.sys_errors_sq_breakdown[sys_name][ibin])
                    sys_err_perc /= h_nominal.GetBinContent(ibin)
                    sys_err_perc *= 100.0
                    h_sys.SetBinContent(ibin, sys_err_perc)
                else:
                    h_sys.SetBinContent(ibin, 0.0)

            self.breakdown_histograms.append(h_sys.Clone())

        self.determine_y_axis_title(h_nominal, "Uncertainty in %")

        line_colors = [2,4,7,8,9,3,5,6]

        for i, h in enumerate(self.breakdown_histograms):
            h.SetLineWidth(3)
            (st,col) = get_mc15c_systematics_style_color(h.GetName())
            h.SetLineStyle(st)
            h.SetLineColorAlpha(col, 0.75)
            h.SetMarkerColorAlpha(col, 0.75)
            h.SetFillStyle(0)
            h.SetMarkerStyle(0)
            h.SetMarkerSize(0)

            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

            if (i == 0):
                h.Draw("hist")
            else:
                h.Draw("hist,same")
            self.leg.AddEntry(h, get_mc15c_systematics_tex_title(h.GetName()))

        self.leg.SetTextSizePixels(16)
        self.leg.SetFillStyle(0)
        self.leg.Draw()

BREAKDOWN_SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM_SUBSTRUCTURE + SYSTEMATICS_MC15C_MEDIUM_KINEMATIC

def make_pythia_systematics_breakdown_plot(var_name, **kwargs):
    h_nominal = RAW.get_hist(["pythia_dijet","nominal"], var_name)
    sys_dict = RAW.get_systematics_dict(var_name, BREAKDOWN_SYSTEMATICS, ["pythia_dijet"])
    return PlotPythiaSystematicsBreakdown(
            TH1Sys(h_nominal, sys_dict),
            name = var_name + "_sys_breakdown",
            lumi_val = "3.5",
            width = 600,
            y_max = 70,
            y_min = 0,
            atlas_mod = "Internal",
            legend_loc = [0.70,0.93,0.94,0.62],
            **kwargs)

DEF_LINES = [
            "Trimmed anti-#it{k_{t}} #it{R}=1.0",
            "Dijet Selection",
            "R_{trk} Uncertainties Breakdown"
            ]

MASS_REBIN = 4

breakdown_plots = [
        make_pythia_systematics_breakdown_plot("h_rljet0_m",
            extra_legend_lines = DEF_LINES,
            x_title = "Large-R Jet Mass",
            x_max = 400,
            rebin = MASS_REBIN,
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_d2",
            extra_legend_lines = DEF_LINES + ["m_{calo} > 50 GeV"],
            x_title = "Large-R Jet #it{D_{2}^{#beta = 1}}",
            x_units = "",
            x_max = 4.0
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_m_w_prerec_50eff_d2cut",
            empty_scale = 1.45,
            extra_legend_lines = DEF_LINES + ["Smooth D_{2}-tagged"],
            x_max = 400,
            rebin = MASS_REBIN,
            x_title = "Large-R Jet Mass"
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_tau32",
            extra_legend_lines = DEF_LINES + ["m_{calo} > 100 GeV"],
            x_title = "Large-R Jet #it{#tau_{32}^{WTA}}",
            x_units = "",
            x_max = 0.9,
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_m_t_prerec_50eff_tau32cut",
            extra_legend_lines = DEF_LINES + ["Smooth #tau_{32}-tagged"],
            x_max = 400,
            rebin = MASS_REBIN,
            x_title = "Large-R Jet Mass"
            )
        ]

for i in range(len(breakdown_plots)):
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + breakdown_plots[i].name + ".pdf")
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + "data16_mc15c_systematics_breakdown_plots.pdf", i, len(breakdown_plots))
