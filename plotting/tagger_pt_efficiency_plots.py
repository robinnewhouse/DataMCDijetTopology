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

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/efficiency_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

class PlotDataPythiaHerwigEfficiency(PlotBase):
    def __init__(self, h_data, hsys_pythia, hsys_herwig, rejection, **kwargs):
        super(PlotDataPythiaHerwigEfficiency, self).__init__(**kwargs)

        self.h_data = h_data
        self.hsys_pythia = hsys_pythia
        self.hsys_herwig = hsys_herwig

        self.hsys_pythia._compute_errors()
        self.hsys_herwig._compute_errors()

        self.h_pythia_nominal = self.hsys_pythia.h_nominal.Clone()
        self.h_herwig_nominal = self.hsys_herwig.h_nominal.Clone()

        self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs(
                include_stat_error = True)
        self.h_herwig_sys = self.hsys_herwig.get_histo_with_systematic_errs(
                include_stat_error = True)

        if rejection:
            self.determine_y_axis_title(h_data, "1/#epsilon_{QCD}")
        else:
            self.determine_y_axis_title(h_data, "#epsilon_{QCD}")

        set_mc_style_marker(self.h_pythia_nominal, kRed, shape = 21)
        set_mc_style_marker(self.h_herwig_nominal, kBlue, shape = 22)
        set_mc_sys_err_style(self.h_pythia_sys, kRed)
        set_mc_sys_err_style(self.h_herwig_sys, kBlue)

        for h in [self.h_data, self.h_pythia_nominal, self.h_herwig_nominal,
                self.h_pythia_sys, self.h_herwig_sys]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)

        self.h_herwig_sys.Draw("E2")
        self.h_pythia_sys.Draw("E2,same")
        self.h_herwig_nominal.Draw("PE,same")
        self.h_pythia_nominal.Draw("PE,same")
        self.h_data.Draw("PE,same")

        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia_nominal, "Pythia8")
        self.leg.AddEntry(self.h_herwig_nominal, "Herwig++")
        self.leg.Draw()

DEF_EXTRA_LINES = [
            "Trimmed anti-#it{k_{t}} #it{R}=1.0",
            "Dijet Selection"
            ]

SYSTEMATICS = SYSTEMATICS_MC15C_MEDIUM_KINEMATIC + SYSTEMATICS_MC15C_MEDIUM_SUBSTRUCTURE

REBIN_PT = 4

