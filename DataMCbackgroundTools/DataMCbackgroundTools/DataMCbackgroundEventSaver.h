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

#include "DataMCbackgroundTools/TruthMatchTool.h"

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
        unsigned m_num_fatjets_keep;
        unsigned m_debug_level;

        /*********/
        /* TOOLS */
        /*********/

        std::unique_ptr<TruthMatchTool> m_truth_match_tool;

        // CA2 Subjet calibration tool
        JetCalibrationTool* m_jetcalib_subjet;

        /***********/
        /* METHODS */
        /***********/

        // grabs the btag weights for the leading two track jets
        // of the ungroomed parent large-R calorimeter reco jet
        void get_trackjet_btags(const xAOD::Jet* jet, double& jet_btag0, double& jet_btag1);

        // grabs the ungroomed parent large-R calorimeter reco jet
        const xAOD::Jet* get_parent_ungroomed(const xAOD::Jet* jet);

        // set all the relevant variables to sane default values
        void reset_containers(const bool on_nominal_branch);

        void initializeSD(void);
        void runSDandFillTree(void);
        void runHTTAndFillTree(void);

        /***********/
        /* TAGGERS */
        /***********/

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

        // W
        std::unique_ptr<SmoothedWZTagger> wTagger16_50eff;
        std::unique_ptr<SmoothedWZTagger> wTagger16_80eff;

        // Z
        std::unique_ptr<SmoothedWZTagger> zTagger16_50eff;
        std::unique_ptr<SmoothedWZTagger> zTagger16_80eff;

        /*****************/
        /* Other taggers */
        /*****************/

        // BDT
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_qqb;
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_inclusive;

        // DNN
        std::unique_ptr<JSSWTopTaggerDNN> m_topTagger_DNN_qqb;
        std::unique_ptr<JSSWTopTaggerDNN> m_topTagger_DNN_inclusive;

        // Shower Deconstruction (SD)
        std::unique_ptr<ShowerDeconstruction> tagger_SDw_win20_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDz_win20_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDt_win50_btag0;

        // HEPTopTagger
        int m_nHttTools;
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

        // lead vs. sub-lead jet variables
        float m_rljet_mjj;
        float m_rljet_ptasym;
        float m_rljet_dy; // rapidity
        float m_rljet_dR;
        float m_rljet_dphi;
        float m_rljet_deta; // pseudorapidity

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

        // 2016 smooth W
        std::vector<int> m_rljet_smooth16W_Tag50eff;
        std::vector<int> m_rljet_smooth16W_Tag80eff;

        // 2016 smooth Z
        std::vector<int> m_rljet_smooth16Z_Tag50eff;
        std::vector<int> m_rljet_smooth16Z_Tag80eff;

        // Shower Deconstruction tags
        std::vector<double> m_rljet_SDw_win20_btag0;
        std::vector<double> m_rljet_SDz_win20_btag0;
        std::vector<double> m_rljet_SDt_win50_btag0;

        // Shower Deconstruction systematics
        std::vector<double> m_rljet_SDw_win20_btag0_UP;
        std::vector<double> m_rljet_SDz_win20_btag0_UP;
        std::vector<double> m_rljet_SDt_win50_btag0_UP;

        std::vector<double> m_rljet_SDw_win20_btag0_DOWN;
        std::vector<double> m_rljet_SDz_win20_btag0_DOWN;
        std::vector<double> m_rljet_SDt_win50_btag0_DOWN;

        // b-tagging
        // std::vector<bool> m_rljet_btag_double;
        // std::vector<bool> m_rljet_btag_single;
        // std::vector<bool> m_rljet_btag_leading_pt;

        ClassDef(top::DataMCbackgroundEventSaver, 0);
    };
}

#endif
