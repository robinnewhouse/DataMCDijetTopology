import sys
sys.path.append('lib/')

import atlas_style

import os
#from math import *
import array
from sys import argv, exit
#
from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *
from plot_systematics_breakdown import *

DO_SYSTEMATICS_DEFAULT = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET

PT_BIN_BOUNDS = array.array('d', [
    200,
    #220,
    250,
    #260,
    #285,
    300,
    #325,
    #350,
    350,
    #400,
    #425,
    500,
    #500,
    750,
    1200
    ]
    )


MU_BIN_BOUNDS = array.array('d', [
        5,
        10,
        15,
        20,
        25,
        30,
        35,
        40,
    ])

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/gammajet_20180504_syst/gammajet.merged.cp.root"
HISTLOADER = GammaJetLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_GammaJet"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/bkg_rej"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

def rej_rebin(h, bin_bounds):
  return h.Rebin(len(bin_bounds)-1, h.GetName()+"_rebinned", bin_bounds)

def get_sys_dict_eff(gen_name, var_name, bin_bounds, do_systematics):
    # get the Rtrk systematics and rebin them
    dict = HISTLOADER.get_systematics_dictionary(gen_name, var_name, do_systematics, norm_to_pretagged = True)
    for sys_name, var_dict in dict.iteritems():
        var_dict["up"] = rej_rebin(var_dict["up"], bin_bounds)
        var_dict["down"] = rej_rebin(var_dict["down"], bin_bounds)
    return dict

def make_rej_TH1SysEff(gen_name, tag_name, do_systematics, x_axis = "pt"):
    is_data = "data" in gen_name

    if x_axis == "mu":
        bin_bounds = MU_BIN_BOUNDS
        total_var_name = "h_mu"
    else:
        bin_bounds = PT_BIN_BOUNDS
        if ("HTT" in tag_name):
          total_var_name = "h_htt_caGroomJet0_pt"
        else:
          total_var_name = "h_rljet0_pt_comb"

    passed_var_name = total_var_name + "_" + tag_name


    if (is_data):
        # if x_axis == "mu": total_var_name+="_corrSF"
        h_total = rej_rebin(HISTLOADER.get_sigsub_data(total_var_name), bin_bounds)
        h_passed = rej_rebin(HISTLOADER.get_sigsub_data(passed_var_name), bin_bounds)
        print("h_total.Integral()",h_total.Integral())
        h_total.Divide(h_passed)
        print("h_total.Integral() (divided by h_passed)",h_total.Integral())
        print("DEBUG" , tag_name, total_var_name, passed_var_name)
        return h_total.Clone()
    else:
        h_total = rej_rebin(HISTLOADER.get_normalized_gamma(gen_name, total_var_name, normalize_to_pretagged = True), bin_bounds)
        h_passed = rej_rebin(HISTLOADER.get_normalized_gamma(gen_name, passed_var_name, normalize_to_pretagged = True), bin_bounds)
        total_sys_dict = {}
        passed_sys_dict = {}
        if do_systematics:
            total_sys_dict = get_sys_dict_eff(gen_name, total_var_name, bin_bounds, do_systematics)
            passed_sys_dict = get_sys_dict_eff(gen_name, passed_var_name, bin_bounds, do_systematics)
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

        self.determine_y_axis_title(self.h_data, "Background rejection (1/#epsilon_{bkg})", show_binwidth = False)

        set_data_style_simple(self.h_data)
        set_mc_style_marker(self.h_pythia, kRed, shape = 21)
        set_mc_style_marker(self.h_sherpa, kBlue, shape = 22, marker_size = 1.0, line_width = 3)

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
            set_style_ratio(h_ratio, y_title = ratio_title, y_min = 0.0, y_max = 2.0)
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

        set_mc_style_marker(self.h_pythia_ratio, kRed, shape = 21, line_width = 3, marker_size = 1.0)
        set_mc_style_marker(self.h_sherpa_ratio, kBlue, shape = 22, line_width = 3, marker_size = 1.0)

        # TODO: necessary?
        # self.canvas.Clear()
        # self.canvas = None
        # self._make_canvas()
        # self._make_decorations()

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
        self.leg.AddEntry(self.h_data, "Data - Sig.")
        self.leg.AddEntry(self.h_pythia, "Pythia8 MC #gamma + jet")
        self.leg.AddEntry(self.h_sherpa, "Sherpa #gamma + jet")
        self.leg.AddEntry(self.h_sherpa_stat_ratio, "Stat. uncert.", "f")
        if (self.hsys_sherpa.num_systematics != 0):
            self.leg.AddEntry(self.h_sherpa_sys_ratio, "Sys. #oplus Stat. uncert.", "f")
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
        # self.print_to_file(OUTPUT_DIR + "/" + self.name + "_gammajet.eps")
        self.canvas.Clear()
        print

