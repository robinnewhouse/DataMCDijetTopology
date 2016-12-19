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

#############
### MC15C ###
#############

SYSTEMATICS_MC15C_MEDIUM_KINEMATIC = [ # pT + mass
        "LARGERJET_Medium_JET_Comb_Baseline_Kin",
        "LARGERJET_Medium_JET_Comb_Modelling_Kin",
        "LARGERJET_Medium_JET_Comb_TotalStat_Kin",
        "LARGERJET_Medium_JET_Comb_Tracking_Kin"
        ]

SYSTEMATICS_MC15C_MEDIUM_SUBSTRUCTURE = [ # D2 + TAU32
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub",
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub",
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub",
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"
        ]

SYSTEMATICS_MC15C_TEX = {
        "LARGERJET_Medium_JET_Comb_Baseline_Kin"  : "R_{trk}^{baseline} Kin.",
        "LARGERJET_Medium_JET_Comb_Modelling_Kin" : "R_{trk}^{modelling} Kin.",
        "LARGERJET_Medium_JET_Comb_TotalStat_Kin" : "R_{trk}^{stat} Kin.",
        "LARGERJET_Medium_JET_Comb_Tracking_Kin"  : "R_{trk}^{tracking} Kin.",
        "LARGERJET_Medium_JET_Rtrk_Baseline_Sub"  : "R_{trk}^{baseline} Sub.",
        "LARGERJET_Medium_JET_Rtrk_Modelling_Sub" : "R_{trk}^{modelling} Sub.",
        "LARGERJET_Medium_JET_Rtrk_TotalStat_Sub" : "R_{trk}^{stat} Sub.",
        "LARGERJET_Medium_JET_Rtrk_Tracking_Sub"  : "R_{trk}^{tracking} Sub."
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
        }

def get_mc15c_systematics_style_color(sys_name):
    return get_closest_match(sys_name, SYSTEMATICS_MC15C_LINESTYLE_COLOR)

##########################
### VARIABLE TEX NAMES ###
##########################

AXIS_TITLES = {
        "rljet0_pt"                       : "Leading Large-R Jet #it{p}_{T}",
        "rljet0_eta"                      : "Leading Large-R Jet #it{#eta}",
        "rljet0_phi"                      : "Leading Large-R Jet #it{#phi}",
        "rljet0_m"                        : "Leading Large-R Jet Mass",
        "rljet0_D2__caloMgt50GeV"         : "Leading Large-R Jet #it{D}_{2}^{#beta=1}",
        "rljet0_Tau32_wta__caloMgt100GeV" : "Leading Large-R Jet #it{#tau}_{32}^{WTA}",
        "rl_dijet_m"                      : "m_{JJ}",
        "rljet0_ungroomed_ntrk500"        : "Leading Large-R Jet n_{trk}",
        "rljet0_width"                    : "Leading Large-R Jet width",
        "caGroomJet0_pt"                  : "Leading Groomed C/A 1.5 Jet #it{p_{T}}",
        "caGroomJet0_eta"                 : "Leading Groomed C/A 1.5 Jet #it{#eta}",
        "caGroomJet0_phi"                 : "Leading Groomed C/A 1.5 Jet #it{#phi}",
        "caGroomJet0_m"                   : "Leading Groomed C/A 1.5 Jet Mass",
        "htt0_pt"                         : "Top Candidate #it{p_{T}}",
        "htt0_eta"                        : "Top Candidate #it{#eta}",
        "htt0_phi"                        : "Top Candidate #it{#phi}",
        "htt0_m"                          : "Top Candidate m_T",
        "htt0_m23m123"                    : "Top Candidate m_{23} / m_{123}",
        "htt0_atan1312"                   : "Top Candidate arctan (m_{13} / m_{12})",
        "BDT_score_TOP"                   : "Leading Large-R Jet BTDG Top Discriminant",
        "BDT_score_W"                     : "Leading Large-R Jet BTDG W Discriminant",
        "DNN_score_TEST"                  : "Leading Large-R Jet DNN Top Discriminant",
        "rljet0_SD"                       : "Leading Large-R Jet log #chi"
        }

def get_axis_title(histo_name):
        return get_closest_match(histo_name, AXIS_TITLES)
