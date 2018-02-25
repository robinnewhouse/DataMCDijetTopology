from ROOT import *
from math import *
import array as arr
import os
import random
import string

def round_to_1(x):
    return round(x, -int(floor(log10(abs(x)))))

def get_systematics_dict(raw_loader, gen_name, var_name, systematic_list):
    systematics = {}
    for acro in systematic_list:
      h_sys_up = raw_loader.get_hist(acro + "U", gen_name, var_name)
      h_sys_down = raw_loader.get_hist(acro + "D", gen_name, var_name)
      systematics[acro] = (h_sys_up, h_sys_down)
    return systematics

def sane_defaults(wide_plot = False):
    # 43 -> Helvetica fixed size, not based on pad/canvas/whatever size
    gStyle.SetLabelFont(43, "X")
    gStyle.SetLabelFont(43, "Y")
    gStyle.SetLabelFont(43, "Z")
    gStyle.SetTitleFont(43, "X")
    gStyle.SetTitleFont(43, "Y")
    gStyle.SetTitleFont(43, "Z")

    s = 22 if wide_plot else 19

    gStyle.SetLabelSize(s, "X")
    gStyle.SetLabelSize(s, "Y")
    gStyle.SetLabelSize(s, "Z")
    gStyle.SetTitleSize(s, "X")
    gStyle.SetTitleSize(s, "Y")
    gStyle.SetTitleSize(s, "Z")

    gStyle.SetLegendFont(43)
    gStyle.SetLegendBorderSize(0)
    gStyle.SetLegendFillColor(0)
    gStyle.SetStatFont(43)
    gROOT.ForceStyle()

def set_highres_color_palette():
    NRGBs      = 5
    NCont      = 255
    stops      = [ 0.00, 0.34, 0.61, 0.84, 1.00 ]
    red        = [ 0.00, 0.00, 0.87, 1.00, 0.51 ]
    green      = [ 0.00, 0.81, 1.00, 0.20, 0.00 ]
    blue       = [ 0.51, 1.00, 0.12, 0.00, 0.00 ]
    stopsArray = arr.array('d', stops)
    redArray   = arr.array('d', red)
    greenArray = arr.array('d', green)
    blueArray  = arr.array('d', blue)
    TColor.CreateGradientColorTable(NRGBs, stopsArray, redArray, greenArray, blueArray, NCont)
    gStyle.SetNumberContours(NCont)

def fill_underflow(hist):
    underflow_count = hist.GetBinContent(0)
    hist.SetBinContent(1, hist.GetBinContent(1) + underflow_count)

def fill_overflow(hist):
    max_vis_bin = hist.GetXaxis().GetLast()
    NUM_BINS_TOTAL = hist.GetNbinsX()
    overflow = 0
    for i in range(max_vis_bin + 1, NUM_BINS_TOTAL + 1):
        overflow += hist.GetBinContent(i + 1)
    hist.SetBinContent(max_vis_bin, hist.GetBinContent(max_vis_bin) + overflow)

def format_bin_width(bin_spacing):
    """ format the bin width decimal string for the Y-axis, in a smart way. """
    if bin_spacing < 0.5:
        return str(round(bin_spacing * 20) / 20)
    if bin_spacing < 1.0:
        return str(round(bin_spacing * 10) / 10)
    elif bin_spacing < 10:
        if (int(bin_spacing) == bin_spacing):
            return str(int(round(bin_spacing * 4) / 4))
        else:
            return str(round(bin_spacing * 4) / 4)
    elif bin_spacing < 100:
        return str(round(bin_spacing * 2) / 2)
    else:
        return str(int(round(bin_spacing)))

def set_data_style_simple(hist, marker_size = 0.8, marker_style = 20):
        hist.SetFillStyle(0)
        hist.SetLineColor(kBlack)
        hist.SetLineWidth(2)
        hist.SetMarkerColor(kBlack)
        hist.SetMarkerSize(marker_size)
        hist.SetMarkerStyle(marker_style)