DEF_EXTRA_LINES = ["Trimmed anti-#it{k_{t}} #it{R}=1.0", "#gamma + jet selection"]
HTT_EXTRA_LINES = ["Trimmed C/A #it{R}=1.5", "#gamma + jet selection"]

def make_pt_rej_plot( tag_name, do_systematics = DO_SYSTEMATICS_DEFAULT, **kwargs):

    histos = {}
    for gen in ["data","sherpa_gammajet","pythia_gammajet"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name, do_systematics)

    return PlotGammaJetBkgRej(
            histos,
            name = tag_name + "_rej_gammajet",
            lumi_val = "36.1",
            atlas_mod = "Internal",
            legend_loc = [0.62,0.93,0.89,0.75],
            x_title = "Leading Groomed C/A 1.5 Jet p_{T}" if "HTT" in tag_name else "Leading Large-#it{R} Jet #it{p_{T}}",
            tex_size_mod = 0.9,
            tex_spacing_mod = 0.75,
            y_min = 0.001,
            x_max = 3000,
            width = 600,
            **kwargs)

def make_mu_rej_plot( tag_name, do_systematics = DO_SYSTEMATICS_DEFAULT, **kwargs):

    histos = {}
    for gen in ["data","sherpa_gammajet","pythia_gammajet"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name, do_systematics, x_axis = "mu")

    return PlotGammaJetBkgRej(
            histos,
            name = tag_name + "_rej_mu_gammajet",
            lumi_val = "36.1",
            atlas_mod = "Internal",
            legend_loc = [0.62,0.93,0.89,0.75],
            x_title = "#mu",
            tex_size_mod = 0.9,
            tex_spacing_mod = 0.75,
            y_min = 0.001,
            x_max = 40,
            width = 600,
            x_units = "",
            **kwargs)

bkg_rej_plots = []
#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16Top_MassTau32Tag50eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 50%" ],
    #    x_min = 350,
    #    y_max = 150,
    #    ))

bkg_rej_plots.append(make_pt_rej_plot(
        "smooth16Top_MassTau32Tag80eff_MassJSSCut",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 80%" ],
        x_min = 350,
        # y_max = 40,
        ))

#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16Top_QwTau32Tag50eff",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 50%" ],
    #    x_min = 350,
    #    y_max = 150
    #    ))

#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16Top_QwTau32Tag80eff",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 80%" ],
    #    x_min = 350,
    #    y_max = 80,
    #    ))

#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16Top_Tau32Split23Tag50eff",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 50%" ],
    #    x_min = 350,
    #    y_max = 150,
    #    ))

# bkg_rej_plots.append(make_pt_rej_plot(
    #     "smooth16Top_Tau32Split23Tag80eff",
    #     extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): #tau_{32} + #sqrt{d_{23}}"],
    #     x_min = 350,
    #     y_max = 45,
    #     ))

bkg_rej_plots.append(make_pt_rej_plot(
        "smooth16WTag_50eff_MassJSSCut",
        extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): D_2 + m^{comb}"],
        x_min = 200,
        # y_max = 175,
        ))

#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16WTag_80eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
    #    x_min = 200,
    #    y_max = 50
    #    ))

#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16ZTag_50eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
    #    x_min = 200,
    #    y_max = 150
    #    ))

#bkg_rej_plots.append(make_pt_rej_plot(
    #    "smooth16ZTag_80eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
    #    x_min = 200,
    #    y_max = 50
    #    ))


bkg_rej_plots.append(make_pt_rej_plot(
        "BDT_Top",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): BDT" ],
        y_max = 80,
        x_min = 350,
        ))

bkg_rej_plots.append(make_pt_rej_plot(
        "BDT_W",
        extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): BDT" ],
        y_max = 200,
        ))

bkg_rej_plots.append(make_pt_rej_plot(
        "DNN_Top",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): DNN" ],
        y_max = 75,
        x_min = 350,
        ))

bkg_rej_plots.append(make_pt_rej_plot(
        "DNN_W",
        extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): DNN" ],
        y_max = 200,
        ))

bkg_rej_plots.append(make_pt_rej_plot(
        "TopoTag_Top_80_qqb",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): Topo" ],
        y_max = 40,
        ))

bkg_rej_mu_plots = []
#bkg_rej_mu_plots.append(make_mu_rej_plot(
    #    "smooth16Top_MassTau32Tag50eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 50%" ],
    #    x_min = 350,
    #    y_max = 150,
    #    ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "smooth16Top_MassTau32Tag80eff_MassJSSCut",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 80%" ],
        x_min = 350,
        y_max = 300,
        # do_systematics = False,
        ))

