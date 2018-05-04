#ifndef DataMCbackgroundEventSaver_H_
#define DataMCbackgroundEventSaver_H_

#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopAnalysis/EventSaverBase.h"

// OLD smoothed taggers (2015) + HEPTopTagger
// #include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"

// NEW smoothed/MVA taggers (2016-2017)
// #include "BoostedJetTaggers/IJSSTagger.h"

#include "ShowerDeconstruction/ShowerDeconstruction.h"

// NEW MVA taggers (2017-2018)
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "BoostedJetTaggers/TopoclusterTopTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "JetSubStructureUtils/BosonTag.h"


#include "JetRec/JetRecTool.h"
#include "JetCalibTools/JetCalibrationTool.h"

#include <utility>
#include <vector>

namespace top {
    class DataMCbackgroundEventSaver : public top::EventSaverFlatNtuple {

    public:
        DataMCbackgroundEventSaver();
        ~DataMCbackgroundEventSaver();

        // Run once at the start of the job
        void initialize(
                std::shared_ptr<top::TopConfig> config,
                TFile* file,
                const std::vector<std::string>& extraBranches
                );

        StatusCode initialize() {
            return StatusCode::SUCCESS;
        }

        // Run for every event (in every systematic) that needs saving
        void saveEvent(const top::Event& event);

    private:

        // static function that prevents the unwanted creation of branches in any eventsaver using the tree manager
        static int getBranchStatus(top::TreeManager const *, std::string const & variableName);

        // to load the AnalysisTop configuration
        std::shared_ptr<top::TopConfig> m_config;

        // variables corresponding to DynamicKeys from AnalysisTop cuts file
        bool m_runHTT;
        bool m_runSD;
        bool m_savePhoton;
        bool m_runSmoothWZtag;
        bool m_runSmoothToptag;
        bool m_runSmoothUncontained;
        bool m_runMVAtag;
        bool m_massOrderJets;
        unsigned m_num_fatjets_keep;
        unsigned m_debug_level;

        /***********/
        /* METHODS */
        /***********/

        // grabs the btag weights for the leading two track jets
        // of the ungroomed parent large-R calorimeter reco jet
        // void get_trackjet_btags(const xAOD::Jet* jet, float& jet_btag0, float& jet_btag1);

        // grabs the ungroomed parent large-R calorimeter reco jet
        const xAOD::Jet* get_parent_ungroomed(const xAOD::Jet* jet);

        // set all the relevant variables to sane default values
        void reset_containers(const bool on_nominal_branch);

        /*********************************/
        /* 2015 pre-rec smoothed taggers */
        /*********************************/

        // Top
        // SubstructureTopTagger *topTagger15_Mass_50eff;
        // SubstructureTopTagger *topTagger15_Mass_80eff;
        // SubstructureTopTagger *topTagger15_Tau32_50eff;
        // SubstructureTopTagger *topTagger15_Tau32_80eff;

        // W
        // std::unique_ptr<JetSubStructureUtils::BosonTag> wTagger15_50eff;
        // std::unique_ptr<JetSubStructureUtils::BosonTag> wTagger15_25eff;

        // Z
        // std::unique_ptr<JetSubStructureUtils::BosonTag> zTagger15_50eff;
        // std::unique_ptr<JetSubStructureUtils::BosonTag> zTagger15_25eff;

        /**************************************/
        /* Late 2016 updated smoothed taggers */
        /**************************************/

        // Top
        std::unique_ptr<SmoothedTopTagger> topTagger16_Tau32Split23_50eff;
        std::unique_ptr<SmoothedTopTagger> topTagger16_Tau32Split23_80eff;
        std::unique_ptr<SmoothedTopTagger> topTagger16_MassTau32_50eff;
        std::unique_ptr<SmoothedTopTagger> topTagger16_MassTau32_80eff;
        std::unique_ptr<SmoothedTopTagger> topTagger16_QwTau32_50eff;
        std::unique_ptr<SmoothedTopTagger> topTagger16_QwTau32_80eff;

