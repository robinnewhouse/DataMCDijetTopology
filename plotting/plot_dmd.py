from plot_util import *
from plot_loader import *
from ROOT import *

# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MC15HadronicPythiaVJets
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MC15HadronicHerwigVJets
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MC15HadronicWJets

PYTHIA_SHERPA_WJETS_SF = 1.2670
PYTHIA_SHERPA_ZJETS_SF = 1.2043

class DMDLoader(PlotLoader):
    def __init__(self, filepath):
        super(DMDLoader, self).__init__(filepath)

    def get_htt_systematics_dict(self,
            hist_name,
            gen_name,
            up_sys_string,
            down_sys_string):

        nominal_dir = self.tfile.Get(gen_name).Get("nominal")
        h_sys_up = nominal_dir.Get(hist_name + "_" + up_sys_string)
        h_sys_down = nominal_dir.Get(hist_name + "_" + down_sys_string)

        systematics = {}
        systematics["sjcalib"] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }

        return systematics

GENERATOR = Enumeration( [
    "PYTHIA",
    "HERWIG",
    "SHERPA"
    ] )

###############
### MC15A/B ###
###############

SYSTEMATICS_MC15AB_KINEMATIC = [
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Tracking",
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Baseline",
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Modelling"
	]

SYSTEMATICS_MC15AB_TOP = [
	"LARGERJET_Medium_TopTagging_JET_Top_CrossCalib_Tau32",
	"LARGERJET_Medium_TopTagging_JET_Top_Run1_Tau32"
	]

SYSTEMATICS_MC15AB_WZ = [
	"LARGERJET_Medium_WZTagging_JET_WZ_CrossCalib_D2",
	"LARGERJET_Medium_WZTagging_JET_WZ_Run1_D2"
	]

SYSTEMATICS_MC15AB_TEX = {
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Tracking"        : "R_{trk}^{Tracking}",
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Baseline"        : "R_{trk}^{Baseline}",
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Modelling"       : "R_{trk}^{Modelling}",
	"LARGERJET_Medium_TopTagging_JET_Top_CrossCalib_Tau32" : "R_{trk}^{Calib} #tau_{32}",
	"LARGERJET_Medium_TopTagging_JET_Top_Run1_Tau32"       : "R_{trk}^{Run1} #tau_{32}",
	"LARGERJET_Medium_WZTagging_JET_WZ_CrossCalib_D2"      : "R_{trk}^{Calib} D_{2}",
	"LARGERJET_Medium_WZTagging_JET_WZ_Run1_D2"            : "R_{trk}^{Run1} D_{2}"
	}

def get_mc15ab_systematics_tex_title(sys_name):
	return get_closest_match(sys_name, SYSTEMATICS_MC15AB_TEX)

SYSTEMATICS_MC15AB_LINESTYLE_COLOR = {
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Tracking"        : (1, 2),
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Baseline"        : (1, 3),
	"LARGERJET_Medium_TopTagging_JET_Rtrk_Modelling"       : (1, 4),
	"LARGERJET_Medium_TopTagging_JET_Top_CrossCalib_Tau32" : (2, 4),
	"LARGERJET_Medium_TopTagging_JET_Top_Run1_Tau32"       : (2, 38),
	"LARGERJET_Medium_WZTagging_JET_WZ_CrossCalib_D2"      : (2, 2),
	"LARGERJET_Medium_WZTagging_JET_WZ_Run1_D2"            : (2, 46),
	}

def get_mc15ab_systematics_style_color(sys_name):
	return get_closest_match(sys_name, SYSTEMATICS_MC15AB_LINESTYLE_COLOR)

#############
### MC15C ###
#############

SYSTEMATICS_MC15C_WEAK_KINEMATIC = [
	"LARGERJET_Weak_JET_Rtrk_Baseline_mass",
	"LARGERJET_Weak_JET_Rtrk_Baseline_pT",
	"LARGERJET_Weak_JET_Rtrk_Modelling_mass",
	"LARGERJET_Weak_JET_Rtrk_Modelling_pT",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_mass",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_pT",
	"LARGERJET_Weak_JET_Rtrk_Tracking_mass",
	"LARGERJET_Weak_JET_Rtrk_Tracking_pT"
	]

SYSTEMATICS_MC15C_WEAK_D2 = [
	"LARGERJET_Weak_JET_Rtrk_Baseline_D2",
	"LARGERJET_Weak_JET_Rtrk_Modelling_D2",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_D2",
	"LARGERJET_Weak_JET_Rtrk_Tracking_D2"
	]

