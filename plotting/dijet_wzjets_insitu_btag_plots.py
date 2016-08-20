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

if len(argv) != 3:
  print "Usage: python2 {0} <control_plots.root> <output_directory>" . format(argv[0])
  exit(1)

gROOT.SetBatch()
sane_defaults()
TGaxis.SetMaxDigits(4)

RAW = PlotLoader(argv[1])

OUTPUT_DIR = argv[2]
make_dir(OUTPUT_DIR)

# method for relative normalization of dijet and W/Z + jets samples to data
NORM_METHOD = Enumeration( [
    "LINE",
    "SIDEBANDS",
    "YIELD"
    ] )

class PlotWZDijets(PlotBase):
    def __init__(self, var_name, norm_method, **kwargs):

        super(PlotWZDijets, self).__init__(**kwargs)

        self.h_data = RAW.get_hist(["data", "nominal"] , var_name)
        set_data_style_simple(self.h_data)

        self.h_pythia = RAW.get_hist(["pythia_dijet", "nominal"] , var_name)
        self.h_pythia.SetFillColor(kRed)

        self.h_wjets = RAW.get_hist(["pythia_wjets", "nominal"] , var_name)
        self.h_wjets.SetFillColor(kBlue)

        self.h_zjets = RAW.get_hist(["pythia_zjets", "nominal"] , var_name)
        self.h_zjets.SetFillColor(kGreen)

        for h in [self.h_pythia, self.h_wjets, self.h_zjets]:
            h.SetLineWidth(0)
            h.SetMarkerSize(0)

        print "### ", var_name, " ###"
        print "Zqq yield: ", int(self.h_zjets.Integral())
        print "Wqq yield: ", int(self.h_wjets.Integral())

        if (self.rebin != None):
            self.h_data.Rebin(self.rebin)
            self.h_pythia.Rebin(self.rebin)
            self.h_wjets.Rebin(self.rebin)
            self.h_zjets.Rebin(self.rebin)

        self.determine_y_axis_title(self.h_data)
        self.pad_empty_space([self.h_data, self.h_pythia, self.h_wjets, self.h_zjets])
        for h in [self.h_data, self.h_pythia, self.h_wjets, self.h_zjets]:
            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            h.GetYaxis().SetTitleOffset(2.0)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

        self.pythia_raw_sf = self.h_data.Integral() / self.h_pythia.Integral()
        self.h_pythia_corrected = self.h_pythia.Clone(self.h_pythia.GetName() + "_corrected")

        print self.pythia_raw_sf

        self.h_pythia.Scale(self.pythia_raw_sf)

        # create (uncorrected) ratio histogram
        self.h_pythia_ratio = self.h_data.Clone("h_pythia_ratio")
        self.h_pythia_ratio.Divide(self.h_data, self.h_pythia, 1, 1, "")

        if norm_method == NORM_METHOD.LINE:
            self.g_pythia_ratio_blinded = TGraph(self.h_pythia_ratio)
            self.g_pythia_ratio_blinded.Set(0)

            tmp_graph = TGraph(self.h_pythia_ratio)
            x, y = Double(0), Double(0)
            for ibin in range(tmp_graph.GetN()):
                tmp_graph.GetPoint(ibin, x, y)
                if ( (x >= self.x_min and x < 60) or (x > 100 and x <= self.x_max) ):
                    self.g_pythia_ratio_blinded.SetPoint(self.g_pythia_ratio_blinded.GetN(), x, y)

            lf = TF1("lf", "[0]*x + [1]",self.x_min, self.x_max)
            self.g_pythia_ratio_blinded.Fit(lf)

            self.h_pythia_corrected.Scale(self.pythia_raw_sf)
            self.h_pythia_corrected.Multiply(lf)

        elif norm_method == NORM_METHOD.SIDEBANDS:
            bin_width = self.h_data.GetBinWidth(1)
            x1 = int(20. / bin_width)
            x2 = int(50. / bin_width)
            x3 = int(120. / bin_width)
            x4 = int(220. / bin_width)

            data_sb = self.h_data.Integral(x1,x2) + self.h_data.Integral(x3,x4)
            pythia_sb = self.h_pythia.Integral(x1,x2) + self.h_pythia.Integral(x3,x4)
            sb_sf = data_sb / pythia_sb
            print "SIDEBAND SF: ", sb_sf
            self.h_pythia_corrected.Scale(self.pythia_raw_sf * sb_sf)

        elif norm_method == NORM_METHOD.YIELD:
            self.h_pythia_corrected.Scale(self.pythia_raw_sf)

        else:
            print "Unrecognized normalization method, exiting..."
            exit(1)


        # make the MC histogram stack
        self.hs = THStack("hs_wzdijet", "hs_wzdijet")
        self.hs.Add(self.h_pythia_corrected.Clone())
        self.hs.Add(self.h_zjets.Clone())
        self.hs.Add(self.h_wjets.Clone())

        self.h_pythia_ratio_corrected = self.h_data.Clone("h_pythia_ratio")
        self.h_pythia_ratio_corrected.Divide(self.h_data, self.h_pythia_corrected, 1, 1, "")
        set_mc_style_marker(self.h_pythia_ratio_corrected, kRed, shape = 21)

        self.h_stack = self.h_pythia_corrected.Clone("h_stack")
        self.h_stack.Add(self.h_wjets)
        self.h_stack.Add(self.h_zjets)

        self.h_stack.SetFillStyle(3005)
        self.h_stack.SetFillColor(1)

        self.hs_ratio = self.h_data.Clone("hs_ratio")
        self.hs_ratio.Divide(self.h_data, self.h_stack, 1, 1, "")
        set_mc_style_marker(self.hs_ratio, kBlue, shape = 22)

        for h_ratio in [self.h_pythia_ratio, self.h_pythia_ratio_corrected]:
            set_style_ratio(h_ratio, y_min = 0.75, y_max = 1.25)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h_ratio)
            h_ratio.GetXaxis().SetTitleOffset(4.0)
            h_ratio.GetYaxis().SetTitleOffset(2.0)

        # SET UP THE CANVAS
        self.canvas.Divide(1,2)
        self.canvas.Modified()

        self.pad1 = self.canvas.cd(1)
        self.pad1.SetPad(0.0, 0.33, 1., 1.)
        self.pad1.SetTopMargin(0.07)
        self.pad1.SetBottomMargin(0.00)
        self.pad1.SetFillColorAlpha(0, 0.)
        if self.log_scale: self.pad1.SetLogy()

        self.pad2 = self.canvas.cd(2)
        self.pad2.SetPad(0.0, 0.0, 1., 0.33)
        self.pad2.SetTopMargin(0.05)
        self.pad2.SetBottomMargin(0.3)
        self.pad2.SetFillColorAlpha(0, 0.)
        self.pad2.SetGridy(1) # grid for ratio plot

        self.pad1.cd()

        self.h_data.Draw("PE,same")
        self.hs.Draw("histo,same")
        self.h_stack.Draw("E2,same")
        self.h_data.Draw("PE,same")

        self.pad1.RedrawAxis()

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "Data 2016")
        self.leg.AddEntry(self.h_pythia, "Pythia8 dijet")
        self.leg.AddEntry(self.h_wjets, "W+jets")
        self.leg.AddEntry(self.h_zjets, "Z+jets")
        self.leg.Draw()

        self.pad2.cd()

        self.h_pythia_ratio_corrected.Draw("PE")
        self.hs_ratio.Draw("PE,same")
        self.pad2.RedrawAxis("g")

        self.pad1.RedrawAxis()

        self.canvas.Update()
        self.canvas.Modified()

