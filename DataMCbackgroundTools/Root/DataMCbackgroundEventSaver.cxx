#include "DataMCbackgroundTools/DataMCbackgroundEventSaver.h"
#include "DataMCbackgroundTools/LocalTools.h"

#include <xAODJet/JetAuxContainer.h>

#include "AthContainers/AuxTypeRegistry.h"
#include "BoostedJetTaggers/configHTT_Run2perf.h"

#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/Event.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopFakes/TopFakesxAODUtils.h"
#include "TopParticleLevel/ParticleLevelEvent.h"

#include "TFile.h"
#include "TLorentzVector.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace top {

DataMCbackgroundEventSaver::DataMCbackgroundEventSaver(void)
{
	top::ConfigurationSettings* config_settings = top::ConfigurationSettings::get();

    /******************************/
    /* CUSTOM CUTS FILE VARIABLES */
    /******************************/

    // read in HTT configurations from cuts file
    try {
        std::string htt_string = config_settings->value("HTTConfigs");
        std::stringstream htt_stream(htt_string);
        std::string substr;

        std::cout << "HTT Configurations Found: " << std::endl;

        while (htt_stream.good()) {
            getline(htt_stream, substr, ',');
            m_httConfigs.push_back(substr);
            std::cout << substr << std::endl;
        }

    } catch (...) {
        std::cout << "No HTT Configs found :( ..." << std::endl;
        m_httConfigs.clear();
    }

    m_nHttTools = m_httConfigs.size();
    std::cout << "Number of HTT Configs: " << m_nHttTools << std::endl;


    // protection against missing HTTJetContainerPrefix variable. Default: CA15LCTopoJets_HTT
    try {
        m_httJetContainerPrefix = config_settings->value("HTTJetContainerPrefix");
    } catch (...) {
        m_httJetContainerPrefix = "CA15LCTopoJets_HTT";
    }

    // configuration switch for HTT
	m_runHTT = false;
	try {
		std::string HTTswitchString = config_settings->value("RunHTT");
		if(HTTswitchString.compare("True") == 0)
			m_runHTT = true;
	} catch (...) {
		std::cout << "Not running HTT because option not set..." << std::endl;
		std::cout << "add 'RunHTT True' to cuts file to enable." << std::endl;
	}

    if (m_nHttTools != 0 && m_runHTT) {
        m_httTool = std::unique_ptr<JetRecTool>(buildFullHTTagger("",
                    m_httJetContainerPrefix, m_httConfigs));
    }

    // configuration switch for shower deconstruction tagging
	m_runSD = false;
	try {
		std::string SDswitchString = config_settings->value("RunShowerDeconstruction");
		if(SDswitchString.compare("True") == 0)
			m_runSD = true;
	} catch (...) {
		std::cout << "Not running Shower Deconstruction because option not set..." << std::endl;
		std::cout << "add 'RunShowerDeconstruction True' to cuts file to enable." << std::endl;
	}

    // configuration switch for track-assisted mass
    m_saveTAmass = false;
    try {
        std::string TAmassSwitchString = config_settings->value("SaveTrackAssistedMass");
        if(TAmassSwitchString.compare("True") == 0)
            m_saveTAmass = true;
    } catch (...) {
        std::cout << "WARNING: Not saving track-assisted mass (option not set)" << std::endl;
		std::cout << "add 'SaveTrackAssistedMass True' to cuts file to enable." << std::endl;
    }

    m_num_fatjets_keep = 1;
    try {
        std::string fatjets_keep_str = config_settings->value("NumFatjetsKeep");
        int fatjets_keep_desired = std::stoi(fatjets_keep_str);
        if (fatjets_keep_desired > 3 || fatjets_keep_desired < 1) {
            std::cout << "WARNING: invalid NumFatjetsKeep value: " << fatjets_keep_desired << std::endl;
            throw;
        } else {
            m_num_fatjets_keep = (unsigned) fatjets_keep_desired;
            std::cout << "DataMCbackgroundEventSaver: Keeping " << m_num_fatjets_keep
                << " Large-R jets." << std::endl;
        }
    } catch (...) {
        std::cout << "Defaulting to saving only the leading pT large R jet" << std::endl;
		std::cout << "add 'NumFatjetsKeep X', where X = 1,2,3,..., to cuts file to enable." << std::endl;
    }

    m_debug_level = 0;
    try {
        std::string debug_level_str = config_settings->value("DebugLevel");
        int debug_level_desired = std::stoi(debug_level_str);
        if (debug_level_desired < 0) {
            std::cout << "WARNING: invalid DebugLevel value: " << debug_level_desired << std::endl;
            throw;
        } else {
            m_debug_level = (unsigned) debug_level_desired;
            std::cout << "DataMCbackgroundEventSaver: DebugLevel set to " << m_debug_level << std::endl;
        }
    } catch (...) {
        std::cout << "DataMCbackgroundEventSaver: Defaulting to DebugLevel 0" << std::endl;
    }
}

DataMCbackgroundEventSaver::~DataMCbackgroundEventSaver(void) {}

