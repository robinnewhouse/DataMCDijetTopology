from plot_util import *
from plot_loader import *
from ROOT import *

# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MC15HadronicPythiaVJets
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MC15HadronicHerwigVJets
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MC15HadronicWJets

PYTHIA_SHERPA_WJETS_SF = 1.2535
PYTHIA_SHERPA_ZJETS_SF = 1.1312

class DijetLoader(PlotLoader):
    def __init__(self, filepath):
        super(DijetLoader, self).__init__(filepath)
        self.PYTHIA_COLOR = kRed - 3
        self.HERWIG_COLOR = kBlue - 3
        self.SHERPA_COLOR = kGreen - 3
        self.WJETS_COLOR  = kOrange + 7
        self.ZJETS_COLOR  = kPink
        self.TTBAR_COLOR  = kViolet

    def get_wjets(self, hist_name, branch = "nominal"):
        h_wjets = self.get_hist(["pythia_wjets", branch] , hist_name)
        h_wjets.Scale(PYTHIA_SHERPA_WJETS_SF)
        #set_mc_style_line(h_wjets, self.WJETS_COLOR, 1, 2)
        set_mc_style_simple_hist(h_wjets, self.WJETS_COLOR)
        return h_wjets.Clone()

    def get_zjets(self, hist_name, branch = "nominal"):
        h_zjets = self.get_hist(["pythia_zjets", branch] , hist_name)
        h_zjets.Scale(PYTHIA_SHERPA_ZJETS_SF)
        #set_mc_style_line(h_zjets, self.ZJETS_COLOR, 1, 2)
        set_mc_style_simple_hist(h_zjets, self.ZJETS_COLOR)
        return h_zjets.Clone()

    def get_ttbar(self, hist_name, branch = "nominal"):
        h_ttbar = self.get_hist(["ttbar_allhad", branch] , hist_name)
        #set_mc_style_line(h_ttbar, self.TTBAR_COLOR, 1, 2)
        set_mc_style_simple_hist(h_ttbar, self.TTBAR_COLOR)
        return h_ttbar.Clone()

    def get_data(self, hist_name):
        h_data = self.get_hist(["data","nominal"], hist_name)
        set_data_style_simple(h_data)
        return h_data.Clone()

    def get_sigsub_data(self, hist_name, sig_sf = 1.0):
      if ("htt" in hist_name and "sjcalib" in hist_name):
        hist_name_data = hist_name.split("_sjcalib")[0]
      elif ("SD" in hist_name and "_UP" in hist_name):
        hist_name_data = hist_name.split("_UP")[0]
      elif ("SD" in hist_name and "_DOWN" in hist_name):
        hist_name_data = hist_name.split("_DOWN")[0]
      else:
        hist_name_data = hist_name

      h_data = self.get_data(hist_name_data)
      h_wjets = self.get_wjets(hist_name)
      h_zjets = self.get_zjets(hist_name)
      h_ttbar = self.get_ttbar(hist_name)

      h_data.Add(h_wjets, -1.0*sig_sf)
      h_data.Add(h_zjets, -1.0*sig_sf)
      h_data.Add(h_ttbar, -1.0*sig_sf)

      return h_data.Clone()

    def get_systematics_dictionary(self, generator, hist_name, branch_list, norm_to_pretagged = False):
        systematics = {}

        if ("htt" in hist_name):
          h_sys_up = self.get_normalized_dijet(generator, hist_name + "_sjcalib1030", "nominal", sig_sf = 1.0, normalize_to_pretagged = norm_to_pretagged)
          h_sys_down = self.get_normalized_dijet(generator, hist_name + "_sjcalib0970", "nominal", sig_sf = 1.0, normalize_to_pretagged = norm_to_pretagged)
          systematics["sjcalib"] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }
        elif ("SD" in hist_name):
          h_sys_up = self.get_normalized_dijet(generator, hist_name + "_UP", "nominal", normalize_to_pretagged = norm_to_pretagged)
          h_sys_down = self.get_normalized_dijet(generator, hist_name + "_DOWN", "nominal", normalize_to_pretagged = norm_to_pretagged)
          systematics["sjcalib"] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }
        else:
          for systematic_name in branch_list:
            h_sys_up = self.get_normalized_dijet(generator, hist_name, branch = systematic_name + "__1up", sig_sf = 1.0, normalize_to_pretagged = norm_to_pretagged)
            h_sys_down = self.get_normalized_dijet(generator, hist_name, branch = systematic_name + "__1down", sig_sf = 1.0, normalize_to_pretagged = norm_to_pretagged)
            systematics[systematic_name] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }

        return systematics

    def get_normalized_dijet(self, generator, hist_name, branch = "nominal", sig_sf = 1.0, normalize_to_pretagged = False):

        if ("pythia" in generator):
            generator = "pythia_dijet"
        elif ("herwig" in generator):
            generator = "herwig_dijet"
        elif ("sherpa" in generator):
            generator = "sherpa_dijet"
        else: raise
        

        h_dijet = self.get_hist([generator, branch], hist_name)
        h_dijet_nominal = self.get_hist([generator, "nominal"], hist_name)

        if (normalize_to_pretagged and is_tagged(hist_name, "h_rljet0_m_comb")):
          h_sigsub_data = self.get_sigsub_data("h_rljet0_m_comb", sig_sf)
          h_dijet_untagged = self.get_hist([generator, "nominal"], "h_rljet0_m_comb")
          dijet_sf = h_sigsub_data.Integral() / h_dijet_untagged.Integral()
        elif (normalize_to_pretagged and is_tagged(hist_name, "h_rljet0_pt_comb")):
          h_sigsub_data = self.get_sigsub_data("h_rljet0_pt_comb", sig_sf)
          h_dijet_untagged = self.get_hist([generator, "nominal"], "h_rljet0_pt_comb")
          dijet_sf = h_sigsub_data.Integral() / h_dijet_untagged.Integral()
        elif (normalize_to_pretagged and is_tagged(hist_name, "h_htt_caGroomJet0_pt")):
          h_sigsub_data = self.get_sigsub_data("h_htt_caGroomJet0_pt", sig_sf)
          h_dijet_untagged = self.get_hist([generator, "nominal"], "h_htt_caGroomJet0_pt")
          dijet_sf = h_sigsub_data.Integral() / h_dijet_untagged.Integral()
        elif (normalize_to_pretagged and is_tagged(hist_name, "h_htt_caGroomJet0_m")):
          h_sigsub_data = self.get_sigsub_data("h_htt_caGroomJet0_m", sig_sf)
          h_dijet_untagged = self.get_hist([generator, "nominal"], "h_htt_caGroomJet0_m")
          dijet_sf = h_sigsub_data.Integral() / h_dijet_untagged.Integral()
        else:
          h_sigsub_data = self.get_sigsub_data(hist_name, sig_sf)
          dijet_sf = h_sigsub_data.Integral() / h_dijet_nominal.Integral()

        h_dijet.Scale(dijet_sf)

        if ("pythia" in generator):
          set_mc_style_line(h_dijet, self.PYTHIA_COLOR, line_width = 3)
        elif ("herwig" in generator):
          set_mc_style_line(h_dijet, self.HERWIG_COLOR, line_width = 3)
        elif ("sherpa" in generator):
          set_mc_style_line(h_dijet, self.SHERPA_COLOR, line_width = 3)

        return h_dijet.Clone()

