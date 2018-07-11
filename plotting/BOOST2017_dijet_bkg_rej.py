import sys
sys.path.append('lib/')

import atlas_style

import array

from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *
# from plot_systematics_breakdown import *

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180504_syst/dijet.merged.cp.root"
HISTLOADER = DijetLoader(CP_ROOT_FILEPATH)
ROOT_OUTPUT_DIR = "../DataMC_Dijet"
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_Dijet"

DO_SYSTEMATICS_DEFAULT = SYSTEMATICS_MC15C_WEAK_NOINPUTS

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/bkg_rej"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

PT_BIN_BOUNDS = array.array('d', [
        450,
        500,
        550,
        600,
        650,
        700,
        750,
        800,
        850,
        900,
        950,
        1000,
        1100,
        1200,
        1300,
        1400,
        1500,
        1700,
        2500,
    ])

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

LABEL_SIZE = 23
TITLE_SIZE = LABEL_SIZE

def rej_rebin(h, bin_bounds):
  return h.Rebin(len(bin_bounds)-1, h.GetName()+"_rebinned", bin_bounds)

def get_sys_dict_eff(gen_name, var_name, bin_bounds, do_systematics):
    # get the Rtrk systematics and rebin them
    dict = HISTLOADER.get_systematics_dictionary(gen_name, var_name, do_systematics, norm_to_pretagged = True)
    for sys_name, var_dict in dict.iteritems():
        var_dict["up"] = rej_rebin(var_dict["up"], bin_bounds)
        var_dict["down"] = rej_rebin(var_dict["down"], bin_bounds)
    # create the sig. norm. systematics
    dict["sig_norm_sf"] = {}
    h_tmp_up = rej_rebin(HISTLOADER.get_normalized_dijet(gen_name, var_name, sig_sf = 1.25, normalize_to_pretagged = True), bin_bounds)
    h_tmp_down = rej_rebin(HISTLOADER.get_normalized_dijet(gen_name, var_name, sig_sf = 0.75, normalize_to_pretagged = True), bin_bounds)
    h_tmp_up.SetName(h_tmp_up.GetName() + "_sig_norm_sf_up")
    dict["sig_norm_sf"]["up"] = h_tmp_up
    h_tmp_down.SetName(h_tmp_down.GetName() + "_sig_norm_sf_down")
    dict["sig_norm_sf"]["down"] = h_tmp_down
    return dict

def make_rej_TH1SysEff(gen_name, tag_name, do_systematics, x_axis = "pt"):
    is_data = "data" in gen_name

    if x_axis == "mu":
        bin_bounds = MU_BIN_BOUNDS
        passed_var_name = "h_mu_" + tag_name 
        total_var_name = passed_var_name + "_pretag"
    else:
        bin_bounds = PT_BIN_BOUNDS
        if ("HTT" in tag_name):
          total_var_name = "h_htt_caGroomJet0_pt"
        else:
          total_var_name = "h_rljet0_pt_comb"
        passed_var_name = total_var_name + "_" + tag_name

    h_total = rej_rebin(
            HISTLOADER.get_sigsub_data(total_var_name)
            if is_data
            else HISTLOADER.get_normalized_dijet(gen_name, total_var_name, normalize_to_pretagged = True),
            bin_bounds)

    h_passed = rej_rebin(
            HISTLOADER.get_sigsub_data(passed_var_name)
            if is_data
            else HISTLOADER.get_normalized_dijet(gen_name, passed_var_name, normalize_to_pretagged = True),
            bin_bounds)

    if (is_data):
        h_divided = get_assym_error_histogram(h_passed.Clone(), h_total.Clone())
        return h_divided
        # Old Method
        h_total.Divide(h_passed)
        return h_total.Clone()
    else:
        total_sys_dict = {}
        passed_sys_dict = {}
        if do_systematics:
            total_sys_dict = get_sys_dict_eff(gen_name, total_var_name, bin_bounds, do_systematics)
            passed_sys_dict = get_sys_dict_eff(gen_name, passed_var_name, bin_bounds, do_systematics)
        return TH1SysEff(h_total, total_sys_dict, h_passed, passed_sys_dict)