def make_pt_efficiency_plot(tag_name, rejection = True, **kwargs):
    h_data_total = RAW.get_hist(["data","nominal"], "h_rljet0_pt")
    h_pythia_total = RAW.get_hist(["pythia_dijet","nominal"], "h_rljet0_pt")
    h_herwig_total = RAW.get_hist(["herwig_dijet","nominal"], "h_rljet0_pt")

    h_data_total.Rebin(REBIN_PT)
    h_pythia_total.Rebin(REBIN_PT)
    h_herwig_total.Rebin(REBIN_PT)

    passed_name = "h_rljet0_pt_" + tag_name

    h_data_passed = RAW.get_hist(["data","nominal"], passed_name)
    h_pythia_passed = RAW.get_hist(["pythia_dijet","nominal"], passed_name)
    h_herwig_passed = RAW.get_hist(["herwig_dijet","nominal"], passed_name)

    h_data_passed.Rebin(REBIN_PT)
    h_pythia_passed.Rebin(REBIN_PT)
    h_herwig_passed.Rebin(REBIN_PT)

    sys_dict_pythia_total = RAW.get_systematics_dict("h_rljet0_pt" , SYSTEMATICS , ["pythia_dijet"])
    sys_dict_herwig_total = RAW.get_systematics_dict("h_rljet0_pt" , SYSTEMATICS , ["herwig_dijet"])
    sys_dict_pythia_passed = RAW.get_systematics_dict(passed_name  , SYSTEMATICS , ["pythia_dijet"])
    sys_dict_herwig_passed = RAW.get_systematics_dict(passed_name  , SYSTEMATICS , ["herwig_dijet"])

    rebin_sys_dict(sys_dict_pythia_total, REBIN_PT)
    rebin_sys_dict(sys_dict_herwig_total, REBIN_PT)
    rebin_sys_dict(sys_dict_pythia_passed, REBIN_PT)
    rebin_sys_dict(sys_dict_herwig_passed, REBIN_PT)

    h_data      = None
    hsys_pythia = None
    hsys_herwig = None
    eff_name    = None

    if rejection:
        h_data = h_data_total.Clone()
        h_data.Divide(h_data_passed)

        hsys_pythia = TH1SysEff(h_pythia_total, sys_dict_pythia_total,
                h_pythia_passed, sys_dict_pythia_passed)

        hsys_herwig = TH1SysEff(h_herwig_total, sys_dict_herwig_total,
                h_herwig_passed, sys_dict_herwig_passed)

        eff_name = passed_name + "_rej"
    else:
        h_data = h_data_passed.Clone()
        h_data.Divide(h_data_total)

        hsys_pythia = TH1SysEff(h_pythia_passed, sys_dict_pythia_passed,
                h_pythia_total, sys_dict_pythia_total)

        hsys_herwig = TH1SysEff(h_herwig_passed, sys_dict_herwig_passed,
                h_herwig_total, sys_dict_herwig_total)

        eff_name = passed_name + "_eff"

    return PlotDataPythiaHerwigEfficiency(
            h_data,
            hsys_pythia,
            hsys_herwig,
            rejection,
            name = eff_name,
            lumi_val = "3.5",
            atlas_mod = "Internal",
            legend_loc = [0.75,0.93,0.94,0.79],
            x_title = "Leading Large-R Jet #it{p_{T}}",
            x_min = 500,
            x_max = 2000,
            width = 600,
            **kwargs)

def make_htt_pt_efficiency_plot(rejection = True, **kwargs):
    h_data_total = RAW.get_hist(["data","nominal"], "h_htt_caGroomJet0_pt_def")
    h_pythia_total = RAW.get_hist(["pythia_dijet","nominal"], "h_htt_caGroomJet0_pt_def")
    h_herwig_total = RAW.get_hist(["herwig_dijet","nominal"], "h_htt_caGroomJet0_pt_def")

    h_data_total.Rebin(REBIN_PT)
    h_pythia_total.Rebin(REBIN_PT)
    h_herwig_total.Rebin(REBIN_PT)

    h_data_passed = RAW.get_hist(["data","nominal"], "h_htt0_pt_def")
    h_pythia_passed = RAW.get_hist(["pythia_dijet","nominal"], "h_htt0_pt_def")
    h_herwig_passed = RAW.get_hist(["herwig_dijet","nominal"], "h_htt0_pt_def")

    h_data_passed.Rebin(REBIN_PT)
    h_pythia_passed.Rebin(REBIN_PT)
    h_herwig_passed.Rebin(REBIN_PT)

    sys_dict_pythia_total  = RAW.get_htt_systematics_dict("h_htt_caGroomJet0_pt" , "pythia_dijet", "sjcalib0955", "sjcalib1045")
    sys_dict_herwig_total  = RAW.get_htt_systematics_dict("h_htt_caGroomJet0_pt" , "herwig_dijet", "sjcalib0955", "sjcalib1045")
    sys_dict_pythia_passed = RAW.get_htt_systematics_dict("h_htt0_pt"            , "pythia_dijet", "sjcalib0955", "sjcalib1045")
    sys_dict_herwig_passed = RAW.get_htt_systematics_dict("h_htt0_pt"            , "herwig_dijet", "sjcalib0955", "sjcalib1045")

    rebin_sys_dict(sys_dict_pythia_total, REBIN_PT)
    rebin_sys_dict(sys_dict_herwig_total, REBIN_PT)
    rebin_sys_dict(sys_dict_pythia_passed, REBIN_PT)
    rebin_sys_dict(sys_dict_herwig_passed, REBIN_PT)

    h_data      = None
    hsys_pythia = None
    hsys_herwig = None
    eff_name    = None

    if rejection:
        h_data = h_data_total.Clone()
        h_data.Divide(h_data_passed)

        hsys_pythia = TH1SysEff(h_pythia_total, sys_dict_pythia_total,
                h_pythia_passed, sys_dict_pythia_passed)

        hsys_herwig = TH1SysEff(h_herwig_total, sys_dict_herwig_total,
                h_herwig_passed, sys_dict_herwig_passed)

        eff_name = "htt0_pt_rej"
    else:
        h_data = h_data_passed.Clone()
        h_data.Divide(h_data_total)

        hsys_pythia = TH1SysEff(h_pythia_passed, sys_dict_pythia_passed,
                h_pythia_total, sys_dict_pythia_total)

        hsys_herwig = TH1SysEff(h_herwig_passed, sys_dict_herwig_passed,
                h_herwig_total, sys_dict_herwig_total)

        eff_name = "htt0_pt_eff"

    return PlotDataPythiaHerwigEfficiency(
            h_data,
            hsys_pythia,
            hsys_herwig,
            rejection,
            name = eff_name,
            lumi_val = "3.5",
            atlas_mod = "Internal",
            legend_loc = [0.75,0.93,0.94,0.79],
            x_title = "Leading Large-R Jet #it{p_{T}}",
            x_min = 500,
            x_max = 2000,
            width = 600,
            **kwargs)