class GammaJetLoader(PlotLoader):
    def __init__(self, filepath):
        super(GammaJetLoader, self).__init__(filepath)
        self.GAMMA_COLOR = kRed - 4
        self.WZGAMMA_COLOR = kAzure - 3
        self.TTBAR_COLOR = kOrange + 7

    def get_gamma(self, hist_name, generator = "sherpa_gammajet", branch = "nominal"):
        h_gamma = self.get_hist([generator, branch] , hist_name)
        h_gamma.SetFillColor(self.GAMMA_COLOR)
        h_gamma.SetMarkerSize(0)
        h_gamma.SetLineWidth(1)
        return h_gamma

    def get_wzgamma(self, hist_name, branch = "nominal"):
        h_wzgamma = self.get_hist(["sherpa_wz_gamma", branch] , hist_name)
        h_wzgamma.SetFillColor(kAzure-3)
        h_wzgamma.SetMarkerSize(0)
        h_wzgamma.SetLineWidth(1)
        return h_wzgamma.Clone()

    def get_ttbar(self, hist_name, branch = "nominal"):
        h_ttbar = self.get_hist(["ttbar_gamma", branch] , hist_name)
        h_ttbar.SetFillColor(kOrange+7)
        h_ttbar.SetMarkerSize(0)
        h_ttbar.SetLineWidth(1)
        return h_ttbar.Clone()

    def get_data(self, hist_name):
        h_data = self.get_hist(["data","nominal"], hist_name)
        set_data_style_simple(h_data)
        return h_data.Clone()

    def get_sigsub_data(self, hist_name, sig_sf = 1.0):
      if ("htt" in hist_name and "sjcalib" in hist_name):
        hist_name_data = hist_name.split("_sjcalib")[0]
      elif ("SD" in hist_name and "_UP" in hist_name):
        hist_name_data = hist_name.split("_UP")[0]
      elif ("SD" in hist_name and "_DOWN" in hist_name):
        hist_name_data = hist_name.split("_DOWN")[0]
      else:
        hist_name_data = hist_name

      h_data    = self.get_data(hist_name_data)
      h_wzgamma = self.get_wzgamma(hist_name)
      h_ttbar   = self.get_ttbar(hist_name)

      h_data.Add(h_wzgamma, -1.0*sig_sf)
      h_data.Add(h_ttbar, -1.0*sig_sf)

      return h_data.Clone()

    def get_normalized_gamma(self, hist_name, generator = "sherpa_gammajet", branch = "nominal", sig_sf = 1.0, normalize_to_pretagged = False):

      if (normalize_to_pretagged and is_tagged(hist_name, "h_rljet0_m_comb")):
        h_sigsub_data = self.get_sigsub_data("h_rljet0_m_comb", sig_sf)
        h_gamma_untagged = self.get_hist([generator, "nominal"], "h_rljet0_m_comb")
        gamma_sf = h_sigsub_data.Integral() / h_gamma_untagged.Integral()
      elif (normalize_to_pretagged and is_tagged(hist_name, "h_rljet0_pt_comb")):
        h_sigsub_data = self.get_sigsub_data("h_rljet0_pt_comb", sig_sf)
        h_gamma_untagged = self.get_hist([generator, "nominal"], "h_rljet0_pt_comb")
        gamma_sf = h_sigsub_data.Integral() / h_gamma_untagged.Integral()
      elif (normalize_to_pretagged and is_tagged(hist_name, "h_htt_caGroomJet0_pt")):
        h_sigsub_data = self.get_sigsub_data("h_htt_caGroomJet0_pt", sig_sf)
        h_gamma_untagged = self.get_hist([generator, "nominal"], "h_htt_caGroomJet0_pt")
        gamma_sf = h_sigsub_data.Integral() / h_gamma_untagged.Integral()
      elif (normalize_to_pretagged and is_tagged(hist_name, "h_htt_caGroomJet0_m")):
        h_sigsub_data = self.get_sigsub_data("h_htt_caGroomJet0_m", sig_sf)
        h_gamma_untagged = self.get_hist([generator, "nominal"], "h_htt_caGroomJet0_m")
        gamma_sf = h_sigsub_data.Integral() / h_gamma_untagged.Integral()
      else:
        h_sigsub_data = self.get_sigsub_data(hist_name, sig_sf)
        h_gamma_nominal = self.get_hist([generator, "nominal"], hist_name)
        gamma_sf = h_sigsub_data.Integral() / h_gamma_nominal.Integral()

      h_gamma = self.get_gamma(hist_name, generator, branch)
      h_gamma.Scale(gamma_sf)

      return h_gamma.Clone()

    def get_systematics_dictionary(self, hist_name, branch_list, generator = "sherpa_dijet", normalize_to_pretagged = False):
        systematics = {}

        if ("htt" in hist_name):
          h_sys_up = self.get_normalized_gamma(hist_name + "_sjcalib1030", generator, "nominal", 1.0, normalize_to_pretagged)
          h_sys_down = self.get_normalized_gamma(hist_name + "_sjcalib0970", generator, "nominal", 1.0, normalize_to_pretagged)
          systematics["sjcalib"] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }
        else:
          for systematic_name in branch_list:
            if ("photon" in systematic_name):
              up_branch_name = systematic_name + "_UP"
              down_branch_name = systematic_name + "_DOWN"
            else:
              up_branch_name = systematic_name + "__1up"
              down_branch_name = systematic_name + "__1down"
              h_sys_up = self.get_normalized_gamma(hist_name, generator, up_branch_name, 1.0, normalize_to_pretagged)
              h_sys_down = self.get_normalized_gamma(hist_name, generator, down_branch_name, 1.0, normalize_to_pretagged)
              systematics[systematic_name] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }

        return systematics

    def get_stack_plot(self, hist_name):
        h_gamma   = self.get_normalized_gamma(hist_name).Clone()
        h_wzgamma = self.get_wzgamma(hist_name).Clone()
        h_ttbar   = self.get_ttbar(hist_name).Clone()

        h_gamma.SetName(hist_name + "_gamma")
        h_wzgamma.SetName(hist_name + "_wz")
        h_ttbar.SetName(hist_name + "_ttbar")

        tmp_name = "hs_gammajet_" + hist_name
        hs = THStack(tmp_name,tmp_name)
        hs.Add(h_ttbar)
        hs.Add(h_wzgamma)
        hs.Add(h_gamma)

        return hs

    def get_sum_plot(self,
        hist_name,
        branch,
        generator = "sherpa_gammajet",
        sig_sf = 1.0,
        normalize_to_pretagged = False):

      h_gamma   = self.get_normalized_gamma(hist_name, generator, branch, sig_sf, normalize_to_pretagged)
      h_wzgamma = self.get_wzgamma(hist_name, branch)
      h_ttbar   = self.get_ttbar(hist_name, branch)

      h_sum = h_gamma.Clone(hist_name + "_gammajet_sum")
      h_sum.Add(h_wzgamma)
      h_sum.Add(h_ttbar)

      return h_sum

    def get_htt_gammajet_systematics_dict(self,
            gen_name,
            hist_name,
            up_sys_string,
            down_sys_string):

        h_sys_up = self.get_hist([gen_name, "nominal"], hist_name + "_" + up_sys_string)
        h_sys_down = self.get_hist([gen_name, "nominal"], hist_name + "_" + down_sys_string)

        systematics = {}
        systematics["sjcalib"] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }

        return systematics

