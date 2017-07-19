from ROOT import *
import atlas_style
import array
import sys

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

DIJET_LOADER = DijetLoader("/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20170610/cp.merged.root")
GAMMAJET_LOADER = GammaJetLoader("/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_gammajet/20170610/cp.merged.root")
OUTPUT_DIR = "/eos/user/z/zmeadows/TopWTag/PLOTS/BOOST2017_REJ_OVERLAY"
make_dir(OUTPUT_DIR)
print "OUTPUT DIRECTORY: ", OUTPUT_DIR

def rebin_rej(h, topo, pt_cut = True):
  if (topo == "dijet"):
    BIN_BOUNDS = array.array('d', [
      450, 550, 650, 750, 850, 950,
      1000, 1150, 1300, 1500, 1700, 2000, 2500
      ])
    return h.Rebin(len(BIN_BOUNDS)-1, h.GetName()+"_rebinned", BIN_BOUNDS)
  elif ("gamma" in topo):
    BIN_BOUNDS = array.array('d',
        [ 200, 300, 350, 500, 750, 1200, 2500 ]
        )

    return h.Rebin(len(BIN_BOUNDS)-1, h.GetName()+"_rebinned", BIN_BOUNDS)

def get_sys_dict(gen_name, var_name, topo):
    # get the Rtrk systematics and rebin them
  if (topo == "gamma"):
    d = GAMMAJET_LOADER.get_systematics_dictionary(
        var_name, SYSTEMATICS_MC15C_MEDIUM, gen_name, True)
  elif (topo == "dijet"):
    d = DIJET_LOADER.get_systematics_dictionary(
        gen_name, var_name, SYSTEMATICS_MC15C_MEDIUM, norm_to_pretagged = True)
  else:
      print "ERROR: topo = ", topo
      sys.exit(1)

  for sys_name, var_dict in d.iteritems():
      var_dict["up"] = rebin_rej(var_dict["up"], topo)
      var_dict["down"] = rebin_rej(var_dict["down"], topo)
  return d

def make_rej_TH1SysEff(gen_name, tag_name, topo):
    is_data = "data" in gen_name

    if ("HTT" in tag_name):
      total_var_name = "h_htt_caGroomJet0_pt"
    else:
      total_var_name = "h_rljet0_pt_comb"

    passed_var_name = total_var_name + "_" + tag_name

    if (topo == "dijet"):
      h_total = rebin_rej(
              DIJET_LOADER.get_sigsub_data(total_var_name)
              if is_data
              else DIJET_LOADER.get_normalized_dijet(gen_name, total_var_name, normalize_to_pretagged = True)
              , topo
              )

      h_passed = rebin_rej(
              DIJET_LOADER.get_sigsub_data(passed_var_name)
              if is_data
              else DIJET_LOADER.get_normalized_dijet(gen_name, passed_var_name, normalize_to_pretagged = True)
              , topo
              )
    else:
      h_total = rebin_rej(
              GAMMAJET_LOADER.get_sigsub_data(total_var_name)
              if is_data
              else GAMMAJET_LOADER.get_normalized_gamma(total_var_name, generator = gen_name, normalize_to_pretagged = True)
              , topo
              )

      h_passed = rebin_rej(
              GAMMAJET_LOADER.get_sigsub_data(passed_var_name)
              if is_data
              else GAMMAJET_LOADER.get_normalized_gamma(passed_var_name, generator = gen_name, normalize_to_pretagged = True)
              , topo
              )

    if (is_data):
        h_total.Divide(h_passed)
        return h_total.Clone()
    else:
        if ("BDT" in tag_name or "DNN" in tag_name):
          total_sys_dict = {}
          passed_sys_dict = {}
        else:
          total_sys_dict = get_sys_dict(gen_name, total_var_name, topo)
          passed_sys_dict = get_sys_dict(gen_name, passed_var_name, topo)
        return TH1SysEff(h_total, total_sys_dict, h_passed, passed_sys_dict)

