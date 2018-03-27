import os
import sys
import argparse
import atlas_style

from MyGenUtil import *
from ROOT import *
from plot_util import *

gROOT.SetBatch()
gStyle.SetOptTitle(0)

def get_all_directories(d, basepath=""):
    "Generator function to recurse into a ROOT file/dir and yield (path, obj) pairs"
    print d
    for key in d.GetListOfKeys():
        kname = key.GetName()
        tobj = d.Get(kname)
        if (not tobj):
            continue
        if tobj.InheritsFrom("TDirectory"):
            yield basepath+kname
            for i in get_all_directories(tobj, basepath+kname+"/"):
                yield i

def get_all_tobjects(d, basepath=""):
    "Generator function to recurse into a ROOT file/dir and yield (path, obj) pairs"
    for key in d.GetListOfKeys():
        kname = key.GetName()
        tobj = d.Get(kname)
        if (not tobj): continue
        if tobj.InheritsFrom("TTree"):
            pass
        elif key.IsFolder():
            for i in get_all_tobjects(d.Get(kname), basepath+kname+"/"):
                yield i
        else:
            yield basepath+kname, d.Get(kname)

class RatioPlotMaker(object):
    def __init__(self, f_num, f_den, dir_out, basepath = None):

	f_out = dir_out + "KalmanGlobalChi2PhysVal.root"

        print "\n\tRATIO PLOT MAKER\n"
        print "ARGUMENTS:"
        print "============================"
        print "numerator   : " , f_num
        print "denominator : " , f_den
        print "output      : " , f_out
        print "============================\n"

        self.f_num = TFile.Open(f_num, "READ")
        self.f_den = TFile.Open(f_den, "READ")
        print self.f_num
        print self.f_den

        self.f_out = TFile(f_out, "RECREATE")
        self.basepath = basepath

        root_dir = None
        if (self.basepath):
            root_dir = self.f_num.Get(basepath)
        else:
            root_dir = self.f_num

        for d in get_all_directories(root_dir):
            if (d): self.f_out.mkdir(d)

        self.num_func         = None
        self.den_func         = None
        self.xbounds_func     = None
        self.log_scale_func   = None
        self.ratio_name       = "Ratio"
        self.num_label        = "X"
        self.den_label        = "Y"
        self.ratio_size       = 0.5
        self.keep_plot_titles = False
        self.png_output_dir   = None

    def set_num_func(self, f):
        self.num_func = f

    def set_den_func(self, f):
        self.den_func = f

    def set_xbounds_func(self, f):
        self.xbounds_func = f

    def set_ratio_name(self, r):
        self.ratio_name = r

    def set_log_scale_func(self, f):
        self.log_scale_func = f

    def set_num_label(self, s):
        self.num_label = s

    def set_den_label(self, s):
        self.den_label = s

    def set_ratio_size(self, s):
        self.ratio_size = s

    def set_keep_plot_titles(self):
        self.keep_plot_titles = True

    def set_png_output_dir(self, pngdir):
        self.png_output_dir = pngdir

    def make_ratio_plots(self, normalize = True):
        for loc,tobj in get_all_tobjects(self.f_num):
            if (not tobj):
                continue

            if tobj.InheritsFrom("TProfile"):
                self.make_combined_TH1F(loc, False)
            elif tobj.InheritsFrom("TEfficiency"):
                self.make_combined_TH1F(loc, False)
            elif tobj.InheritsFrom("TH1F"):
                self.make_combined_TH1F(loc, normalize)
            else:
                print "skipping object with name: ", tobj.GetName()
        self.f_num.Close()
        self.f_den.Close()
        self.f_out.Close()

    def write_canvas_to_png(self, canvas, path):
        canvas.Print(self.png_output_dir + "/" + path + "/" + canvas.GetName() + ".png")

    def make_combined_TH1F(self, loc, normalize):
        print ""
        print "PROCESSING TH1F: ", loc
        print ""

        new_dir, new_name = os.path.split(loc)

        h_num = self.f_num.Get(loc).Clone()
        h_den = self.f_den.Get(loc).Clone()

        was_TEfficiency = False
        if (h_num.InheritsFrom("TEfficiency")):
            h_ratio = divide_tefficiency(h_num, h_den)
            h_num = convert_tefficiency(h_num)
            h_den = convert_tefficiency(h_den)
            was_TEfficiency = True

        if (h_num.InheritsFrom("TProfile") or was_TEfficiency):
            h_num.SetStats(0)
            h_den.SetStats(0)
        else:
            h_num.SetStats(1)
            h_den.SetStats(1)

        h_num.SetDirectory(0)
        h_den.SetDirectory(0)

        nbins = h_num.GetNbinsX()
        histos_are_the_same = True
        for i in range(nbins):
            if (h_num.GetBinContent(i+1) != h_den.GetBinContent(i+1)):
                histos_are_the_same = False
                break

        # no need to write out histograms if they are the same (or blank)
        if (histos_are_the_same
                or h_num.Integral() == 0.  or h_den.Integral() == 0.
                or h_num.GetEntries() == 0 or h_den.GetEntries() == 0):
            return

        if normalize:
            sf = h_num.Integral() / h_den.Integral()
            h_den.Scale(sf)

        c_name = "c_" + new_name
        c_tmp = TCanvas(c_name, c_name, 800, 800)

        pad1 = TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1.SetBottomMargin(0)
        pad1.Draw()
        pad1.cd()

        set_log_scale = False
        if (self.log_scale_func != None and self.log_scale_func(h_num.GetName())):
            set_log_scale = True
            pad1.SetLogy()

        y_min = 0.8 * min(map(lambda h: h.GetMinimum(), [h_num,h_den]))
        y_max = 1.15 * min(map(lambda h: h.GetMaximum(), [h_num,h_den]))

        if (abs(y_max) < 0.1 * abs(y_max - y_min)):
            y_max += 0.2 * abs(y_max - y_min)

        if (set_log_scale and y_min == 0):
            y_min = 0.1

        h_num.SetMinimum(y_min)
        h_den.SetMinimum(y_min)
        h_num.SetMaximum(y_max)
        h_den.SetMaximum(y_max)

        if (self.xbounds_func != None):
            bounds = self.xbounds_func(h_num.GetName())
            if (bounds != None):
                (x_min, x_max) = bounds
                for h in [h_num,h_den]:
                    h.GetXaxis().SetRangeUser(x_min, x_max)

        h_num.SetBit(TH1.kNoTitle);
        h_den.SetBit(TH1.kNoTitle);

        if (not self.keep_plot_titles):
            h_num.SetTitle("")
            h_den.SetTitle("")

        if (h_num.InheritsFrom("TProfile") or was_TEfficiency):
            h_num.Draw("PE,same")
            h_den.Draw("PE,same")
        else:
            h_den.Draw("PE,sames")
            h_num.Draw("PE,sames")

        l1 = TLatex(0.73,0.952,"ATLAS")
        l1.SetNDC()
        l1.SetTextFont(72)
        l1.Draw()

        l2 = TLatex(0.85,0.952,"Internal")
        l2.SetNDC()
        l2.SetTextFont(42)
        l2.Draw()

        c_tmp.Update()
        c_tmp.Modified()

        h_num.GetYaxis().SetTitleSize(20)
        h_num.GetYaxis().SetTitleFont(43)
        h_num.GetYaxis().SetTitleOffset(1.75)

        leg = TLegend(0.7,0.75,0.9,0.9)
        leg.AddEntry(h_num, self.num_label)
        leg.AddEntry(h_den, self.den_label)
        leg.SetTextSize(0.03)
        leg.Draw("same")

        c_tmp.Update()
        c_tmp.Modified()

        if (self.num_func != None): self.num_func(h_num)
        if (self.den_func != None): self.den_func(h_den)

        if h_num.InheritsFrom("TProfile"):
            h_num.Draw("PE,same")
            h_den.Draw("PE,same")
        else:
            h_den.Draw("PE,sames")
            h_num.Draw("PE,sames")

        TGaxis.SetMaxDigits(4)
        pad1.Update()
        pad1.Modified()
        c_tmp.Modified()
        c_tmp.Update()

        c_tmp.cd()
        pad2 = TPad("pad2", "pad2", 0, 0.05, 1, 0.3)
        pad2.SetTopMargin(0)
        pad2.SetBottomMargin(0.2)
        pad2.SetGridy()
        pad2.Draw()
        pad2.cd()

        if h_num.InheritsFrom("TProfile"):
            h_ratio = h_num.ProjectionX()
            h_ratio.Divide(h_den.ProjectionX())
        else:
            h_ratio = h_num.Clone("h_ratio")
            h_ratio.Divide(h_den)


        h_ratio.SetLineColor(kBlack)
        h_ratio.SetMarkerColor(kBlack)
        h_ratio.SetMinimum(1.0 - self.ratio_size)
        h_ratio.SetMaximum(1.0 + self.ratio_size)
        h_ratio.SetStats(0)
        h_ratio.SetMarkerStyle(20)
        h_ratio.SetMarkerSize(1)
        h_ratio.SetTitle("")
        h_ratio.GetYaxis().CenterTitle()
        h_ratio.Draw("ep")

        c_tmp.Update()
        c_tmp.Modified()

        h_ratio.GetYaxis().SetTitle(self.ratio_name)
        h_ratio.GetYaxis().SetNdivisions(505)
        h_ratio.GetYaxis().SetTitleSize(20)
        h_ratio.GetYaxis().SetTitleFont(43)
        h_ratio.GetYaxis().SetTitleOffset(1.75)
        h_ratio.GetYaxis().SetLabelFont(43)
        h_ratio.GetYaxis().SetLabelSize(20)

        h_ratio.GetXaxis().SetTitleSize(20)
        h_ratio.GetXaxis().SetTitleFont(43)
        h_ratio.GetXaxis().SetTitleOffset(3.75)
        h_ratio.GetXaxis().SetLabelFont(43)
        h_ratio.GetXaxis().SetLabelSize(20)

        c_tmp.Update()
        c_tmp.Modified()
        pad2.Update()
        pad2.Modified()

        if (self.png_output_dir != None):
        	make_dir(self.png_output_dir + new_dir)
		self.write_canvas_to_png(c_tmp, new_dir)

        print "new_dir: ", new_dir
        d = self.f_out.GetDirectory(new_dir)
        d.cd()
        c_tmp.Write()
        c_tmp.Close()

    def make_combined_TH2F(self, loc):
        print ""
        print "PROCESSING TH2F: ", loc
        print ""

        new_dir, new_name = os.path.split(loc)

        h_num = self.f_num.Get(loc).Clone()
        h_den = self.f_den.Get(loc).Clone()

        h_num.SetDirectory(0)
        h_den.SetDirectory(0)

        nbins = h_num.GetNbinsX()
        histos_are_the_same = True
        for i in range(nbins):
            if (h_num.GetBinContent(i+1) != h_den.GetBinContent(i+1)):
                histos_are_the_same = False
                break

        # no need to write out histograms if they are the same (or blank)
        if (histos_are_the_same or h_num.Integral() == 0. or h_den.Integral() == 0.):
            return

        c_name = "c_" + new_name
        c_tmp = TCanvas(c_name, c_name, 800, 800)

        set_log_scale = False
        if (self.log_scale_func != None and self.log_scale_func(h_num.GetXaxis().GetTitle())):
            set_log_scale = True
            c_tmp.SetLogy()

        if (not self.keep_plot_titles):
            h_num.SetTitle("")
            h_den.SetTitle("")

        l1 = TLatex(0.73,0.952,"ATLAS")
        l1.SetNDC()
        l1.SetTextFont(72)
        l1.Draw()

        l2 = TLatex(0.85,0.952,"Internal")
        l2.SetNDC()
        l2.SetTextFont(42)
        l2.Draw()

        c_tmp.Update()
        c_tmp.Modified()

        h_num.GetYaxis().SetTitleSize(20)
        h_num.GetYaxis().SetTitleFont(43)
        h_num.GetYaxis().SetTitleOffset(1.75)

        leg = TLegend(0.7,0.75,0.9,0.9)
        leg.AddEntry(h_num, self.num_label)
        leg.AddEntry(h_den, self.den_label)
        leg.SetTextSize(0.03)
        leg.SetFillStyle(0)
        leg.Draw("same")

        c_tmp.Update()
        c_tmp.Modified()

        h_num.Draw("")
        h_den.Draw("same")

        if (self.num_func != None): self.num_func(h_num)
        if (self.den_func != None): self.den_func(h_den)
        h_num.SetMarkerSize(0.3)
        h_den.SetMarkerSize(0.3)

        TGaxis.SetMaxDigits(4)
        c_tmp.Update()
        c_tmp.Modified()

        if (self.png_output_dir != None):
        	make_dir(self.png_output_dir + new_dir)
		self.write_canvas_to_png(c_tmp, new_dir)

        d = self.f_out.GetDirectory(new_dir)
        d.cd()
        c_tmp.Write()
        c_tmp.Close()
