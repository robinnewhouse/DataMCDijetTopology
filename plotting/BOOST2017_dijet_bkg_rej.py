from ROOT import *
import atlas_style
import array

from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *
from plot_systematics import *
from plot_systematics_breakdown import *

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)
gStyle.SetOptStat(0)

CP_ROOT_FILEPATH = "/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/dijet/26-05-2017__12:22:15__15052017_dijet_newSD_newHTT_gridMVA/cp.merged.root"
LOADER = DijetLoader(CP_ROOT_FILEPATH)
LOADER_SMOOTH = DijetLoader("/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/dijet/17-02-2017__15:37:30__02072017_gridjobs_withQwSplit23Sys/cp.merged.root")
ROOT_OUTPUT_DIR = os.path.dirname(CP_ROOT_FILEPATH) + "/plots"

OUTPUT_DIR = ROOT_OUTPUT_DIR + "/BOOST2017_REJ"
make_dir(ROOT_OUTPUT_DIR)
make_dir(OUTPUT_DIR)

def rej_rebin(h):
  BIN_BOUNDS = array.array('d', [
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
    2000,
    2300,
    2600
    ])
  return h.Rebin(len(BIN_BOUNDS)-1, h.GetName()+"_rebinned", BIN_BOUNDS)


def get_sys_dict_eff(gen_name, var_name):
    tmp_loader = LOADER_SMOOTH if "smooth" in var_name else LOADER
    # get the Rtrk systematics and rebin them
    dict = tmp_loader.get_systematics_dictionary(gen_name, var_name, SYSTEMATICS_MC15C_MEDIUM, norm_to_pretagged = True)
    for sys_name, var_dict in dict.iteritems():
        var_dict["up"] = rej_rebin(var_dict["up"])
        var_dict["down"] = rej_rebin(var_dict["down"])
    # create the sig. norm. systematics
    dict["sig_norm_sf"] = {}
    h_tmp_up = rej_rebin(tmp_loader.get_normalized_dijet(gen_name, var_name, sig_sf = 1.25, normalize_to_pretagged = True))
    h_tmp_down = rej_rebin(tmp_loader.get_normalized_dijet(gen_name, var_name, sig_sf = 0.75, normalize_to_pretagged = True))
    h_tmp_up.SetName(h_tmp_up.GetName() + "_sig_norm_sf_up")
    h_tmp_down.SetName(h_tmp_down.GetName() + "_sig_norm_sf_own")
    dict["sig_norm_sf"]["up"] = h_tmp_up
    dict["sig_norm_sf"]["down"] = h_tmp_down
    return dict

