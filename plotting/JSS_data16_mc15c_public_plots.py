from ROOT import *
import atlas_style

import os
import math
from sys import argv, exit

from plot_base import *
from plot_util import *

if len(argv) != 3:
	print "Usage: python2 {0} <control_plots.root> <output_directory>" . format(argv[0])
	exit(1)

gROOT.SetBatch()
sane_defaults(wide_plot = True)
TGaxis.SetMaxDigits(4)

RAW = DMDRawLoader(argv[1])

OUTPUT_DIR = argv[2]
make_dir(OUTPUT_DIR)

class PlotPubData16MC15C(PlotBase):
    def __init__(self,
            h_data,
            h_pythia,
            pythia_systematics_dict,
            show_overflow = True,
            **kwargs):

        super(PlotPubData16MC15C, self).__init__(**kwargs)

        self.h_data   = h_data
        self.h_pythia = h_pythia
        self.pythia_systematics_dict = pythia_systematics_dict

        # make sure the histograms exist
        if (not self.h_data):
            raise IOError("could not locate histogram: " + self.h_data.GetName())

        if (not self.h_pythia):
            raise IOError("could not locate histogram: " + self.h_pythia.GetName())

        assert(self.h_data.GetSize() == self.h_pythia.GetSize())

        set_data_style_simple(self.h_data)
        set_mc_style_simple_hist(self.h_pythia, kRed)

        # we show the x-axis label underneath the ratio plot later
        self.h_data.GetXaxis().SetLabelSize(0)
        self.h_pythia.GetXaxis().SetLabelSize(0)

        # scale the MC
        self.pythia_norm_ratio = self.h_data.Integral() / self.h_pythia.Integral()
        self.h_pythia.Scale(self.pythia_norm_ratio)
        for sys, (h_up, h_down) in self.pythia_systematics_dict.iteritems():
            h_up.Scale(self.pythia_norm_ratio)
            h_down.Scale(self.pythia_norm_ratio)

        # now that nominal+systematics have been scaled, make the systematisc plot
        self.h_pythia_sys = make_histo_symm_errs(self.h_pythia, self.pythia_systematics_dict)
        set_mc_sys_err_style(self.h_pythia_sys)

        # y-axis title
        x_units_str = ""
        if self.x_units != "":
            x_units_str = "[" + self.x_units + "]"

        bin_width = self.h_data.GetXaxis().GetBinWidth(1)
        y_axis_label_str = "Events / " + format_bin_width(bin_width) + " " + self.x_units

        self.h_pythia.GetYaxis().SetTitle(y_axis_label_str)
        self.h_pythia.GetYaxis().SetTitleOffset(1.5)
        self.h_pythia.GetYaxis().SetLabelOffset(0.01)

        # SETUP THE CANVAS
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

        self.pad1.cd()

        new_x_min = self.h_data.GetXaxis().GetXmin()
        new_x_max = self.h_data.GetXaxis().GetXmax()

        if (self.x_max): new_x_max = self.x_max
        if (self.x_min): new_x_min = self.x_min

        self.h_data.GetXaxis().SetRangeUser(new_x_min , new_x_max)
        self.h_pythia.GetXaxis().SetRangeUser(new_x_min , new_x_max)
        self.h_pythia_sys.GetXaxis().SetRangeUser(new_x_min , new_x_max)
        self.pad1.RedrawAxis()
        if show_overflow:
            fill_overflow(self.h_data)
            fill_overflow(self.h_pythia)
            fill_overflow(self.h_pythia_sys)

        # draw mc first, and data on top of that
        self.h_pythia.Draw("hist")
        self.h_pythia_sys.Draw("E2,same")
        self.h_data.Draw("PE1,same")

        self.h_data.SetMinimum(0.01)
        self.h_pythia.SetMinimum(0.01)
        self.h_pythia_sys.SetMinimum(0.01)

        # rescale y-axis to add/subtract empty space (for readability)
        if self.empty_scale != 1:
            y_max = max([self.h_pythia.GetMaximum(), self.h_data.GetMaximum()])

            if self.log_scale:
                y_max *= 10 * self.empty_scale
            else:
                y_max *= self.empty_scale

            self.h_data.SetMaximum(y_max)
            self.h_pythia.SetMaximum(y_max)
            self.h_pythia_sys.SetMaximum(y_max)
            self.pad1.RedrawAxis()

        if (self.y_min != None):
            self.h_data.SetMinimum(self.y_min)
            self.h_pythia.SetMinimum(self.y_min)
            self.h_pythia_sys.SetMinimum(self.y_min)

        self.canvas.cd()
        self.leg.AddEntry(self.h_data, "data 2016")
        self.leg.AddEntry(self.h_pythia, "Pythia 8 dijet")
        self.leg.AddEntry(self.h_pythia_sys, "syst. #oplus stat. unc")
        self.leg.Draw()

        self.pad2.cd()

        # create ratio plots
        self.h_pythia_ratio = self.h_data.Clone("h_pythia_ratio")
        self.h_pythia_sys_ratio = self.h_data.Clone("h_pythia_sys_ratio")

        self.h_pythia_ratio.Divide(self.h_data, self.h_pythia, 1, 1, "")
        self.h_pythia_sys_ratio.Divide(self.h_data, self.h_pythia_sys, 1, 1, "")

        for ibin in range(0, self.h_pythia_sys_ratio.GetSize()):
            self.h_pythia_sys_ratio.SetBinContent(ibin, 1.0)

        set_mc_sys_err_style(self.h_pythia_sys_ratio)

        for h_ratio in [ self.h_pythia_ratio, self.h_pythia_sys_ratio ]:
            h_ratio.GetXaxis().SetLabelSize(22)
            h_ratio.GetYaxis().SetRangeUser(0.5, 1.5)
            h_ratio.GetYaxis().SetNdivisions(504, 0)
            h_ratio.GetXaxis().SetTitle(self.x_title + " " + x_units_str)
            h_ratio.GetYaxis().SetTitle("Data/Sim.")
            h_ratio.GetYaxis().CenterTitle()
            h_ratio.GetXaxis().SetTitleOffset(3.3)
            h_ratio.GetYaxis().SetTitleOffset(1.5)
            h_ratio.GetYaxis().SetLabelOffset(0.01)
            h_ratio.GetXaxis().SetLabelOffset(0.03)

        self.pad2.SetGridy(1)
        self.h_pythia_ratio.Draw("same")
        self.h_pythia_sys_ratio.Draw("E2,same")

        self.canvas.Update()
        self.canvas.Modified()