DEF_LINES = [ "Trimmed anti-#it{k_{t}} #it{R}=1.0", "Dijet Selection" ]

norm_method_strs = {}
norm_method_strs[NORM_METHOD.LINE] = "line_norm"
norm_method_strs[NORM_METHOD.SIDEBANDS] = "sideband_norm"
norm_method_strs[NORM_METHOD.YIELD] = "yield_norm"

wz_dijet_plots = []

def make_wz_dijets_plot(var_name, norm_method, **kwargs):
    extra_name_str = ""
    if (norm_method in norm_method_strs):
        extra_name_str = norm_method_strs[norm_method]

    wz_dijet_plots.append(
            PlotWZDijets(var_name,
                norm_method,
                name = var_name + "_wz_plus_dijets_" + extra_name_str,
                lumi_val = "3.5",
                width = 600,
                atlas_mod = "Internal",
                legend_loc = [0.70,0.93,0.94,0.76],
                #atlas_loc = [0.27, 0.9],
                #extra_lines_loc = [0.27, 0.8],
                x_title = get_axis_title(var_name),
                x_min = 20,
                x_max = 220,
                y_min = 0.01,
                **kwargs)
            )

BTAG_REBIN = 6

for nmeth in [NORM_METHOD.LINE, NORM_METHOD.YIELD, NORM_METHOD.SIDEBANDS]:
    make_wz_dijets_plot("h_rljet0_m_D2W50wp_btag_double",
            nmeth,
            empty_scale = 1.5,
            rebin = BTAG_REBIN,
            extra_legend_lines = ["Smooth D_{2}-tagged (50% wp)", "b-tag double"])

    make_wz_dijets_plot("h_rljet0_m_D2W50wp_btag_single",
            nmeth,
            empty_scale = 1.5,
            rebin = BTAG_REBIN,
            extra_legend_lines = ["Smooth D_{2}-tagged (50% wp)", "b-tag single"])

    make_wz_dijets_plot("h_rljet0_m_D2W50wp_btag_leading_pt",
            nmeth,
            empty_scale = 1.5,
            rebin = BTAG_REBIN,
            extra_legend_lines = ["Smooth D_{2}-tagged (50% wp)", "b-tag leading"])

    make_wz_dijets_plot("h_rljet0_m_w_prerec_50eff_d2cut",
            nmeth,
            empty_scale = 0.75,
            rebin = 2,
            extra_legend_lines = ["Smooth D_{2}-tagged (50% wp)"])

    make_wz_dijets_plot("h_rljet0_m_w_prerec_25eff_d2cut",
            nmeth,
            empty_scale = 0.75,
            rebin = 4,
            extra_legend_lines = ["Smooth D_{2}-tagged (25% wp)"])

for i in range(len(wz_dijet_plots)):
    wz_dijet_plots[i].print_to_file(OUTPUT_DIR + "/" + wz_dijet_plots[i].name + ".pdf")
    wz_dijet_plots[i].print_to_file(OUTPUT_DIR + "/" + "dijet_wz_plus_jets_btagging_fun", i, len(wz_dijet_plots))
