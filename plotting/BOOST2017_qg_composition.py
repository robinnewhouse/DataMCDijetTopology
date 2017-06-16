from ROOT import *
import atlas_style

import os
import math
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

DIJET_LOADER = DijetLoader("/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20170610/cp.merged.root")
GAMMAJET_LOADER = GammaJetLoader("/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_gammajet/20170610/cp.merged.root")

OUTPUT_DIR = "/eos/user/z/zmeadows/TopWTag/PLOTS/BOOST2017_COMPOSITION"

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" , "p_{T} > 450 GeV"]
HTT_DEF_LINES = [ "Trimmed C/A #it{R}=1.5", "Dijet Selection" , "p_{T} > 450 GeV"]

GAMMAJET_BIN_BOUNDS = []
for x in range(200,500,50):
  GAMMAJET_BIN_BOUNDS.append(x)
for x in range(500,1000,250):
  GAMMAJET_BIN_BOUNDS.append(x)
GAMMAJET_BIN_BOUNDS.append(1000)
GAMMAJET_BIN_BOUNDS.append(1500)
GAMMAJET_BIN_BOUNDS.append(2000)
GAMMAJET_BIN_BOUNDS.append(2500)

GAMMAJET_BIN_BOUNDS = array.array('d', GAMMAJET_BIN_BOUNDS)
print GAMMAJET_BIN_BOUNDS

def rej_rebin(h):
    return h.Rebin(len(GAMMAJET_BIN_BOUNDS)-1, h.GetName()+"_rebinned", GAMMAJET_BIN_BOUNDS)

class PlotQGComposition(PlotBase):
    def __init__(self, flip_legend = False, do_systematics = True,
                 wzjets_sf = 50, ttbar_sf = 50, **kwargs):

        super(PlotQGComposition, self).__init__(
                hide_lumi = True,
                width = 600,
                name = "dijet_gammajet_quark_gluon_composition",
                atlas_mod = "Simulation Internal",
                legend_loc = [0.20,0.93,0.56,0.70] if flip_legend else [0.7,0.4,0.9,0.3],
                atlas_loc = [0.2,0.85],
                extra_lines_loc = [0.6,0.82],
                #tex_size_mod = 0.9,
                #tex_spacing_mod = 0.75,
                x_title = get_axis_title("rljet0_pt"),
                y_max = 1.0,
                y_min = 0.4,
                x_min = 200,
                x_max = 4000,
                rebin = 16,
                **kwargs)

        # GET PLOTS FROM FILE
        histos = {}
        histos["dijet"] = {}
        histos["gamma"] = {}
        histos["dijet"]["pt_quark"] = DIJET_LOADER.get_hist(["pythia_dijet", "nominal"], "h_rljet0_pt_comb_lightquark_b")
        histos["dijet"]["pt_gluon"] = DIJET_LOADER.get_hist(["pythia_dijet", "nominal"], "h_rljet0_pt_comb_gluon")
        histos["gamma"]["pt_quark"] = rej_rebin(GAMMAJET_LOADER.get_hist(["sherpa_gammajet", "nominal"], "h_rljet0_pt_comb_lightquark_b"))
        histos["gamma"]["pt_gluon"] = rej_rebin(GAMMAJET_LOADER.get_hist(["sherpa_gammajet", "nominal"], "h_rljet0_pt_comb_gluon"))
        histos["dijet"]["pt_inclusive"] = DIJET_LOADER.get_hist(["pythia_dijet", "nominal"], "h_rljet0_pt_comb")
        histos["gamma"]["pt_inclusive"] = rej_rebin(GAMMAJET_LOADER.get_hist(["sherpa_gammajet", "nominal"], "h_rljet0_pt_comb"))

        for h in histos["dijet"].itervalues():
          h.Rebin(16)

        histos["dijet"]["quark_frac"] = TGraphAsymmErrors()
        histos["dijet"]["quark_frac"].Divide(histos["dijet"]["pt_quark"], histos["dijet"]["pt_inclusive"], "n")

        histos["gamma"]["quark_frac"] = TGraphAsymmErrors()
        histos["gamma"]["quark_frac"].Divide(histos["gamma"]["pt_quark"], histos["gamma"]["pt_inclusive"], "n")

        #histos["gamma"]["quark_frac"].RemovePoint(histos["gamma"]["quark_frac"].GetN())

        for hist_dict in histos.itervalues():
          for h in hist_dict.itervalues():
            h.GetYaxis().SetTitle("Quark Fraction")
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)
            h.GetYaxis().SetTitleOffset(2.0)
            h.GetXaxis().SetLabelSize(19)
            h.GetXaxis().SetTitleOffset(1.6)
            h.GetYaxis().SetTitleOffset(1.5)

        set_mc_style_line(histos["gamma"]["quark_frac"], kRed, line_width = 1)
        set_mc_style_line(histos["dijet"]["quark_frac"], kBlue, line_width = 1)

        histos["gamma"]["quark_frac"].SetFillStyle(1001)
        histos["gamma"]["quark_frac"].SetFillColorAlpha(kRed, 0.3)
        histos["dijet"]["quark_frac"].SetFillStyle(1001)
        histos["dijet"]["quark_frac"].SetFillColorAlpha(kBlue, 0.3)

        histos["gamma"]["quark_frac"].SetMarkerSize(0)
        histos["gamma"]["quark_frac"].SetMarkerStyle(0)
        histos["dijet"]["quark_frac"].SetMarkerSize(0)
        histos["dijet"]["quark_frac"].SetMarkerStyle(0)

        # TODO: necessary?
        #self.canvas.Clear()
        #self.canvas = None
        #self._make_canvas()
        #self._make_decorations()

        # SET UP THE CANVAS
        self.pad1 = self.canvas.cd(1)
        self.pad1.SetPad(0.0, 0.0, 1., 1.)
        self.pad1.SetTopMargin(0.07)
        self.pad1.SetRightMargin(0.07)
        self.pad1.SetBottomMargin(0.13)
        self.pad1.SetLeftMargin(0.13)
        self.pad1.SetFillColorAlpha(0, 0.)
        self.pad1.SetBorderSize(0)

        histos["dijet"]["quark_frac"].Draw("ACE3")
        histos["gamma"]["quark_frac"].Draw("CE3,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(histos["gamma"]["quark_frac"], "Sherpa #gamma + jet", "f")
        self.leg.AddEntry(histos["dijet"]["quark_frac"], "Pythia8 dijet", "f")
        self.leg.Draw()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(OUTPUT_DIR + "/" + self.name + ".pdf")
        self.canvas.Clear()

PlotQGComposition()