#############
### MC15C ###
#############

SYSTEMATICS_MC15C_WEAK = [
	"LARGERJET_Weak_JET_Comb_Baseline_mass",
	"LARGERJET_Weak_JET_Comb_Modelling_mass",
	"LARGERJET_Weak_JET_Comb_TotalStat_mass",
	"LARGERJET_Weak_JET_Comb_Tracking_mass",
	"LARGERJET_Weak_JET_Rtrk_Baseline_D2Beta1",
	"LARGERJET_Weak_JET_Rtrk_Baseline_Qw",
	"LARGERJET_Weak_JET_Rtrk_Baseline_Split12",
	"LARGERJET_Weak_JET_Rtrk_Baseline_Split23",
	"LARGERJET_Weak_JET_Rtrk_Baseline_Tau21WTA",
	"LARGERJET_Weak_JET_Rtrk_Baseline_Tau32WTA",
	"LARGERJET_Weak_JET_Rtrk_Baseline_pT",
	"LARGERJET_Weak_JET_Rtrk_Modelling_D2Beta1",
	"LARGERJET_Weak_JET_Rtrk_Modelling_Qw",
	"LARGERJET_Weak_JET_Rtrk_Modelling_Split12",
	"LARGERJET_Weak_JET_Rtrk_Modelling_Split23",
	"LARGERJET_Weak_JET_Rtrk_Modelling_Tau21WTA",
	"LARGERJET_Weak_JET_Rtrk_Modelling_Tau32WTA",
	"LARGERJET_Weak_JET_Rtrk_Modelling_pT",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_D2Beta1",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_Qw",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_Split12",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_Split23",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_Tau21WTA",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_Tau32WTA",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_pT",
	"LARGERJET_Weak_JET_Rtrk_Tracking_D2Beta1",
	"LARGERJET_Weak_JET_Rtrk_Tracking_Qw",
	"LARGERJET_Weak_JET_Rtrk_Tracking_Split12",
	"LARGERJET_Weak_JET_Rtrk_Tracking_Split23",
	"LARGERJET_Weak_JET_Rtrk_Tracking_Tau21WTA",
	"LARGERJET_Weak_JET_Rtrk_Tracking_Tau32WTA",
	"LARGERJET_Weak_JET_Rtrk_Tracking_pT",
	]

