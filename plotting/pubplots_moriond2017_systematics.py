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

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

RAW = DMDLoader("./raw/dijet/17-02-2017__15:37:30__02072017_gridjobs_withQwSplit23Sys/cp.merged.root")
ROOT_OUTPUT_DIR = "./raw/dijet/17-02-2017__15:37:30__02072017_gridjobs_withQwSplit23Sys/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/pubplots_moriond2017_systematics"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

BREAKDOWN_SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM
MASS_PLOT_REBIN = 16

class PlotPythiaSystematicsBreakdown(PlotBase):
    def __init__(self, hsys_pythia, **kwargs):

        super(PlotPythiaSystematicsBreakdown, self).__init__(
                x_title = get_axis_title(hsys_pythia.h_nominal.GetName()),
                **kwargs)

        self.hsys_pythia = hsys_pythia
        if (self.rebin != None): self.hsys_pythia.rebin(self.rebin)
        self.hsys_pythia._compute_errors()

        h_nominal = self.hsys_pythia.h_nominal.Clone()
        self.canvas.SetRightMargin(0.07)

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


def make_pythia_systematics_breakdown_plot(var_name, **kwargs):
    h_nominal = RAW.get_hist(["pythia_dijet","nominal"], var_name)
    sys_dict  = RAW.get_systematics_dict(var_name, BREAKDOWN_SYSTEMATICS, ["pythia_dijet"])
    return PlotPythiaSystematicsBreakdown(
            TH1Sys(h_nominal, sys_dict),
            name = var_name + "_sys_breakdown",
            lumi_val = "36.5",
            width = 600,
            y_min = 0,
            atlas_mod = "Internal",
            legend_loc = [0.70,0.93,0.94,0.62],
            **kwargs)

DEF_LINES = [
            "Trimmed anti-#it{k_{t}} #it{R}=1.0",
            "Dijet Selection",
            "R_{trk} Uncertainties Breakdown",
            "#color[2]{NOT FOR APPROVAL}"
            ]


breakdown_plots = []

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag50eff_JSSCut",
        empty_scale = 2.0,
        extra_legend_lines = DEF_LINES + ["m^{comb} + #tau_{32}^{WTA}, 50% w.p."],
        log_scale = True,
        x_min = 0,
        x_max = 400,
        y_max = 40,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16Top_MassTau32Tag80eff_JSSCut",
        empty_scale = 1.8,
        extra_legend_lines = DEF_LINES + ["m^{comb} + #tau_{32}^{WTA}, 80% w.p."],
        log_scale = True,
        x_min = 0,
        x_max = 400,
        y_max = 30,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag50eff",
        empty_scale = 2.15,
        extra_legend_lines = DEF_LINES + ["Q_{w} + #tau_{32}^{WTA}, 50% w.p."],
        x_min = 90,
        x_max = 400,
        y_max = 50,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16Top_QwTau32Tag80eff",
        empty_scale = 1.8,
        extra_legend_lines = DEF_LINES + ["Q_{w} + #tau_{32}^{WTA}, 80% w.p."],
        x_min = 70,
        x_max = 400,
        y_max = 40,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag50eff",
        empty_scale = 2.45,
        extra_legend_lines = DEF_LINES + ["#sqrt{d_{23}}  + #tau_{32}^{WTA}, 50% w.p."],
        x_min = 50.01,
        x_max = 400,
        y_max = 40,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16Top_Tau32Split23Tag80eff",
        empty_scale = 1.8,
        extra_legend_lines = DEF_LINES + ["#sqrt{d_{23}}  + #tau_{32}^{WTA}, 80% w.p."],
        x_min = 50,
        x_max = 400,
        y_max = 40,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16WTag_50eff_JSSCut",
        empty_scale = 1.3,
        extra_legend_lines = DEF_LINES + ["m^{comb} + D_{2}^{#beta = 1}, 50% w.p."],
        x_min = 0,
        x_max = 400,
        y_max = 40,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16WTag_80eff_JSSCut",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["m^{comb} + D_{2}^{#beta = 1}, 80% w.p."],
        x_min = 0,
        x_max = 400,
        y_max = 30,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16ZTag_50eff_JSSCut",
        empty_scale = 1.3,
        extra_legend_lines = DEF_LINES + ["m^{comb} + D_{2}^{#beta = 1}, 50% w.p."],
        x_min = 0,
        x_max = 400,
        y_max = 40,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots.append(make_pythia_systematics_breakdown_plot( "h_rljet0_m_comb_" + "smooth16ZTag_80eff_JSSCut",
        empty_scale = 1.75,
        extra_legend_lines = DEF_LINES + ["m^{comb} + D_{2}^{#beta = 1}, 80% w.p."],
        x_min = 0,
        x_max = 400,
        y_max = 30,
        rebin = MASS_PLOT_REBIN,
        ))

breakdown_plots += [
        make_pythia_systematics_breakdown_plot("h_rljet0_m_comb",
            empty_scale = 2.5,
            extra_legend_lines = DEF_LINES,
            x_max = 400,
            y_max = 40,
            log_scale = True,
            rebin = MASS_PLOT_REBIN,
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_Split23__combMgt100GeV",
            empty_scale = 6,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200e3,
            x_units = "MeV",
            y_max = 150,
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_D2__combMgt50GeV",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 50 GeV"],
            x_units = "",
            rebin = 4,
            y_max = 25,
            x_max = 4.0,
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_Tau32_wta__combMgt100GeV",
            empty_scale = 1.9,
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            x_units = "",
            x_min = 0.01,
            x_max = 0.9,
            y_max = 35,
            rebin = 4
            ),

        make_pythia_systematics_breakdown_plot("h_rljet0_Qw__combMgt100GeV",
            extra_legend_lines = DEF_LINES + ["m^{comb} > 100 GeV"],
            log_scale = True,
            x_max = 200e3,
            x_units = "MeV",
            rebin = 5,
            y_max = 100,
            empty_scale = 5
            )
        ]

for i in range(len(breakdown_plots)):
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + breakdown_plots[i].name + ".pdf")
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + "data16_mc15c_systematics_breakdown_plots.pdf", i, len(breakdown_plots))
