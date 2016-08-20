from sys import argv, exit
import math

from ROOT import *
from plot_loader import *
from plot_systematics import *

if len(argv) != 2:
  print "Usage: python2 {0} <control_plots.root>" . format(argv[0])
  exit(1)

RAW = PlotLoader(argv[1])

def calculate_efficiency(h_tagged, h_inclusive):
    assert(h_tagged.GetSize() == h_inclusive.GetSize())

    integral_lower_bound = 1 # ignore underflow
    integral_upper_bound = h_inclusive.GetSize()+1

    tagged_err, inclusive_err = Double(), Double()

    tagged_count = h_tagged.IntegralAndError(integral_lower_bound,
            integral_upper_bound, tagged_err)

    inclusive_count = h_inclusive.IntegralAndError(integral_lower_bound,
            integral_upper_bound, inclusive_err)

    nominal_efficiency = tagged_count / inclusive_count

    total_err = nominal_efficiency * math.sqrt(
            (tagged_err / tagged_count)**2 + (inclusive_err / inclusive_count)**2
            )

    return nominal_efficiency, total_err

def calculate_rejection(h_tagged, h_inclusive):
    eff, eff_err = calculate_efficiency(h_tagged, h_inclusive)
    return 1.0 / eff, (1.0 / eff**2) * eff_err

def calculate_rejection_data(var_tagged, var_inclusive):
    h_tagged = RAW.get_hist(["data","nominal"], var_tagged)
    h_inclusive = RAW.get_hist(["data","nominal"], var_inclusive)
    return calculate_rejection(h_tagged, h_inclusive)

def calculate_rejection_prerec_mc(gen, var_tagged, var_inclusive, sys_list):
    sys_dict_tagged = RAW.get_systematics_dict(var_tagged, sys_list, [gen])
    sys_dict_inclusive = RAW.get_systematics_dict(var_tagged, sys_list, [gen])

    hsys_tagged =
    hsys_inclusive
    return calculate_rejection(h_tagged, h_inclusive)

print(calculate_rejection_data("h_rljet0_m_w_prerec_25eff_d2masscut", "h_rljet0_m"))
print(calculate_rejection_data("h_rljet0_m_w_prerec_50eff_d2masscut", "h_rljet0_m"))
print(calculate_rejection_data("h_rljet0_m_t_prerec_50eff_tau32masscut", "h_rljet0_m"))
print(calculate_rejection_data("h_rljet0_m_t_prerec_80eff_tau32masscut", "h_rljet0_m"))