class PlotCombinedBkgRej(PlotBase):
    def __init__(self, tag_name, **kwargs):
        super(PlotCombinedBkgRej, self).__init__(
            name = tag_name + "_datamc_dijet_gamma_rej",
            tex_size_mod = 0.8,
            tex_spacing_mod = 0.7,
            lumi_val = "36.1",
            atlas_mod = "Internal",
            legend_loc = [0.60,0.93,0.91,0.75],
            x_title = "Leading Groomed C/A 1.5 Jet p_{T}" if "HTT" in tag_name else "Leading large-R Jet #it{p_{T}}",
            x_min = 200,
            x_max = 2000 if "BDT_W" in tag_name else 2500,
            width = 600,
            **kwargs)

        hsys = {}
        hsys["data_dijet"] = make_rej_TH1SysEff("data", tag_name, "dijet")
        hsys["pythia_dijet"] = make_rej_TH1SysEff("pythia_dijet", tag_name, "dijet")
        hsys["herwig_dijet"] = make_rej_TH1SysEff("herwig_dijet", tag_name, "dijet")

        hsys["data_gamma"] = make_rej_TH1SysEff("data", tag_name, "gamma")
        hsys["pythia_gamma"] = make_rej_TH1SysEff("pythia_gammajet", tag_name, "gamma")
        hsys["sherpa_gamma"] = make_rej_TH1SysEff("sherpa_gammajet", tag_name, "gamma")

        histos = {}
        for n, h in hsys.iteritems():
          if "data" not in n:
            h._compute_errors()
            histos[n] = h.get_histo_with_systematic_errs()
            histos[n + "_nominal"] = hsys[n].h_nominal.Clone()
          else:
            histos[n] = hsys[n]

        self.determine_y_axis_title(histos["data_dijet"], "Background rejection (1/#epsilon_{bkg})", show_binwidth = False)

        set_data_style_simple(histos["data_dijet"], marker_style = 24, marker_size = 1.2)
        set_data_style_simple(histos["data_gamma"], marker_style = 26, marker_size = 1.2)



        histos["data_dijet"].SetFillStyle(3004)
        histos["data_gamma"].SetFillStyle(3005)
        histos["data_dijet"].SetFillColorAlpha(kBlack, 0.7)
        histos["data_gamma"].SetFillColorAlpha(kBlack, 0.7)

        set_mc_style_marker(histos["pythia_dijet_nominal"], kRed, line_width = 2)
        set_mc_style_marker(histos["herwig_dijet_nominal"], kRed, line_width = 2)
        set_mc_style_marker(histos["pythia_gamma_nominal"], kBlue, line_width = 2)
        set_mc_style_marker(histos["sherpa_gamma_nominal"], kBlue, line_width = 2)

        # histos["pythia_dijet_nominal"].SetFillColorAlpha(kRed, 0.3)
        # histos["herwig_dijet_nominal"].SetFillColorAlpha(kRed, 0.3)
        # histos["pythia_gamma_nominal"].SetFillColorAlpha(kBlue, 0.3)
        # histos["sherpa_gamma_nominal"].SetFillColorAlpha(kBlue, 0.3)

        # histos["pythia_dijet_nominal"].SetFillStyle(1001)
        # histos["herwig_dijet_nominal"].SetFillStyle(1001)
        # histos["pythia_gamma_nominal"].SetFillStyle(1001)
        # histos["sherpa_gamma_nominal"].SetFillStyle(1001)

        histos["pythia_dijet_nominal"].SetMarkerSize(1.2)
        histos["pythia_dijet_nominal"].SetMarkerStyle(20)
        histos["herwig_dijet_nominal"].SetMarkerSize(1.2)
        histos["herwig_dijet_nominal"].SetMarkerStyle(20)
        histos["sherpa_gamma_nominal"].SetMarkerSize(1.2)
        histos["sherpa_gamma_nominal"].SetMarkerStyle(22)
        histos["pythia_gamma_nominal"].SetMarkerSize(1.2)
        histos["pythia_gamma_nominal"].SetMarkerStyle(22)

        for h in histos.itervalues():
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)

        ratios = {}
        for n,h in histos.iteritems():
          if "data" not in n:
            ratios[n] = histos[n].Clone(histos[n].GetName() + "_data_ratio")

        ratios["pythia_dijet_sys"] = ratios["pythia_dijet"].Clone(ratios["pythia_dijet"].GetName() + "_sys")
        ratios["sherpa_gamma_sys"] = ratios["sherpa_gamma"].Clone(ratios["sherpa_gamma"].GetName() + "_sys")


        ratios["pythia_dijet_sys"].Divide(histos["data_dijet"], ratios["pythia_dijet"], 1, 1, "n")
        ratios["sherpa_gamma_sys"].Divide(histos["data_gamma"], ratios["sherpa_gamma"], 1, 1, "n")
        ratios["pythia_dijet"].Divide(histos["data_dijet"], ratios["pythia_dijet_nominal"], 1, 1, "n")
        ratios["herwig_dijet"].Divide(histos["data_dijet"], ratios["herwig_dijet_nominal"], 1, 1, "n")
        ratios["pythia_gamma"].Divide(histos["data_gamma"], ratios["pythia_gamma_nominal"], 1, 1, "n")
        ratios["sherpa_gamma"].Divide(histos["data_gamma"], ratios["sherpa_gamma_nominal"], 1, 1, "n")

        ratios["pythia_dijet_nominal"].Divide(histos["data_dijet"], ratios["pythia_dijet_nominal"], 1, 1, "n")
        ratios["herwig_dijet_nominal"].Divide(histos["data_dijet"], ratios["herwig_dijet_nominal"], 1, 1, "n")
        ratios["pythia_gamma_nominal"].Divide(histos["data_gamma"], ratios["pythia_gamma_nominal"], 1, 1, "n")
        ratios["sherpa_gamma_nominal"].Divide(histos["data_gamma"], ratios["sherpa_gamma_nominal"], 1, 1, "n")

        ratio_title = "#frac{Data}{MC}"
        for n,r in ratios.iteritems():
            set_style_ratio(r, y_title = ratio_title, y_min = 0.5, y_max = 1.5)
            self.set_x_axis_bounds(r)
            r.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            r.GetXaxis().SetTitleOffset(4.0)
            r.GetYaxis().SetTitleOffset(2.0)
            r.GetXaxis().SetLabelSize(19)
            if ("_sys" in n):
              for ibin in range(r.GetSize()):
                r.SetBinContent(ibin, 1.0)

        set_mc_sys_err_style(ratios["pythia_dijet_sys"], col = kRed, alpha = 0.5)
        set_mc_sys_err_style(ratios["sherpa_gamma_sys"], col = kBlue, alpha = 0.5)

        #ratios["pythia_dijet"].SetMarkerSize(1.2)
        #ratios["sherpa_gamma"].SetMarkerSize(1.2)
        #ratios["pythia_dijet"].SetMarkerStyle(24)
        #ratios["sherpa_gamma"].SetMarkerStyle(26)

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

        #histos["herwig_dijet_nominal"].Draw("PE")
        #histos["pythia_gamma_nominal"].Draw("PE,same")
        histos["sherpa_gamma_nominal"].Draw("PE1,same")
        histos["pythia_dijet_nominal"].Draw("PE1,same")
        #histos["data_dijet"].Draw("E3,same")
        histos["data_dijet"].Draw("PE3,same")
        #histos["data_gamma"].Draw("E3,same")
        histos["data_gamma"].Draw("PE3,same")

        self.pad1.RedrawAxis()
        self.canvas.Update()
        self.canvas.Modified()

        self.canvas.cd()

        self.leg.AddEntry(histos["pythia_dijet_nominal"], "Pythia8 dijet")
        self.leg.AddEntry(histos["sherpa_gamma_nominal"], "Sherpa #gamma + jet")
        self.leg.AddEntry(histos["data_dijet"], "Data - Sig. (dijet)")
        self.leg.AddEntry(histos["data_gamma"], "Data - Sig. (#gamma  + jet)")

        self.pad2.cd()

        ratios["sherpa_gamma_nominal"].Draw("PE")
        ratios["pythia_dijet_nominal"].Draw("PE,same")

        self.pad2.RedrawAxis()
        self.pad1.RedrawAxis()
        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".eps")
        self.canvas.Clear()

