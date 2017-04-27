from ROOT import *
import atlas_style

#import os
#from math import *
import array
#from sys import argv, exit
#
from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *
from plot_systematics_breakdown import *

WRITE_PLOTS = True
BIN_BOUNDS = array.array('d', [
    450,
    500,
    550,
    600,
    700,
    800,
    900,
    1000,
    1100,
    1200,
    1300,
    1400,
    1500,
    1700,
    2000,
    2500]
    )

TOTAL_VAR_NAME = "h_rljet0_pt_comb"

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

CP_ROOT_FILEPATH = "/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/dijet/17-02-2017__15:37:30__02072017_gridjobs_withQwSplit23Sys/cp.merged.root"
RAW = DMDLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/efficiency_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

def calculate_inclusive_efficiency(h_tagged, h_inclusive):
    assert(h_tagged.GetSize() == h_inclusive.GetSize())

    integral_lower_bound = 1 # ignore underflow
    integral_upper_bound = h_inclusive.GetSize()+1

    tagged_err, inclusive_err = Double(), Double()

    tagged_count = h_tagged.IntegralAndError(integral_lower_bound,
            integral_upper_bound, tagged_err)

    inclusive_count = h_inclusive.IntegralAndError(integral_lower_bound,
            integral_upper_bound, inclusive_err)

    nominal_efficiency = tagged_count / inclusive_count

    total_err = nominal_efficiency * sqrt(
            (tagged_err / tagged_count)**2 + (inclusive_err / inclusive_count)**2
            )

    return nominal_efficiency, total_err

def calculate_inclusive_rejection(h_tagged, h_inclusive):
    eff, eff_err = calculate_inclusive_efficiency(h_tagged, h_inclusive)
    rej = 1.0 / eff
    rej_err = (1.0 / eff**2) * eff_err

    sig_figs = -int(floor(log10(abs(rej_err))))
    rej = round(rej, sig_figs)
    rej_err = round(rej_err, sig_figs)

    return rej, rej_err

def rej_rebin(h):
    return h.Rebin(len(BIN_BOUNDS)-1, h.GetName()+"_rebinned", BIN_BOUNDS)

def get_sys_dict_eff(gen_name, var_name):
    # get the Rtrk systematics and rebin them
    dict = RAW.get_systematics_dict(var_name, SYSTEMATICS_MC15C_MEDIUM, [gen_name + "_dijet"])
    for sys_name, var_dict in dict.iteritems():
        var_dict["up"] = rej_rebin(var_dict["up"])
        var_dict["down"] = rej_rebin(var_dict["down"])
    # create the sig. norm. systematics
    dict["sig_norm_sf"] = {}
    h_tmp_up = rej_rebin(RAW.get_normalized_dijet(gen_name, var_name, sig_sf = 1.25))
    h_tmp_down = rej_rebin(RAW.get_normalized_dijet(gen_name, var_name, sig_sf = 0.75))
    h_tmp_up.SetName(h_tmp_up.GetName() + "_sig_norm_sf_up")
    h_tmp_down.SetName(h_tmp_down.GetName() + "_sig_norm_sf_own")
    dict["sig_norm_sf"]["up"] = h_tmp_up
    dict["sig_norm_sf"]["down"] = h_tmp_down

    return dict

def make_rej_TH1SysEff(gen_name, tag_name):
    is_data = "data" in gen_name
    passed_var_name = TOTAL_VAR_NAME + "_" + tag_name

    h_total = rej_rebin(
            RAW.get_sigsub_data(TOTAL_VAR_NAME)
            if is_data
            else RAW.get_normalized_dijet(gen_name, TOTAL_VAR_NAME)
            )

    h_passed = rej_rebin(
            RAW.get_sigsub_data(passed_var_name)
            if is_data
            else RAW.get_normalized_dijet(gen_name, passed_var_name)
            )

    if (is_data):
        h_total.Divide(h_passed)
        return h_total.Clone()
    else:
        total_sys_dict = get_sys_dict_eff(gen_name, TOTAL_VAR_NAME)
        passed_sys_dict = get_sys_dict_eff(gen_name, passed_var_name)
        return TH1SysEff(h_total, total_sys_dict, h_passed, passed_sys_dict)