SYSTEMATICS_MC15C_MEDIUM = [
        "LARGERJET_Medium_JET_Comb_Baseline_Kin",
        "LARGERJET_Medium_JET_Comb_Modelling_Kin",
        "LARGERJET_Medium_JET_Comb_TotalStat_Kin",
        "LARGERJET_Medium_JET_Comb_Tracking_Kin",
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub",
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub",
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub",
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"
        ]

SYSTEMATICS_MC15C_STRONG = [
	"LARGERJET_Strong_JET_Comb_Baseline_All",
	"LARGERJET_Strong_JET_Comb_Modelling_All",
	"LARGERJET_Strong_JET_Comb_TotalStat_All",
	"LARGERJET_Strong_JET_Comb_Tracking_All",
	]

SYSTEMATICS_MC15C_TEX = {
        "LARGERJET_Medium_JET_Comb_Baseline_Kin"  : "R_{trk}^{baseline} Kin.",
        "LARGERJET_Medium_JET_Comb_Modelling_Kin" : "R_{trk}^{modelling} Kin.",
        "LARGERJET_Medium_JET_Comb_TotalStat_Kin" : "R_{trk}^{stat} Kin.",
        "LARGERJET_Medium_JET_Comb_Tracking_Kin"  : "R_{trk}^{tracking} Kin.",
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub"  : "R_{trk}^{baseline} Sub.",
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub" : "R_{trk}^{modelling} Sub.",
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub" : "R_{trk}^{stat} Sub.",
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"  : "R_{trk}^{tracking} Sub.",
        "sig_norm_sf"  : "Sig. Norm."
        }