DEF_EXTRA_LINES = ["Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" ]
HTT_EXTRA_LINES = ["Trimmed C/A #it{R}=1.5", "#gamma + jet selection"]

PlotCombinedBkgRej(
    "smooth16Top_Tau32Split23Tag80eff",
    extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): #tau_{32} + #sqrt{d_{23}}"],
    y_max = 45,
    y_min = 0.001,
    )

PlotCombinedBkgRej(
    "smooth16WTag_50eff_MassJSSCut",
    extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): D_2 + m^{comb}"],
    y_max = 175,
    y_min = 0.001,
    )

PlotCombinedBkgRej(
    "HTT_CAND",
    extra_legend_lines = HTT_EXTRA_LINES + [ "Top tagger: HTT" ],
    y_max = 2000,
    y_min = 4,
    log_scale = True,
    )

PlotCombinedBkgRej(
    "BDT_Top",
    extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): BDT" ],
    y_max = 2000,
    y_min = 4,
    log_scale = True,
    )

PlotCombinedBkgRej(
    "BDT_W",
    extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): BDT" ],
    y_max = 250,
    y_min = 0.001,
    )

PlotCombinedBkgRej(
    "DNN_Top",
    extra_legend_lines = DEF_EXTRA_LINES + [ "Top tagger (#epsilon_{sig} = 80%): DNN" ],
    y_max = 2000,
    y_min = 4,
    log_scale = True
    )

PlotCombinedBkgRej(
    "DNN_W",
    extra_legend_lines = DEF_EXTRA_LINES + [ "#font[52]{W} tagger (#epsilon_{sig} = 50%): DNN" ],
    y_max = 250,
            y_min = 0.001,
    )