SYSTEMATICS_MC15C_WEAK_TAU32 = [
	"LARGERJET_Weak_JET_Rtrk_Baseline_Tau32",
	"LARGERJET_Weak_JET_Rtrk_Modelling_Tau32",
	"LARGERJET_Weak_JET_Rtrk_TotalStat_Tau32",
	"LARGERJET_Weak_JET_Rtrk_Tracking_Tau32"
	]

SYSTEMATICS_MC15C_MEDIUM_KINEMATIC = [ # pT + mass
	"LARGERJET_Medium_JET_Rtrk_Baseline_Kin",
	"LARGERJET_Medium_JET_Rtrk_Modelling_Kin",
	"LARGERJET_Medium_JET_Rtrk_TotalStat_Kin",
	"LARGERJET_Medium_JET_Rtrk_Tracking_Kin"
	]

SYSTEMATICS_MC15C_MEDIUM_SUBSTRUCTURE = [ # D2 + TAU32
	"LARGERJET_Medium_JET_Rtrk_Baseline_Sub",
	"LARGERJET_Medium_JET_Rtrk_Modelling_Sub",
	"LARGERJET_Medium_JET_Rtrk_TotalStat_Sub",
	"LARGERJET_Medium_JET_Rtrk_Tracking_Sub"
	]

SYSTEMATICS_MC15C_STRONG = [
        "LARGERJET_Strong_JET_Rtrk_Baseline_All",
        "LARGERJET_Strong_JET_Rtrk_Modelling_All",
        "LARGERJET_Strong_JET_Rtrk_TotalStat_All",
        "LARGERJET_Strong_JET_Rtrk_Tracking_All"
        ]

SYSTEMATICS_MC15C_TEX = {
        "LARGERJET_Strong_JET_Rtrk_Baseline_All"  : "R_{trk}^{baseline}",
        "LARGERJET_Strong_JET_Rtrk_Modelling_All" : "R_{trk}^{modelling}",
        "LARGERJET_Strong_JET_Rtrk_TotalStat_All" : "R_{trk}^{stat}",
        "LARGERJET_Strong_JET_Rtrk_Tracking_All"  : "R_{trk}^{tracking}",
        "LARGERJET_Weak_JET_Rtrk_Baseline_mass"   : "R_{trk}^{baseline} mass",
        "LARGERJET_Weak_JET_Rtrk_Baseline_pT"     : "R_{trk}^{baseline} #p_{T}",
        "LARGERJET_Weak_JET_Rtrk_Modelling_mass"  : "R_{trk}^{modelling} mass",
        "LARGERJET_Weak_JET_Rtrk_Modelling_pT"    : "R_{trk}^{modelling} #p_{T}",
        "LARGERJET_Weak_JET_Rtrk_TotalStat_mass"  : "R_{trk}^{stat} mass",
        "LARGERJET_Weak_JET_Rtrk_TotalStat_pT"    : "R_{trk}^{stat} #p_{T}",
        "LARGERJET_Weak_JET_Rtrk_Tracking_mass"   : "R_{trk}^{tracking} mass",
        "LARGERJET_Weak_JET_Rtrk_Tracking_pT"     : "R_{trk}^{tracking} #p_{T}",
        "LARGERJET_Weak_JET_Rtrk_Baseline_D2"     : "R_{trk}^{baseline} #D_{2}",
        "LARGERJET_Weak_JET_Rtrk_Modelling_D2"    : "R_{trk}^{modelling} #D_{2}",
        "LARGERJET_Weak_JET_Rtrk_TotalStat_D2"    : "R_{trk}^{stat} #D_{2}",
        "LARGERJET_Weak_JET_Rtrk_Tracking_D2"     : "R_{trk}^{tracking} #D_{2}",
        "LARGERJET_Weak_JET_Rtrk_Baseline_Tau32"  : "R_{trk}^{baseline} #tau_{32}",
        "LARGERJET_Weak_JET_Rtrk_Modelling_Tau32" : "R_{trk}^{modelling} #tau_{32}",
        "LARGERJET_Weak_JET_Rtrk_TotalStat_Tau32" : "R_{trk}^{stat} #tau_{32}",
        "LARGERJET_Weak_JET_Rtrk_Tracking_Tau32"  : "R_{trk}^{tracking} #tau_{32}",
        "LARGERJET_Medium_JET_Rtrk_Baseline_Kin"  : "R_{trk}^{baseline} Kin.",
        "LARGERJET_Medium_JET_Rtrk_Modelling_Kin" : "R_{trk}^{modelling} Kin.",
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Kin" : "R_{trk}^{stat} Kin.",
        "LARGERJET_Medium_JET_Rtrk_Tracking_Kin"  : "R_{trk}^{tracking} Kin.",
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub"  : "R_{trk}^{baseline} Sub.",
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub" : "R_{trk}^{modelling} Sub.",
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub" : "R_{trk}^{stat} Sub.",
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"  : "R_{trk}^{tracking} Sub."
	}