def get_mc15c_systematics_tex_title(sys_name):
    return get_closest_match(sys_name, SYSTEMATICS_MC15C_TEX)

SYSTEMATICS_MC15C_LINESTYLE_COLOR = {
        "LARGERJET_Medium_JET_Comb_Baseline_Kin"  : (1, 3),
        "LARGERJET_Medium_JET_Comb_Modelling_Kin" : (1, 4),
        "LARGERJET_Medium_JET_Comb_TotalStat_Kin" : (1, 6),
        "LARGERJET_Medium_JET_Comb_Tracking_Kin"  : (1, 2),
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub"  : (2, 3),
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub" : (2, 4),
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub" : (2, 6),
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"  : (2, 2),
        "sig_norm_sf"  : (1, 1),
        "sjcalib"  : (1, 1),
        }

def get_mc15c_systematics_style_color(sys_name):
    return get_closest_match(sys_name, SYSTEMATICS_MC15C_LINESTYLE_COLOR)

##########################
### VARIABLE TEX NAMES ###
##########################

AXIS_TITLES = {
        "rljet0_pt"                : "Leading Trimmed large-#it{R} jet #it{p_{T}}",
        "rljet0_eta"               : "Leading Trimmed large-#it{R} Jet #it{#eta}",
        "rljet0_phi"               : "Leading Trimmed large-#it{R} Jet #it{#phi}",
        "rljet0_m"                 : "Leading Trimmed large-#it{R} jet combined mass",
        "rljet0_D2"                : "Leading Trimmed large-#it{R} Jet #it{D}_{2}",
        "rljet0_Tau32"             : "Leading Trimmed large-#it{R} Jet #it{#tau}_{32}",
        "rljet0_Qw"                : "Leading Trimmed large-#it{R} Jet Q_{w}",
        "rljet0_Split23"           : "Leading Trimmed large-#it{R} Jet #sqrt{d_{23}}",
        "rl_dijet_m"               : "m_{JJ}",
        "rljet0_ungroomed_ntrk500" : "Leading large-#it{R} Jet n_{trk}",
        "rljet0_NTrimSubjets"      : "Leading large-#it{R} Jet n_{subjets}",
        "rljet0_width"             : "Leading large-#it{R} Jet width",
        "caGroomJet0_pt"           : "Leading Trimmed C/A large-#it{R} jet #it{p_{T}}",
        "caGroomJet0_eta"          : "Leading large-#it{R} jet #it{#eta}",
        "caGroomJet0_phi"          : "Leading large-#it{R} jet #it{#phi}",
        "caGroomJet0_m"            : "Leading Trimmed C/A large-#it{R} jet mass",
        "htt0_pt"                  : "Top Candidate #it{p_{T}}",
        "htt0_eta"                 : "Top Candidate #it{#eta}",
        "htt0_phi"                 : "Top Candidate #it{#phi}",
        "htt0_m"                   : "Top Candidate m_{T}",
        "htt0_m23m123"             : "Top Candidate m_{23} / m_{123}",
        "htt0_atan1312"            : "Top Candidate arctan (m_{13} / m_{12})",
        "BDT_score_top"            : "Leading large-#it{R} jet BDT #it{top} tag discriminant",
        "BDT_score_w"              : "Leading large-#it{R} jet BDT #it{W} tag discriminant",
        "DNN_score_top"            : "Leading large-#it{R} jet DNN #it{top} tag discriminant",
        "DNN_score_w"              : "Leading large-#it{R} jet DNN #it{W} tag discriminant",
        "rljet0_SD"                : "Leading large-#it{R} jet log #chi",
        "h_mu_"                    : "<#mu>",
        "h_NPV"                    : "NPV"
        }

def get_axis_title(histo_name):
        return get_closest_match(histo_name, AXIS_TITLES)