        std::unique_ptr<SmoothedTopTagger> topTagger16_MassTau32_50eff_nocontain;
        std::unique_ptr<SmoothedTopTagger> topTagger16_MassTau32_80eff_nocontain;

        // W
        std::unique_ptr<SmoothedWZTagger> wTagger16_50eff;
        std::unique_ptr<SmoothedWZTagger> wTagger16_80eff;

        std::unique_ptr<SmoothedWZTagger> wTagger16_50eff_nocontain;
        std::unique_ptr<SmoothedWZTagger> wTagger16_80eff_nocontain;

        // Z
        std::unique_ptr<SmoothedWZTagger> zTagger16_50eff;
        std::unique_ptr<SmoothedWZTagger> zTagger16_80eff;

        std::unique_ptr<SmoothedWZTagger> zTagger16_50eff_nocontain;
        std::unique_ptr<SmoothedWZTagger> zTagger16_80eff_nocontain;

        /*****************/
        /* Other taggers */
        /*****************/

        // Shower Deconstruction (SD)
        void initializeSD(void);
        void runSDandFillTree(std::vector<const xAOD::Jet*>& rljets, bool doSystShifts);
        std::vector<fastjet::PseudoJet> JetconVecToPseudojet(xAOD::JetConstituentVector input);
        std::unique_ptr<ShowerDeconstruction> tagger_SDw_win20_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDt_win50_btag1;

        // HEPTopTagger
        int m_nHttTools;
        void runHTTAndFillTree(void);
        std::string m_httJetContainerPrefix;
        std::vector<std::string> m_httConfigs;

        // ML Taggers
        // top/W taggers -- BDTs
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_qqb;
        std::unique_ptr<JSSWTopTaggerBDT> m_wTagger_BDT_qq;
        // top/W taggers -- DNNs
        std::unique_ptr<JSSWTopTaggerDNN> m_topTagger_DNN_qqb;
        std::unique_ptr<JSSWTopTaggerDNN> m_wTagger_DNN_qq;
        // top/W taggers -- TopoclusterTagger
        std::unique_ptr<TopoclusterTopTagger> m_topTagger_TopoTagger;


        std::unique_ptr<JetRecTool> m_httTool;

        int m_NPV;


        /*************************/
        /* LARGE-R JET VARIABLES */
        /*************************/

        // nClusters study
        TH1 * l_jet_nClusters_hist;
        // TH1 * s_jet_nClusters_hist;
        // TH1 * l_jet_fractional_pt
        std::vector<float> m_rljet_fractional_pt_0;
        std::vector<float> m_rljet_fractional_pt_1;
        std::vector<float> m_rljet_fractional_pt_2;
        std::vector<float> m_rljet_fractional_pt_3;
        std::vector<float> m_rljet_fractional_pt_4;
        std::vector<float> m_rljet_fractional_pt_5;
        std::vector<float> m_rljet_fractional_pt_6;
        std::vector<float> m_rljet_fractional_pt_7;
        std::vector<float> m_rljet_fractional_pt_8;
        std::vector<float> m_rljet_fractional_pt_9;

        // reco large leading jets
        int m_rljet_count;
        std::vector<float> m_rljet_eta;
        std::vector<float> m_rljet_phi;

        // different pt/mass 'scales'
        std::vector<float> m_rljet_pt_calo;
        std::vector<float> m_rljet_pt_ta;
        std::vector<float> m_rljet_pt_comb;

        std::vector<float> m_rljet_m_calo;
        std::vector<float> m_rljet_m_ta;
        std::vector<float> m_rljet_m_comb;

        std::vector<int> m_rljet_pdgid;
        std::vector<float> m_rljet_matched_parton_pt;
        std::vector<float> m_rljet_matched_parton_eta;
        std::vector<float> m_rljet_matched_parton_phi;
        std::vector<float> m_rljet_matched_parton_m;

        std::vector<float> m_tljet_pt;
        std::vector<float> m_tljet_eta;
        std::vector<float> m_tljet_phi;
        std::vector<float> m_tljet_m;

        int m_pid1, m_pid2;