def get_mc15c_systematics_tex_title(sys_name):
    return get_closest_match(sys_name, SYSTEMATICS_MC15C_TEX)

SYSTEMATICS_MC15C_LINESTYLE_COLOR = {
        "LARGERJET_Strong_JET_Rtrk_Baseline_All"  : (1, kRed),
        "LARGERJET_Strong_JET_Rtrk_Modelling_All" : (1, kBlue),
        "LARGERJET_Strong_JET_Rtrk_TotalStat_All" : (1, kGreen),
        "LARGERJET_Strong_JET_Rtrk_Tracking_All"  : (1, 7),
        "LARGERJET_Weak_JET_Rtrk_Baseline_mass"   : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Baseline_pT"     : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Modelling_mass"  : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Modelling_pT"    : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_TotalStat_mass"  : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_TotalStat_pT"    : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Tracking_mass"   : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Tracking_pT"     : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Baseline_D2"     : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Modelling_D2"    : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_TotalStat_D2"    : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Tracking_D2"     : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Baseline_Tau32"  : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Modelling_Tau32" : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_TotalStat_Tau32" : (1, kRed),
        "LARGERJET_Weak_JET_Rtrk_Tracking_Tau32"  : (1, kRed),
        "LARGERJET_Medium_JET_Rtrk_Baseline_Kin"  : (1, 3),
        "LARGERJET_Medium_JET_Rtrk_Modelling_Kin" : (1, 4),
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Kin" : (1, 6),
        "LARGERJET_Medium_JET_Rtrk_Tracking_Kin"  : (1, 2),
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub"  : (2, 3),
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub" : (2, 4),
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub" : (2, 6),
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"  : (2, 2),
	}

def get_mc15c_systematics_style_color(sys_name):
    return get_closest_match(sys_name, SYSTEMATICS_MC15C_LINESTYLE_COLOR)

##########################
### VARIABLE TEX NAMES ###
##########################

AXIS_TITLES = {
        "rljet0_pt"                : "Leading Large-R Jet #it{p}_{T}",
        "rljet1_pt"                : "Subleading Large-R Jet #it{p}_{T}",
        "rljet0_eta"               : "Leading Large-R Jet #it{#eta}",
        "rljet1_eta"               : "Subleading Large-R Jet #it{#eta}",
        "rljet0_phi"               : "Leading Large-R Jet #it{#phi}",
        "rljet1_phi"               : "Subleading Large-R Jet #it{#phi}",
        "rljet0_m"                 : "Leading Large-R Jet Mass",
        "rljet1_m"                 : "Subleading Large-R Jet Mass",
        "rljet0_D2__caloMgt50GeV"                : "Leading Large-R Jet #it{D}_{2}^{#beta=1}",
        "rljet1_d2"                : "Subleading Large-R Jet #it{D}_{2}^{#beta=1}",
        "rljet0_Tau32_wta__caloMgt100GeV"             : "Leading Large-R Jet #it{#tau}_{32}^{WTA}",
        "rljet1_tau32"             : "Subleading Large-R Jet #it{#tau}_{32}^{WTA}",
        "rl_dijet_m"               : "m_{JJ}",
        "rljet0_ungroomed_ntrk500" : "Leading Large-R Jet n_{trk}",
        "rljet0_width"             : "Leading Large-R Jet width",
        "caGroomJet0_pt"           : "Leading Groomed C/A 1.5 Jet #it{p_{T}}",
        "caGroomJet0_eta"          : "Leading Groomed C/A 1.5 Jet #it{#eta}",
        "caGroomJet0_phi"          : "Leading Groomed C/A 1.5 Jet #it{#phi}",
        "caGroomJet0_m"            : "Leading Groomed C/A 1.5 Jet Mass",
        "htt0_pt"                  : "Top Candidate #it{p_{T}}",
        "htt0_eta"                 : "Top Candidate #it{#eta}",
        "htt0_phi"                 : "Top Candidate #it{#phi}",
        "htt0_m"                   : "Top Candidate m_T",
        "htt0_m23m123"             : "Top Candidate m_{23} / m_{123}",
        "htt0_atan1312"            : "Top Candidate arctan (m_{13} / m_{12})",
        "BDT_score_TOP"               : "Leading Large-R Jet BTDG Top Discriminant",
        "BDT_score_W"              : "Leading Large-R Jet BTDG W Discriminant",
        "DNN_score_TEST"              : "Leading Large-R Jet DNN Top Discriminant",
        }

def get_axis_title(histo_name):
	return get_closest_match(histo_name, AXIS_TITLES)