class PlotDataPythiaHerwigEfficiency(PlotBase):
    def __init__(self, histos, **kwargs):
        super(PlotDataPythiaHerwigEfficiency, self).__init__(**kwargs)

        self.h_data        = histos["data"]
        self.hsys_pythia   = histos["pythia"]
        self.h_pythia      = histos["pythia"].h_nominal
        self.h_herwig      = histos["herwig"].h_nominal
        self.h_pythia_stat = self.h_pythia.Clone("h_pythia_stat")

        self.hsys_pythia._compute_errors()
        self.h_pythia_sys = self.hsys_pythia.get_histo_with_systematic_errs()

        self.determine_y_axis_title(self.h_data,
            "Background rejection (1/#it{#epsilon}_{bkg})", show_binwidth = False)

        set_data_style_simple(self.h_data)
        set_mc_style_marker(self.h_pythia, kRed, shape = 21)
        set_mc_style_marker(self.h_herwig, kBlue, shape = 22)

        for h in [self.h_data, self.h_pythia, self.h_herwig, self.h_pythia_sys, self.h_pythia_stat]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetYaxis().SetLabelSize(LABEL_SIZE)
            h.GetYaxis().SetTitleSize(TITLE_SIZE)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)

        if self.log_scale: self.canvas.SetLogy()

        self.h_pythia_ratio      = self.h_pythia.Clone("h_pythia_ratio")
        self.h_herwig_ratio      = self.h_herwig.Clone("h_herwig_ratio")
        self.h_pythia_sys_ratio  = self.h_pythia_sys.Clone("h_pythia_sys_ratio")
        self.h_pythia_stat_ratio = self.h_pythia.Clone("h_pythia_stat_ratio")

        self.h_pythia_ratio.Divide(self.h_data, self.h_pythia, 1, 1, "")
        self.h_herwig_ratio.Divide(self.h_data, self.h_herwig, 1, 1, "")
        self.h_pythia_sys_ratio.Divide(self.h_data, self.h_pythia_sys, 1, 1, "")
        self.h_pythia_stat_ratio.Divide(self.h_data, self.h_pythia, 1, 1, "")

        self.h_pythia_sys_ratio.SetMarkerSize(0)
        self.h_pythia_stat_ratio.SetMarkerSize(0)


        # ratio_title = "#frac{Data}{MC}"
        ratio_title = "Data/Pred."
        for h_ratio in [ self.h_pythia_ratio, self.h_herwig_ratio, self.h_pythia_sys_ratio, self.h_pythia_stat_ratio ]:
            set_style_ratio(h_ratio, y_title = ratio_title, y_min = -0.1, y_max = 2.1, Ndiv = 505)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)
            h_ratio.GetXaxis().SetLabelSize(LABEL_SIZE)
            h_ratio.GetYaxis().SetLabelSize(LABEL_SIZE)
            h_ratio.GetXaxis().SetTitleSize(TITLE_SIZE)
            h_ratio.GetYaxis().SetTitleSize(TITLE_SIZE)

        for ibin in range(self.h_pythia_sys_ratio.GetSize()):
            self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)
            self.h_pythia_stat_ratio.SetBinContent(ibin, 1.0)

        set_mc_sys_err_style(self.h_pythia_sys_ratio, col = kGreen-8)
        set_mc_sys_err_style(self.h_pythia_stat_ratio, col = kGreen-5)
        set_mc_sys_err_style(self.h_pythia_sys, col = kGreen-8)
        set_mc_sys_err_style(self.h_pythia_stat, col = kGreen-5)

        self.h_pythia_stat_ratio.SetFillColor(kGreen-5)
        self.h_pythia_stat_ratio.SetFillStyle(1001)

        # SET UP THE CANVAS
        self.canvas.Divide(1,2)
        self.canvas.Modified()

        self.pad1 = self.canvas.cd(1)
        self.pad1.SetPad(0.0, 0.33, 1., 1.)
        self.pad1.SetTopMargin(0.07)
        self.pad1.SetRightMargin(0.1)
        self.pad1.SetBottomMargin(0.00)
        self.pad1.SetFillColorAlpha(0, 0.)
        self.pad1.SetBorderSize(0)
        if self.log_scale: self.pad1.SetLogy()

        self.pad2 = self.canvas.cd(2)
        self.pad2.SetPad(0.0, 0.0, 1., 0.33)
        self.pad2.SetTopMargin(0.05)
        self.pad2.SetRightMargin(0.1)
        self.pad2.SetBottomMargin(0.4)
        self.pad2.SetFillColorAlpha(0, 0.)
        self.pad2.SetGridy(1) # grid for ratio plot
        self.pad2.SetBorderSize(0)

        self.pad1.cd()

        #self.h_pythia_sys.Draw("E2")
        #self.h_pythia_stat.Draw("E2,same")
        self.h_herwig.Draw("PE1,same")
        self.h_pythia.Draw("PE1,same")
        self.h_data.Draw("PE1,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2015+2016","ple")
        self.leg.AddEntry(self.h_pythia, "Pythia8","ple")
        self.leg.AddEntry(self.h_herwig, "Herwig++","ple")
        self.leg.AddEntry(self.h_pythia_stat_ratio, "Stat. uncert.", "f")
        if (self.hsys_pythia.num_systematics != 0):
          self.leg.AddEntry(self.h_pythia_sys, "Total uncert.", "f")
        self.leg.Draw()

        self.pad2.cd()
        if (self.hsys_pythia.num_systematics != 0):
            self.h_pythia_sys_ratio.Draw("E2,same")
        self.h_pythia_stat_ratio.Draw("E2,same")
        self.h_herwig_ratio.Draw("PE1,same")
        self.h_pythia_ratio.Draw("PE1,same")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.canvas.Clear()

