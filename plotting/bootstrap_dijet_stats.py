from ROOT import *
import atlas_style

import os
import math
from sys import argv, exit

from plot_base import *
from plot_util import *
from plot_dmd import *
from plot_loader import *

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)

RAW = DMDLoader("/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/dijet/27-01-2017__17:25:56__16122016_gridjobs_nominalOnly_JZXtagtest_v0/cp.merged.root")

OUTPUT_DIR = "/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/plotting/raw/dijet/27-01-2017__17:25:56__16122016_gridjobs_nominalOnly_JZXtagtest_v0/bootstrap"
make_dir(OUTPUT_DIR)

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" ]

REBIN_NUM = 4

ALL_GENERATORS = ["pythia","herwig","sherpa"]

ALL_TAG_NAMES = [
        "smooth16Top_MassTau32Tag50eff_JSSCut",
        "smooth16Top_MassTau32Tag80eff_JSSCut",
        "smooth16Top_QwTau32Tag50eff",
        "smooth16Top_QwTau32Tag80eff",
        "smooth16Top_Tau32Split23Tag50eff",
        "smooth16Top_Tau32Split23Tag80eff",
        "smooth16WTag_50eff_JSSCut",
        "smooth16WTag_80eff_JSSCut",
        "smooth16ZTag_50eff_JSSCut",
        "smooth16ZTag_80eff_JSSCut"
        ]

ALL_SLICES = range(3,13)

def print_dijet_stats(gen_name, tag_name, slice_num = None):
    name = "h_rljet0_m_comb_" + tag_name
    if (slice_num): name += "_JZ" + str(slice_num)

    h_dijet = RAW.get_hist([gen_name + "_dijet", "nominal"], name)
    print gen_name, ": ", tag_name, ": ", slice_num, ": ", h_dijet.Integral()

class PlotSmoothPDF(PlotBase):
    def __init__(self, gen_name, tag_name, smooth_times = 1, **kwargs):

        self.gen_name = gen_name
        self.tag_name = tag_name

        super(PlotSmoothPDF, self).__init__(
                lumi_val = "36.5",
                width = 600,
                name = gen_name + "_" + tag_name + "_smoothPDF",
                atlas_mod = "Internal",
                legend_loc = [0.55,0.93,0.91,0.55] if plot_systematics else [0.6,0.93,0.91,0.7],
                x_title = get_axis_title("h_rljet0_m_comb"),
                **kwargs)

        self.h_dijet = RAW.get_normalized_dijet(gen_name, "h_rljet0_m_comb_" + tag_name)
        self.h_dijet.Rebin(REBIN_NUM)

        self.h_dijet_smooth = h_dijet.Clone(h_dijet.GetName() + "_smooth" + str(smooth_times))
        self.h_dijet_sooth.Smooth(smooth_times)

        self.determine_y_axis_title(h_dijet)
        for h in [self.h_dijet, self.h_dijet_smooth]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetLabelSize(0)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pad_empty_space([self.h_dijet,self.h_dijet_smooth])
        if self.log_scale: self.name += "_log"

    def make_smoothpdf_plot(self, output_dir):
        set_mc_style_marker(self.h_dijet, kBlack)#, shape = 21, line_style = 3)
        set_mc_style_line(self.h_dijet_smoothed, kRed, line_width = 2)

        self.canvas.Clear()
        self.canvas = None
        self._make_canvas()
        self._make_decorations()

        self.canvas.cd()
        self.h_dijet.Draw("PE,same")
        self.h_dijet_smooth.Draw("hist,same")

        self.leg.AddEntry(self.h_dijet, gen)
        self.leg.AddEntry(self.h_dijet_smooth, gen + " smooth")
        self.leg.Draw()

        self.canvas.Update()
        self.canvas.Modified()

        self.print_to_file(output_dir + "/" + self.name + ".pdf")
        self.canvas.Clear()

for gen in ALL_GENERATORS:
    for tag in ALL_TAG_NAMES:
        for jet_slice in ALL_SLICES:
            print_dijet_stats(gen,tag,jet_slice)

