from ROOT import *
import atlas_style

import os
from math import *
import array
from sys import argv, exit

from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)

RAW = DMDLoader("./raw/dijet/12-01-2017__15:26:26__16122016_gridjobs_nominalOnly_v1/cp.merged.root")
ROOT_OUTPUT_DIR = "./raw/dijet/12-01-2017__15:26:26__16122016_gridjobs_nominalOnly_v1/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/efficiency_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

class PlotDataPythiaHerwigEfficiency(PlotBase):
    def __init__(
            self,
            h_data,
            h_data_ref,
            h_pythia,
            h_herwig,
            h_sherpa,
            **kwargs):
        super(PlotDataPythiaHerwigEfficiency, self).__init__(**kwargs)

        show_ref = h_data_ref != None

        self.h_data = h_data
        self.h_data_ref = h_data_ref if show_ref else h_data.Clone()
        self.h_pythia = h_pythia
        self.h_herwig = h_herwig
        self.h_sherpa = h_sherpa

        self.determine_y_axis_title(h_data, "1/#epsilon_{QCD}", show_binwidth = False)

        set_mc_style_marker(self.h_pythia, kRed, shape = 21)
        set_mc_style_marker(self.h_herwig, kBlue, shape = 22)
        set_mc_style_marker(self.h_sherpa, kGreen, shape = 23)

        self.h_data_ref.SetLineColor(kGray+1)
        self.h_data_ref.SetMarkerSize(0)

        for h in [self.h_data, self.h_data_ref, self.h_pythia, self.h_herwig, self.h_sherpa]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)

        if self.log_scale: self.canvas.SetLogy()

        self.h_sherpa.Draw("PE1,same")
        self.h_herwig.Draw("PE1,same")
        self.h_pythia.Draw("PE1,same")
        self.h_data.Draw("PE1,same")
        if (show_ref):
            self.h_data_ref.Draw("hist,same")

        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia, "Pythia8 dijet")
        self.leg.AddEntry(self.h_herwig, "Herwig++ dijet")
        self.leg.AddEntry(self.h_sherpa, "Sherpa dijet")
        if (show_ref):
            self.leg.AddEntry(self.h_data_ref, "2015 Tagger Ref.")
        self.leg.Draw()

DEF_EXTRA_LINES = [
            "Trimmed anti-#it{k_{t}} #it{R}=1.0",
            "Dijet Selection"
            ]