def set_mc_style_marker(hist, col, shape = 20,
      line_style = 1, line_width = 1, line_alpha = 1.0, marker_size = 0.8):
        hist.SetFillStyle(0)
        hist.SetFillColor(0)
        hist.SetLineColorAlpha(col, line_alpha)
        hist.SetMarkerColor(col)
        hist.SetMarkerSize(marker_size)
        hist.SetMarkerStyle(shape)
        hist.SetLineStyle(line_style)
        hist.SetLineWidth(line_width)


def set_mc_style_line(hist, col, line_style = 1, line_width = 1, alpha = 1):
        hist.SetFillStyle(0)
        hist.SetMarkerStyle(0)
        hist.SetMarkerSize(0)
        hist.SetMarkerColor(col)
        hist.SetLineStyle(line_style)
        hist.SetLineColorAlpha(col, alpha)
        hist.SetLineWidth(line_width)

def set_mc_style_simple_hist(hist, col):
        hist.SetFillStyle(1001)
        hist.SetFillColor(col)
        hist.SetLineWidth(0)
        hist.SetMarkerSize(0)

def set_mc_sys_err_style(hist, col = kGreen + 2, alpha = 0.7):
        hist.SetFillStyle(1001)
        hist.SetFillColorAlpha(col,alpha)
        hist.SetLineWidth(0)
        hist.SetMarkerSize(0)
        hist.SetMarkerStyle(0)

def make_dir(dir_path):
    try:
        os.makedirs(dir_path)
    except OSError:
        if not os.path.isdir(dir_path):
            raise

def make_histo_symm_errs(h_nominal, systematics):
    NUM_BINS = h_nominal.GetSize()

    h_sys = h_nominal.Clone(h_nominal.GetName() + "_sys")

    total_err = [0.0] * NUM_BINS

    for ibin in range(0, NUM_BINS):
        nom_stat_err = h_nominal.GetBinError(ibin)
        total_err[ibin] += nom_stat_err**2

    for sys_name, (h_sys_up, h_sys_down) in systematics.iteritems():
        assert(h_sys_up.GetSize() == h_nominal.GetSize())
        assert(h_sys_down.GetSize() == h_nominal.GetSize())

        for ibin in range(0, NUM_BINS):
            bin_err_up = h_sys_up.GetBinContent(ibin) - h_nominal.GetBinContent(ibin)
            bin_err_down = h_sys_down.GetBinContent(ibin) - h_nominal.GetBinContent(ibin)

            max_bin_err = max( abs(bin_err_up), abs(bin_err_down) )

            total_err[ibin] += max_bin_err**2

    for ibin in range(0, NUM_BINS):
        h_sys.SetBinError(ibin, sqrt(total_err[ibin]))

    return h_sys

def make_tgraph_asymm_errs(h_nominal, systematics):
    tgraph = TGraphAsymmErrors(h_nominal)
    tgraph.SetName(h_nominal.GetName() + "_graph_errs")

    total_errs_up = [0.0] * (h_nominal.GetSize() - 2)
    total_errs_down = [0.0] * (h_nominal.GetSize() - 2)

    #for ibin in range(1, h_nominal.GetSize() - 1):
    #    nom_stat_err = h_nominal.GetBinError(ibin)
    #    total_errs_up[ibin - 1] += nom_stat_err**2
    #    total_errs_down[ibin - 1] += nom_stat_err**2

    for sys_name, (h_sys_up, h_sys_down) in systematics.iteritems():
        assert(h_sys_up.GetSize() == h_nominal.GetSize())
        assert(h_sys_down.GetSize() == h_nominal.GetSize())

        sys_errs_up = [0.0] * h_nominal.GetSize()
        sys_errs_down = [0.0] * h_nominal.GetSize()

        for ibin in range(1, h_nominal.GetSize() - 1):
            bin_err_up = h_sys_up.GetBinContent(ibin) - h_nominal.GetBinContent(ibin)
            bin_err_down = h_sys_down.GetBinContent(ibin) - h_nominal.GetBinContent(ibin)

            if (bin_err_up * bin_err_down > 0): # same sign up/down shift
                max_bin_err = max( abs(bin_err_up), abs(bin_err_down) )
                bin_err_up = max_bin_err
                bin_err_down = max_bin_err

            sys_errs_up[ibin - 1] += bin_err_up**2.
            sys_errs_down[ibin - 1] += bin_err_down**2.

            total_errs_up[ibin - 1] += bin_err_up**2.
            total_errs_down[ibin - 1] += bin_err_down**2.

    for ibin in range(0, h_nominal.GetSize() - 2):
        x_err = tgraph.GetErrorX(ibin)
        tgraph.SetPointError(ibin, x_err, x_err, sqrt(total_errs_down[ibin]), sqrt(total_errs_up[ibin]))

    return tgraph