def make_rej_TH1SysEff(gen_name, tag_name):
    tmp_loader = LOADER_SMOOTH if "smooth" in tag_name else LOADER
    is_data = "data" in gen_name
    if ("HTT" in tag_name):
      total_var_name = "h_htt_caGroomJet0_pt"
    else:
      total_var_name = "h_rljet0_pt_comb"

    passed_var_name = total_var_name + "_" + tag_name

    h_total = rej_rebin(
            tmp_loader.get_hist(["data","nominal"], total_var_name)
            if is_data
            else tmp_loader.get_normalized_dijet(gen_name, total_var_name, normalize_to_pretagged = True)
            )

    h_passed = rej_rebin(
            tmp_loader.get_hist(["data","nominal"], passed_var_name)
            if is_data
            else tmp_loader.get_normalized_dijet(gen_name, passed_var_name, normalize_to_pretagged = True)
            )

    if (is_data):
        h_total.Divide(h_passed)
        return h_total.Clone()
    else:
        total_sys_dict = get_sys_dict_eff(gen_name, total_var_name)
        passed_sys_dict = get_sys_dict_eff(gen_name, passed_var_name)
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
            "1/#epsilon_{QCD}", show_binwidth = False)

        set_data_style_simple(self.h_data)
        set_mc_style_marker(self.h_pythia, kRed, shape = 21)
        set_mc_style_marker(self.h_herwig, kBlue, shape = 22)
        for h in [self.h_data, self.h_pythia, self.h_herwig, self.h_pythia_sys, self.h_pythia_stat]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
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

        set_mc_sys_err_style(self.h_pythia_sys_ratio, col = kGreen-8)
        set_mc_sys_err_style(self.h_pythia_stat_ratio, col = kGreen-2)
        set_mc_sys_err_style(self.h_pythia_sys, col = kGreen-8)
        set_mc_sys_err_style(self.h_pythia_stat, col = kGreen-2)

        ratio_title = "#frac{Data}{MC}"
        for h_ratio in [ self.h_pythia_ratio, self.h_herwig_ratio, self.h_pythia_sys_ratio, self.h_pythia_stat_ratio ]:
            set_style_ratio(h_ratio, y_title = ratio_title, y_min = 0.5, y_max = 1.5)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)
            h_ratio.GetXaxis().SetLabelSize(19)

        for ibin in range(self.h_pythia_sys_ratio.GetSize()):
            self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)
            self.h_pythia_stat_ratio.SetBinContent(ibin, 1.0)

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

        self.h_pythia_sys.Draw("E2")
        self.h_pythia_stat.Draw("E2,same")
        self.h_herwig.Draw("PE1,same")
        self.h_pythia.Draw("PE1,same")
        self.h_data.Draw("PE1,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia, "Pythia8 dijet")
        self.leg.AddEntry(self.h_herwig, "Herwig++ dijet")
        self.leg.AddEntry(self.h_pythia_stat, "Stat. uncert.")
        self.leg.AddEntry(self.h_pythia_sys, "Stat. #oplus syst. uncert.")
        self.leg.Draw()

        self.pad2.cd()
        self.h_pythia_sys_ratio.Draw("E2")
        self.h_pythia_stat_ratio.Draw("E2,same")
        self.h_herwig_ratio.Draw("PE1,same")
        self.h_pythia_ratio.Draw("PE1,same")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.canvas.Clear()

DEF_EXTRA_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" ]

def make_pt_efficiency_plot( tag_name, ref_tag_name = None, **kwargs):

    histos = {}
    for gen in ["data","pythia","herwig"]:
        histos[gen] = make_rej_TH1SysEff(gen, tag_name)

    if (ref_tag_name != None):
        histos["data_ref"] = make_rej_TH1SysEff("data_ref", ref_tag_name)

    p = PlotPythiaSystematicsBreakdown(histos["pythia"], name = tag_name + "_SYS", y_min = 0, y_max = 30, x_title = "Leading Large-R Jet #it{p_{T}}", hide_lumi = True)
    p.print_to_file(OUTPUT_DIR + "/" + p.name + ".pdf")

    return PlotDataPythiaHerwigEfficiency(
            histos,
            name = tag_name + "_rej",
            lumi_val = "36.5",
            atlas_mod = "Internal",
            legend_loc = [0.60,0.93,0.91,0.65],
            x_title = "Leading Groomed C/A 1.5 Jet p_{T}" if "HTT" in tag_name else "Leading Large-R Jet #it{p_{T}}",
            x_min = 450,
            x_max = 3500,
            y_min = 0.001,
            width = 600,
            **kwargs)

bkg_rej_plots = [
        make_pt_efficiency_plot(
            "smooth16Top_MassTau32Tag50eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 50%" ],
            y_max = 110,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_MassTau32Tag80eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + m_{comb}", "#epsilon_{sig} = 80%" ],
            y_max = 40,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_QwTau32Tag50eff",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 50%" ],
            y_max = 130
            ),

        make_pt_efficiency_plot(
            "smooth16Top_QwTau32Tag80eff",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + Q_{w}", "#epsilon_{sig} = 80%" ],
            y_max = 40,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_Tau32Split23Tag50eff",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 50%" ],
            y_max = 115,
            ),

        make_pt_efficiency_plot(
            "smooth16Top_Tau32Split23Tag80eff",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: #tau_{32} + #sqrt{d_{23}}", "#epsilon_{sig} = 80%" ],
            y_max = 40,
            ),

        make_pt_efficiency_plot(
            "smooth16WTag_50eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
            y_max = 175
            ),

        make_pt_efficiency_plot(
            "smooth16WTag_80eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
            y_max = 35
            ),

        make_pt_efficiency_plot(
            "smooth16ZTag_50eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 50%" ],
            y_max = 175
            ),

        make_pt_efficiency_plot(
            "smooth16ZTag_80eff_MassJSSCut",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "Smooth Tag: D_{2} + m_{comb}", "#epsilon_{sig} = 80%" ],
            y_max = 35
            ),

        make_pt_efficiency_plot(
            "HTT_CAND",
            None,
            extra_legend_lines = DEF_EXTRA_LINES + [ "HTT-Tagged" ],
            y_max = 30
            )

        ]