def make_pt_efficiency_plot(
        tag_name,
        ref_tag_name,
        rejection = True,
        **kwargs):
    BIN_BOUNDS = array.array('d', [450,500,550,600,700,800,900,1000,1100,1200,1300,1400,1500,1700,2000,2500])

    base_var_name = "h_rljet0_pt_comb"

    h_data_total_tmp     = RAW.get_sigsub_data(base_var_name)
    h_pythia_total_tmp   = RAW.get_normalized_dijet("pythia", base_var_name)
    h_herwig_total_tmp   = RAW.get_normalized_dijet("herwig", base_var_name)
    h_sherpa_total_tmp   = RAW.get_normalized_dijet("sherpa", base_var_name)

    h_data_total     = h_data_total_tmp.Rebin(len(BIN_BOUNDS)-1, h_data_total_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    h_pythia_total   = h_pythia_total_tmp.Rebin(len(BIN_BOUNDS)-1, h_pythia_total_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    h_herwig_total   = h_herwig_total_tmp.Rebin(len(BIN_BOUNDS)-1, h_herwig_total_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    h_sherpa_total   = h_sherpa_total_tmp.Rebin(len(BIN_BOUNDS)-1, h_sherpa_total_tmp.GetName()+"_rebinned", BIN_BOUNDS)

    passed_name = base_var_name + "_" + tag_name
    if (ref_tag_name != None):
        ref_passed_name = base_var_name + "_" + ref_tag_name

    h_data_passed_tmp     = RAW.get_sigsub_data(passed_name)
    h_pythia_passed_tmp   = RAW.get_normalized_dijet("pythia", passed_name)
    h_herwig_passed_tmp   = RAW.get_normalized_dijet("herwig", passed_name)
    h_sherpa_passed_tmp   = RAW.get_normalized_dijet("herwig", passed_name)
    if (ref_tag_name != None):
        h_data_passed_ref_tmp = RAW.get_sigsub_data(ref_passed_name)

    h_data_passed     = h_data_passed_tmp.Rebin(len(BIN_BOUNDS)-1, h_data_passed_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    h_pythia_passed   = h_pythia_passed_tmp.Rebin(len(BIN_BOUNDS)-1, h_pythia_passed_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    h_herwig_passed   = h_herwig_passed_tmp.Rebin(len(BIN_BOUNDS)-1, h_herwig_passed_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    h_sherpa_passed   = h_sherpa_passed_tmp.Rebin(len(BIN_BOUNDS)-1, h_sherpa_passed_tmp.GetName()+"_rebinned", BIN_BOUNDS)
    if (ref_tag_name != None):
        h_data_passed_ref = h_data_passed_ref_tmp.Rebin(len(BIN_BOUNDS)-1, h_data_passed_ref_tmp.GetName()+"_rebinned", BIN_BOUNDS)

    # h_data      = None
    # h_data_ref  = None
    # hsys_pythia = None
    # hsys_herwig = None
    # hsys_sherpa = None
    # eff_name    = None

    if rejection:
        h_data = h_data_total.Clone()
        h_data.Divide(h_data_passed)
        h_data_ref = h_data_total.Clone()

        if (ref_tag_name != None):
            h_data_ref.Divide(h_data_passed_ref)
        else:
            h_data_ref = None

        hsys_pythia = TH1SysEff(h_pythia_total, None, h_pythia_passed, None)
        hsys_herwig = TH1SysEff(h_herwig_total, None, h_herwig_passed, None)
        hsys_sherpa = TH1SysEff(h_sherpa_total, None, h_sherpa_passed, None)

        eff_name = passed_name + "_rej"
    else:
        h_data = h_data_passed.Clone()
        h_data.Divide(h_data_total)
        h_data_ref = h_data_passed_ref.Clone()

        if (ref_tag_name != None):
            h_data_ref.Divide(h_data_total)
        else:
            h_data_ref = None

        hsys_pythia = TH1SysEff(h_pythia_passed, None, h_pythia_total, None)
        hsys_herwig = TH1SysEff(h_herwig_passed, None, h_herwig_total, None)
        hsys_sherpa = TH1SysEff(h_sherpa_passed, None, h_sherpa_total, None)

        eff_name = passed_name + "_eff"

    return PlotDataPythiaHerwigEfficiency(
            h_data,
            h_data_ref,
            hsys_pythia.h_nominal,
            hsys_herwig.h_nominal,
            hsys_sherpa.h_nominal,
            name = eff_name,
            lumi_val = "36.5",
            atlas_mod = "Internal",
            legend_loc = [0.67,0.93,0.92,0.69],
            x_title = "Leading Large-R Jet #it{p_{T}}",
            x_min = 450,
            x_max = 3000,
            y_min = 0.001,
            width = 600,
            **kwargs)

breakdown_plots = [
        make_pt_efficiency_plot(
            "smooth16Top_MassTau32Tag50eff_MassJSSCut",
            "smooth15Top_MassTau32Tag50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 50%" ],
            y_max = 100,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_MassTau32Tag80eff_MassJSSCut",
            "smooth15Top_MassTau32Tag80eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 80%" ],
            y_max = 35,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_QwTau32Tag50eff",
            "smooth15Top_MassTau32Tag50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 50%" ],
            y_max = 120
            ),

        make_pt_efficiency_plot(
            "smooth16Top_QwTau32Tag80eff",
            "smooth15Top_MassTau32Tag80eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 80%" ],
            y_max = 35,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_Tau32Split23Tag50eff",
            "smooth15Top_MassTau32Tag50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 50%" ],
            y_max = 100,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_Tau32Split23Tag80eff",
            "smooth15Top_MassTau32Tag80eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 80%" ],
            y_max = 35,
            ),

        make_pt_efficiency_plot(
            "smooth16WTag_50eff_MassJSSCut",
            "smooth15WTag_50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
            y_max = 160
            ),

        make_pt_efficiency_plot(
            "smooth16WTag_80eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
            y_max = 30
            ),

        make_pt_efficiency_plot(
            "smooth16ZTag_50eff_MassJSSCut",
            "smooth15ZTag_50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
            y_max = 160
            ),

        make_pt_efficiency_plot(
            "smooth16ZTag_80eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
            y_max = 30
            ),

        # make_pt_efficiency_plot(
        #     "BDT_top_qqb_JSSCut",
        #     "smooth15Top_MassTau32Tag50eff_MassJSSCut",
        #     extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth #tau_{32}-Tagged", "#epsilon_{sig} = 50%" ],
        #     y_max = 200
        #     ),

        # make_pt_efficiency_plot(
        #     "BDT_w_JSSCut",
        #     "smooth15WTag_50eff_MassJSSCut",
        #     extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth #tau_{32}-Tagged", "#epsilon_{sig} = 50%" ],
        #     y_max = 200
        #     ),

        ]

for i in range(len(breakdown_plots)):
    breakdown_plots[i].print_to_file(OUTPUT_DIR + "/" + breakdown_plots[i].name + ".pdf")