def divide_tgraph_asymm_errs(tgraph_num, tgraph_den):
    assert(tgraph_num.GetN() == tgraph_den.GetN())

    NUM_POINTS = tgraph_num.GetN()

    tgraph_ratio = TGraphAsymmErrors(NUM_POINTS)

    for ibin in range(NUM_POINTS):
        num_valX, num_valY = Double(0), Double(0)
        den_valX, den_valY = Double(0), Double(0)

        tgraph_num.GetPoint(ibin, num_valX, num_valY)
        tgraph_den.GetPoint(ibin, den_valX, den_valY)

        assert(num_valX == den_valX)

        num_exl = tgraph_num.GetErrorXlow(ibin)
        num_exh = tgraph_num.GetErrorXhigh(ibin)
        den_exl = tgraph_den.GetErrorXlow(ibin)
        den_exh = tgraph_den.GetErrorXhigh(ibin)

        # the assumption is that we are working with tgraphs with only
        # the Y errors varying, and this check is only to ensure that this is actually true
        assert(num_exl == num_exh == den_exl == den_exh)

        num_eyl = tgraph_num.GetErrorYlow(ibin)
        num_eyh = tgraph_num.GetErrorYhigh(ibin)
        den_eyl = tgraph_den.GetErrorYlow(ibin)
        den_eyh = tgraph_den.GetErrorYhigh(ibin)

        tgraph_num.GetPoint(ibin, num_valX, num_valY)
        tgraph_den.GetPoint(ibin, den_valX, den_valY)

        if (den_valY == 0 and num_valY == 0):
            ratio_valY = -1000
        else:
            ratio_valY = num_valY / den_valY

        ratio_eyl = 0
        ratio_eyh = 0

        if (num_valY != 0 and den_valY != 0):
            ratio_eyl = sqrt( ratio_valY**2 * ( (num_eyl / num_valY)**2 + (den_eyl / den_valY)**2 ) )
            ratio_eyh = sqrt( ratio_valY**2 * ( (num_eyh / num_valY)**2 + (den_eyh / den_valY)**2 ) )
        else:
            ratio_eyl = 99
            ratio_eyh = 99

        ratio_exl = num_exl
        ratio_exh = num_exh

        # symmetrize (for now)
        ratio_y_error = max(ratio_eyl, ratio_eyh)

        tgraph_ratio.SetPoint(ibin, num_valX, ratio_valY)

        tgraph_ratio.SetPointError(ibin, ratio_exl, ratio_exh, ratio_y_error, ratio_y_error)

    return tgraph_ratio

def get_closest_match(full_name, match_dict):
    tmp_key_match = ""
    tmp_val_match = ""
    for k, v in match_dict.iteritems():
        if k in full_name and len(k) > len(tmp_key_match):
            tmp_key_match = k
            tmp_val_match = v
    return tmp_val_match