DEF_EXTRA_LINES = [ "Trimmed anti-#it{k}_{t} #it{R}=1.0 jets", "Multijet Selection" ]
HTT_EXTRA_LINES = ["Trimmed C/A #it{R}=1.5 jets", "Dijet Selection"]

def make_pt_efficiency_plot( tag_name, ref_tag_name = None, do_systematics = DO_SYSTEMATICS_DEFAULT, **kwargs):

    histos = {}
    for gen in ["data","pythia","herwig"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name, do_systematics)

    if (ref_tag_name != None):
        histos["data_ref"] = make_rej_TH1SysEff("data_ref", ref_tag_name, do_systematics)

    return PlotDataPythiaHerwigEfficiency(
            histos,
            name = tag_name + "_rej_dijet",
            tex_size_mod = 0.9,
            tex_spacing_mod = 0.8,
            lumi_val = "36.7",
            atlas_mod = "Internal",
            legend_loc = [0.62,0.93,0.89,0.75],
            x_title = "Leading large-#it{R} Jet #it{p}_{T}",
            x_min = 450,
            x_max = PT_BIN_BOUNDS[-1],
            y_min = 0.001,
            width = 600,
            **kwargs)

def make_mu_efficiency_plot( tag_name, ref_tag_name = None, do_systematics = DO_SYSTEMATICS_DEFAULT, **kwargs):

    histos = {}
    for gen in ["data","pythia","herwig"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name, do_systematics, x_axis = "mu")

    if (ref_tag_name != None):
        histos["data_ref"] = make_rej_TH1SysEff("data_ref", ref_tag_name, do_systematics, x_axis = "mu")

    return PlotDataPythiaHerwigEfficiency(
            histos,
            name = tag_name + "_rej_mu_dijet",
            tex_size_mod = 0.9,
            tex_spacing_mod = 0.8,
            lumi_val = "36.7",
            atlas_mod = "Internal",
            legend_loc = [0.60,0.93,0.91,0.75],
            x_title = "#mu",
            x_min = 0,
            x_max = MU_BIN_BOUNDS[-1],
            y_min = 0.001,
            width = 600,
            x_units = "",
            **kwargs)

pt_bkg_rej_plots = []
#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16Top_MassTau32Tag50eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 100,
        #    ))

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
           "smooth16Top_MassTau32Tag80eff_MassJSSCut",
           extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%):","#it{m}^{comb} + #tau_{32}" ],
           y_max = 30,
           ))

#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16Top_QwTau32Tag50eff",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 100
        #    ))

#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16Top_QwTau32Tag80eff",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#it{#epsilon}_{sig} = 80%" ],
        #    y_max = 40,
        #    ))

#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16Top_Tau32Split23Tag50eff",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 100,
        #    ))

# pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #     "smooth16Top_Tau32Split23Tag80eff",
        #     extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): #tau_{32} + #sqrt{d_{23}}"],
        #     y_max = 45,
        #     ))

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "smooth16WTag_50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%):", "#it{m}^{comb} + #it{D}_{2}"],
            y_max = 175
            ))

#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16WTag_80eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #it{D}_{2} + m_{comb}", "#it{#epsilon}_{sig} = 80%" ],
        #    y_max = 35
        #    ))

#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16ZTag_50eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #it{D}_{2} + m_{comb}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 175
        #    ))

#pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #    "smooth16ZTag_80eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #it{D}_{2} + m_{comb}", "#it{#epsilon}_{sig} = 80%" ],
        #    y_max = 35
        #    ))


pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "BDT_Top",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): BDT" ],
            y_max = 80,
            ))

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "BDT_W",
            extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%): BDT" ],
            y_max = 200,
            ))

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "DNN_Top",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): DNN" ],
            y_max = 75,
            ))

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "DNN_W",
            extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%): DNN" ],
            y_max = 200,
            ))

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "TopoTag_Top_80_qqb",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): TopoDNN" ],
            y_max = 40,
            ))


mu_bkg_rej_plots = []
#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16Top_MassTau32Tag50eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 100,
        #    ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
           "smooth16Top_MassTau32Tag80eff_MassJSSCut",
           extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%):", "#it{m}^{comb} + #tau_{32}" ],
           y_max = 30,
           ))

#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16Top_QwTau32Tag50eff",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 100
        #    ))

#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16Top_QwTau32Tag80eff",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#it{#epsilon}_{sig} = 80%" ],
        #    y_max = 40,
        #    ))

#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16Top_Tau32Split23Tag50eff",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 100,
        #    ))

# mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #     "smooth16Top_Tau32Split23Tag80eff",
        #     extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): #tau_{32} + #sqrt{d_{23}}"],
        #     y_max = 45,
        #     ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "smooth16WTag_50eff_MassJSSCut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%):", "#it{m}^{comb} + #it{D}_{2}"],
            y_max = 175
            ))

#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16WTag_80eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #it{D}_{2} + m_{comb}", "#it{#epsilon}_{sig} = 80%" ],
        #    y_max = 35
        #    ))

#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16ZTag_50eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #it{D}_{2} + m_{comb}", "#it{#epsilon}_{sig} = 50%" ],
        #    y_max = 175
        #    ))

#mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #    "smooth16ZTag_80eff_MassJSSCut",
        #    extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #it{D}_{2} + m_{comb}", "#it{#epsilon}_{sig} = 80%" ],
        #    y_max = 35
        #    ))



mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "BDT_Top",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): BDT" ],
            y_max = 80,
            ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "BDT_W",
            extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%): BDT" ],
            y_max = 200,
            ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "DNN_Top",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): DNN" ],
            y_max = 75,
            ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "DNN_W",
            extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%): DNN" ],
            y_max = 200,
            ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "TopoTag_Top_80_qqb",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): TopoDNN" ],
            y_max = 50,
            # do_systematics = False,
            ))


pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "HTT_CAND",
            extra_legend_lines = HTT_EXTRA_LINES + [ "Top tagger: HTT" ],
            y_max = 80,
            do_systematics=SYSTEMATICS_MC15C_CAJET_NOINPUTS,
            ))

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "HTT_CAND",
            extra_legend_lines = HTT_EXTRA_LINES + [ "Top tagger: HTT" ],
            y_max = 80,
            # y_max = 10,
            do_systematics=SYSTEMATICS_MC15C_CAJET_NOINPUTS,
            ))

CP_ROOT_FILEPATH = "/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/dijet_20180515_syst/dijet.merged.cp.root"
HISTLOADER = DijetLoader(CP_ROOT_FILEPATH)
# ROOT_OUTPUT_DIR = "../DataMC_Dijet"
# ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/DataMC_Dijet"

DO_SYSTEMATICS_DEFAULT = SYSTEMATICS_MC15C_WEAK_NOINPUTS

# OUTPUT_DIR = ROOT_OUTPUT_DIR + "/bkg_rej"
# make_dir(ROOT_OUTPUT_DIR)
# make_dir(OUTPUT_DIR)

pt_bkg_rej_plots.append(make_pt_efficiency_plot(
            "SDt_dcut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): SD" ],
            y_max = 40,
            do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
            ))

# pt_bkg_rej_plots.append(make_pt_efficiency_plot(
        #     "SDw_dcut",
        #     extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#it{#epsilon}_{sig} = 50%): SD" ],
        #     y_max = 60,
        #     ))
        

mu_bkg_rej_plots.append(make_mu_efficiency_plot(
            "SDt_dcut",
            extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#it{#epsilon}_{sig} = 80%): SD" ],
            y_max = 40,
            do_systematics = SYSTEMATICS_MC15C_WEAK_NOINPUTS,
            ))

# mu_bkg_rej_plots.append(make_mu_efficiency_plot(
        #     "SDw_dcut",
        #     extra_legend_lines = DEF_EXTRA_LINES + [ "ShD #font[52]{W}, #it{#epsilon}_{sig} = 50%" ],
        #     y_max = 60,
        #     ))
