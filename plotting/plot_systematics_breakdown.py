from ROOT import *

import os
from math import *
from sys import argv, exit

from plot_base import *
from plot_util import *
from plot_systematics import *
from plot_dmd import *

class PlotPythiaSystematicsBreakdown(PlotBase):
    def __init__(self, hsys_pythia, **kwargs):

        super(PlotPythiaSystematicsBreakdown, self).__init__(
                legend_loc = [0.65,0.9,0.9,0.6],
                width = 600,
                **kwargs)

        self.hsys_pythia = hsys_pythia
        if (self.rebin != None): self.hsys_pythia.rebin(self.rebin)
        self.hsys_pythia._compute_errors()

        h_nominal = self.hsys_pythia.h_nominal.Clone()
        self.canvas.SetRightMargin(0.07)

        self.breakdown_histograms = []

        for sys_name in self.hsys_pythia.systematics_dictionary.iterkeys():
            # clone the nominal histogram so we have the right bin spacing/bounds
            h_sys = h_nominal.Clone(h_nominal.GetName() + "_" + sys_name)

            for ibin in range(hsys_pythia.num_bins):
                if (h_nominal.GetBinContent(ibin) != 0):
                    sys_err_perc = sqrt(hsys_pythia.sys_errors_sq_breakdown[sys_name][ibin])
                    sys_err_perc /= h_nominal.GetBinContent(ibin)
                    sys_err_perc *= 100.0
                    h_sys.SetBinContent(ibin, sys_err_perc)
                else:
                    h_sys.SetBinContent(ibin, 0.0)

            self.breakdown_histograms.append(h_sys.Clone())

        self.determine_y_axis_title(h_nominal, "Uncertainty in %")

        line_colors = [2,4,7,8,9,3,5,6]

        for i, h in enumerate(self.breakdown_histograms):
            h.SetLineWidth(3)
            (st,col) = get_mc15c_systematics_style_color(h.GetName())
            h.SetLineStyle(st)
            h.SetLineColorAlpha(col, 0.75)
            h.SetMarkerColorAlpha(col, 0.75)
            h.SetFillStyle(0)
            h.SetMarkerStyle(0)
            h.SetMarkerSize(0)

            h.GetYaxis().SetTitle(self.y_title)
            h.GetXaxis().SetTitle(self.x_title + " " + self.x_units_str)
            self.set_x_axis_bounds(h)
            self.set_y_min(h)
            self.set_y_max(h)

            if (i == 0):
                h.Draw("hist")
            else:
                h.Draw("hist,same")
            self.leg.AddEntry(h, get_mc15c_systematics_tex_title(h.GetName()))

        self.leg.SetTextSizePixels(16)
        self.leg.SetFillStyle(0)
        self.leg.Draw()