class PlotDataPythiaHerwigEfficiency(PlotBase):
    def __init__(self, histos, **kwargs):
        super(PlotDataPythiaHerwigEfficiency, self).__init__(**kwargs)

        show_ref = "data_ref" in histos

        self.h_data      = histos["data"]
        self.h_data_ref  = histos["data_ref"] if show_ref else self.h_data.Clone()
        self.hsys_pythia = histos["pythia"]
        self.h_pythia = histos["pythia"].h_nominal
        self.h_herwig = histos["herwig"].h_nominal
        self.h_sherpa = histos["sherpa"].h_nominal

        self.hsys_pythia._compute_errors()
        self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs()
        set_mc_sys_err_style(self.h_pythia_sys)

        self.determine_y_axis_title(self.h_data, "1/#epsilon_{QCD}", show_binwidth = False)

        set_mc_style_marker(self.h_pythia, kRed, shape = 21)
        set_mc_style_marker(self.h_herwig, kBlue, shape = 22)
        set_mc_style_marker(self.h_sherpa, kGreen, shape = 23)

        self.h_data_ref.SetLineColor(kGray+1)
        self.h_data_ref.SetMarkerSize(0)

        for h in [self.h_data, self.h_data_ref, self.h_pythia, self.h_herwig, self.h_sherpa, self.h_pythia_sys]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)

        if self.log_scale: self.canvas.SetLogy()

        self.h_pythia_sys.Draw("E2")
        #self.h_sherpa.Draw("PE1,same")
        self.h_herwig.Draw("PE1,same")
        self.h_pythia.Draw("PE1,same")
        self.h_data.Draw("PE1,same")
        # if (show_ref):
        #     self.h_data_ref.Draw("hist,same")

        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia, "Pythia8 dijet")
        self.leg.AddEntry(self.h_herwig, "Herwig++ dijet")
        #self.leg.AddEntry(self.h_sherpa, "Sherpa dijet")
        # if (show_ref):
        #     self.leg.AddEntry(self.h_data_ref, "2015 Tagger Ref.")
        self.leg.AddEntry(self.h_pythia_sys, "Stat. #oplus syst. uncert.")
        self.leg.Draw()

DEF_EXTRA_LINES = [
            "Trimmed anti-#it{k_{t}} #it{R}=1.0",
            "Dijet Selection"
            ]

def make_pt_efficiency_plot( tag_name, ref_tag_name = None, **kwargs):

    histos = {}
    for gen in ["data","pythia","herwig","sherpa"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name)

    if (ref_tag_name != None):
        histos["data_ref"] = make_rej_TH1SysEff("data_ref", ref_tag_name)

    p = PlotPythiaSystematicsBreakdown(histos["pythia"], name = tag_name + "_SYS", y_min = 0, y_max = 30)
    p.print_to_file(OUTPUT_DIR + "/" + p.name + ".pdf")

    return PlotDataPythiaHerwigEfficiency(
            histos,
            name = tag_name + "_rej",
            lumi_val = "36.5",
            atlas_mod = "Internal",
            legend_loc = [0.67,0.93,0.92,0.69],
            x_title = "Leading Large-R Jet #it{p_{T}}",
            x_min = 450,
            x_max = 2500,
            y_min = 0.001,
            width = 600,
            **kwargs)

bkg_rej_plots = [
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
            )

        ]

if (WRITE_PLOTS):
    for i in range(len(bkg_rej_plots)):
        bkg_rej_plots[i].print_to_file(OUTPUT_DIR + "/" + bkg_rej_plots[i].name + ".pdf")
