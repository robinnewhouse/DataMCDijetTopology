#include "DataMCbackgroundTools/DataMCbackgroundEventSaver.h"
#include "DataMCbackgroundTools/LocalTools.h"

// xAOD/Athena
#include <AthContainers/AuxTypeRegistry.h>
#include <xAODBase/IParticle.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCore/AuxContainerBase.h>
#include <xAODJet/JetAuxContainer.h>
#include <xAODTruth/TruthParticle.h>
#include <xAODTruth/TruthParticleContainer.h>

#include "BoostedJetTaggers/configHTT_Run2perf.h"

// AnalysisTop
#include "TopAnalysis/EventSaverFlatNtuple.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopEventSelectionTools/TreeManager.h"
#include "TopFakes/TopFakesxAODUtils.h"
#include "TopParticleLevel/ParticleLevelEvent.h"

// ROOT
#include "TFile.h"
#include "TLorentzVector.h"

// c++ stdlib
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

    /*************************************************/
    /* CUSTOM ANALYSISTOP CONFIG/CUTS FILE VARIABLES */
    /*************************************************/

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

    // configuration switch for HTT
    m_gammaJetOverlapRemoval = false;
    try {
        std::string GammaJetOverlapRemovalString = config_settings->value("GammaJetOverlapRemoval");
        if(GammaJetOverlapRemovalString.compare("True") == 0)
            m_gammaJetOverlapRemoval = true;
    } catch (...) {
        std::cout << "Not running Gamma + LargeR Jets overlap removal..." << std::endl;
        std::cout << "add 'GammaJetOverlapRemoval True' to cuts file to enable." << std::endl;
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

        systematicTree->makeOutputVariable(m_rljet_eta     , "rljet_eta");
        systematicTree->makeOutputVariable(m_rljet_phi     , "rljet_phi");
        systematicTree->makeOutputVariable(m_rljet_m_comb  , "rljet_m_comb");
        systematicTree->makeOutputVariable(m_rljet_pt_comb , "rljet_pt_comb");

        systematicTree->makeOutputVariable(m_rljet_D2        , "rljet_D2");
        systematicTree->makeOutputVariable(m_rljet_Tau32_wta , "rljet_Tau32_wta");
        systematicTree->makeOutputVariable(m_rljet_Qw        , "rljet_Qw");
        systematicTree->makeOutputVariable(m_rljet_Split23   , "rljet_Split23");

        systematicTree->makeOutputVariable(m_rljet_smooth16Top_Tau32Split23Tag50eff , "m_rljet_smooth16Top_Tau32Split23Tag50eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Top_Tau32Split23Tag80eff , "m_rljet_smooth16Top_Tau32Split23Tag80eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag50eff    , "m_rljet_smooth16Top_MassTau32Tag50eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag80eff    , "m_rljet_smooth16Top_MassTau32Tag80eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Top_QwTau32Tag50eff      , "m_rljet_smooth16Top_QwTau32Tag50eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Top_QwTau32Tag80eff      , "m_rljet_smooth16Top_QwTau32Tag80eff");

        systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag50eff_nocontain    , "m_rljet_smooth16Top_MassTau32Tag50eff_nocontain");
        systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag80eff_nocontain    , "m_rljet_smooth16Top_MassTau32Tag80eff_nocontain");

        systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag50eff , "rljet_smooth16WTag_50eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag80eff , "rljet_smooth16WTag_80eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag50eff , "rljet_smooth16ZTag_50eff");
        systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag80eff , "rljet_smooth16ZTag_80eff");

        systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag50eff_nocontain , "rljet_smooth16WTag_50eff_nocontain");
        systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag80eff_nocontain , "rljet_smooth16WTag_80eff_nocontain");
        systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag50eff_nocontain , "rljet_smooth16ZTag_50eff_nocontain");
        systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag80eff_nocontain , "rljet_smooth16ZTag_80eff_nocontain");

        /*
        if (m_config->isMC()) {
            systematicTree->makeOutputVariable(m_rljet_pdgid , "rljet_pdgid");
        }
        */

        /*************************************/
        /* PARAMETERS SAVED ONLY FOR NOMINAL */
        /*************************************/

        if (systematicTree->name() == "nominal") {
            systematicTree->makeOutputVariable(m_NPV, "NPV");

            // more large-R jet kinematic variables
            systematicTree->makeOutputVariable(m_rljet_count  , "rljet_count");
            systematicTree->makeOutputVariable(m_rljet_mjj    , "rljet_mjj");
            systematicTree->makeOutputVariable(m_rljet_ptasym , "rljet_ptasym");
            systematicTree->makeOutputVariable(m_rljet_dy     , "rljet_dy");
            systematicTree->makeOutputVariable(m_rljet_dR     , "rljet_dR");
            systematicTree->makeOutputVariable(m_rljet_dphi   , "rljet_dphi");
            systematicTree->makeOutputVariable(m_rljet_deta   , "rljet_deta");

            systematicTree->makeOutputVariable(m_rljet_m_calo  , "rljet_m_calo");
            systematicTree->makeOutputVariable(m_rljet_pt_calo , "rljet_pt_calo");
            systematicTree->makeOutputVariable(m_rljet_m_ta    , "rljet_m_ta");
            systematicTree->makeOutputVariable(m_rljet_pt_ta   , "rljet_pt_ta");

            systematicTree->makeOutputVariable(m_rljet_smooth15Top_MassTau32Tag50eff, "m_rljet_smooth15Top_MassTau32Tag50eff");
            systematicTree->makeOutputVariable(m_rljet_smooth15Top_MassTau32Tag80eff, "m_rljet_smooth15Top_MassTau32Tag80eff");

            systematicTree->makeOutputVariable(m_rljet_smooth15W_Tag50eff , "rljet_smooth15WTag_50eff");
            systematicTree->makeOutputVariable(m_rljet_smooth15W_Tag25eff , "rljet_smooth15WTag_25eff");
            systematicTree->makeOutputVariable(m_rljet_smooth15Z_Tag50eff , "rljet_smooth15ZTag_50eff");
            systematicTree->makeOutputVariable(m_rljet_smooth15Z_Tag25eff , "rljet_smooth15ZTag_25eff");

            // more substructure variables
            systematicTree->makeOutputVariable(m_rljet_Tau1_wta    , "rljet_Tau1_wta");
            systematicTree->makeOutputVariable(m_rljet_Tau2_wta    , "rljet_Tau2_wta");
            systematicTree->makeOutputVariable(m_rljet_Tau3_wta    , "rljet_Tau3_wta");
            systematicTree->makeOutputVariable(m_rljet_ECF1        , "rljet_ECF1");
            systematicTree->makeOutputVariable(m_rljet_ECF2        , "rljet_ECF2");
            systematicTree->makeOutputVariable(m_rljet_ECF3        , "rljet_ECF3");
            systematicTree->makeOutputVariable(m_rljet_FoxWolfram0 , "rljet_FoxWolfram0");
            systematicTree->makeOutputVariable(m_rljet_FoxWolfram2 , "rljet_FoxWolfram2");
            systematicTree->makeOutputVariable(m_rljet_Angularity  , "rljet_Angularity");
            systematicTree->makeOutputVariable(m_rljet_Aplanarity  , "rljet_Aplanarity");
            systematicTree->makeOutputVariable(m_rljet_Dip12       , "rljet_Dip12");
            systematicTree->makeOutputVariable(m_rljet_KtDR        , "rljet_KtDR");
            systematicTree->makeOutputVariable(m_rljet_Mu12        , "rljet_Mu12");
            systematicTree->makeOutputVariable(m_rljet_PlanarFlow  , "rljet_PlanarFlow");
            systematicTree->makeOutputVariable(m_rljet_Sphericity  , "rljet_Sphericity");
            systematicTree->makeOutputVariable(m_rljet_Split12     , "rljet_Split12");
            systematicTree->makeOutputVariable(m_rljet_Split34     , "rljet_Split34");
            systematicTree->makeOutputVariable(m_rljet_ThrustMaj   , "rljet_ThrustMaj");
            systematicTree->makeOutputVariable(m_rljet_ThrustMin   , "rljet_ThrustMin");
            systematicTree->makeOutputVariable(m_rljet_ZCut12      , "rljet_ZCut12");

            systematicTree->makeOutputVariable(m_rljet_NTrimSubjets      , "rljet_NTrimSubjets");
            systematicTree->makeOutputVariable(m_rljet_ungroomed_ntrk500 , "rljet_ungroomed_ntrk500");

            /*
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
            */

            /*
            //trigger jet
            systematicTree->makeOutputVariable(m_hltjet_count , "hltjet_count");
            systematicTree->makeOutputVariable(m_hltjet_m     , "hltjet_m");
            systematicTree->makeOutputVariable(m_hltjet_pt    , "hltjet_pt");
            systematicTree->makeOutputVariable(m_hltjet_eta   , "hltjet_eta");
            systematicTree->makeOutputVariable(m_hltjet_phi   , "hltjet_phi");
            systematicTree->makeOutputVariable(m_hltjet_dR    , "hltjet_dR");
            */

            // have to resize in order to makeOutputVariable below
            m_htt_tag       . resize(m_nHttTools);
            m_htt_pt        . resize(m_nHttTools);
            m_htt_eta       . resize(m_nHttTools);
            m_htt_phi       . resize(m_nHttTools);
            m_htt_m         . resize(m_nHttTools);
            m_htt_m123      . resize(m_nHttTools);
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
                systematicTree->makeOutputVariable(m_htt_m123[i]      , getHTTBranchName("m123"      , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_m23m123[i]   , getHTTBranchName("m23m123"   , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_atan1312[i]  , getHTTBranchName("atan1312"  , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_nTagCands[i] , getHTTBranchName("nTagCands" , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_tag[i]       , getHTTBranchName("tag"       , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_pts1[i]      , getHTTBranchName("pts1"      , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_pts2[i]      , getHTTBranchName("pts2"      , m_httConfigs[i]));
                systematicTree->makeOutputVariable(m_htt_pts3[i]      , getHTTBranchName("pts3"      , m_httConfigs[i]));
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
            systematicTree->makeOutputVariable(m_rljet_SDw_calib         , "rljet_SDw_calib");
            systematicTree->makeOutputVariable(m_rljet_SDw_uncalib       , "rljet_SDw_uncalib");
            systematicTree->makeOutputVariable(m_rljet_SDw_combined      , "rljet_SDw_combined");
            systematicTree->makeOutputVariable(m_rljet_SDw_dcut          , "rljet_SDw_dcut");
            systematicTree->makeOutputVariable(m_rljet_SDt_calib         , "rljet_SDt_calib");
            systematicTree->makeOutputVariable(m_rljet_SDt_uncalib       , "rljet_SDt_uncalib");
            systematicTree->makeOutputVariable(m_rljet_SDt_combined      , "rljet_SDt_combined");
            systematicTree->makeOutputVariable(m_rljet_SDt_dcut          , "rljet_SDt_dcut");

            if (m_config->isMC()) {
              systematicTree->makeOutputVariable(m_rljet_SDw_calib_DOWN    , "rljet_SDw_calib_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDw_uncalib_DOWN  , "rljet_SDw_uncalib_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDw_combined_DOWN , "rljet_SDw_combined_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDw_dcut_DOWN     , "rljet_SDw_dcut_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDt_calib_DOWN    , "rljet_SDt_calib_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDt_uncalib_DOWN  , "rljet_SDt_uncalib_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDt_combined_DOWN , "rljet_SDt_combined_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDt_dcut_DOWN     , "rljet_SDt_dcut_DOWN");
              systematicTree->makeOutputVariable(m_rljet_SDw_calib_UP      , "rljet_SDw_calib_UP");
              systematicTree->makeOutputVariable(m_rljet_SDw_uncalib_UP    , "rljet_SDw_uncalib_UP");
              systematicTree->makeOutputVariable(m_rljet_SDw_combined_UP   , "rljet_SDw_combined_UP");
              systematicTree->makeOutputVariable(m_rljet_SDw_dcut_UP       , "rljet_SDw_dcut_UP");
              systematicTree->makeOutputVariable(m_rljet_SDt_calib_UP      , "rljet_SDt_calib_UP");
              systematicTree->makeOutputVariable(m_rljet_SDt_uncalib_UP    , "rljet_SDt_uncalib_UP");
              systematicTree->makeOutputVariable(m_rljet_SDt_combined_UP   , "rljet_SDt_combined_UP");
              systematicTree->makeOutputVariable(m_rljet_SDt_dcut_UP       , "rljet_SDt_dcut_UP");
            }

            // b-tagging output variables
            // systematicTree->makeOutputVariable(m_rljet_btag_double     , "rljet_btag_double");
            // systematicTree->makeOutputVariable(m_rljet_btag_single     , "rljet_btag_single");
            // systematicTree->makeOutputVariable(m_rljet_btag_leading_pt , "rljet_btag_leading_pt");

        } // end making nominal branch only output variables
    } // end making all output variables

    m_truth_match_tool = make_unique<TruthMatchTool>();

    /************************/
    /* SUBSTRUCTURE TAGGERS */
    /************************/

    if (m_runSD) this->initializeSD();

    // 2015 smoothed taggers
    topTagger15_Mass_50eff  = STTHelpers::configSubstTagger("SmoothMassCutOnly50eff", std::vector<std::string>{"SmoothMassCut_50"});
    topTagger15_Mass_80eff  = STTHelpers::configSubstTagger("SmoothMassCutOnly80eff", std::vector<std::string>{"SmoothMassCut_80"});
    topTagger15_Tau32_50eff = STTHelpers::configSubstTagger("SmoothTau32CutOnly50eff", std::vector<std::string>{"SmoothTau32Cut_50"});
    topTagger15_Tau32_80eff = STTHelpers::configSubstTagger("SmoothTau32CutOnly80eff", std::vector<std::string>{"SmoothTau32Cut_80"});

    wTagger15_50eff = make_unique<JetSubStructureUtils::BosonTag>("medium", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat", false, false);
    wTagger15_25eff = make_unique<JetSubStructureUtils::BosonTag>("tight", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat", false, false);

    zTagger15_50eff = make_unique<JetSubStructureUtils::BosonTag>("medium", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Ztagging_MC15_Prerecommendations_20150809.dat", false, false);
    zTagger15_25eff = make_unique<JetSubStructureUtils::BosonTag>("tight", "smooth", "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Ztagging_MC15_Prerecommendations_20150809.dat", false, false);

    // 2016 smoothed taggers
    topTagger16_Tau32Split23_50eff = make_unique<SmoothedTopTagger>("Smooth16Tau32Split23Eff50");
    top::check(topTagger16_Tau32Split23_50eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_Tau32Split23FixedSignalEfficiency50_MC15c_20161209.dat"), "FAILURE");
    top::check(topTagger16_Tau32Split23_50eff->initialize(), "FAILURE");

    topTagger16_Tau32Split23_80eff = make_unique<SmoothedTopTagger>("Smooth16Tau32Split23Eff80");
    top::check(topTagger16_Tau32Split23_80eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_Tau32Split23FixedSignalEfficiency80_MC15c_20161209.dat"), "FAILURE");
    top::check(topTagger16_Tau32Split23_80eff->initialize(), "FAILURE");

    topTagger16_MassTau32_50eff = make_unique<SmoothedTopTagger>("Smooth16MassTau32Eff50");
    top::check(topTagger16_MassTau32_50eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency50_MC15c_20161209.dat"), "FAILURE");
    top::check(topTagger16_MassTau32_50eff->initialize(), "FAILURE");

    topTagger16_MassTau32_80eff = make_unique<SmoothedTopTagger>("Smooth16MassTau32Eff80");
    top::check(topTagger16_MassTau32_80eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency80_MC15c_20161209.dat"), "FAILURE");
    top::check(topTagger16_MassTau32_80eff->initialize(), "FAILURE");

    topTagger16_MassTau32_50eff_nocontain = make_unique<SmoothedTopTagger>("Smooth16MassTau32Eff50_nocontain");
    top::check(topTagger16_MassTau32_50eff_nocontain->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedNotContainedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency50_MC15c_20170318.dat"), "FAILURE");
    top::check(topTagger16_MassTau32_50eff_nocontain->initialize(), "FAILURE");

    topTagger16_MassTau32_80eff_nocontain = make_unique<SmoothedTopTagger>("Smooth16MassTau32Eff80");
    top::check(topTagger16_MassTau32_80eff_nocontain->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedNotContainedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency80_MC15c_20170318.dat"), "FAILURE");
    top::check(topTagger16_MassTau32_80eff_nocontain->initialize(), "FAILURE");

    topTagger16_QwTau32_50eff = make_unique<SmoothedTopTagger>("Smooth16QwTau32Eff50");
    top::check(topTagger16_QwTau32_50eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_QwTau32FixedSignalEfficiency50_MC15c_20161209.dat"), "FAILURE");
    top::check(topTagger16_QwTau32_50eff->initialize(), "FAILURE");

    topTagger16_QwTau32_80eff = make_unique<SmoothedTopTagger>("Smooth16QwTau32Eff80");
    top::check(topTagger16_QwTau32_80eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_QwTau32FixedSignalEfficiency80_MC15c_20161209.dat"), "FAILURE");
    top::check(topTagger16_QwTau32_80eff->initialize(), "FAILURE");

    wTagger16_50eff = make_unique<SmoothedWZTagger>("Smooth16WEff50");
    top::check(wTagger16_50eff->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161215.dat"), "FAILURE");
    top::check(wTagger16_50eff->initialize(), "FAILURE");

    wTagger16_80eff = make_unique<SmoothedWZTagger>("Smooth16WEff80");
    top::check(wTagger16_80eff->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161215.dat"), "FAILURE");
    top::check(wTagger16_80eff->initialize(), "FAILURE");

    zTagger16_50eff = make_unique<SmoothedWZTagger>("Smooth16ZEff50");
    top::check(zTagger16_50eff->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161215.dat"), "FAILURE");
    top::check(zTagger16_50eff->initialize(), "FAILURE");

    zTagger16_80eff = make_unique<SmoothedWZTagger>("Smooth16ZEff80");
    top::check(zTagger16_80eff->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161215.dat"), "FAILURE");
    top::check(zTagger16_80eff->initialize(), "FAILURE");

    wTagger16_50eff_nocontain = make_unique<SmoothedWZTagger>("Smooth16WEff50_nocontain");
    top::check(wTagger16_50eff_nocontain->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161219.dat"), "FAILURE");
    top::check(wTagger16_50eff_nocontain->initialize(), "FAILURE");

    wTagger16_80eff_nocontain = make_unique<SmoothedWZTagger>("Smooth16WEff80_nocontain");
    top::check(wTagger16_80eff_nocontain->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161219.dat"), "FAILURE");
    top::check(wTagger16_80eff_nocontain->initialize(), "FAILURE");

    zTagger16_50eff_nocontain = make_unique<SmoothedWZTagger>("Smooth16ZEff50");
    top::check(zTagger16_50eff_nocontain->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161219.dat"), "FAILURE");
    top::check(zTagger16_50eff_nocontain->initialize(), "FAILURE");

    zTagger16_80eff_nocontain = make_unique<SmoothedWZTagger>("Smooth16ZEff80");
    top::check(zTagger16_80eff_nocontain->setProperty("ConfigFile", "SmoothedWZTaggers/SmoothedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161219.dat"), "FAILURE");
    top::check(zTagger16_80eff_nocontain->initialize(), "FAILURE");
} // end of intialization

void
DataMCbackgroundEventSaver::initializeSD(void)
{

        tagger_SDw_win20_btag0 = make_unique<ShowerDeconstruction>("SD_W_win20_Btag0");
        top::check( tagger_SDw_win20_btag0->setProperty("mode", "w/q"), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("R", 1.0), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("topMass", 170), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("topMassWindow", 40.0), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("wMass", 80.403), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("wMassWindow", 20.0), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("hMass", 125.0), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("hMassWindow", 20.0), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("maxNjets", 6), "FAILURE");
        top::check (tagger_SDw_win20_btag0->setProperty("useBtag", 0), "FAILURE");
        top::check (tagger_SDw_win20_btag0->initialize(), "FAILURE" );
        tagger_SDt_win50_btag0 = make_unique<ShowerDeconstruction>("SD_T_win50_Btag0");
        top::check (tagger_SDt_win50_btag0->setProperty("mode", "t/g"), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("R", 1.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("topMass", 170), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("topMassWindow", 50.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("wMass", 80.403), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("wMassWindow", 25.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("hMass", 125.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("hMassWindow", 20.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("maxNjets", 6), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->setProperty("useBtag", 0), "FAILURE" );
        top::check (tagger_SDt_win50_btag0->initialize(), "FAILURE" );
        tagger_SDt_win50_btag1 = make_unique<ShowerDeconstruction>("SD_T_win50_Btag1");
        top::check (tagger_SDt_win50_btag1->setProperty("mode", "t/g"), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("R", 1.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("topMass", 172), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("topMassWindow", 40.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("wMass", 80.403), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("wMassWindow", 25.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("hMass", 125.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("hMassWindow", 20.0), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("maxNjets", 6), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->setProperty("useBtag", 0), "FAILURE" );
        top::check (tagger_SDt_win50_btag1->initialize(), "FAILURE" );

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

    //The default constructor can also be used if the arguments are set with python configuration instead
    m_jetcalib_subjet = new JetCalibrationTool(name_thread);
    top::check( m_jetcalib_subjet->setProperty("JetCollection", jetAlgo_subjet.Data()), "FAILURE setting property 'JetCollection' for SD subjet calibration!" );
    top::check( m_jetcalib_subjet->setProperty("ConfigFile", config_subjetcalib.Data()), "FAILURE setting property 'ConfigFile' for SD subjet calibration!" );
    top::check( m_jetcalib_subjet->setProperty("CalibSequence", calibSeq_subjet.Data()), "FAILURE setting property 'CalibSequence' for SD subjet calibration!" );
    top::check( m_jetcalib_subjet->setProperty("IsData", !m_config->isMC()), "FAILURE setting property 'IsData' for SD subjet calibration!" );

    //Initialize the tool
    top::check ( m_jetcalib_subjet->initializeTool(name_thread), "FAILURE: failed to initialize subject calibration tool for SD");
}

void
DataMCbackgroundEventSaver::reset_containers(const bool on_nominal_branch)
{
    m_rljet_pt_comb . assign(m_num_fatjets_keep, -1000. );
    m_rljet_m_comb  . assign(m_num_fatjets_keep, -1000. );
    m_rljet_eta     . assign(m_num_fatjets_keep, -1000. );
    m_rljet_phi     . assign(m_num_fatjets_keep, -1000. );

    m_rljet_D2        . assign(m_num_fatjets_keep, -1000. );
    m_rljet_Tau32_wta . assign(m_num_fatjets_keep, -1000. );
    m_rljet_Qw        . assign(m_num_fatjets_keep, -1000. );
    m_rljet_Split23   . assign(m_num_fatjets_keep, -1000. );

    m_rljet_smooth16Top_Tau32Split23Tag50eff . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_Tau32Split23Tag80eff . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_MassTau32Tag50eff    . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_MassTau32Tag80eff    . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_MassTau32Tag50eff_nocontain    . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_MassTau32Tag80eff_nocontain    . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_QwTau32Tag50eff      . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Top_QwTau32Tag80eff      . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16W_Tag50eff               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16W_Tag80eff               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Z_Tag50eff               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Z_Tag80eff               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16W_Tag50eff_nocontain               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16W_Tag80eff_nocontain               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Z_Tag50eff_nocontain               . assign(m_num_fatjets_keep, -1000);
    m_rljet_smooth16Z_Tag80eff_nocontain               . assign(m_num_fatjets_keep, -1000);

    // m_rljet_pdgid . assign(m_num_fatjets_keep, -1000 );

    if (on_nominal_branch) {
        m_NPV = -1000;

        m_rljet_count  = 0;
        m_rljet_mjj    = -1000.;
        m_rljet_ptasym = -1000.;
        m_rljet_dy     = -1000.;
        m_rljet_dR     = -1000.;
        m_rljet_dphi   = -1000.;
        m_rljet_deta   = -1000.;

        m_rljet_pt_calo . assign(m_num_fatjets_keep, -1000. );
        m_rljet_m_calo  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_m_ta  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_pt_ta . assign(m_num_fatjets_keep, -1000. );

        m_rljet_Tau1_wta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Tau2_wta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Tau3_wta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ECF1        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ECF2        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ECF3        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_FoxWolfram0 . assign(m_num_fatjets_keep, -1000. );
        m_rljet_FoxWolfram2 . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Angularity  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Aplanarity  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Dip12       . assign(m_num_fatjets_keep, -1000. );
        m_rljet_KtDR        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Mu12        . assign(m_num_fatjets_keep, -1000. );
        m_rljet_PlanarFlow  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Sphericity  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Split12     . assign(m_num_fatjets_keep, -1000. );
        m_rljet_Split34     . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ThrustMaj   . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ThrustMin   . assign(m_num_fatjets_keep, -1000. );
        m_rljet_ZCut12      . assign(m_num_fatjets_keep, -1000. );

        m_rljet_NTrimSubjets      . assign(m_num_fatjets_keep, -1000 );
        m_rljet_ungroomed_ntrk500 . assign(m_num_fatjets_keep, -1000 );

        /*
        if (m_config->isMC()) {
            m_tljet_pt  . assign(m_num_fatjets_keep, -1000.);
            m_tljet_eta . assign(m_num_fatjets_keep, -1000.);
            m_tljet_phi . assign(m_num_fatjets_keep, -1000.);
            m_tljet_m   . assign(m_num_fatjets_keep, -1000.);
            m_tljet_dR  . assign(m_num_fatjets_keep, -1000.);

            m_tljet_Tau32_wta . assign(m_num_fatjets_keep, -1000. );
            m_tljet_D2        . assign(m_num_fatjets_keep, -1000. );
        }
        */

        // m_rljet_btag_double     . assign(m_num_fatjets_keep, false);
        // m_rljet_btag_single     . assign(m_num_fatjets_keep, false);
        // m_rljet_btag_leading_pt . assign(m_num_fatjets_keep, false);

        /*
        m_hltjet_pt  . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_eta . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_phi . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_m   . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_dR  . assign(m_num_fatjets_keep, -1000. );
        m_hltjet_count = 0;
        */

        m_rljet_smooth15Top_MassTau32Tag50eff    . assign(m_num_fatjets_keep, -1000);
        m_rljet_smooth15Top_MassTau32Tag80eff    . assign(m_num_fatjets_keep, -1000);
        m_rljet_smooth15W_Tag50eff               . assign(m_num_fatjets_keep, -1000);
        m_rljet_smooth15W_Tag25eff               . assign(m_num_fatjets_keep, -1000);
        m_rljet_smooth15Z_Tag50eff               . assign(m_num_fatjets_keep, -1000);
        m_rljet_smooth15Z_Tag25eff               . assign(m_num_fatjets_keep, -1000);

        if (m_runSD) {
          m_rljet_SDw_calib . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDw_uncalib . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDw_combined . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDw_dcut . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDt_calib . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDt_uncalib . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDt_combined . assign(m_num_fatjets_keep, -1000.);
          m_rljet_SDt_dcut . assign(m_num_fatjets_keep, -1000.);
          if (m_config->isMC()) {
            m_rljet_SDw_calib_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_uncalib_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_combined_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_dcut_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_calib_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_uncalib_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_combined_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_dcut_DOWN . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_calib_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_uncalib_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_combined_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDw_dcut_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_calib_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_uncalib_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_combined_UP . assign(m_num_fatjets_keep, -1000.);
            m_rljet_SDt_dcut_UP . assign(m_num_fatjets_keep, -1000.);
          }
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
                m_htt_m123[i]      . assign(m_num_fatjets_keep, -1000.);
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

    std::vector<const xAOD::Jet*> rljets;

    auto pt_sort_func = [&](const xAOD::IParticle* p1, const xAOD::IParticle* p2){
      return p1->pt() > p2->pt();
    };

    if (m_gammaJetOverlapRemoval && !event.m_photons.empty()) {

      const xAOD::Photon* lead_photon = *std::max_element(event.m_photons.begin(),
          event.m_photons.end(), pt_sort_func);

      if (lead_photon) {
        std::copy_if(event.m_largeJets.begin(),
            event.m_largeJets.end(),
            std::back_inserter(rljets),
            [& lead_photon](const xAOD::Jet* j) {
                return top::deltaR(*lead_photon, *j) >= 1.0;
            });
      }

    } else {
      for (auto jet : event.m_largeJets) rljets.push_back(jet);
    }

    if (rljets.empty()) return;

    std::sort(rljets.begin(), rljets.end(), pt_sort_func);

    /******************/
    /* TRUTH MATCHING */
    /******************/

    // truth matching not currently needed

    /*
    if(m_config->isMC()) {
            const xAOD::JetContainer* truth_large_jets = nullptr;
            const xAOD::TruthParticleContainer* truth_particles = nullptr;

            top::check( evtStore()->retrieve(truth_large_jets,
                                    m_config->sgKeyTruthLargeRJets()), "FAILURE" );

            top::check( evtStore()->retrieve(truth_particles,
                                    m_config->sgKeyMCParticle()), "FAILURE" );

            m_truth_match_tool->execute(&rljets, truth_large_jets, truth_particles);
    }
    */

    /* code (bugged) for Arthur's version
    if(m_config->isMC()) {
        const float dRMax = 0.75;
        const int topPdgId = 6;
        const int wPdgId = 24;
        const int zPdgId = 23;
        const int bPdgId = 5;
        m_truth_match_tool->DecorateMatchJets(rljets, rljets, 0.,
                        "deltaRMatchedTruthJet", "deltaRMatchedRecoJet");

        m_truth_match_tool->DecorateMatchTruth(tljets, truth_particles,
                        topPdgId, dRMax, "dRMatchedTop", true);
        m_truth_match_tool->DecorateMatchTruth(tljets, truth_particles,
                        wPdgId, dRMax, "dRMatchedW", true);
        m_truth_match_tool->DecorateMatchTruth(tljets, truth_particles,
                        zPdgId, dRMax, "dRMatchedZ", true);
        m_truth_match_tool->DecorateMatchTruth(tljets, truth_particles,
                        bPdgId, dRMax, "dRMatchedB", true);

    }
    */

    for (unsigned i = 0; i < m_num_fatjets_keep && i < rljets.size(); i++) {

        if (m_debug_level >= 3) {
            if (i > 0) std::cout << std::endl;
            std::cout << "### LARGE-R JET NO. " << i << " ###" << std::endl;
        }

        /***********************/
        /* KINEMATIC VARIABLES */
        /***********************/

        m_rljet_eta[i] = rljets[i]->eta();
        m_rljet_phi[i] = rljets[i]->phi();
        m_rljet_m_comb[i]  = rljets[i]->m();
        m_rljet_pt_comb[i] = rljets[i]->pt();

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

        /**************/
        /* Qw/Split23 */
        /**************/

        m_rljet_Qw[i]          = rljets[i]->auxdata<float>("Qw");
        m_rljet_Split23[i]     = rljets[i]->auxdata<float>("Split23");

        /**********************************/
        /* SMOOTH (2016) TAGGER VARIABLES */
        /**********************************/

        SmoothedTopTagger::Result res = topTagger16_Tau32Split23_50eff->result(*rljets[i], false);
        m_rljet_smooth16Top_Tau32Split23Tag50eff[i] = combine_bits( res.tau32Passed(), res.split23Passed() );

        res = topTagger16_Tau32Split23_80eff->result(*rljets[i], false);
        m_rljet_smooth16Top_Tau32Split23Tag80eff[i] = combine_bits( res.tau32Passed(), res.split23Passed() );

        res = topTagger16_MassTau32_50eff->result(*rljets[i], false);
        m_rljet_smooth16Top_MassTau32Tag50eff[i]    = combine_bits( res.massPassed(), res.tau32Passed() );

        res = topTagger16_MassTau32_80eff->result(*rljets[i], false);
        m_rljet_smooth16Top_MassTau32Tag80eff[i]    = combine_bits( res.massPassed(), res.tau32Passed() );

        res = topTagger16_MassTau32_50eff_nocontain->result(*rljets[i], false);
        m_rljet_smooth16Top_MassTau32Tag50eff_nocontain[i]    = combine_bits( res.massPassed(), res.tau32Passed() );

        res = topTagger16_MassTau32_80eff_nocontain->result(*rljets[i], false);
        m_rljet_smooth16Top_MassTau32Tag80eff_nocontain[i]    = combine_bits( res.massPassed(), res.tau32Passed() );

        res = topTagger16_QwTau32_50eff->result(*rljets[i], false);
        m_rljet_smooth16Top_QwTau32Tag50eff[i]      = combine_bits( res.qwPassed(), res.tau32Passed() );

        res = topTagger16_QwTau32_80eff->result(*rljets[i], false);
        m_rljet_smooth16Top_QwTau32Tag80eff[i]      = combine_bits( res.qwPassed(), res.tau32Passed() );

        m_rljet_smooth16W_Tag50eff[i] = wTagger16_50eff->result(*rljets[i]);
        m_rljet_smooth16W_Tag80eff[i] = wTagger16_80eff->result(*rljets[i]);

        m_rljet_smooth16Z_Tag50eff[i] = zTagger16_50eff->result(*rljets[i]);
        m_rljet_smooth16Z_Tag80eff[i] = zTagger16_80eff->result(*rljets[i]);

        m_rljet_smooth16W_Tag50eff_nocontain[i] = wTagger16_50eff_nocontain->result(*rljets[i]);
        m_rljet_smooth16W_Tag80eff_nocontain[i] = wTagger16_80eff_nocontain->result(*rljets[i]);

        m_rljet_smooth16Z_Tag50eff_nocontain[i] = zTagger16_50eff_nocontain->result(*rljets[i]);
        m_rljet_smooth16Z_Tag80eff_nocontain[i] = zTagger16_80eff_nocontain->result(*rljets[i]);

        /*****************************/
        /* PDGID FROM TRUTH MATCHING */
        /*****************************/

        /*
        if (m_config->isMC()) {
            bool was_matched_to_truth_jet    = false;
            int z_w_top_id                   = -1;
            int num_quarks_contained         = -1;
            bool top_child_b_quark_contained = false;
            int maxE_parton                  = -1;

            if (rljets[i]->isAvailable<int>("dRmatched_reco_truth"))
                was_matched_to_truth_jet = rljets[i]->auxdata<int>("dRmatched_reco_truth") == 1;

            if (rljets[i]->isAvailable<int>("dRmatched_particle_flavor"))
                z_w_top_id = abs(rljets[i]->auxdata<int>("dRmatched_particle_flavor"));

            if (rljets[i]->isAvailable<int>("dRmatched_maxEParton_flavor"))
                maxE_parton = abs(rljets[i]->auxdata<int>("dRmatched_maxEParton_flavor"));

            if (rljets[i]->isAvailable<int>("dRmatched_topBChild"))
                top_child_b_quark_contained = rljets[i]->auxdata<int>("dRmatched_topBChild") == 1;

            if (rljets[i]->isAvailable<int>("dRmatched_nQuarkChildren"))
                num_quarks_contained = rljets[i]->auxdata<int>("dRmatched_nQuarkChildren");

            if (was_matched_to_truth_jet) {
                if (z_w_top_id == 0)
                    m_rljet_pdgid[i] = maxE_parton;

                else if (z_w_top_id == 6 && top_child_b_quark_contained && num_quarks_contained >= 3)
                    m_rljet_pdgid[i] = 6;

                else if (z_w_top_id == 23 && num_quarks_contained >= 2)
                    m_rljet_pdgid[i] = 23;

                else if (z_w_top_id == 24 && num_quarks_contained >= 2)
                    m_rljet_pdgid[i] = 24;
            }
        }
        */

        /*****************************************/
        /* PROCESS NOMINAL BRANCH ONLY VARIABLES */
        /*****************************************/

        if (on_nominal_branch) {
            m_rljet_count = event.m_largeJets.size();

            // retreive TA mass and TA-mass-corrected pT
            const xAOD::JetFourMom_t &calibP4_TA =
                rljets[i]->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA");
            const xAOD::JetFourMom_t &calibP4_calo =
                rljets[i]->getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo");

            m_rljet_pt_ta[i]   = calibP4_TA.Pt();
            m_rljet_m_ta[i]    = calibP4_TA.M();
            m_rljet_m_calo[i]  = calibP4_calo.M();
            m_rljet_pt_calo[i] = calibP4_calo.Pt();

            // substructure variables
            m_rljet_Tau1_wta[i]    = tau1;
            m_rljet_Tau2_wta[i]    = tau2;
            m_rljet_Tau3_wta[i]    = tau3;
            m_rljet_ECF1[i]        = ecf1;
            m_rljet_ECF2[i]        = ecf2;
            m_rljet_ECF3[i]        = ecf3;
            m_rljet_FoxWolfram0[i] = rljets[i]->auxdata<float>("FoxWolfram0");
            m_rljet_FoxWolfram2[i] = rljets[i]->auxdata<float>("FoxWolfram2");
            m_rljet_Angularity[i]  = rljets[i]->auxdata<float>("Angularity");
            m_rljet_Aplanarity[i]  = rljets[i]->auxdata<float>("Aplanarity");
            m_rljet_Dip12[i]       = rljets[i]->auxdata<float>("Dip12");
            m_rljet_KtDR[i]        = rljets[i]->auxdata<float>("KtDR");
            m_rljet_Mu12[i]        = rljets[i]->auxdata<float>("Mu12");
            m_rljet_PlanarFlow[i]  = rljets[i]->auxdata<float>("PlanarFlow");
            m_rljet_Sphericity[i]  = rljets[i]->auxdata<float>("Sphericity");
            m_rljet_Split12[i]     = rljets[i]->auxdata<float>("Split12");
            m_rljet_Split34[i]     = rljets[i]->auxdata<float>("Split34");
            m_rljet_ThrustMaj[i]   = rljets[i]->auxdata<float>("ThrustMaj");
            m_rljet_ThrustMin[i]   = rljets[i]->auxdata<float>("ThrustMin");
            m_rljet_ZCut12[i]      = rljets[i]->auxdata<float>("ZCut12");

            m_rljet_NTrimSubjets[i] = rljets[i]->auxdata<int>("NTrimSubjets");

            const xAOD::Jet* rljet_ungroomed = this->get_parent_ungroomed(rljets[i]);

            std::vector<int> ntrk_tmp;
            rljet_ungroomed->getAttribute(xAOD::JetAttribute::NumTrkPt500, ntrk_tmp);
            m_rljet_ungroomed_ntrk500[i] = ntrk_tmp[0];

            // 2015 smoothed taggers

            m_rljet_smooth15Top_MassTau32Tag50eff[i] = combine_bits( topTagger15_Mass_50eff->isTagged(*rljets[i]), topTagger15_Tau32_50eff->isTagged(*rljets[i]) );
            m_rljet_smooth15Top_MassTau32Tag80eff[i] = combine_bits( topTagger15_Mass_80eff->isTagged(*rljets[i]), topTagger15_Tau32_80eff->isTagged(*rljets[i]) );

            // W-tag info
            // 0: failed both mass and substructure cut
            // 1: passed substructure cut *only*
            // 2: passed mass window criteria *only*
            // 3: passed mass window + substructure criteria
            m_rljet_smooth15W_Tag50eff[i] = wTagger15_50eff->result(*rljets[i]);
            m_rljet_smooth15W_Tag25eff[i] = wTagger15_25eff->result(*rljets[i]);

            // Z-tag info
            // 0: failed both mass and substructure cut
            // 1: passed substructure cut *only*
            // 2: passed mass window criteria *only*
            // 3: passed mass window + substructure criteria
            m_rljet_smooth15Z_Tag50eff[i] = zTagger15_50eff->result(*rljets[i]);
            m_rljet_smooth15Z_Tag25eff[i] = zTagger15_25eff->result(*rljets[i]);

            // if(m_config->isMC())
            // { // save corresponding truth jet variables
            //     const xAOD::JetContainer* truth_large_jets = nullptr;
            //     top::check( evtStore()->retrieve(truth_large_jets, m_config->sgKeyTruthLargeRJets()), "FAILURE" );

            //     const xAOD::Jet* matched_tljet = get_nearest_jet_in_collection(rljets[i], truth_large_jets);

            //     if (matched_tljet != nullptr) {
            //         const float dR = top::deltaR(*rljets[i], *matched_tljet);

            //         if (dR < 0.4) {
            //             m_tljet_m[i]   = matched_tljet->m();
            //             m_tljet_pt[i]  = matched_tljet->pt();
            //             m_tljet_eta[i] = matched_tljet->eta();
            //             m_tljet_phi[i] = matched_tljet->phi();
            //             m_tljet_dR[i]  = dR;

            //             const float ecf1 = matched_tljet->auxdata<float>("ECF1");
            //             const float ecf2 = matched_tljet->auxdata<float>("ECF2");
            //             const float ecf3 = matched_tljet->auxdata<float>("ECF3");

            //             m_tljet_D2[i] = fabs(ecf2) > 1.e-6 ? pow(ecf1/ecf2,3)*ecf3 : -1000.;

            //             /*********/
            //             /* TAU32 */
            //             /*********/

            //             const float tau2 = matched_tljet->auxdata<float>("Tau2_wta");
            //             const float tau3 = matched_tljet->auxdata<float>("Tau3_wta");

            //             m_tljet_Tau32_wta[i] = fabs(tau2) > 1.e-6 ? tau3 / tau2 : -1000.;
            //         }
            //     }
            // } // end of saving truth jet variables

            /*
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
            */

            /*************/
            /* B-TAGGING */
            /*************/

            // float rljet_btag0, rljet_btag1;
            // this->get_trackjet_btags(rljets[i], rljet_btag0, rljet_btag1);

            // const float BTAG_CUT = 0.6455;

            // m_rljet_btag_double[i]     = rljet_btag0 > BTAG_CUT && rljet_btag1 > BTAG_CUT;
            // m_rljet_btag_single[i]     = rljet_btag0 > BTAG_CUT || rljet_btag1 > BTAG_CUT;
            // m_rljet_btag_leading_pt[i] = rljet_btag0 > BTAG_CUT;
        } // end of saving nominal branch large-R jet variables
    } // end of saving individual large-R jet variables

    if (on_nominal_branch) { // other event-level and misc. variables
        m_NPV = event.m_primaryVertices->size();

        // compute lead/sublead jet quantities
        if (rljets.size() >= 2) {
            TLorentzVector v_jet0, v_jet1;
            v_jet0.SetPtEtaPhiM(rljets[0]->pt(), rljets[0]->eta(), rljets[0]->phi(), rljets[0]->m());
            v_jet1.SetPtEtaPhiM(rljets[1]->pt(), rljets[1]->eta(), rljets[1]->phi(), rljets[1]->m());
            m_rljet_mjj = (v_jet0 + v_jet1).M();

            m_rljet_ptasym = ( rljets[0]->pt() - rljets[1]->pt() ) / ( rljets[0]->pt() + rljets[1]->pt() );
            m_rljet_dy     = rljets[0]->rapidity() - rljets[1]->rapidity();
            m_rljet_dR     = top::deltaR(*rljets[0], *rljets[1]);
            m_rljet_dphi   = rljets[0]->phi() - rljets[1]->phi();
            m_rljet_deta   = rljets[0]->eta() - rljets[1]->eta();
        }

        if (m_runHTT && !event.m_isLoose)
            this->runHTTAndFillTree();

        if (m_runSD)
            this->runSDandFillTree(rljets, m_config->isMC());
    }

    // Oh yeah, we want to set all the default AnalysisTop variables too and the call fill on the TTree
    EventSaverFlatNtuple::saveEvent(event);
}

void DataMCbackgroundEventSaver::runHTTAndFillTree(void) {
    const xAOD::CaloClusterContainer *caloCalClusters = nullptr;
    top::check(evtStore()->retrieve(caloCalClusters, "CaloCalTopoClusters"), "FAILURE");
    fakeClusterEMScale(caloCalClusters); // a little hack for filling in the missing (unused) rawM variable

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
            pt_sorted_ca15jets[0]->pt() / 1000. < 525. ||
            pt_sorted_ca15jets[1]->pt() / 1000. < 225.)
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
                pt_sorted_ca15jets[ijet]->getAssociatedObject("groomedFatJetTrimR20F5" + m_httConfigs[ihtt], groomed_ca15jet);
            } else {
                pt_sorted_ca15jets[ijet]->getAssociatedObject("groomedFatJetTrimR20F5", groomed_ca15jet);
            }

            if (groomed_ca15jet != nullptr) {
                m_htt_caGroomJet_pt[ihtt][ijet]  = groomed_ca15jet->pt();
                m_htt_caGroomJet_eta[ihtt][ijet] = groomed_ca15jet->eta();
                m_htt_caGroomJet_phi[ihtt][ijet] = groomed_ca15jet->phi();
                m_htt_caGroomJet_m[ihtt][ijet]   = groomed_ca15jet->m();
            }

            // now save the actual HTT tagging information
            const xAOD::Jet* topCandidate = nullptr;
            const bool tagged = pt_sorted_ca15jets[ijet]->getAssociatedObject("HTT_" + m_httConfigs[ihtt], topCandidate);

            if( tagged && topCandidate != nullptr ) {
                m_htt_tag[ihtt][ijet] = tagged;
                m_htt_pt[ihtt][ijet]  = topCandidate->pt();
                m_htt_eta[ihtt][ijet] = topCandidate->eta();
                m_htt_phi[ihtt][ijet] = topCandidate->phi();
                m_htt_m[ihtt][ijet]   = topCandidate->m();

                if( topCandidate->isAvailable<float>("M123") )
                    m_htt_m123[ihtt][ijet] = topCandidate->getAttribute<float>("M123");

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

void DataMCbackgroundEventSaver::runSDandFillTree(std::vector<const xAOD::Jet*>& rljets, bool doSystShifts)
{
  for (unsigned i = 0; i < m_num_fatjets_keep && i < rljets.size(); i++) {
    // Obtain ungroomed parent jet
    const ElementLink<xAOD::JetContainer>& linkToUngroomed  = rljets[i]->getAttribute<ElementLink<xAOD::JetContainer>>("Parent");
    const xAOD::Jet* ungroomed_fatjet = *linkToUngroomed;

    xAOD::JetConstituentVector AssociatedClusters_forSD = ungroomed_fatjet->getConstituents();
    vector<fastjet::PseudoJet> constituents;

    // also get groomed constituents
    const xAOD::JetConstituentVector groomed_clusters = rljets[i]->getConstituents();
    std::vector<fastjet::PseudoJet> groomed_constituents;
    groomed_constituents = JetconVecToPseudojet(groomed_clusters);
    constituents = JetconVecToPseudojet(AssociatedClusters_forSD);

    // Now, obtain traditional subjets, uncalib subjet and force 3 subjets
    fastjet::JetDefinition large_r_def(fastjet::kt_algorithm, 1.0 );
    fastjet::JetDefinition microjet_def(fastjet::cambridge_algorithm, 0.2);
    fastjet::ClusterSequence subjet_cs(constituents, microjet_def);
    fastjet::ClusterSequence forcedN_cs(groomed_constituents, large_r_def);
    fastjet::ClusterSequence d32_cs(groomed_constituents,large_r_def);

    float pt_cut = 20e3; // 20 GeV pt cut
    std::vector<fastjet::PseudoJet> subjets = fastjet::sorted_by_pt(subjet_cs.inclusive_jets(pt_cut));
    std::vector<fastjet::PseudoJet> uncalib_subjets = fastjet::sorted_by_pt(subjet_cs.inclusive_jets(10e3)); // use 10 GeV cut
    std::vector<fastjet::PseudoJet> new_subjets_3 = fastjet::sorted_by_pt(forcedN_cs.exclusive_jets_up_to(3)) ;

    double dcut2 = 15.0e3*15.0e3;
    double dcut3 = 20.0e3*20.0e3;
    std::vector<fastjet::PseudoJet> dcut2_subjets = fastjet::sorted_by_pt(d32_cs.exclusive_jets(dcut2));
    std::vector<fastjet::PseudoJet> dcut3_subjets = fastjet::sorted_by_pt(d32_cs.exclusive_jets(dcut3));
    // Calibrate subjets

    // Create a subjet container for calibration
    xAOD::JetContainer* subjet_container = new xAOD::JetContainer();
    subjet_container->setStore(new xAOD::JetAuxContainer);

    top::check ( evtStore()->record(subjet_container, "subjet_container_name"), "FAILURE" );
    top::check ( evtStore()->record(dynamic_cast<xAOD::JetAuxContainer*>(subjet_container->getStore()), "subjet_container_nameAux."), "FAILURE");

    // Create xAOD::Jet's
    vector<const xAOD::Jet*> subj_ptrs_const;
    vector<xAOD::Jet*> subj_ptrs;
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


    // To hold the callibrated subjets
    vector<fastjet::PseudoJet> calib_subjets;
    vector<fastjet::PseudoJet> calib_subjets_UP;
    vector<fastjet::PseudoJet> calib_subjets_DOWN;
    // Now calibrate the subjets of the container
    for ( auto isubjet : *subjet_container) {
      xAOD::Jet * jet = 0;

      // SATUS CODE
      top::check ( m_jetcalib_subjet->calibratedCopy(*isubjet,jet), "FAILURE "); //make a calibrated copy, assuming a copy hasn't been made already

      // Here is your calibrated subjet: "jet"

      fastjet::PseudoJet p(0,0,0,0);
      float pt = jet->pt();
      float y = jet->rapidity();
      float phi = jet->phi();
      // No mass calibration, set mass for each subjet as 1 MeV
      float m = 1.0;
      //float m = jet->m();
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
      }

      // Clean
      delete jet;
    }
    top::check ( evtStore()->tds()->remove("subjet_container_name"), "FAILURE" );
    top::check ( evtStore()->tds()->remove("subjet_container_nameAux."), "FAILURE" );
    // Get trackjets and their B tag status
    vector<int> btag_uncalib(uncalib_subjets.size(), 0);
    //std::vector<int> btag_special2(new_subjets_2.size(), 0);
    vector<int> btag_special3(new_subjets_3.size(), 0);
    vector<int> btag(calib_subjets.size(), 0);
    vector<int> tracktag;
    vector<int> btag_special5(dcut2_subjets.size(),0);
    vector<int> btag_special6(dcut3_subjets.size(),0);
    std::vector<fastjet::PseudoJet> uncalib_subjets_UP;
    std::vector<fastjet::PseudoJet> uncalib_subjets_DOWN;
    for ( unsigned int i = 0; i < uncalib_subjets.size(); i++ ){
      uncalib_subjets_UP.push_back( uncalib_subjets[i] * 1.03 );
      uncalib_subjets_DOWN.push_back( uncalib_subjets[i] * 0.97 );

    }
    std::vector<fastjet::PseudoJet> dcut2_subjets_UP;
    std::vector<fastjet::PseudoJet> dcut2_subjets_DOWN;
    for ( unsigned int i = 0; i < dcut2_subjets.size(); i++ ){
      dcut2_subjets_UP.push_back( dcut2_subjets[i] * 1.03 );
      dcut2_subjets_DOWN.push_back( dcut2_subjets[i] * 0.97 );

    }
    std::vector<fastjet::PseudoJet> new_subjets_3_UP;
    std::vector<fastjet::PseudoJet> new_subjets_3_DOWN;
    for ( unsigned int i = 0; i < new_subjets_3.size(); i++ ){
      new_subjets_3_UP.push_back( new_subjets_3[i] * 1.03 );
      new_subjets_3_DOWN.push_back( new_subjets_3[i] * 0.97 );
    }

    m_rljet_SDw_calib[i]    = tagger_SDw_win20_btag0->tagJetFromSubjets(calib_subjets, btag);
    m_rljet_SDw_uncalib[i]  = tagger_SDw_win20_btag0->tagJetFromSubjets(uncalib_subjets, btag_uncalib);
    m_rljet_SDw_dcut[i]     = tagger_SDw_win20_btag0->tagJetFromSubjets(dcut2_subjets,btag_special5);
    m_rljet_SDw_combined[i] = m_rljet_SDw_uncalib[i];
    m_rljet_SDt_calib[i]    = tagger_SDt_win50_btag0->tagJetFromSubjets(calib_subjets, btag);
    m_rljet_SDt_uncalib[i]  = tagger_SDt_win50_btag0->tagJetFromSubjets(uncalib_subjets, btag_uncalib);
    m_rljet_SDt_dcut[i]     = tagger_SDt_win50_btag1->tagJetFromSubjets(dcut2_subjets,btag_special5);
    m_rljet_SDt_combined[i] = m_rljet_SDt_uncalib[i];

    if ( uncalib_subjets.size() < 4 ){
      m_rljet_SDw_combined[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(new_subjets_3, btag_special3);
      m_rljet_SDt_combined[i] = tagger_SDt_win50_btag0->tagJetFromSubjets(new_subjets_3, btag_special3);
    }

    if(doSystShifts) {
      m_rljet_SDw_calib_UP[i]    = tagger_SDw_win20_btag0->tagJetFromSubjets(calib_subjets_UP, btag);
      m_rljet_SDw_uncalib_UP[i]  = tagger_SDw_win20_btag0->tagJetFromSubjets(uncalib_subjets_UP, btag_uncalib);
      m_rljet_SDw_dcut_UP[i]     = tagger_SDw_win20_btag0->tagJetFromSubjets(dcut2_subjets_UP,btag_special5);
      m_rljet_SDw_combined_UP[i] = m_rljet_SDw_uncalib_UP[i];
      m_rljet_SDt_calib_UP[i]    = tagger_SDt_win50_btag0->tagJetFromSubjets(calib_subjets_UP, btag);
      m_rljet_SDt_uncalib_UP[i]  = tagger_SDt_win50_btag0->tagJetFromSubjets(uncalib_subjets_UP, btag_uncalib);
      m_rljet_SDt_dcut_UP[i]     = tagger_SDt_win50_btag1->tagJetFromSubjets(dcut2_subjets_UP,btag_special5);
      m_rljet_SDt_combined_UP[i] = m_rljet_SDt_uncalib_UP[i];

      if ( uncalib_subjets.size() < 4 ){
        m_rljet_SDw_combined_UP[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(new_subjets_3_UP, btag_special3);
        m_rljet_SDt_combined_UP[i] = tagger_SDt_win50_btag0->tagJetFromSubjets(new_subjets_3_UP, btag_special3);
      }

      m_rljet_SDw_calib_DOWN[i]    = tagger_SDw_win20_btag0->tagJetFromSubjets(calib_subjets_DOWN, btag);
      m_rljet_SDw_uncalib_DOWN[i]  = tagger_SDw_win20_btag0->tagJetFromSubjets(uncalib_subjets_DOWN, btag_uncalib);
      m_rljet_SDw_dcut_DOWN[i]     = tagger_SDw_win20_btag0->tagJetFromSubjets(dcut2_subjets_DOWN,btag_special5);
      m_rljet_SDw_combined_DOWN[i] = m_rljet_SDw_uncalib_DOWN[i];
      m_rljet_SDt_calib_DOWN[i]    = tagger_SDt_win50_btag0->tagJetFromSubjets(calib_subjets_DOWN, btag);
      m_rljet_SDt_uncalib_DOWN[i]  = tagger_SDt_win50_btag0->tagJetFromSubjets(uncalib_subjets_DOWN, btag_uncalib);
      m_rljet_SDt_dcut_DOWN[i]     = tagger_SDt_win50_btag1->tagJetFromSubjets(dcut2_subjets_DOWN,btag_special5);
      m_rljet_SDt_combined_DOWN[i] = m_rljet_SDt_uncalib_DOWN[i];

      if ( uncalib_subjets.size() < 4 ){
        m_rljet_SDw_combined_DOWN[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(new_subjets_3_DOWN, btag_special3);
        m_rljet_SDt_combined_DOWN[i] = tagger_SDt_win50_btag0->tagJetFromSubjets(new_subjets_3_DOWN, btag_special3);
      }
    }
  }
}

const xAOD::Jet*
DataMCbackgroundEventSaver::get_parent_ungroomed(const xAOD::Jet* jet)
{
  const ElementLink<xAOD::JetContainer>& link_to_ungroomed =
    jet->getAttribute<ElementLink<xAOD::JetContainer>>("Parent");

  return *link_to_ungroomed;
}

/*
void
DataMCbackgroundEventSaver::get_trackjet_btags(const xAOD::Jet* jet,
    float& jet_btag0, float& jet_btag1)
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

    float tjet0_mv2c10_discriminant, tjet1_mv2c10_discriminant;

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
*/

vector<fastjet::PseudoJet>
DataMCbackgroundEventSaver::JetconVecToPseudojet (xAOD::JetConstituentVector input)
{
  vector<fastjet::PseudoJet> constituents;

  for ( const auto* con : input){
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

  return constituents;
}

}