        // lead vs. sub-lead jet variables
        float m_rljet_mjj;
        float m_tljet_mjj;
        float m_parton_mjj;
        float m_rljet_ptasym;
        float m_rljet_mass_asym;
        float m_rljet_dy;
        float m_rljet_dR;
        float m_rljet_dphi;
        float m_rljet_deta;

        float m_photon0_pt;
        float m_photon0_eta;
        float m_photon0_phi;
        float m_photon0_ptcone20;
        float m_photon0_ptcone40;
        float m_photon0_topoetcone20;
        float m_photon0_topoetcone40;

        // substructure variables
        std::vector<float> m_rljet_Tau1_wta;
        std::vector<float> m_rljet_Tau2_wta;
        std::vector<float> m_rljet_Tau3_wta;
        std::vector<float> m_rljet_Tau32_wta;
        std::vector<float> m_rljet_ECF1;
        std::vector<float> m_rljet_ECF2;
        std::vector<float> m_rljet_ECF3;
        std::vector<float> m_rljet_C2;
        std::vector<float> m_rljet_D2;
        std::vector<float> m_rljet_FoxWolfram0;
        std::vector<float> m_rljet_FoxWolfram2;
        std::vector<float> m_rljet_Qw;
        std::vector<float> m_rljet_Angularity;
        std::vector<float> m_rljet_Aplanarity;
        std::vector<float> m_rljet_Dip12;
        std::vector<float> m_rljet_KtDR;
        std::vector<float> m_rljet_Mu12;
        std::vector<float> m_rljet_PlanarFlow;
        std::vector<float> m_rljet_Sphericity;
        std::vector<float> m_rljet_Split12;
        std::vector<float> m_rljet_Split23;
        std::vector<float> m_rljet_Split34;
        std::vector<float> m_rljet_ThrustMaj;
        std::vector<float> m_rljet_ThrustMin;
        std::vector<float> m_rljet_ZCut12;

        std::vector<int> m_rljet_NTrimSubjets;
        std::vector<int> m_rljet_ungroomed_ntrk500;
        std::vector<int> m_rljet_n_constituents;

        /*****************/
        /* HTT VARIABLES */
        /*****************/

        // nominal (independent of HTT config)
        int m_cajet_count;
        std::vector<float> m_htt_caJet_pt;
        std::vector<float> m_htt_caJet_eta;
        std::vector<float> m_htt_caJet_phi;
        std::vector<float> m_htt_caJet_m;

        // groomed (NOT independent of HTT config)
        std::vector<std::vector<float>> m_htt_caGroomJet_pt;
        std::vector<std::vector<float>> m_htt_caGroomJet_eta;
        std::vector<std::vector<float>> m_htt_caGroomJet_phi;
        std::vector<std::vector<float>> m_htt_caGroomJet_m;

        // tagged HTT jets (NOT independent of HTT config)
        std::vector<std::vector<int>> m_htt_tag;
        std::vector<std::vector<float>> m_htt_pt;
        std::vector<std::vector<float>> m_htt_eta;
        std::vector<std::vector<float>> m_htt_phi;
        std::vector<std::vector<float>> m_htt_m;
        std::vector<std::vector<float>> m_htt_m123;
        std::vector<std::vector<float>> m_htt_m23m123;
        std::vector<std::vector<float>> m_htt_atan1312;
        std::vector<std::vector<int>> m_htt_nTagCands;
        std::vector<std::vector<float>> m_htt_pts1;
        std::vector<std::vector<float>> m_htt_pts2;
        std::vector<std::vector<float>> m_htt_pts3;

        /********************/
        /* TAGGER VARIABLES */
        /********************/

        // 2016 smooth top
        std::vector<int> m_rljet_smooth16Top_Tau32Split23Tag50eff;
        std::vector<int> m_rljet_smooth16Top_Tau32Split23Tag80eff;
        std::vector<int> m_rljet_smooth16Top_MassTau32Tag50eff;
        std::vector<int> m_rljet_smooth16Top_MassTau32Tag80eff;
        std::vector<int> m_rljet_smooth16Top_QwTau32Tag50eff;
        std::vector<int> m_rljet_smooth16Top_QwTau32Tag80eff;