def make_pub_data16_mc15c_plot(var_name, **kwargs):
    return PlotPubData16MC15C(
            RAW.get_hist("nominal", "data", var_name),
            RAW.get_hist("nominal", "pythia", var_name),
            get_systematics_dict(RAW, "pythia", var_name, SYSTEMATIC_LIST_2016),
            name               = var_name + "_data16_mc15c",
            extra_legend_lines = [ "anti-k_{t} #it{R} = 1.0 jets", "Trimmed (f_{cut} = 0.05, #it{R}_{sub} = 0.2)"  ],
            legend_loc = [0.65,0.93,0.92,0.76],
            lumi_val           = "2.6",
            width              = 800,
            **kwargs)

data_mc_plots = [
        make_pub_data16_mc15c_plot("h_rljet0_m",
            empty_scale = 1.9,
            x_min = 0,
            x_max = 200,
            x_title = "Leading jet mass"
            ),

        make_pub_data16_mc15c_plot("h_rljet0_pt",
            log_scale = True,
            y_min = 0.011,
            x_min = 500,
            x_max = 3500,
            empty_scale = 1e4,
            show_overflow = False,
            x_title = "Leading jet #it{p_{T}}"
            ),

        make_pub_data16_mc15c_plot("h_rljet0_d2",
            empty_scale = 1.9,
            x_title = "Leading jet D_{2}^{#beta = 1}",
            x_min = 0,
            x_max = 5,
            x_units = ""
            ),

        make_pub_data16_mc15c_plot("h_rljet0_tau32",
            empty_scale = 1.8,
            x_title = "Leading jet #tau_{32}^{WTA}",
            x_min = 0,
            x_max = 1.0,
            x_units = ""
            )
        ]

for i in range(len(data_mc_plots)):
    data_mc_plots[i].print_to_file(OUTPUT_DIR + "/" + data_mc_plots[i].name + ".pdf")
