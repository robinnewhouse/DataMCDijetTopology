#ifndef DataMCbackgroundEventSaver_H_
#define DataMCbackgroundEventSaver_H_

#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopAnalysis/EventSaverBase.h"

// OLD smoothed taggers (2015) + HEPTopTagger
#include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"
#include "JetSubStructureUtils/BosonTag.h"

// NEW smoothed/MVA taggers (2016-2017)
#include "BoostedJetTaggers/IJSSTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"
#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

#include "ShowerDeconstruction/ShowerDeconstruction.h"

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
        // to load the AnalysisTop configuration
        std::shared_ptr<top::TopConfig> m_config;

        // variables corresponding to DynamicKeys from AnalysisTop cuts file
        bool m_runHTT;
        bool m_runSD;
        bool m_savePhoton;
        unsigned m_num_fatjets_keep;
        unsigned m_debug_level;

        /*********/
        /* TOOLS */
        /*********/

        // CA2 Subjet calibration tool
        JetCalibrationTool* m_jetcalib_subjet;

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
        SubstructureTopTagger *topTagger15_Mass_50eff;
        SubstructureTopTagger *topTagger15_Mass_80eff;
        SubstructureTopTagger *topTagger15_Tau32_50eff;
        SubstructureTopTagger *topTagger15_Tau32_80eff;

        // W
        std::unique_ptr<JetSubStructureUtils::BosonTag> wTagger15_50eff;
        std::unique_ptr<JetSubStructureUtils::BosonTag> wTagger15_25eff;

        // Z
        std::unique_ptr<JetSubStructureUtils::BosonTag> zTagger15_50eff;
        std::unique_ptr<JetSubStructureUtils::BosonTag> zTagger15_25eff;

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

        // MVA taggers
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_qqb;
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_inclusive;
        std::unique_ptr<JSSWTopTaggerBDT> m_wTagger_BDT;
        std::unique_ptr<JSSWTopTaggerDNN> m_topTagger_DNN;
        std::unique_ptr<JSSWTopTaggerDNN> m_wTagger_DNN;

        // Shower Deconstruction (SD)
        void initializeSD(void);
        void runSDandFillTree(std::vector<const xAOD::Jet*>& rljets, bool doSystShifts);
        std::vector<fastjet::PseudoJet> JetconVecToPseudojet(xAOD::JetConstituentVector input);
        std::unique_ptr<ShowerDeconstruction> tagger_SDw_win20_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDt_win50_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDt_win50_btag1;

        // HEPTopTagger
        int m_nHttTools;
        void runHTTAndFillTree(void);
        std::string m_httJetContainerPrefix;
        std::vector<std::string> m_httConfigs;
        std::unique_ptr<JetRecTool> m_httTool;

        int m_NPV;

        /*************************/
        /* LARGE-R JET VARIABLES */
        /*************************/

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

        std::vector<float> m_rljet_pdgid;

        // lead vs. sub-lead jet variables
        float m_rljet_mjj;
        float m_rljet_ptasym;
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

        // 2015 smooth top
        std::vector<int> m_rljet_smooth15Top_MassTau32Tag50eff;
        std::vector<int> m_rljet_smooth15Top_MassTau32Tag80eff;

        // 2015 smooth W
        std::vector<int> m_rljet_smooth15W_Tag50eff;
        std::vector<int> m_rljet_smooth15W_Tag25eff;

        // 2015 smooth Z
        std::vector<int> m_rljet_smooth15Z_Tag50eff;
        std::vector<int> m_rljet_smooth15Z_Tag25eff;

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

        // 2017 MVA
        std::vector<float> m_rljet_BDT_score_top_qqb;
        std::vector<float> m_rljet_BDT_score_top_inclusive;
        std::vector<float> m_rljet_BDT_score_w;
        std::vector<float> m_rljet_DNN_score_top;
        std::vector<float> m_rljet_DNN_score_w;

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

        ClassDef(top::DataMCbackgroundEventSaver, 0);
    };
}

#endif
