#ifndef DataMCbackgroundEventSaver_H_
#define DataMCbackgroundEventSaver_H_

#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopAnalysis/EventSaverBase.h"

#include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"
#include "ShowerDeconstruction/ShowerDeconstruction.h"

#include "JetRec/JetRecTool.h"
#include "JetCalibTools/JetCalibrationTool.h"
#include "JetSubStructureUtils/BosonTag.h"
#include "JetSubStructureUtils/JSSWTopTaggerBDT.h"

#include "DataMCbackgroundTools/TruthMatchTool.h"

#include <vector>
#include <utility>

namespace top {
	class DataMCbackgroundEventSaver : public top::EventSaverFlatNtuple {

	public:
		// Default - so root can load based on a name
		DataMCbackgroundEventSaver();

		// Default - so we can clean up
		~DataMCbackgroundEventSaver();

		// Run once at the start of the job
		void initialize(std::shared_ptr<top::TopConfig> config, TFile* file,
                const std::vector<std::string>& extraBranches);

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
        bool m_saveTAmass;
        unsigned m_num_fatjets_keep;
        unsigned m_debug_level;

        /*********/
        /* TOOLS */
        /*********/

        std::unique_ptr<TruthMatchTool> m_truth_match_tool;
        std::unique_ptr<JSSWTopTaggerBDT> m_bdt_tool;

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

		// simple/smoothed top taggers
		SubstructureTopTagger *tagger_smoothMassTau32_50eff;
		SubstructureTopTagger *tagger_smoothMassTau32_80eff;
		SubstructureTopTagger *tagger_smoothMass_50eff;
		SubstructureTopTagger *tagger_smoothMass_80eff;
		SubstructureTopTagger *tagger_smoothTau32_50eff;
		SubstructureTopTagger *tagger_smoothTau32_80eff;

		// simple/smoothed W taggers
        std::unique_ptr<JetSubStructureUtils::BosonTag> wTagger_smooth_50eff;
		std::unique_ptr<JetSubStructureUtils::BosonTag> wTagger_smooth_25eff;

		// simple/smoothed Z taggers
		std::unique_ptr<JetSubStructureUtils::BosonTag> zTagger_smooth_50eff;
		std::unique_ptr<JetSubStructureUtils::BosonTag> zTagger_smooth_25eff;

        // Shower Deconstruction (SD)
        std::unique_ptr<ShowerDeconstruction> tagger_SDw_win20_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDz_win20_btag0;
        std::unique_ptr<ShowerDeconstruction> tagger_SDt_win50_btag0;

		// CA2 Subjet calibration tool
        std::unique_ptr<JetCalibrationTool> m_jet_calib_tool;

        // HEPTopTagger
		int m_nHttTools;
		std::string m_httJetContainerPrefix;
		std::vector<std::string> m_httConfigs;
        std::unique_ptr<JetRecTool> m_httTool;

        /*************************/
        /* LARGE-R JET VARIABLES */
        /*************************/

		// reco large leading jets
        int m_rljet_count;
        std::vector<float> m_rljet_pt;
		std::vector<float> m_rljet_eta;
		std::vector<float> m_rljet_phi;
        std::vector<float> m_rljet_m;

        // dijet mass = mass(leading + subleading)
        float m_rljet_mjj;

        // pt asymmetry (pt1 - pt2) / (pt1 + pt2)
        float m_rljet_ptasym;

        // track assisted masses
        std::vector<float> m_rljet_m_ta;
        std::vector<float> m_rljet_m_ta_nocalib;

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

		// matched truth jets
        int m_tljet_count;
		std::vector<float> m_tljet_pt;
		std::vector<float> m_tljet_eta;
		std::vector<float> m_tljet_phi;
        std::vector<float> m_tljet_m;
        std::vector<float> m_tljet_dR;

        // truth parton matching information
        std::vector<int> m_rljet_dRmatched_reco_truth;
        std::vector<int> m_rljet_dRmatched_particle_flavor;
        std::vector<int> m_rljet_dRmatched_maxEParton_flavor;
        std::vector<int> m_rljet_dRmatched_topBChild;
        std::vector<int> m_rljet_dRmatched_nQuarkChildren;

        // matched trigger jets
        int m_hltjet_count;
        std::vector<float> m_hltjet_pt;
        std::vector<float> m_hltjet_eta;
        std::vector<float> m_hltjet_phi;
        std::vector<float> m_hltjet_m;
        std::vector<float> m_hltjet_dR;

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
        std::vector<std::vector<float>> m_htt_m23m123;
        std::vector<std::vector<float>> m_htt_atan1312;
        std::vector<std::vector<int>> m_htt_nTagCands;
        std::vector<std::vector<float>> m_htt_pts1;
        std::vector<std::vector<float>> m_htt_pts2;
        std::vector<std::vector<float>> m_htt_pts3;

        /********************/
		/* TAGGER VARIABLES */
        /********************/

		// prerec top tags (reco)
        std::vector<bool> m_rljet_smoothMassTag50eff;
        std::vector<bool> m_rljet_smoothMassTag80eff;
		std::vector<bool> m_rljet_smoothTau32Tag50eff;
		std::vector<bool> m_rljet_smoothTau32Tag80eff;
		std::vector<bool> m_rljet_smoothMassTau32Tag50eff;
		std::vector<bool> m_rljet_smoothMassTau32Tag80eff;

		// prerec W tags (reco)
        std::vector<int> m_rljet_smoothWTag50eff;
		std::vector<int> m_rljet_smoothWTag25eff;

        // prerec Z tags (reco)
        std::vector<int> m_rljet_smoothZTag50eff;
		std::vector<int> m_rljet_smoothZTag25eff;

        // BDT
        std::vector<float> m_rljet_BDT_Top_Score;
        std::vector<float> m_rljet_BDT_W_Score;

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
        std::vector<bool> m_rljet_btag_double;
        std::vector<bool> m_rljet_btag_single;
        std::vector<bool> m_rljet_btag_leading_pt;

		ClassDef(top::DataMCbackgroundEventSaver, 0);
	};
}

#endif