void DataMCbackgroundEventSaver::initialize(std::shared_ptr<top::TopConfig> config, TFile* file,const std::vector<std::string>& extraBranches)
{
	EventSaverFlatNtuple::initialize(config, file, extraBranches);

	m_config = config;

    for(auto systematicTree : treeManagers()) {

        /**************************************************/
        /* PARAMETERS SAVED FOR NOMINAL + ALL SYSTEMATICS */
        /**************************************************/

        systematicTree->makeOutputVariable(m_rljet_pt        , "rljet_pt");
        systematicTree->makeOutputVariable(m_rljet_eta       , "rljet_eta");
        systematicTree->makeOutputVariable(m_rljet_phi       , "rljet_phi");
        systematicTree->makeOutputVariable(m_rljet_m         , "rljet_m");

        systematicTree->makeOutputVariable(m_rljet_D2        , "rljet_D2");
        systematicTree->makeOutputVariable(m_rljet_Tau32_wta , "rljet_Tau32_wta");

        // top tagger output variables
        systematicTree->makeOutputVariable(m_rljet_smoothMassTag50eff      , "rljet_smoothMassTag50eff");
        systematicTree->makeOutputVariable(m_rljet_smoothMassTag80eff      , "rljet_smoothMassTag80eff");
        systematicTree->makeOutputVariable(m_rljet_smoothTau32Tag50eff     , "rljet_smoothTau32Tag50eff");
        systematicTree->makeOutputVariable(m_rljet_smoothTau32Tag80eff     , "rljet_smoothTau32Tag80eff");
        systematicTree->makeOutputVariable(m_rljet_smoothMassTau32Tag50eff , "rljet_smoothMassTau32Tag50eff");
        systematicTree->makeOutputVariable(m_rljet_smoothMassTau32Tag80eff , "rljet_smoothMassTau32Tag80eff");

        // W tagger output variables
        systematicTree->makeOutputVariable(m_rljet_smoothWTag50eff , "rljet_smoothWTag50eff");
        systematicTree->makeOutputVariable(m_rljet_smoothWTag25eff , "rljet_smoothWTag25eff");

        // Z tagger output variables
        systematicTree->makeOutputVariable(m_rljet_smoothZTag50eff , "rljet_smoothZTag50eff");
        systematicTree->makeOutputVariable(m_rljet_smoothZTag25eff , "rljet_smoothZTag25eff");


        if (m_config->isMC()) {
            systematicTree->makeOutputVariable(m_rljet_dRmatched_reco_truth, "rljet_dRmatched_reco_truth");
            systematicTree->makeOutputVariable(m_rljet_dRmatched_particle_flavor, "rljet_dRmatched_particle_flavor");
            systematicTree->makeOutputVariable(m_rljet_dRmatched_maxEParton_flavor, "rljet_dRmatched_maxEParton_flavor");
            systematicTree->makeOutputVariable(m_rljet_dRmatched_topBChild, "rljet_dRmatched_topBChild");
            systematicTree->makeOutputVariable(m_rljet_dRmatched_nQuarkChildren, "rljet_dRmatched_nQuarkChildren");
        }

        /*************************************/
        /* PARAMETERS SAVED ONLY FOR NOMINAL */
        /*************************************/

        if (systematicTree->name() == "nominal") {
            // more large-R jet kinematic variables
            systematicTree->makeOutputVariable(m_rljet_count        , "rljet_count");
            systematicTree->makeOutputVariable(m_rljet_mjj          , "rljet_mjj");
            systematicTree->makeOutputVariable(m_rljet_ptasym       , "rljet_ptasym");
            systematicTree->makeOutputVariable(m_rljet_dy           , "rljet_dy");
            systematicTree->makeOutputVariable(m_rljet_dR           , "rljet_dR");
            systematicTree->makeOutputVariable(m_rljet_m_ta         , "rljet_m_ta");
            systematicTree->makeOutputVariable(m_rljet_m_ta_nocalib , "rljet_m_ta_nocalib");

            // more substructure variables
            systematicTree->makeOutputVariable(m_rljet_Tau1_wta    , "rljet_Tau1_wta");
            systematicTree->makeOutputVariable(m_rljet_Tau2_wta    , "rljet_Tau2_wta");
            systematicTree->makeOutputVariable(m_rljet_Tau3_wta    , "rljet_Tau3_wta");
            systematicTree->makeOutputVariable(m_rljet_ECF1        , "rljet_ECF1");
            systematicTree->makeOutputVariable(m_rljet_ECF2        , "rljet_ECF2");
            systematicTree->makeOutputVariable(m_rljet_ECF3        , "rljet_ECF3");
            systematicTree->makeOutputVariable(m_rljet_FoxWolfram0 , "rljet_FoxWolfram0");
            systematicTree->makeOutputVariable(m_rljet_FoxWolfram2 , "rljet_FoxWolfram2");
            systematicTree->makeOutputVariable(m_rljet_Qw          , "rljet_Qw");
            systematicTree->makeOutputVariable(m_rljet_Angularity  , "rljet_Angularity");
            systematicTree->makeOutputVariable(m_rljet_Aplanarity  , "rljet_Aplanarity");
            systematicTree->makeOutputVariable(m_rljet_Dip12       , "rljet_Dip12");
            systematicTree->makeOutputVariable(m_rljet_KtDR        , "rljet_KtDR");
            systematicTree->makeOutputVariable(m_rljet_Mu12        , "rljet_Mu12");
            systematicTree->makeOutputVariable(m_rljet_PlanarFlow  , "rljet_PlanarFlow");
            systematicTree->makeOutputVariable(m_rljet_Sphericity  , "rljet_Sphericity");
            systematicTree->makeOutputVariable(m_rljet_Split12     , "rljet_Split12");
            systematicTree->makeOutputVariable(m_rljet_Split23     , "rljet_Split23");
            systematicTree->makeOutputVariable(m_rljet_Split34     , "rljet_Split34");
            systematicTree->makeOutputVariable(m_rljet_ThrustMaj   , "rljet_ThrustMaj");
            systematicTree->makeOutputVariable(m_rljet_ThrustMin   , "rljet_ThrustMin");
            systematicTree->makeOutputVariable(m_rljet_ZCut12      , "rljet_ZCut12");

            systematicTree->makeOutputVariable(m_rljet_NTrimSubjets      , "rljet_NTrimSubjets");
            systematicTree->makeOutputVariable(m_rljet_ungroomed_ntrk500 , "rljet_ungroomed_ntrk500");

            if (m_config->isMC()) {
                //truth large jet
                systematicTree->makeOutputVariable(m_tljet_count , "tljet_count");
                systematicTree->makeOutputVariable(m_tljet_pt    , "tljet_pt");
                systematicTree->makeOutputVariable(m_tljet_eta   , "tljet_eta");
                systematicTree->makeOutputVariable(m_tljet_phi   , "tljet_phi");
                systematicTree->makeOutputVariable(m_tljet_m     , "tljet_m");
                systematicTree->makeOutputVariable(m_tljet_dR    , "tljet_dR");

                systematicTree->makeOutputVariable(m_tljet_D2        , "tljet_D2");
                systematicTree->makeOutputVariable(m_tljet_Tau32_wta , "tljet_Tau32_wta");
            }

            //trigger jet
            systematicTree->makeOutputVariable(m_hltjet_count , "hltjet_count");
            systematicTree->makeOutputVariable(m_hltjet_m     , "hltjet_m");
            systematicTree->makeOutputVariable(m_hltjet_pt    , "hltjet_pt");
            systematicTree->makeOutputVariable(m_hltjet_eta   , "hltjet_eta");
            systematicTree->makeOutputVariable(m_hltjet_phi   , "hltjet_phi");
            systematicTree->makeOutputVariable(m_hltjet_dR    , "hltjet_dR");

            // have to resize in order to makeOutputVariable below
            m_htt_tag       . resize(m_nHttTools);
            m_htt_pt        . resize(m_nHttTools);
            m_htt_eta       . resize(m_nHttTools);
            m_htt_phi       . resize(m_nHttTools);
            m_htt_m         . resize(m_nHttTools);
            m_htt_m23m123   . resize(m_nHttTools);
            m_htt_atan1312  . resize(m_nHttTools);
            m_htt_nTagCands . resize(m_nHttTools);
            m_htt_pts1      . resize(m_nHttTools);
            m_htt_pts2      . resize(m_nHttTools);
            m_htt_pts3      . resize(m_nHttTools);

            for (int i=0; i<m_nHttTools; i++) {
                systematicTree->makeOutputVariable(m_htt_pt[i]        , getHTTBranchName("pt"        , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_eta[i]       , getHTTBranchName("eta"       , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_phi[i]       , getHTTBranchName("phi"       , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_m[i]         , getHTTBranchName("m"         , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_m23m123[i]   , getHTTBranchName("m23m123"   , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_atan1312[i]  , getHTTBranchName("atan1312"  , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_nTagCands[i] , getHTTBranchName("nTagCands" , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_tag[i]       , getHTTBranchName("tag"       , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_pts1[i]       , getHTTBranchName("pts1"       , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_pts2[i]       , getHTTBranchName("pts2"       , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_pts3[i]       , getHTTBranchName("pts3"       , m_httConfigs[i]));
            }

            systematicTree->makeOutputVariable(m_htt_caJet_pt  , getHTTBranchName("caJet_pt"  , ""));
            systematicTree->makeOutputVariable(m_htt_caJet_eta , getHTTBranchName("caJet_eta" , ""));
            systematicTree->makeOutputVariable(m_htt_caJet_phi , getHTTBranchName("caJet_phi" , ""));
            systematicTree->makeOutputVariable(m_htt_caJet_m   , getHTTBranchName("caJet_m"   , ""));

            systematicTree->makeOutputVariable(m_cajet_count, "caJet_count");

            m_htt_caGroomJet_pt  . resize(m_nHttTools);
            m_htt_caGroomJet_eta . resize(m_nHttTools);
            m_htt_caGroomJet_phi . resize(m_nHttTools);
            m_htt_caGroomJet_m   . resize(m_nHttTools);

            for (int i=0; i<m_nHttTools; i++) {
                systematicTree->makeOutputVariable(m_htt_caGroomJet_pt[i]  , getHTTBranchName("caGroomJet_pt"  , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_caGroomJet_eta[i] , getHTTBranchName("caGroomJet_eta" , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_caGroomJet_phi[i] , getHTTBranchName("caGroomJet_phi" , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_caGroomJet_m[i]   , getHTTBranchName("caGroomJet_m"   , m_httConfigs[i]));
            }

            // Shower Deconstruction tagger output variables
            systematicTree->makeOutputVariable(m_rljet_SDw_win20_btag0, "rljet_SDw_win20_btag0");
            systematicTree->makeOutputVariable(m_rljet_SDz_win20_btag0, "rljet_SDz_win20_btag0");
            systematicTree->makeOutputVariable(m_rljet_SDt_win50_btag0, "rljet_SDt_win50_btag0");

            systematicTree->makeOutputVariable(m_rljet_SDw_win20_btag0_UP, "rljet_SDw_win20_btag0_UP");
            systematicTree->makeOutputVariable(m_rljet_SDz_win20_btag0_UP, "rljet_SDz_win20_btag0_UP");
            systematicTree->makeOutputVariable(m_rljet_SDt_win50_btag0_UP, "rljet_SDt_win50_btag0_UP");

            systematicTree->makeOutputVariable(m_rljet_SDw_win20_btag0_DOWN, "rljet_SDw_win20_btag0_DOWN");
            systematicTree->makeOutputVariable(m_rljet_SDz_win20_btag0_DOWN, "rljet_SDz_win20_btag0_DOWN");
            systematicTree->makeOutputVariable(m_rljet_SDt_win50_btag0_DOWN, "rljet_SDt_win50_btag0_DOWN");

            // b-tagging output variables
            systematicTree->makeOutputVariable(m_rljet_btag_double     , "rljet_btag_double");
            systematicTree->makeOutputVariable(m_rljet_btag_single     , "rljet_btag_single");
            systematicTree->makeOutputVariable(m_rljet_btag_leading_pt , "rljet_btag_leading_pt");

            // BDT tagger output variables
            systematicTree->makeOutputVariable(m_rljet_BDT_Top_Score , "rljet_BDT_Top_Score");
            systematicTree->makeOutputVariable(m_rljet_BDT_W_Score   , "rljet_BDT_W_Score");
        } // end making nominal branch only output variables
    } // end making all output variables

    /************************/
	/* SUBSTRUCTURE TAGGERS */
    /************************/

	if (m_runSD) this->initializeSD();

	// smooth top taggers
	tagger_smoothMassTau32_50eff = STTHelpers::configSubstTagger("SmoothCut50eff", "SmoothCut_50");
	tagger_smoothMassTau32_80eff = STTHelpers::configSubstTagger("SmoothCut80eff", "SmoothCut_80");
	tagger_smoothMass_50eff      = STTHelpers::configSubstTagger("SmoothMassCutOnly50eff", std::vector<std::string>{"SmoothMassCut_50"});
	tagger_smoothMass_80eff      = STTHelpers::configSubstTagger("SmoothMassCutOnly80eff", std::vector<std::string>{"SmoothMassCut_80"});
	tagger_smoothTau32_50eff     = STTHelpers::configSubstTagger("SmoothTau32CutOnly50eff", std::vector<std::string>{"SmoothTau32Cut_50"});
	tagger_smoothTau32_80eff     = STTHelpers::configSubstTagger("SmoothTau32CutOnly80eff", std::vector<std::string>{"SmoothTau32Cut_80"});

	// smooth W taggers
	wTagger_smooth_50eff = make_unique<JetSubStructureUtils::BosonTag>("medium", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat", false, false);
	wTagger_smooth_25eff = make_unique<JetSubStructureUtils::BosonTag>("tight", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat", false, false);

    // smooth Z taggers
	zTagger_smooth_50eff = make_unique<JetSubStructureUtils::BosonTag>("medium", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Ztagging_MC15_Prerecommendations_20150809.dat", false, false);
	zTagger_smooth_25eff = make_unique<JetSubStructureUtils::BosonTag>("tight", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Ztagging_MC15_Prerecommendations_20150809.dat", false, false);

    m_bdt_tool = std::unique_ptr<JSSWTopTaggerBDT>( new JSSWTopTaggerBDT(true,true,"NvarM") );
    m_bdt_tool->initialize();

    m_truth_match_tool = make_unique<TruthMatchTool>();
}

void
DataMCbackgroundEventSaver::initializeSD(void)
{
    tagger_SDw_win20_btag0 = make_unique<ShowerDeconstruction>("SD_W_win20_Btag0");
    top::check (tagger_SDw_win20_btag0->setProperty("mode", "w/q"), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("R", 1.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("topMass", 170), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("topMassWindow", 40.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("wMass", 80.403), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("wMassWindow", 20.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("hMass", 125.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("hMassWindow", 20.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("maxNjets", 7), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->setProperty("useBtag", 0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE");
    top::check (tagger_SDw_win20_btag0->initialize(), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );

    tagger_SDz_win20_btag0 = make_unique<ShowerDeconstruction>("SD_Z_win20_Btag0");
    top::check (tagger_SDz_win20_btag0->setProperty("mode", "w/q"), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("R", 1.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("topMass", 170), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("topMassWindow", 40.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("wMass", 90.2), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("wMassWindow", 20.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("hMass", 125.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("hMassWindow", 20.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("maxNjets", 7), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->setProperty("useBtag", 0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDz_win20_btag0->initialize(), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );

    tagger_SDt_win50_btag0 = make_unique<ShowerDeconstruction>("SD_T_win50_Btag0");
    top::check (tagger_SDt_win50_btag0->setProperty("mode", "t/g"), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("R", 1.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("topMass", 168), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("topMassWindow", 50.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("wMass", 80.403), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("wMassWindow", 25.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("hMass", 125.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("hMassWindow", 20.0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("maxNjets", 7), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->setProperty("useBtag", 0), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );
    top::check (tagger_SDt_win50_btag0->initialize(), "SHOWER DECONSTRUCTION INITIALIZATION FAILURE" );

    /*********************************************/
    /* INITIALIZE SUBJET CALIBRATION TOOL FOR SD */
    /*********************************************/

    //string describing the current thread, for logging
    const std::string name_thread = "DataMCbackgroundEventSaver";

    //String describing your jet collection, for example AntiKt4EMTopo or AntiKt4LCTopo (see below)
    TString jetAlgo_subjet = "CamKt020LCTopo";

    //Path to global config used to initialize the tool (see below)
    TString config_subjetcalib = "CamKt2LCTopoAreaCalib_JES_HTTmodified.config";

    // may need to change this?
    TString calibSeq_subjet = "EtaJES_DEV"; //String describing the calibration sequence to apply (see below)
    // Old calibration
    //  TString config_subjetcalib = "CamKt_JES_HTT.config"; //Path to global config used to initialize the tool (see below)
    //  TString calibSeq_subjet = "Origin_EtaJES" ; //String describing the calibration sequence to apply (see below)

    bool is_data = !m_config->isMC();

    //The default constructor can also be used if the arguments are set with python configuration instead
    m_jet_calib_tool = make_unique<JetCalibrationTool>(name_thread, jetAlgo_subjet,
            config_subjetcalib, calibSeq_subjet, is_data);

    //Initialize the tool
    top::check ( m_jet_calib_tool->initializeTool(name_thread), "FAILURE");
}

void
DataMCbackgroundEventSaver::reset_containers(const bool on_nominal_branch)
{
	m_rljet_pt  . assign(m_num_fatjets_keep, -1000.);
	m_rljet_eta . assign(m_num_fatjets_keep, -1000.);
	m_rljet_phi . assign(m_num_fatjets_keep, -1000.);
	m_rljet_m   . assign(m_num_fatjets_keep, -1000.);

	m_rljet_D2        . assign(m_num_fatjets_keep, -1000. );
	m_rljet_Tau32_wta . assign(m_num_fatjets_keep, -1000. );

	m_rljet_smoothMassTag50eff      . assign(m_num_fatjets_keep, false);
	m_rljet_smoothMassTag80eff      . assign(m_num_fatjets_keep, false);
	m_rljet_smoothTau32Tag50eff     . assign(m_num_fatjets_keep, false);
	m_rljet_smoothTau32Tag80eff     . assign(m_num_fatjets_keep, false);
	m_rljet_smoothMassTau32Tag50eff . assign(m_num_fatjets_keep, false);
	m_rljet_smoothMassTau32Tag80eff . assign(m_num_fatjets_keep, false);

	m_rljet_smoothWTag50eff . assign(m_num_fatjets_keep, 0);
	m_rljet_smoothWTag25eff . assign(m_num_fatjets_keep, 0);

    m_rljet_smoothZTag50eff . assign(m_num_fatjets_keep, 0);
    m_rljet_smoothZTag25eff . assign(m_num_fatjets_keep, 0);

    m_rljet_dRmatched_reco_truth        . assign(m_num_fatjets_keep, -1000);
    m_rljet_dRmatched_particle_flavor   . assign(m_num_fatjets_keep, -1000);
    m_rljet_dRmatched_maxEParton_flavor . assign(m_num_fatjets_keep, -1000);
    m_rljet_dRmatched_topBChild         . assign(m_num_fatjets_keep, -1000);
    m_rljet_dRmatched_nQuarkChildren    . assign(m_num_fatjets_keep, -1000);

    if (on_nominal_branch) {
        m_rljet_count  = 0;
        m_rljet_mjj    = -1000.;
        m_rljet_ptasym = -1000.;
        m_rljet_dy     = -1000.;
        m_rljet_dR     = -1000.;

        if (m_saveTAmass) {
            m_rljet_m_ta         . assign(m_num_fatjets_keep, -1000.);
            m_rljet_m_ta_nocalib . assign(m_num_fatjets_keep, -1000.);
        }

        m_rljet_Tau1_wta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Tau2_wta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Tau3_wta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ECF1        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ECF2        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ECF3        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_FoxWolfram0 . assign(m_num_fatjets_keep, -1000. );
        m_rljet_FoxWolfram2 . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Qw          . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Angularity  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Aplanarity  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Dip12       . assign(m_num_fatjets_keep, -1000. );
        m_rljet_KtDR        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Mu12        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_PlanarFlow  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Sphericity  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Split12     . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Split23     . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Split34     . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ThrustMaj   . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ThrustMin   . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ZCut12      . assign(m_num_fatjets_keep, -1000. );

        m_rljet_NTrimSubjets          . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ungroomed_ntrk500     . assign(m_num_fatjets_keep, -1000. );

        if (m_config->isMC()) {
            m_tljet_pt  . assign(m_num_fatjets_keep, -1000.);
            m_tljet_eta . assign(m_num_fatjets_keep, -1000.);
            m_tljet_phi . assign(m_num_fatjets_keep, -1000.);
            m_tljet_m   . assign(m_num_fatjets_keep, -1000.);
            m_tljet_dR  . assign(m_num_fatjets_keep, -1000.);

            m_tljet_Tau32_wta . assign(m_num_fatjets_keep, -1000. );
            m_tljet_D2        . assign(m_num_fatjets_keep, -1000. );
        }

        m_rljet_btag_double     . assign(m_num_fatjets_keep, false);
        m_rljet_btag_single     . assign(m_num_fatjets_keep, false);
        m_rljet_btag_leading_pt . assign(m_num_fatjets_keep, false);

        m_rljet_BDT_Top_Score . assign(m_num_fatjets_keep, -1000. );
        m_rljet_BDT_W_Score   . assign(m_num_fatjets_keep, -1000. );

        m_hltjet_pt  . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_eta . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_phi . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_m   . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_dR  . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_count = 0;

        if (m_runSD) {
            m_rljet_SDw_win20_btag0      . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDz_win20_btag0      . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_win50_btag0      . assign(m_num_fatjets_keep, -1000.);

            m_rljet_SDw_win20_btag0_UP   . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDz_win20_btag0_UP   . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_win50_btag0_UP   . assign(m_num_fatjets_keep, -1000.);

            m_rljet_SDw_win20_btag0_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDz_win20_btag0_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_win50_btag0_DOWN . assign(m_num_fatjets_keep, -1000.);
        }

        if (m_runHTT) {
            m_htt_caJet_pt  . assign(m_num_fatjets_keep, -1000.);
            m_htt_caJet_eta . assign(m_num_fatjets_keep, -1000.);
            m_htt_caJet_phi . assign(m_num_fatjets_keep, -1000.);
            m_htt_caJet_m   . assign(m_num_fatjets_keep, -1000.);
            m_cajet_count = 0;

            for (int i = 0; i < m_nHttTools; i++) {
                m_htt_tag[i]       . assign(m_num_fatjets_keep, 0);
                m_htt_pt[i]        . assign(m_num_fatjets_keep, -1000.);
                m_htt_eta[i]       . assign(m_num_fatjets_keep, -1000.);
                m_htt_phi[i]       . assign(m_num_fatjets_keep, -1000.);
                m_htt_m[i]         . assign(m_num_fatjets_keep, -1000.);
                m_htt_m23m123[i]   . assign(m_num_fatjets_keep, -1000.);
                m_htt_atan1312[i]  . assign(m_num_fatjets_keep, -1000.);
                m_htt_nTagCands[i] . assign(m_num_fatjets_keep, 0);
                m_htt_pts1[i]      . assign(m_num_fatjets_keep, -1000.);
                m_htt_pts2[i]      . assign(m_num_fatjets_keep, -1000.);
                m_htt_pts3[i]      . assign(m_num_fatjets_keep, -1000.);

                m_htt_caGroomJet_pt[i]  . assign(m_num_fatjets_keep, -1000.);
                m_htt_caGroomJet_eta[i] . assign(m_num_fatjets_keep, -1000.);
                m_htt_caGroomJet_phi[i] . assign(m_num_fatjets_keep, -1000.);
                m_htt_caGroomJet_m[i]   . assign(m_num_fatjets_keep, -1000.);
            }
        }
    }
}

void
DataMCbackgroundEventSaver::saveEvent(const top::Event& event)
{
    if (!event.m_saveEvent && m_config->saveOnlySelectedEvents()) {
        // if the event did not pass the given cuts, don't bother processing it
        return;
    }

    if (m_debug_level >= 3) {
        std::cout << std::endl <<
            "==============================================" << std::endl <<
            "RUN NUMBER: " << event.m_info->runNumber() << std::endl <<
            "EVENT NUMBER: " << event.m_info->eventNumber() << std::endl <<
            "==============================================" << std::endl;
    }

    const bool on_nominal_branch = event.m_hashValue == m_config->nominalHashValue();
    this->reset_containers(on_nominal_branch);

    const xAOD::JetContainer* reco_large_jets = nullptr;

    top::check(evtStore()->retrieve(reco_large_jets,
                m_config->sgKeyLargeRJets()), "FAILURE" );

    if(m_config->isMC()) {
        const xAOD::JetContainer* truth_large_jets = nullptr;
        const xAOD::TruthParticleContainer* truth_particles = nullptr;

        top::check( evtStore()->retrieve(truth_large_jets,
                    m_config->sgKeyTruthLargeRJets()), "FAILURE" );

        top::check( evtStore()->retrieve(truth_particles,
                    m_config->sgKeyMCParticle()), "FAILURE" );

        m_truth_match_tool->execute(
                reco_large_jets,
                truth_large_jets,
                truth_particles
                );
    }

    // make a pt-sorted copy of the large-R jet container
    const xAOD::JetContainer rljets = sort_container_pt(reco_large_jets);

    // const xAOD::PhotonContainer* photons = nullptr;
    // top::check( evtStore()->retrieve(photons, m_config->sgKeyPhotons()), "FAILURE" );
    // std::cout << "NUM PHOTONS: " << photons->size();

    for (unsigned i = 0; i < m_num_fatjets_keep && i < rljets.size(); i++) {

        if (m_debug_level >= 3) {
            if (i > 0) std::cout << std::endl;
            std::cout << "### LARGE-R JET NO. " << i << " ###" << std::endl;
        }

        /***********************/
        /* KINEMATIC VARIABLES */
        /***********************/

        m_rljet_pt[i]  = rljets[i]->pt();
        m_rljet_eta[i] = rljets[i]->eta();
        m_rljet_phi[i] = rljets[i]->phi();
        m_rljet_m[i]   = rljets[i]->m();

        /****************************/
        /* TRUTH-MATCHING VARIABLES */
        /****************************/

        if (m_config->isMC()) {
            if (rljets[i]->isAvailable<int>("dRmatched_reco_truth")) {
                m_rljet_dRmatched_reco_truth[i] = rljets[i]->auxdata<int>("dRmatched_reco_truth");
                if (m_debug_level >= 3)
                    std::cout << "dRmatched_reco_truth: " << m_rljet_dRmatched_reco_truth[i] << std::endl;
            }

            if (rljets[i]->isAvailable<int>("dRmatched_particle_flavor")) {
                m_rljet_dRmatched_particle_flavor[i] = rljets[i]->auxdata<int>("dRmatched_particle_flavor");
                if (m_debug_level >= 3)
                    std::cout << "dRmatched_particle_flavor: " << m_rljet_dRmatched_particle_flavor[i] << std::endl;
            }

            if (rljets[i]->isAvailable<int>("dRmatched_maxEParton_flavor")) {
                m_rljet_dRmatched_maxEParton_flavor[i] = rljets[i]->auxdata<int>("dRmatched_maxEParton_flavor");
                if (m_debug_level >= 3)
                    std::cout << "dRmatched_maxEParton_flavor: " << m_rljet_dRmatched_maxEParton_flavor[i] << std::endl;
            }

            if (rljets[i]->isAvailable<int>("dRmatched_topBChild")) {
                m_rljet_dRmatched_topBChild[i] = rljets[i]->auxdata<int>("dRmatched_topBChild");
                if (m_debug_level >= 3)
                    std::cout << "dRmatched_topBChild: " << m_rljet_dRmatched_topBChild[i] << std::endl;
            }

            if (rljets[i]->isAvailable<int>("dRmatched_nQuarkChildren")) {
                m_rljet_dRmatched_nQuarkChildren[i] = rljets[i]->auxdata<int>("dRmatched_nQuarkChildren");
                if (m_debug_level >= 3)
                    std::cout << "dRmatched_nQuarkChildren: " << m_rljet_dRmatched_nQuarkChildren[i] << std::endl;
            }
        } // end of saving truth-matching information

        /******/
        /* D2 */
        /******/

        const float ecf1 = rljets[i]->auxdata<float>("ECF1");
        const float ecf2 = rljets[i]->auxdata<float>("ECF2");
        const float ecf3 = rljets[i]->auxdata<float>("ECF3");

        if (rljets[i]->isAvailable<float>("D2")) {
            m_rljet_D2[i] = rljets[i]->auxdata<float>("D2");
        } else {
            m_rljet_D2[i] = fabs(ecf2) > 1.e-6 ? pow(ecf1/ecf2,3)*ecf3 : -999.;
        }

        /*********/
        /* TAU32 */
        /*********/

        const float tau1 = rljets[i]->auxdata<float>("Tau1_wta");
        const float tau2 = rljets[i]->auxdata<float>("Tau2_wta");
        const float tau3 = rljets[i]->auxdata<float>("Tau3_wta");

        if(rljets[i]->isAvailable<float>("Tau32_wta")) {
            m_rljet_Tau32_wta[i] = rljets[i]->auxdata<float>("Tau32_wta");
        } else {
            m_rljet_Tau32_wta[i] = fabs(tau2) > 1.e-6 ? tau3 / tau2 : -999.;
        }

        // top-tag info
        // simple booleans, true = tagged, false = not tagged
        m_rljet_smoothMassTag50eff[i]      = tagger_smoothMass_50eff->isTagged(*rljets[i]);
        m_rljet_smoothMassTag80eff[i]      = tagger_smoothMass_80eff->isTagged(*rljets[i]);
        m_rljet_smoothTau32Tag50eff[i]     = tagger_smoothTau32_50eff->isTagged(*rljets[i]);
        m_rljet_smoothTau32Tag80eff[i]     = tagger_smoothTau32_80eff->isTagged(*rljets[i]);
        m_rljet_smoothMassTau32Tag50eff[i] = tagger_smoothMassTau32_50eff->isTagged(*rljets[i]);
        m_rljet_smoothMassTau32Tag80eff[i] = tagger_smoothMassTau32_80eff->isTagged(*rljets[i]);

        // W-tag info
        // 0: failed both mass and substructure cut
        // 1: passed substructure cut *only*
        // 2: passed mass window criteria *only*
        // 3: passed mass window + substructure criteria
        m_rljet_smoothWTag50eff[i] = wTagger_smooth_50eff->result(*rljets[i]);
        m_rljet_smoothWTag25eff[i] = wTagger_smooth_25eff->result(*rljets[i]);

        // Z-tag info
        // 0: failed both mass and substructure cut
        // 1: passed substructure cut *only*
        // 2: passed mass window criteria *only*
        // 3: passed mass window + substructure criteria
        m_rljet_smoothZTag50eff[i] = zTagger_smooth_50eff->result(*rljets[i]);
        m_rljet_smoothZTag25eff[i] = zTagger_smooth_25eff->result(*rljets[i]);

        if (on_nominal_branch) {
            m_rljet_count    = rljets.size();

            if(m_saveTAmass) {
                if(rljets[i]->isAvailable<float>("JetTrackAssistedMassCalibrated"))
                    m_rljet_m_ta[i] = rljets[i]->auxdata<float>("JetTrackAssistedMassCalibrated");
                if(rljets[i]->isAvailable<float>("JetTrackAssistedMassUnCalibrated"))
                    m_rljet_m_ta_nocalib[i] = rljets[i]->auxdata<float>("JetTrackAssistedMassUnCalibrated");
            }

            // substructure variables
            m_rljet_Tau1_wta[i]     = tau1;
            m_rljet_Tau2_wta[i]     = tau2;
            m_rljet_Tau3_wta[i]     = tau3;
            m_rljet_ECF1[i]         = ecf1;
            m_rljet_ECF2[i]         = ecf2;
            m_rljet_ECF3[i]         = ecf3;
            m_rljet_FoxWolfram0[i]  = rljets[i]->auxdata<float>("FoxWolfram0");
            m_rljet_FoxWolfram2[i]  = rljets[i]->auxdata<float>("FoxWolfram2");
            m_rljet_Qw[i]           = rljets[i]->auxdata<float>("Qw");
            m_rljet_Angularity[i]   = rljets[i]->auxdata<float>("Angularity");
            m_rljet_Aplanarity[i]   = rljets[i]->auxdata<float>("Aplanarity");
            m_rljet_Dip12[i]        = rljets[i]->auxdata<float>("Dip12");
            m_rljet_KtDR[i]         = rljets[i]->auxdata<float>("KtDR");
            m_rljet_Mu12[i]         = rljets[i]->auxdata<float>("Mu12");
            m_rljet_PlanarFlow[i]   = rljets[i]->auxdata<float>("PlanarFlow");
            m_rljet_Sphericity[i]   = rljets[i]->auxdata<float>("Sphericity");
            m_rljet_Split12[i]      = rljets[i]->auxdata<float>("Split12");
            m_rljet_Split23[i]      = rljets[i]->auxdata<float>("Split23");
            m_rljet_Split34[i]      = rljets[i]->auxdata<float>("Split34");
            m_rljet_ThrustMaj[i]    = rljets[i]->auxdata<float>("ThrustMaj");
            m_rljet_ThrustMin[i]    = rljets[i]->auxdata<float>("ThrustMin");
            m_rljet_ZCut12[i]       = rljets[i]->auxdata<float>("ZCut12");

            m_rljet_NTrimSubjets[i] = rljets[i]->auxdata<int>("NTrimSubjets");

            const xAOD::Jet* rljet_ungroomed = this->get_parent_ungroomed(rljets[i]);

            // std::vector<int> tjet_ntrks = rljet_ungroomed->auxdata<std::vector<int>>("NumTrkPt500");
            // m_rljet_ungroomed_ntrk500[i] = std::accumulate(tjet_ntrks.begin(),
            //         tjet_ntrks.end(), 0);

            std::vector<int> ntrk_tmp;
            rljet_ungroomed-> getAttribute(xAOD::JetAttribute::NumTrkPt500, ntrk_tmp);
            m_rljet_ungroomed_ntrk500[i] = ntrk_tmp[0];

            if(m_config->isMC())
            { // save corresponding truth jet variables
                const xAOD::JetContainer* truth_large_jets = nullptr;

                top::check( evtStore()->retrieve(truth_large_jets,
                            m_config->sgKeyTruthLargeRJets()), "FAILURE" );

                const xAOD::Jet* tljet_near =
                    get_nearest_jet_in_collection(rljets[i], truth_large_jets);

                // not yet veto-ing on deltaR(reco,truth), so this must be cut on later
                if (tljet_near != nullptr) {
                    m_tljet_m[i]   = tljet_near->m();
                    m_tljet_pt[i]  = tljet_near->pt();
                    m_tljet_eta[i] = tljet_near->eta();
                    m_tljet_phi[i] = tljet_near->phi();
                    m_tljet_dR[i]  = top::deltaR(*rljets[i], *tljet_near);

                    const float ecf1 = tljet_near->auxdata<float>("ECF1");
                    const float ecf2 = tljet_near->auxdata<float>("ECF2");
                    const float ecf3 = tljet_near->auxdata<float>("ECF3");

                    m_tljet_D2[i] = fabs(ecf2) > 1.e-6 ? pow(ecf1/ecf2,3)*ecf3 : -1000.;

                    /*********/
                    /* TAU32 */
                    /*********/

                    const float tau2 = tljet_near->auxdata<float>("Tau2_wta");
                    const float tau3 = tljet_near->auxdata<float>("Tau3_wta");

                    m_tljet_Tau32_wta[i] = fabs(tau2) > 1.e-6 ? tau3 / tau2 : -1000.;
                }

            } // end of saving truth jet variables

            // save corresponding trigger jet
            const xAOD::JetContainer* hlt_jets_a10r_tcemsubjesFS = nullptr;
            top::check ( evtStore()->retrieve(hlt_jets_a10r_tcemsubjesFS,
                        "HLT_xAOD__JetContainer_a10r_tcemsubjesFS"), "FAILURE" );

            if (hlt_jets_a10r_tcemsubjesFS != nullptr) {
                m_hltjet_count = hlt_jets_a10r_tcemsubjesFS->size();

                const xAOD::Jet* hltjet_near = get_nearest_jet_in_collection(rljets[i],
                        hlt_jets_a10r_tcemsubjesFS);

                if (hltjet_near != nullptr) {
                    m_hltjet_m[i]   = hltjet_near->m();
                    m_hltjet_pt[i]  = hltjet_near->pt();
                    m_hltjet_eta[i] = hltjet_near->eta();
                    m_hltjet_phi[i] = hltjet_near->phi();
                    m_hltjet_dR[i]  = top::deltaR(*rljets[i], *hltjet_near);
                }
            } // end of saving trigger jet variables

            // BDT
            m_rljet_BDT_Top_Score[i] = m_bdt_tool->score(*rljets[i], "toptag");
            m_rljet_BDT_W_Score[i] = m_bdt_tool->score(*rljets[i], "wtag");

            /*************/
            /* B-TAGGING */
            /*************/

            double rljet_btag0, rljet_btag1;
            this->get_trackjet_btags(rljets[i], rljet_btag0, rljet_btag1);

            const double BTAG_CUT = 0.6455;

            m_rljet_btag_double[i]     = rljet_btag0 > BTAG_CUT && rljet_btag1 > BTAG_CUT;
            m_rljet_btag_single[i]     = rljet_btag0 > BTAG_CUT || rljet_btag1 > BTAG_CUT;
            m_rljet_btag_leading_pt[i] = rljet_btag0 > BTAG_CUT;



        } // end of saving nominal branch large-R jet variables
    } // end of saving individual large-R jet variables

    // compute lead/sublead jet quantities
    if (on_nominal_branch && rljets.size() >= 2) {
        TLorentzVector v_jet0, v_jet1;
        v_jet0.SetPtEtaPhiM(rljets[0]->pt(), rljets[0]->eta(), rljets[0]->phi(), rljets[0]->m());
        v_jet1.SetPtEtaPhiM(rljets[1]->pt(), rljets[1]->eta(), rljets[1]->phi(), rljets[1]->m());
        m_rljet_mjj = (v_jet0 + v_jet1).M();

        m_rljet_ptasym = ( rljets[0]->pt() - rljets[1]->pt() ) / ( rljets[0]->pt() + rljets[1]->pt() );
        m_rljet_dy = rljets[0]->rapidity() - rljets[1]->rapidity();
        m_rljet_dR = top::deltaR(*rljets[0], *rljets[1]);
    }

    if (m_runHTT && !event.m_isLoose && on_nominal_branch)
        this->runHTTAndFillTree();

    if (m_runSD && !event.m_isLoose && on_nominal_branch)
        this->runSDandFillTree();

    // Oh yeah, we want to set all the original variables too and the call fill on the TTree
    EventSaverFlatNtuple::saveEvent(event);
}

void DataMCbackgroundEventSaver::runHTTAndFillTree(void) {
    if(! evtStore()->contains<xAOD::JetContainer>(m_httJetContainerPrefix)) {
        m_httTool->execute(); // produce a CA15 HTT container for the event
    }

    const xAOD::JetContainer* ca15jets_HTT = nullptr;
    top::check(evtStore()->retrieve(ca15jets_HTT,  m_httJetContainerPrefix), "FAILURE");

    std::vector<const xAOD::Jet*> pt_sorted_ca15jets;

    // mimick eta < 2.0 selection from anti-kt R=1.0 trimmed jets collection
    std::copy_if(ca15jets_HTT->begin(), ca15jets_HTT->end(), std::back_inserter(pt_sorted_ca15jets),
            [](const xAOD::Jet* j) {
                return fabs(j->eta()) < 2.0;
            });

    m_cajet_count = pt_sorted_ca15jets.size();

    // and make sure it is sorted by pT
    std::sort(pt_sorted_ca15jets.begin(), pt_sorted_ca15jets.end(),
            [](const xAOD::Jet* j1, const xAOD::Jet* j2) -> bool {
                return j1->pt() > j2->pt();
            });

    // mimic simple dijet selection
    if ( pt_sorted_ca15jets.size() < 2 ||
            pt_sorted_ca15jets[0] == nullptr ||
            pt_sorted_ca15jets[1] == nullptr ||
            pt_sorted_ca15jets[0]->pt() / 1000. < 450. ||
            pt_sorted_ca15jets[1]->pt() / 1000. < 200.)
        return;

    for (unsigned ijet = 0; ijet < m_num_fatjets_keep && ijet < pt_sorted_ca15jets.size(); ijet++) {
        // save the corresponding C/A 1.5 jet variables (ungroomed, nominal)
        m_htt_caJet_pt[ijet]  = pt_sorted_ca15jets[ijet]->pt();
        m_htt_caJet_eta[ijet] = pt_sorted_ca15jets[ijet]->eta();
        m_htt_caJet_phi[ijet] = pt_sorted_ca15jets[ijet]->phi();
        m_htt_caJet_m[ijet]   = pt_sorted_ca15jets[ijet]->m();

        // loop over all HTT configs (nominal + U/D sys.)
        for (int ihtt = 0; ihtt < m_nHttTools; ihtt++) {

            // get the groomed version of this jet, for this particular HTT config
            const xAOD::Jet* groomed_ca15jet=nullptr;

            if (m_httConfigs[ihtt] != "def") {
                pt_sorted_ca15jets[ijet]->getAssociatedObject("groomedFatJetFiltR30N5" + m_httConfigs[ihtt],
                        groomed_ca15jet);
            } else {
                pt_sorted_ca15jets[ijet]->getAssociatedObject("groomedFatJetFiltR30N5", groomed_ca15jet);
            }

            if (groomed_ca15jet != nullptr) {
                m_htt_caGroomJet_pt[ihtt][ijet]  = groomed_ca15jet->pt();
                m_htt_caGroomJet_eta[ihtt][ijet] = groomed_ca15jet->eta();
                m_htt_caGroomJet_phi[ihtt][ijet] = groomed_ca15jet->phi();
                m_htt_caGroomJet_m[ihtt][ijet]   = groomed_ca15jet->m();
            }

            // now save the actual HTT tagging information
            const xAOD::Jet* topCandidate = nullptr;
            bool tagged = pt_sorted_ca15jets[ijet]->getAssociatedObject("HTT_" + m_httConfigs[ihtt], topCandidate);

            if( tagged && topCandidate != nullptr ) {
                m_htt_tag[ihtt][ijet] = tagged;
                m_htt_pt[ihtt][ijet]  = topCandidate->pt();
                m_htt_eta[ihtt][ijet] = topCandidate->eta();
                m_htt_phi[ihtt][ijet] = topCandidate->phi();
                m_htt_m[ihtt][ijet]   = topCandidate->m();

                if( topCandidate->isAvailable<float>("M23") && topCandidate->isAvailable<float>("M123") )
                    m_htt_m23m123[ihtt][ijet] = topCandidate->getAttribute<float>("M23")/topCandidate->getAttribute<float>("M123");

                if( topCandidate->isAvailable<float>("Atan1312") )
                    m_htt_atan1312[ihtt][ijet] = topCandidate->getAttribute<float>("Atan1312");

                if( topCandidate->isAvailable<int>("n_top_cands") )
                    m_htt_nTagCands[ihtt][ijet] = topCandidate->getAttribute<int>("n_top_cands");

                if( topCandidate->isAvailable<float>("ptSub1") )
                    m_htt_pts1[ihtt][ijet] = topCandidate->getAttribute<float>("ptSub1");

                if( topCandidate->isAvailable<float>("ptSub2") )
                    m_htt_pts2[ihtt][ijet] = topCandidate->getAttribute<float>("ptSub2");

                if( topCandidate->isAvailable<float>("ptSub3") )
                    m_htt_pts3[ihtt][ijet] = topCandidate->getAttribute<float>("ptSub3");
            } // end of saving HTT tagging vars for this config
        } // end loop over all HTT configs
    } // end loop over C/A 1.5 jets
}

void DataMCbackgroundEventSaver::runSDandFillTree(void)
{
    const xAOD::JetContainer* reco_large_jets = nullptr;

    top::check(evtStore()->retrieve(reco_large_jets,
                m_config->sgKeyLargeRJets()), "FAILURE" );

    // make a pt-sorted copy of the large-R jet container
    const xAOD::JetContainer rljets = sort_container_pt(reco_large_jets);

    for (unsigned i = 0; i < m_num_fatjets_keep && i < rljets.size(); i++) {
        // Obtain ungroomed parent jet
        const xAOD::Jet* ungroomed_fatjet = this->get_parent_ungroomed(rljets[i]);

        xAOD::JetConstituentVector ungroomed_clusters = ungroomed_fatjet->getConstituents();
        std::vector<fastjet::PseudoJet> constituents;

        // Get constituents in fastjet format
        for ( const auto* con : ungroomed_clusters){
            fastjet::PseudoJet p(0,0,0,0);
            float pt = con->pt();
            float y = con->rapidity();
            float phi = con->phi();
            float m = con->m();
            if (y != y) {
                continue; // null vectors cause NaNs
            } else {
                p.reset_PtYPhiM(pt, y, phi, m);
                constituents.push_back(p);
            }
        }

        fastjet::JetDefinition microjet_def(fastjet::cambridge_algorithm, 0.2);
        fastjet::ClusterSequence subjet_cs(constituents, microjet_def);

        const float pt_cut = 20e3; // 20 GeV pt cut
        std::vector<fastjet::PseudoJet> subjets = fastjet::sorted_by_pt(subjet_cs.inclusive_jets(pt_cut));

        // Calibrate subjets

        // Create a subjet container for calibration
        xAOD::JetContainer* subjet_container = new xAOD::JetContainer();
        subjet_container->setStore(new xAOD::JetAuxContainer);

        top::check ( evtStore()->record(subjet_container, "subjet_container_name"), "FAILURE" );
        top::check ( evtStore()->record(dynamic_cast<xAOD::JetAuxContainer*>(subjet_container->getStore()), "subjet_container_nameAux."), "FAILURE");

        // Create xAOD::Jet's
        std::vector<const xAOD::Jet *> subj_ptrs_const;
        std::vector<xAOD::Jet *> subj_ptrs;
        for(auto it = subjets.begin(); it != subjets.end(); it++) {
            xAOD::Jet *subj = new xAOD::Jet();
            subjet_container->push_back(subj);
            subj_ptrs.push_back(subj);
            subj_ptrs_const.push_back(subj);
            // Set 4-mom
            subj->setJetP4(xAOD::JetFourMom_t(it->pt(), it->eta(), it->phi(), it->m()));
            subj->setJetP4(xAOD::JetConstitScaleMomentum, subj->jetP4());
            subj->setAttribute<xAOD::JetFourMom_t>("JetPileupScaleMomentum", subj->jetP4()); // This is to workaound JetCalibTools issue
        }

        // To hold the calibrated subjets
        std::vector<fastjet::PseudoJet> calib_subjets;
        std::vector<fastjet::PseudoJet> calib_subjets_UP;
        std::vector<fastjet::PseudoJet> calib_subjets_DOWN;

        // Now calibrate the subjets of the container
        for ( auto isubjet : *subjet_container) {
            xAOD::Jet* jet = nullptr;

            top::check (m_jet_calib_tool->calibratedCopy(*isubjet,jet), "FAILURE ");

            fastjet::PseudoJet p(0,0,0,0);
            float pt = jet->pt();
            float y = jet->rapidity();
            float phi = jet->phi();
            float m = jet->m();
            if (y != y) {
                // Clean
                delete jet;
                continue; // null vectors cause NaNs
            } else {
                p.reset_PtYPhiM(pt, y, phi, m);
                calib_subjets.push_back(p);

                p *= 1.03;
                calib_subjets_UP.push_back(p);

                p *= 0.97/1.03;
                calib_subjets_DOWN.push_back(p);
            } // remember: push_back makes a copy of non-pointer objects

            // Clean
            delete jet;
        }

        top::check ( evtStore()->tds()->remove("subjet_container_name"), "FAILURE" );
        top::check ( evtStore()->tds()->remove("subjet_container_nameAux."), "FAILURE" );

        // not using b-tagging for these studies
        std::vector<int> btag(calib_subjets.size(), 0);

        m_rljet_SDw_win20_btag0[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(calib_subjets, btag);
        m_rljet_SDz_win20_btag0[i] = tagger_SDz_win20_btag0->tagJetFromSubjets(calib_subjets, btag);
        m_rljet_SDt_win50_btag0[i] = tagger_SDt_win50_btag0->tagJetFromSubjets(calib_subjets, btag);

        m_rljet_SDw_win20_btag0_UP[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(calib_subjets_UP, btag);
        m_rljet_SDz_win20_btag0_UP[i] = tagger_SDz_win20_btag0->tagJetFromSubjets(calib_subjets_UP, btag);
        m_rljet_SDt_win50_btag0_UP[i] = tagger_SDt_win50_btag0->tagJetFromSubjets(calib_subjets_UP, btag);

        m_rljet_SDw_win20_btag0_DOWN[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(calib_subjets_DOWN, btag);
        m_rljet_SDz_win20_btag0_DOWN[i] = tagger_SDz_win20_btag0->tagJetFromSubjets(calib_subjets_DOWN, btag);
        m_rljet_SDt_win50_btag0_DOWN[i] = tagger_SDt_win50_btag0->tagJetFromSubjets(calib_subjets_DOWN, btag);
    }
}

const xAOD::Jet*
DataMCbackgroundEventSaver::get_parent_ungroomed(const xAOD::Jet* jet)
{
    const ElementLink<xAOD::JetContainer>& link_to_ungroomed =
        jet->getAttribute<ElementLink<xAOD::JetContainer>>("Parent");

    return *link_to_ungroomed;
}

void
DataMCbackgroundEventSaver::get_trackjet_btags(const xAOD::Jet* jet,
        double& jet_btag0, double& jet_btag1)
{
    // sane default values
    jet_btag0 = -1000;
    jet_btag1 = -1000;

    // need R=0.2 track jets ghost-associated to ungroomed large-R jet
    const xAOD::Jet* ungroomed_rljet = this->get_parent_ungroomed(jet);

    // grab the ghost-associated track jets
    std::vector<const xAOD::Jet*> track_jets;
    if (ungroomed_rljet->getAssociatedObjects<xAOD::Jet>("GhostAntiKt2TrackJet", track_jets)
            && track_jets.size() >= 2)
    {
        std::sort(track_jets.begin(), track_jets.end(),
                [](const xAOD::Jet* j1, const xAOD::Jet* j2) -> bool {
                    return j1->pt() > j2->pt();
                });

        double tjet0_mv2c10_discriminant, tjet1_mv2c10_discriminant;

        const bool tjet0_has_mv2c10 = track_jets[0]->btagging()->MVx_discriminant(
                "MV2c10", tjet0_mv2c10_discriminant);

        const bool tjet1_has_mv2c10 = track_jets[1]->btagging()->MVx_discriminant(
                "MV2c10", tjet1_mv2c10_discriminant);

        if (tjet0_has_mv2c10 && tjet1_has_mv2c10) {
            jet_btag0 = tjet0_mv2c10_discriminant;
            jet_btag1 = tjet1_mv2c10_discriminant;
        }
    }
}

}
