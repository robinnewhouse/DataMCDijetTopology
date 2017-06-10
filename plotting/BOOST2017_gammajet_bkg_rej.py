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

BIN_BOUNDS = array.array('d', [
    200,
    250,
    300,
    350,
    400,
    450,
    500,
    650,
    ]
    )

TOTAL_VAR_NAME = "h_rljet0_pt_comb"

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

CP_ROOT_FILEPATH = "/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/gammajet/11-05-2017__20:38:48__08052017_gammajet_tight_v0/cp.merged.root"
RAW = DMDLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/efficiency_plots"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

def rej_rebin(h):
    return h.Rebin(len(BIN_BOUNDS)-1, h.GetName()+"_rebinned", BIN_BOUNDS)

def get_sys_dict_eff(var_name):
    # get the Rtrk systematics and rebin them
    dict = RAW.get_systematics_dict(var_name, SYSTEMATICS_MC15C_MEDIUM, ["sherpa_gammajet"])
    for sys_name, var_dict in dict.iteritems():
        var_dict["up"] = rej_rebin(var_dict["up"])
        var_dict["down"] = rej_rebin(var_dict["down"])
    return dict

def make_rej_TH1SysEff(gen_name, tag_name):
    is_data = "data" in gen_name
    passed_var_name = TOTAL_VAR_NAME + "_" + tag_name

    h_total = rej_rebin(RAW.get_hist([gen_name, "nominal"], TOTAL_VAR_NAME))
    h_total_dijet = rej_rebin(RAW.get_hist(["pythia_dijet", "nominal"], TOTAL_VAR_NAME))
    h_total.Add(h_total_dijet)

    h_passed = rej_rebin(RAW.get_hist([gen_name, "nominal"], passed_var_name))
    h_passed_dijet = rej_rebin(RAW.get_hist(["pythia_dijet", "nominal"], passed_var_name))
    h_passed.Add(h_passed_dijet)

    if (is_data):
        h_total.Divide(h_passed)
        return h_total.Clone()
    else:
        h_total_data = rej_rebin(RAW.get_hist(["data", "nominal"], TOTAL_VAR_NAME))
        h_passed_data = rej_rebin(RAW.get_hist(["data", "nominal"], passed_var_name))
        h_total.Scale(h_total_data.Integral() / h_total.Integral())
        h_passed.Scale(h_passed_data.Integral() / h_passed.Integral())
        total_sys_dict = {}
        passed_sys_dict = {}
        return TH1SysEff(h_total, total_sys_dict, h_passed, passed_sys_dict)