#bkg_rej_mu_plots.append(make_mu_rej_plot(
    #    "smooth16Top_QwTau32Tag50eff",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 50%" ],
    #    x_min = 350,
    #    y_max = 150
    #    ))

#bkg_rej_mu_plots.append(make_mu_rej_plot(
    #    "smooth16Top_QwTau32Tag80eff",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 80%" ],
    #    x_min = 350,
    #    y_max = 80,
    #    ))

#bkg_rej_mu_plots.append(make_mu_rej_plot(
    #    "smooth16Top_Tau32Split23Tag50eff",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 50%" ],
    #    x_min = 350,
    #    y_max = 150,
    #    ))

# bkg_rej_mu_plots.append(make_mu_rej_plot(
    #     "smooth16Top_Tau32Split23Tag80eff",
    #     extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): #tau_{32} + #sqrt{d_{23}}"],
    #     x_min = 350,
    #     y_max = 45,
    #     do_systematics = False,
    #     ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "smooth16WTag_50eff_MassJSSCut",
        extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): D_2 + m^{comb}"],
        x_min = 200,
        # y_max = 175,
        do_systematics = False,
        ))

    #make_pt_rej_plot(
    #    "smooth16WTag_80eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
    #    x_min = 200,
    #    y_max = 50
    #    ))

    #make_pt_rej_plot(
    #    "smooth16ZTag_50eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
    #    x_min = 200,
    #    y_max = 150
    #    ))

    #make_pt_rej_plot(
    #    "smooth16ZTag_80eff_MassJSSCut",
    #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
    #    x_min = 200,
    #    y_max = 50
    #    ))


bkg_rej_mu_plots.append(make_mu_rej_plot(
        "BDT_Top",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): BDT" ],
        y_max = 400,
        x_min = 350,
        ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "BDT_W",
        extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): BDT" ],
        y_max = 200,
        ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "DNN_Top",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): DNN" ],
        y_max = 400,
        ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "DNN_W",
        extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): DNN" ],
        y_max = 200,
        ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "TopoTag_Top_80_qqb",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): Topo" ],
        y_max = 50,
        ))


## SD Workaround Section

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/gammajet_20180520_syst/gammajet.merged.cp.root"
HISTLOADER = GammaJetLoader(CP_ROOT_FILEPATH)
# ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_GammaJet"

# OUTPUT_DIR = ROOT_OUTPUT_DIR + "/bkg_rej"
# make_dir(ROOT_OUTPUT_DIR)
# make_dir(OUTPUT_DIR)


# There are still some systematics missing, this is a hack to let it plot with the ones that still are there
SD_SYSTEMATICS = SYSTEMATICS_MC15C_WEAK_NOINPUTS_GAMMAJET
MISSING_SYSTEMATICS = [
# "EG_SCALE_ALL",
# "EG_RESOLUTION_ALL",
]

for systematic in MISSING_SYSTEMATICS:
    while systematic in SD_SYSTEMATICS: SD_SYSTEMATICS.remove(systematic)
    while "RES_"+systematic in SD_SYSTEMATICS: SD_SYSTEMATICS.remove("RES_"+systematic)

bkg_rej_plots.append(make_pt_rej_plot(
        "SDt_dcut",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): SD" ],
        y_max = 40,
        do_systematics=SD_SYSTEMATICS,
        ))

# bkg_rej_plots.append(make_pt_rej_plot(
    #     "SDw_dcut",
    #     extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): SD" ],
    #     y_max = 60,
    #     do_systematics = SD_SYSTEMATICS, # for now
    #     ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "SDt_dcut",
        extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): SD" ],
        y_max = 300,
        # do_systematics = SD_SYSTEMATICS,
        ))

# bkg_rej_mu_plots.append(make_mu_rej_plot(
    #     "SDw_dcut",
    #     extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): SD" ],
    #     y_max = 60,
    #     do_systematics = SD_SYSTEMATICS,
    #    ))

bkg_rej_plots.append(make_pt_rej_plot(
        "HTT_CAND",
        extra_legend_lines = HTT_EXTRA_LINES + [ "Top tagger: HTT" ],
        y_max = 80,
        x_min = 350,
        # do_systematics=SYSTEMATICS_MC15C_CAJET_NOINPUTS_GAMMAJET,
        do_systematics=False,
        ))

bkg_rej_mu_plots.append(make_mu_rej_plot(
        "HTT_CAND",
        extra_legend_lines = HTT_EXTRA_LINES + [ "Top tagger: HTT" ],
        y_max = 10,
        x_min = 350,
        # do_systematics=SYSTEMATICS_MC15C_CAJET_NOINPUTS_GAMMAJET,
        do_systematics=False,
        ))