        std::vector<int> m_rljet_smooth16Top_MassTau32Tag50eff_nocontain;
        std::vector<int> m_rljet_smooth16Top_MassTau32Tag80eff_nocontain;

        // 2016 smooth W
        std::vector<int> m_rljet_smooth16W_Tag50eff;
        std::vector<int> m_rljet_smooth16W_Tag80eff;

        std::vector<int> m_rljet_smooth16W_Tag50eff_nocontain;
        std::vector<int> m_rljet_smooth16W_Tag80eff_nocontain;

        // 2016 smooth Z
        std::vector<int> m_rljet_smooth16Z_Tag50eff;
        std::vector<int> m_rljet_smooth16Z_Tag80eff;

        std::vector<int> m_rljet_smooth16Z_Tag50eff_nocontain;
        std::vector<int> m_rljet_smooth16Z_Tag80eff_nocontain;

        // Shower deconstruction output variables
        std::vector<float> m_rljet_SDw_calib;
        std::vector<float> m_rljet_SDw_uncalib;
        std::vector<float> m_rljet_SDw_combined;
        std::vector<float> m_rljet_SDw_dcut;
        std::vector<float> m_rljet_SDt_calib;
        std::vector<float> m_rljet_SDt_uncalib;
        std::vector<float> m_rljet_SDt_combined;
        std::vector<float> m_rljet_SDt_dcut;
        std::vector<float> m_rljet_SDw_calib_DOWN;
        std::vector<float> m_rljet_SDw_uncalib_DOWN;
        std::vector<float> m_rljet_SDw_combined_DOWN;
        std::vector<float> m_rljet_SDw_dcut_DOWN;
        std::vector<float> m_rljet_SDt_calib_DOWN;
        std::vector<float> m_rljet_SDt_uncalib_DOWN;
        std::vector<float> m_rljet_SDt_combined_DOWN;
        std::vector<float> m_rljet_SDt_dcut_DOWN;
        std::vector<float> m_rljet_SDw_calib_UP;
        std::vector<float> m_rljet_SDw_uncalib_UP;
        std::vector<float> m_rljet_SDw_combined_UP;
        std::vector<float> m_rljet_SDw_dcut_UP;
        std::vector<float> m_rljet_SDt_calib_UP;
        std::vector<float> m_rljet_SDt_uncalib_UP;
        std::vector<float> m_rljet_SDt_combined_UP;
        std::vector<float> m_rljet_SDt_dcut_UP;

        // b-tagging
        // std::vector<bool> m_rljet_btag_double;
        // std::vector<bool> m_rljet_btag_single;
        // std::vector<bool> m_rljet_btag_leading_pt;
            // BDT's
        std::vector<int> m_rljet_topTag_BDT_qqb;
        std::vector<float> m_rljet_topTag_BDT_qqb_score;
        std::vector<int> m_rljet_wTag_BDT_qq;
        std::vector<float> m_rljet_wTag_BDT_qq_score;
        // DNN's
        std::vector<int> m_rljet_topTag_DNN_qqb;
        std::vector<float> m_rljet_topTag_DNN_qqb_score;
        std::vector<int> m_rljet_wTag_DNN_qq;
        std::vector<float> m_rljet_wTag_DNN_qq_score;
        // Topocluster DNN's
        std::vector<int> m_rljet_topTag_TopoTagger_20wp;
        std::vector<int> m_rljet_topTag_TopoTagger_50wp;
        std::vector<int> m_rljet_topTag_TopoTagger_80wp;
        std::vector<float> m_rljet_topTag_TopoTagger_score;
        // // W taggers -- new 2016
        // int m_rljet_wTag_cut16_80eff_incl;
        // int m_rljet_wTag_cut16_50eff_incl;
        // int m_rljet_wTag_cut16_80eff_cont;
        // int m_rljet_wTag_cut16_50eff_cont;
        // BDT's
        std::vector<int> m_rljet_wTag_BDT;
        std::vector<float> m_rljet_wTag_BDT_score;

        ClassDef(top::DataMCbackgroundEventSaver, 0);
    };
}

#endif