class PlotGammaJetBkgRej(PlotBase):
    def __init__(self, histos, **kwargs):
        super(PlotGammaJetBkgRej, self).__init__(**kwargs)

        self.h_data      = histos["data"]
        self.hsys_sherpa = histos["sherpa_gammajet"]
        self.h_sherpa    = histos["sherpa_gammajet"].h_nominal
        self.hsys_pythia = histos["pythia_gammajet"]
        self.h_pythia    = histos["pythia_gammajet"].h_nominal

        self.hsys_pythia._compute_errors()
        self.hsys_sherpa._compute_errors()

        self.h_sherpa_sys = self.hsys_sherpa.get_histo_with_systematic_errs()
        self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs()

        set_mc_sys_err_style(self.h_pythia_sys)
        set_mc_sys_err_style(self.h_sherpa_sys)

        self.determine_y_axis_title(self.h_data, "1/#epsilon_{QCD}", show_binwidth = False)

        set_data_style_simple(self.h_data)
        set_mc_style_marker(self.h_pythia, kRed, shape = 21)
        set_mc_style_marker(self.h_sherpa, kBlue, shape = 22)

        for h in [self.h_data, self.h_pythia, self.h_pythia_sys, self.h_sherpa, self.h_sherpa_sys]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)

        if self.log_scale: self.canvas.SetLogy()

        self.h_sherpa_ratio = self.h_sherpa.Clone("h_sherpa_ratio")
        self.h_sherpa_ratio.Divide(self.h_data, self.h_sherpa_ratio, 1, 1, "")

        self.h_pythia_ratio = self.h_pythia.Clone("h_pythia_ratio")
        self.h_pythia_ratio.Divide(self.h_data, self.h_pythia_ratio, 1, 1, "")

        self.h_sherpa_sys_ratio = self.h_sherpa_sys.Clone("h_sherpa_sys_ratio")
        self.h_sherpa_sys_ratio.Divide(self.h_data, self.h_sherpa_sys_ratio, 1, 1, "")
        self.h_sherpa_stat_ratio = self.h_sherpa_ratio.Clone("h_sherpa_stat_ratio")

        # center the stat. and syst. errors around data/mc ratio = 1.0
        for ibin in range(self.h_sherpa_sys_ratio.GetSize()):
            self.h_sherpa_sys_ratio.SetBinContent(ibin, 1.0)
            self.h_sherpa_stat_ratio.SetBinContent(ibin, 1.0)

        ratio_title = "#frac{Data}{MC}"
        for h_ratio in [ self.h_sherpa_ratio, self.h_sherpa_sys_ratio, self.h_sherpa_stat_ratio ]:
            set_style_ratio(h_ratio, y_title = ratio_title, y_min = 0.5, y_max = 1.5)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)
            h_ratio.GetXaxis().SetLabelSize(19)

        set_mc_sys_err_style(self.h_sherpa_sys)
        set_mc_style_line(self.h_sherpa_ratio, kBlue, line_width = 3)
        set_mc_sys_err_style(self.h_sherpa_sys_ratio)

        self.h_sherpa_sys_ratio.SetMarkerSize(0)
        self.h_sherpa_stat_ratio.SetMarkerSize(0)
        self.h_sherpa_sys_ratio.SetFillColor(kGreen-8)
        self.h_sherpa_stat_ratio.SetFillColor(kGreen-5)

        self.h_sherpa_stat_ratio.SetFillStyle(1001)
        self.h_sherpa_stat_ratio.SetLineWidth(0)

        self.h_sherpa_ratio.SetFillStyle(0)
        self.h_sherpa_ratio.SetFillColorAlpha(kBlue, 0.85)

        set_mc_style_marker(self.h_pythia_ratio, kRed, shape = 21)
        set_mc_style_marker(self.h_sherpa_ratio, kBlue, shape = 22)

        # TODO: necessary?
        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

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

        self.h_pythia.Draw("PE1,same")
        self.h_sherpa.Draw("PE1,same")
        self.h_data.Draw("PE1,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2015 + 2016")
        self.leg.AddEntry(self.h_pythia, "Pythia8 MC")
        self.leg.AddEntry(self.h_sherpa, "Sherpa MC")
        self.leg.AddEntry(self.h_pythia_sys, "Stat. uncert.")
        self.leg.Draw()

        self.pad2.cd()
        if (self.hsys_sherpa.num_systematics != 0):
            self.h_sherpa_sys_ratio.Draw("E2,same")
        self.h_sherpa_stat_ratio.Draw("E2,same")
        self.h_pythia_ratio.Draw("PE,same")
        self.h_sherpa_ratio.Draw("PE,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.canvas.Clear()

DEF_EXTRA_LINES = ["Trimmed anti-#it{k_{t}} #it{R}=1.0", "#gamma + jet selection"]

def make_pt_rej_plot( tag_name, **kwargs):

    histos = {}
    for gen in ["data","sherpa_gammajet","pythia_gammajet"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name)

    return PlotGammaJetBkgRej(
            histos,
            name = tag_name + "_rej",
            lumi_val = "36.5",
            atlas_mod = "Internal",
            legend_loc = [0.62,0.93,0.89,0.66],
            x_title = "Leading Large-R Jet #it{p_{T}}",
            x_max = 3000,
            y_min = 0.001,
            width = 600,
            **kwargs)

bkg_rej_plots = [
        make_pt_rej_plot(
            "smooth16Top_MassTau32Tag50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 50%" ],
            x_min = 350,
            y_max = 300,
            ),

        make_pt_rej_plot(
            "smooth16Top_MassTau32Tag80eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 80%" ],
            x_min = 350,
            y_max = 100,
            ),

        make_pt_rej_plot(
            "smooth16Top_QwTau32Tag50eff",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 50%" ],
            x_min = 350,
            y_max = 300
            ),

        make_pt_rej_plot(
            "smooth16Top_QwTau32Tag80eff",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 80%" ],
            x_min = 350,
            y_max = 100,
            ),

        make_pt_rej_plot(
            "smooth16Top_Tau32Split23Tag50eff",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 50%" ],
            x_min = 350,
            y_max = 300,
            ),

        make_pt_rej_plot(
            "smooth16Top_Tau32Split23Tag80eff",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 80%" ],
            x_min = 350,
            y_max = 100,
            ),

        make_pt_rej_plot(
            "smooth16WTag_50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
            x_min = 200,
            y_max = 175
            ),

        make_pt_rej_plot(
            "smooth16WTag_80eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
            x_min = 200,
            y_max = 50
            ),

        make_pt_rej_plot(
            "smooth16ZTag_50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
            x_min = 200,
            y_max = 150
            ),

        make_pt_rej_plot(
            "smooth16ZTag_80eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
            x_min = 200,
            y_max = 50
            )

        ]