def set_style_ratio(hist, y_title = "Data/Pred.", y_min = 0.5, y_max = 1.5):
     hist.GetYaxis().SetRangeUser(y_min, y_max)
     hist.GetYaxis().SetNdivisions(504, 0)
     hist.GetYaxis().SetTitle(y_title)
     hist.GetYaxis().CenterTitle()

class Enumeration(object):
    def __init__(self, names):  # or *names, with no .split()
        for number, name in enumerate(names):
            setattr(self, name, number)

def divide_tefficiency(t_num, t_den):
    NBINS = t_num.GetCopyTotalHisto().GetSize()
    h_ratio = t_num.GetCopyTotalHisto()
    for ibin in range(NBINS+2):
        num_eff = t_num.GetEfficiency(ibin)
        den_eff = t_den.GetEfficiency(ibin)
        if (num_eff == 0 or den_eff == 0):
          h_ratio.SetBinContent(ibin, 0)
          h_ratio.SetBinError(ibin, 0)
        else:
          num_err = max(t_num.GetEfficiencyErrorLow(ibin), t_num.GetEfficiencyErrorUp(ibin))
          den_err = max(t_den.GetEfficiencyErrorLow(ibin), t_den.GetEfficiencyErrorUp(ibin))
          nom_val = num_eff / den_eff
          nom_err = nom_val * sqrt( pow(num_err/num_eff, 2) + pow(den_err/den_eff,2) )
          h_ratio.SetBinContent(ibin, nom_val)
          h_ratio.SetBinError(ibin, nom_err)
    return h_ratio

def convert_tefficiency(teff):
    NBINS = teff.GetCopyTotalHisto().GetSize()
    h_new = teff.GetCopyTotalHisto()
    for ibin in range(NBINS+2):
        val = teff.GetEfficiency(ibin)
        err = max(teff.GetEfficiencyErrorLow(ibin), teff.GetEfficiencyErrorUp(ibin))
        h_new.SetBinContent(ibin, val)
        h_new.SetBinError(ibin, err)
    return h_new

def format_bin_width(bin_spacing):
    if bin_spacing < 0.5:
        return str(round(bin_spacing * 20) / 20)
    elif bin_spacing < 1.0:
        return str(round(bin_spacing * 10) / 10)
    elif bin_spacing < 10:
        if (int(bin_spacing) == bin_spacing):
            return str(int(round(bin_spacing * 4) / 4))
        else:
            return str(round(bin_spacing * 4) / 4)
    elif bin_spacing < 100:
        return str(round(bin_spacing * 2) / 2)
    else:
        return str(int(round(bin_spacing)))

def calculate_inclusive_efficiency(h_tagged, h_inclusive):
    assert(h_tagged.GetSize() == h_inclusive.GetSize())

    integral_lower_bound = 1 # ignore underflow
    integral_upper_bound = h_inclusive.GetSize()+1

    tagged_err, inclusive_err = Double(), Double()

    tagged_count = h_tagged.IntegralAndError(integral_lower_bound,
            integral_upper_bound, tagged_err)

    inclusive_count = h_inclusive.IntegralAndError(integral_lower_bound,
            integral_upper_bound, inclusive_err)

    nominal_efficiency = tagged_count / inclusive_count

    total_err = nominal_efficiency * sqrt(
            (tagged_err / tagged_count)**2 + (inclusive_err / inclusive_count)**2
            )

    return nominal_efficiency, total_err

def calculate_inclusive_rejection(h_tagged, h_inclusive):
    eff, eff_err = calculate_inclusive_efficiency(h_tagged, h_inclusive)
    rej = 1.0 / eff
    rej_err = (1.0 / eff**2) * eff_err

    sig_figs = -int(floor(log10(abs(rej_err))))
    rej = round(rej, sig_figs)
    rej_err = round(rej_err, sig_figs)

    return rej, rej_err

def is_tagged(var_name, untagged_var_name):
  return untagged_var_name in var_name and len(var_name) > len(untagged_var_name)

