breakdown_plots = [
        make_pt_efficiency_plot(
            "w_prerec_50eff_D2masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth D_{2}-Tagged", "#epsilon_{sig} = 50%" ],
            y_max = 150
            ),

        make_pt_efficiency_plot(
            "w_prerec_25eff_D2masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth D_{2}-Tagged", "#epsilon_{sig} = 25%" ],
            y_max = 600
            ),

        make_pt_efficiency_plot(
            "t_prerec_50eff_Tau32masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth #tau_{32}-Tagged", "#epsilon_{sig} = 50%" ],
            y_max = 35
            ),

        make_pt_efficiency_plot(
            "t_prerec_80eff_Tau32masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth #tau_{32}-Tagged", "#epsilon_{sig} = 80%" ],
            y_max = 10
            ),

        make_pt_efficiency_plot(
            "w_prerec_50eff_D2masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth D_{2}-Tagged", "#epsilon_{sig} = 50%" ],
            rejection = False,
            y_max = 0.1
            ),

        make_pt_efficiency_plot(
            "w_prerec_25eff_D2masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth D_{2}-Tagged", "#epsilon_{sig} = 25%" ],
            rejection = False,
            y_max = 0.015
            ),

        make_pt_efficiency_plot(
            "t_prerec_50eff_Tau32masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth #tau_{32}-Tagged", "#epsilon_{sig} = 50%" ],
            rejection = False,
            y_max = 0.3
            ),

        make_pt_efficiency_plot(
            "t_prerec_80eff_Tau32masscut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth #tau_{32}-Tagged", "#epsilon_{sig} = 80%" ],
            rejection = False,
            y_max = 0.5
            ),

        make_htt_pt_efficiency_plot(
            rejection = False,
            extra_legend_lines = DEF_EXTRA_LINES + [ "HTT Top Candidate" ],
            y_max = 0.07
            ),

        make_htt_pt_efficiency_plot(
            rejection = True,
            extra_legend_lines = DEF_EXTRA_LINES + [ "HTT Top Candidate" ],
            y_max = 325
            )
        ]

for i in range(len(breakdown_plots)):
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + breakdown_plots[i].name + ".pdf")
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + "data16_mc15c_efficiency_plots", i, len(breakdown_plots))
