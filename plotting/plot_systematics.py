from ROOT import *
from math import *
from plot_util import *


SYS_METHOD = Enumeration( [
    "SYMMETRIC_MAX",
    "SYMMETRIC_QUAD",
    "ASYMMETRIC_MAX",
    "ASYMMETRIC_QUAD"
    ] )

def rebin_sys_dict(sys_dict, rebin):
    for sys_name, h_sys in sys_dict.iteritems():
        h_sys["up"].Rebin(rebin)
        h_sys["down"].Rebin(rebin)

class TH1Sys(object):
    '''
    a nominal TH1, along with up/down systematic variation histograms
    '''
    def __init__(self, h_nominal, systematics_dictionary):
        self.h_nominal = h_nominal.Clone()
        self.systematics_dictionary = systematics_dictionary

        self._reset()

    def _reset(self):
        self.num_bins = self.h_nominal.GetSize()
        self.num_systematics = len(self.systematics_dictionary)

        self.differences = {}
        self.sys_errors_sq = [0.0] * self.num_bins
        self.stat_errors_sq = [0.0] * self.num_bins
        self.sys_errors_sq_breakdown = {}

    def _compute_differences(self):
        for sys_name in self.systematics_dictionary.iterkeys():
            up_errs = [0.0] * self.num_bins
            down_errs = [0.0] * self.num_bins

            h_sys_up = self.systematics_dictionary[sys_name]["up"].Clone()
            h_sys_down = self.systematics_dictionary[sys_name]["down"].Clone()

            for ibin in range(self.num_bins):
                bin_err_up = h_sys_up.GetBinContent(ibin) - self.h_nominal.GetBinContent(ibin)
                bin_err_down = h_sys_down.GetBinContent(ibin) - self.h_nominal.GetBinContent(ibin)

                up_errs[ibin] = bin_err_up
                down_errs[ibin] = bin_err_down

            self.differences[sys_name] = {}
            self.differences[sys_name]["up"] = list(up_errs)
            self.differences[sys_name]["down"] = list(down_errs)

    def _compute_errors(self):
        self._reset()
        self._compute_differences()

        for sys_name in self.systematics_dictionary.iterkeys():
            up_errs = self.differences[sys_name]["up"]
            down_errs = self.differences[sys_name]["down"]

            self.sys_errors_sq_breakdown[sys_name] = [0.0] * self.num_bins

            for ibin in range(self.num_bins):
                bin_err_up = up_errs[ibin]
                bin_err_down = down_errs[ibin]

                max_bin_err = max( abs(bin_err_up), abs(bin_err_down) )
                self.sys_errors_sq[ibin] += max_bin_err**2
                self.sys_errors_sq_breakdown[sys_name][ibin] += max_bin_err**2

        for ibin in range(self.num_bins):
            nom_stat_err = self.h_nominal.GetBinError(ibin)
            self.stat_errors_sq[ibin] += nom_stat_err**2

    def get_differences(self):
        if (not self.differences): self._compute_differences()
        return dict(self.differences)

    def get_sys_errors_sq(self):
        if (not self.stat_errors_sq or not self.sys_errors_sq):
            self._compute_errors()
        return dict(self.sys_errors_sq)

    def get_stat_errors_sq(self):
        if (not self.stat_errors_sq or not self.sys_errors_sq):
            self._compute_errors()
        return dict(self.stat_errors_sq)

    def get_histo_with_systematic_errs(self, include_stat_error = True):
        if (not self.stat_errors_sq or not self.sys_errors_sq):
            self._compute_errors()

        total_errors = list(self.sys_errors_sq)

        if (include_stat_error):
            for ibin in range(len(self.stat_errors_sq)):
                total_errors[ibin] += self.stat_errors_sq[ibin]

        h_sys = self.h_nominal.Clone(self.h_nominal.GetName() + "_sys")
        for ibin in range(self.num_bins):
            h_sys.SetBinError(ibin, sqrt(total_errors[ibin]))

        return h_sys

    def rebin(self, rebin_num):
        self.h_nominal.Rebin(rebin_num)
        for sys_name in self.systematics_dictionary.iterkeys():
            self.systematics_dictionary[sys_name]["up"].Rebin(rebin_num)
            self.systematics_dictionary[sys_name]["down"].Rebin(rebin_num)

    def scale(self, scale_factor):
        ''' Scales the nominal and systematic histograms by the same value. '''
        self.h_nominal.Scale(scale_factor)
        for sys_name in self.systematics_dictionary.iterkeys():
            self.systematics_dictionary[sys_name]["up"].Scale(scale_factor)
            self.systematics_dictionary[sys_name]["down"].Scale(scale_factor)

class TH1SysEff(TH1Sys):
    def __init__(self,
            h_nominal_passed,
            systematics_dictionary_passed,
            h_nominal_total,
            systematics_dictionary_total
            ):
        ''' Takes care of the division necessary for an efficiency plot.
            NOTE: it is not actually necessary that BinContent(passed) < BinContent(total)
                  for the case of making a rejection (1/selection efficiency) plot
        '''

        self.h_nominal = h_nominal_passed.Clone(h_nominal_passed.GetName() + "_eff")
        self.h_nominal.Divide(h_nominal_total)

        self.systematics_dictionary = {}
        for sys_name, h_sys in systematics_dictionary_passed.iteritems():
            self.systematics_dictionary[sys_name] = {}
            for variation in ["up", "down"]:
                self.systematics_dictionary[sys_name][variation] = h_sys[variation].Clone(h_sys[variation].GetName() + "_eff")
                self.systematics_dictionary[sys_name][variation].Divide(systematics_dictionary_total[sys_name][variation])

        self._reset()
