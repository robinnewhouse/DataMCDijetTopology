#include "DataMCbackgroundTools/DataMCbackgroundEventSaver.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "DataMCbackgroundTools/MiscTools.h"

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
    // apply branch filters to remove unneeded branches from NTuples
    branchFilters().push_back(std::bind(&getBranchStatus, std::placeholders::_1, std::placeholders::_2));

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
        } std::cout << std::endl;
    } catch (...) {
        std::cout << "No HTT Configs found :( ..." << std::endl;
        m_httConfigs.clear();
    }

    m_nHttTools = m_httConfigs.size();
    std::cout << "Number of HTT Configs: " << m_nHttTools << std::endl;

    try {
        m_httJetContainerPrefix = config_settings->value("HTTJetContainerPrefix");
    } catch (...) {
        m_httJetContainerPrefix = "CA15LCTopoJets_HTT";
    }

    // configuration switch for HTT
    m_runHTT = m_nHttTools > 0;
    if (m_nHttTools > 0) {
      try {
        if(config_settings->value("RunHTT").compare("False") == 0) m_runHTT = false;
      } catch (...) {
        std::cout << "add 'RunHTT True/False' to cuts file to ensure HTT enabled/disabled." << std::endl;
      }
    }

    if (m_nHttTools > 0 && m_runHTT)
        m_httTool = std::unique_ptr<JetRecTool>(buildFullHTTagger("", m_httJetContainerPrefix, m_httConfigs));

    m_runMVAtag = false;
    try {
        if(config_settings->value("RunMVATag").compare("True") == 0) m_runMVAtag = true;
    } catch (...) {
        std::cout << "Disabling MVA taggers since option not set..." << std::endl;
        std::cout << "add 'RunMVAtag True' to cuts file to disable." << std::endl;
    }

    m_massOrderJets = false;
    try {
        if(config_settings->value("MassOrderJets").compare("True") == 0) m_massOrderJets = true;
    } catch (...) {
        std::cout << "add 'MassOrderJets True' to cuts file to mass order (instead of pT-order) jets." << std::endl;
    }

    m_runSmoothWZtag = true;
    try {
        if(config_settings->value("RunSmoothWZTag").compare("True") != 0) m_runSmoothWZtag = false;
    } catch (...) {
        std::cout << "Running Smooth W/Z taggers by default because option not set..." << std::endl;
        std::cout << "add 'RunSmoothWZtag False' to cuts file to disable." << std::endl;
    }

    m_runSmoothToptag = true;
    try {
        if(config_settings->value("RunSmoothTopTag").compare("True") != 0) m_runSmoothToptag = false;
    } catch (...) {
        std::cout << "Running Smooth Top taggers by default because option not set..." << std::endl;
        std::cout << "add 'RunSmoothToptag False' to cuts file to disable." << std::endl;
    }

    m_runSmoothUncontained = false;
    try {
        if(config_settings->value("RunSmoothUncontained").compare("True") == 0) m_runSmoothUncontained = true;
    } catch (...) {
        std::cout << "Uncontained Smooth Taggers disabled by default..." << std::endl;
        std::cout << "add 'RunSmoothUncontained True' to cuts file to enable." << std::endl;
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

    m_savePhoton = false;
    try {
        std::string PhotonSwitchString = config_settings->value("SavePhoton");
        if(PhotonSwitchString.compare("True") == 0)
            m_savePhoton = true;
    } catch (...) {
        std::cout << "Not saving lead photon because option not set..." << std::endl;
        std::cout << "add 'SavePhoton True' to cuts file to enable." << std::endl;
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
        m_num_fatjets_keep = 1;
        std::cout << "Defaulting to saving only the leading pT large R jet" << std::endl;
        std::cout << "add 'NumFatjetsKeep X', where X = 1, 2, or 3 to cuts file to enable." << std::endl;
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


        // nClusters study
        // l_jet_nClusters_hist = new TH1F("n_l_jet_clusters","Number of Clusters Per Large-R Jet",50,0,50);
        // s_jet_nClusters_hist = new TH1F("n_s_jet_clusters","Number of Clusters Per Small-R Jet",50,0,50);

        // l_jet_fractional_pt = new TH1F("l_jet_fractional_pt","Fractional pt of 10th cluster of each large-R jet",200,0,1);

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

        if (m_runSmoothToptag) {
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_Tau32Split23Tag50eff , "m_rljet_smooth16Top_Tau32Split23Tag50eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_Tau32Split23Tag80eff , "m_rljet_smooth16Top_Tau32Split23Tag80eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag50eff    , "m_rljet_smooth16Top_MassTau32Tag50eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag80eff    , "m_rljet_smooth16Top_MassTau32Tag80eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_QwTau32Tag50eff      , "m_rljet_smooth16Top_QwTau32Tag50eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_QwTau32Tag80eff      , "m_rljet_smooth16Top_QwTau32Tag80eff");
        }

        if (m_runSmoothToptag && m_runSmoothUncontained) {
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag50eff_nocontain    , "m_rljet_smooth16Top_MassTau32Tag50eff_nocontain");
          systematicTree->makeOutputVariable(m_rljet_smooth16Top_MassTau32Tag80eff_nocontain    , "m_rljet_smooth16Top_MassTau32Tag80eff_nocontain");
        }

        if (m_runSmoothWZtag) {
          systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag50eff , "rljet_smooth16WTag_50eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag80eff , "rljet_smooth16WTag_80eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag50eff , "rljet_smooth16ZTag_50eff");
          systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag80eff , "rljet_smooth16ZTag_80eff");
        }

        if (m_runSmoothWZtag && m_runSmoothUncontained) {
          systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag50eff_nocontain , "rljet_smooth16WTag_50eff_nocontain");
          systematicTree->makeOutputVariable(m_rljet_smooth16W_Tag80eff_nocontain , "rljet_smooth16WTag_80eff_nocontain");
          systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag50eff_nocontain , "rljet_smooth16ZTag_50eff_nocontain");
          systematicTree->makeOutputVariable(m_rljet_smooth16Z_Tag80eff_nocontain , "rljet_smooth16ZTag_80eff_nocontain");
        }

        // counting number of clusters per jet

        if(m_runMVAtag) {
          // BDT tagger outputs
          // top taggers
          systematicTree->makeOutputVariable( m_rljet_topTag_BDT_qqb              , "rljet_topTag_BDT_qqb");
          systematicTree->makeOutputVariable( m_rljet_topTag_BDT_qqb_score        , "rljet_topTag_BDT_qqb_score");
          // W taggers
          systematicTree->makeOutputVariable( m_rljet_wTag_BDT_qq              , "rljet_wTag_BDT_qq");
          systematicTree->makeOutputVariable( m_rljet_wTag_BDT_qq_score        , "rljet_wTag_BDT_qq_score");

          // DNN tagger outputs
          // top taggers
          systematicTree->makeOutputVariable( m_rljet_topTag_DNN_qqb_score        , "rljet_topTag_DNN_qqb_score");
          systematicTree->makeOutputVariable( m_rljet_topTag_DNN_qqb              , "rljet_topTag_DNN_qqb");
          // W taggers
          systematicTree->makeOutputVariable( m_rljet_wTag_DNN_qq_score        , "rljet_wTag_DNN_qq_score");
          systematicTree->makeOutputVariable( m_rljet_wTag_DNN_qq              , "rljet_wTag_DNN_qq");
          // Topocluster Tagger
          systematicTree->makeOutputVariable( m_rljet_topTag_TopoTagger_20wp   , "rljet_topTag_TopoTagger_20wp");
          systematicTree->makeOutputVariable( m_rljet_topTag_TopoTagger_50wp   , "rljet_topTag_TopoTagger_50wp");
          systematicTree->makeOutputVariable( m_rljet_topTag_TopoTagger_80wp   , "rljet_topTag_TopoTagger_80wp");
          systematicTree->makeOutputVariable( m_rljet_topTag_TopoTagger_score  , "rljet_topTag_TopoTagger_score");
        }

        if (m_savePhoton) {
          systematicTree->makeOutputVariable(m_photon0_pt           , "photon0_pt");
          systematicTree->makeOutputVariable(m_photon0_eta          , "photon0_eta");
          systematicTree->makeOutputVariable(m_photon0_phi          , "photon0_phi");
          systematicTree->makeOutputVariable(m_photon0_ptcone20     , "photon0_ptcone20");
          systematicTree->makeOutputVariable(m_photon0_ptcone40     , "photon0_ptcone40");
          systematicTree->makeOutputVariable(m_photon0_topoetcone20 , "photon0_topoetcone20");
          systematicTree->makeOutputVariable(m_photon0_topoetcone40 , "photon0_topoetcone40");
        }

        /*************************************/
        /* PARAMETERS SAVED ONLY FOR NOMINAL */
        /*************************************/

        if (systematicTree->name() == "nominal") {
            systematicTree->makeOutputVariable(m_NPV, "NPV");

            if (m_config->isMC()) {
              systematicTree->makeOutputVariable(m_rljet_pdgid              , "rljet_pdgid");
              systematicTree->makeOutputVariable(m_rljet_matched_parton_pt  , "rljet_matched_parton_pt");
              systematicTree->makeOutputVariable(m_rljet_matched_parton_eta , "rljet_matched_parton_eta");
              systematicTree->makeOutputVariable(m_rljet_matched_parton_phi , "rljet_matched_parton_phi");
              systematicTree->makeOutputVariable(m_rljet_matched_parton_m   , "rljet_matched_parton_m");
              systematicTree->makeOutputVariable(m_tljet_pt                 , "tljet_pt");
              systematicTree->makeOutputVariable(m_tljet_eta                , "tljet_eta");
              systematicTree->makeOutputVariable(m_tljet_phi                , "tljet_phi");
              systematicTree->makeOutputVariable(m_tljet_m                  , "tljet_m");
              systematicTree->makeOutputVariable(m_tljet_mjj                , "tljet_mjj");
              systematicTree->makeOutputVariable(m_parton_mjj               , "parton_mjj");

              systematicTree->makeOutputVariable(m_pid1 , "pid1");
              systematicTree->makeOutputVariable(m_pid2 , "pid2");
            }

            // more large-R jet kinematic variables
            systematicTree->makeOutputVariable(m_rljet_count     , "rljet_count");
            systematicTree->makeOutputVariable(m_rljet_mjj       , "rljet_mjj");
            systematicTree->makeOutputVariable(m_rljet_ptasym    , "rljet_ptasym");
            systematicTree->makeOutputVariable(m_rljet_mass_asym , "rljet_mass_asym");
            systematicTree->makeOutputVariable(m_rljet_dy        , "rljet_dy");
            systematicTree->makeOutputVariable(m_rljet_dR        , "rljet_dR");
            systematicTree->makeOutputVariable(m_rljet_dphi      , "rljet_dphi");
            systematicTree->makeOutputVariable(m_rljet_deta      , "rljet_deta");

            systematicTree->makeOutputVariable(m_rljet_m_calo  , "rljet_m_calo");
            systematicTree->makeOutputVariable(m_rljet_pt_calo , "rljet_pt_calo");
            systematicTree->makeOutputVariable(m_rljet_m_ta    , "rljet_m_ta");
            systematicTree->makeOutputVariable(m_rljet_pt_ta   , "rljet_pt_ta");

            // more substructure variables
            systematicTree->makeOutputVariable(m_rljet_C2          , "rljet_C2");
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
            systematicTree->makeOutputVariable(m_rljet_n_constituents    , "rljet_n_constituents");


            // nClusters study
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_0     , "rljet_fractional_pt_0");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_1     , "rljet_fractional_pt_1");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_2     , "rljet_fractional_pt_2");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_3     , "rljet_fractional_pt_3");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_4     , "rljet_fractional_pt_4");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_5     , "rljet_fractional_pt_5");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_6     , "rljet_fractional_pt_6");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_7     , "rljet_fractional_pt_7");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_8     , "rljet_fractional_pt_8");
            systematicTree->makeOutputVariable(m_rljet_fractional_pt_9     , "rljet_fractional_pt_9");


            if (m_runHTT) {
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
            }

            if (m_runSD) {
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
            }
        } // end making nominal branch only output variables
    } // end making all output variables

    /************************/
    /* SUBSTRUCTURE TAGGERS */
    /************************/

    if (m_runSD) this->initializeSD();

    // 2016 smoothed taggers

    if (m_runSmoothToptag)  {
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

      topTagger16_QwTau32_50eff = make_unique<SmoothedTopTagger>("Smooth16QwTau32Eff50");
      top::check(topTagger16_QwTau32_50eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_QwTau32FixedSignalEfficiency50_MC15c_20161209.dat"), "FAILURE");
      top::check(topTagger16_QwTau32_50eff->initialize(), "FAILURE");

      topTagger16_QwTau32_80eff = make_unique<SmoothedTopTagger>("Smooth16QwTau32Eff80");
      top::check(topTagger16_QwTau32_80eff->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_QwTau32FixedSignalEfficiency80_MC15c_20161209.dat"), "FAILURE");
      top::check(topTagger16_QwTau32_80eff->initialize(), "FAILURE");
    }

    if (m_runSmoothToptag && m_runSmoothUncontained)  {
      topTagger16_MassTau32_50eff_nocontain = make_unique<SmoothedTopTagger>("Smooth16MassTau32Eff50_nocontain");
      top::check(topTagger16_MassTau32_50eff_nocontain->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedNotContainedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency50_MC15c_20170318.dat"), "FAILURE");
      top::check(topTagger16_MassTau32_50eff_nocontain->initialize(), "FAILURE");

      topTagger16_MassTau32_80eff_nocontain = make_unique<SmoothedTopTagger>("Smooth16MassTau32Eff80");
      top::check(topTagger16_MassTau32_80eff_nocontain->setProperty("ConfigFile", "SmoothedTopTaggers/SmoothedNotContainedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency80_MC15c_20170318.dat"), "FAILURE");
      top::check(topTagger16_MassTau32_80eff_nocontain->initialize(), "FAILURE");
    }

    if (m_runSmoothWZtag) {
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
    }

    if (m_runSmoothWZtag && m_runSmoothUncontained) {
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
    }

    if(m_runMVAtag) {
      std::string cfg_topTagger_TopoTagger_205080eff = "TopoclusterTopTagger/TopoclusterTopTagger_AntiKt10LCTopoTrimmed_TopQuark_MC15c_20170511_ptweighted.dat";
      std::string cfg_topTagger_DNN_80eff_qqb = "JSSWTopTaggerDNN/JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC15c_20170824_BOOSTSetup80Eff.dat";
      std::string cfg_topTagger_BDT_80eff_qqb = "JSSWTopTaggerBDT/JSSBDTTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC15c_20170824_BOOSTSetup80Eff.dat";
      std::string cfg_wTagger_DNN_50eff_qq = "JSSWTopTaggerDNN/JSSDNNTagger_AntiKt10LCTopoTrimmed_WBosonContained_MC15c_20170824_BOOSTSetup50Eff.dat";
      std::string cfg_wTagger_BDT_50eff_qq = "JSSWTopTaggerBDT/JSSBDTTagger_AntiKt10LCTopoTrimmed_WBosonContained_MC15c_20170824_BOOSTSetup50Eff.dat";

      // BDT
      // top tagger (80% eff WP)
      m_topTagger_BDT_qqb = make_unique<JSSWTopTaggerBDT>("m_topTagger_BDT_qqb");
      top::check(m_topTagger_BDT_qqb->setProperty("ConfigFile", cfg_topTagger_BDT_80eff_qqb), "ERROR while setting BDT top qqb tagger properties");
      top::check(m_topTagger_BDT_qqb->initialize(), "ERROR while initializing BDT top qqb tagger");

      // W tagger (50% eff WP)
      m_wTagger_BDT_qq = make_unique<JSSWTopTaggerBDT>("m_wTagger_BDT_qq");
      top::check(m_wTagger_BDT_qq->setProperty("ConfigFile", cfg_wTagger_BDT_50eff_qq), "ERROR while setting BDT W tagger properties");
      top::check(m_wTagger_BDT_qq->initialize(), "ERROR while initializing BDT W tagger");

      // DNN
      // top tagger (80% eff WP)
      m_topTagger_DNN_qqb = make_unique<JSSWTopTaggerDNN>("m_topTagger_DNN_qqb");
      top::check(m_topTagger_DNN_qqb->setProperty("ConfigFile", cfg_topTagger_DNN_80eff_qqb), "ERROR while setting DNN top qqb tagger properties");
      top::check(m_topTagger_DNN_qqb->initialize(), "ERROR while initializing DNN top qqb tagger");
      // W tagger (50% eff WP)
      m_wTagger_DNN_qq = make_unique<JSSWTopTaggerDNN>("m_wTagger_DNN_qq");
      top::check(m_wTagger_DNN_qq->setProperty("ConfigFile", cfg_wTagger_DNN_50eff_qq), "ERROR while setting DNN W qq tagger properties");
      top::check(m_wTagger_DNN_qq->initialize(), "ERROR while initializing DNN W qq tagger");
  
      // Topocluster Tagger
      // top tagger (20%, 50%, 80% wp. all cuts provided from single tag)
      m_topTagger_TopoTagger = make_unique<TopoclusterTopTagger>("m_topTagger_TopoTagger");
      top::check(m_topTagger_TopoTagger->setProperty("ConfigFile", cfg_topTagger_TopoTagger_205080eff), "ERROR while setting Topocluster top tagger properties");
      top::check(m_topTagger_TopoTagger->initialize(), "ERROR while initializing Topocluster top tagger");

    }


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

    if (m_runSmoothToptag) {
      m_rljet_smooth16Top_Tau32Split23Tag50eff . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Top_Tau32Split23Tag80eff . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Top_MassTau32Tag50eff    . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Top_MassTau32Tag80eff    . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Top_QwTau32Tag50eff      . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Top_QwTau32Tag80eff      . assign(m_num_fatjets_keep, -1000. );
    }

    if (m_runSmoothToptag && m_runSmoothUncontained) {
      m_rljet_smooth16Top_MassTau32Tag50eff_nocontain . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Top_MassTau32Tag80eff_nocontain . assign(m_num_fatjets_keep, -1000. );
    }

    if (m_runSmoothWZtag) {
      m_rljet_smooth16W_Tag50eff . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16W_Tag80eff . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Z_Tag50eff . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Z_Tag80eff . assign(m_num_fatjets_keep, -1000. );
    }

    if (m_runSmoothWZtag && m_runSmoothUncontained) {
      m_rljet_smooth16W_Tag50eff_nocontain . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16W_Tag80eff_nocontain . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Z_Tag50eff_nocontain . assign(m_num_fatjets_keep, -1000. );
      m_rljet_smooth16Z_Tag80eff_nocontain . assign(m_num_fatjets_keep, -1000. );
    }

    if (m_savePhoton) {
      m_photon0_ptcone20     = -1000.;
      m_photon0_ptcone40     = -1000.;
      m_photon0_topoetcone20 = -1000.;
      m_photon0_topoetcone40 = -1000.;
      m_photon0_pt           = -1000.;
      m_photon0_eta          = -1000.;
      m_photon0_phi          = -1000.;
    }

    if(m_runMVAtag) {
      m_rljet_topTag_BDT_qqb            . assign(m_num_fatjets_keep, -1000. );
      m_rljet_topTag_BDT_qqb_score      . assign(m_num_fatjets_keep, -1000. );
      m_rljet_wTag_BDT_qq               . assign(m_num_fatjets_keep, -1000. );
      m_rljet_wTag_BDT_qq_score         . assign(m_num_fatjets_keep, -1000. );

      m_rljet_topTag_DNN_qqb            . assign(m_num_fatjets_keep, -1000. );
      m_rljet_topTag_DNN_qqb_score      . assign(m_num_fatjets_keep, -1000. );
      m_rljet_wTag_DNN_qq               . assign(m_num_fatjets_keep, -1000. );
      m_rljet_wTag_DNN_qq_score         . assign(m_num_fatjets_keep, -1000. );

      m_rljet_topTag_TopoTagger_20wp    . assign(m_num_fatjets_keep, -1000. );
      m_rljet_topTag_TopoTagger_50wp    . assign(m_num_fatjets_keep, -1000. );
      m_rljet_topTag_TopoTagger_80wp    . assign(m_num_fatjets_keep, -1000. );
      m_rljet_topTag_TopoTagger_score   . assign(m_num_fatjets_keep, -1000. );
    }

    if (on_nominal_branch) {
        m_NPV = -1000;

        if (m_config->isMC()) {
          m_rljet_pdgid . assign(m_num_fatjets_keep, 0);
          m_rljet_matched_parton_pt  . assign(m_num_fatjets_keep, -1000.);
          m_rljet_matched_parton_eta . assign(m_num_fatjets_keep, -1000.);
          m_rljet_matched_parton_phi . assign(m_num_fatjets_keep, -1000.);
          m_rljet_matched_parton_m   . assign(m_num_fatjets_keep, -1000.);

          m_tljet_pt  . assign(m_num_fatjets_keep, -1000.);
          m_tljet_eta . assign(m_num_fatjets_keep, -1000.);
          m_tljet_phi . assign(m_num_fatjets_keep, -1000.);
          m_tljet_m   . assign(m_num_fatjets_keep, -1000.);

          m_tljet_mjj  = -1000.;
          m_parton_mjj = -1000.;

          m_pid1 = 0;
          m_pid2 = 0;
        }

        m_rljet_count     = 0;
        m_rljet_mjj       = -1000.;
        m_rljet_ptasym    = -1000.;
        m_rljet_mass_asym = -1000.;
        m_rljet_dy        = -1000.;
        m_rljet_dR        = -1000.;
        m_rljet_dphi      = -1000.;
        m_rljet_deta      = -1000.;

        m_rljet_pt_calo . assign(m_num_fatjets_keep, -1000. );
        m_rljet_m_calo  . assign(m_num_fatjets_keep, -1000. );
        m_rljet_m_ta    . assign(m_num_fatjets_keep, -1000. );
        m_rljet_pt_ta   . assign(m_num_fatjets_keep, -1000. );

        m_rljet_C2          . assign(m_num_fatjets_keep, -1000. );
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
        m_rljet_n_constituents    . assign(m_num_fatjets_keep, -1000 );

        // nClusters study
        m_rljet_fractional_pt_0     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_1     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_2     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_3     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_4     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_5     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_6     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_7     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_8     . assign(m_num_fatjets_keep, -1000 );
        m_rljet_fractional_pt_9     . assign(m_num_fatjets_keep, -1000 );

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
    // int pt_cut = 600000; // MeV
    // Before any cuts
    // count clusters in large r jets
    // for (auto large_jet : event.m_largeJets){
    //   // printf("large pt: %f\n", large_jet->pt());
    //   if (large_jet->pt() > pt_cut){
    //     l_jet_nClusters_hist->Fill(large_jet->numConstituents());

        // double large_r_jet_pt = large_jet->pt();
        // try{

        //   // Extract jet constituents from jet
        //   std::vector<xAOD::JetConstituent> clusters = large_jet->getConstituents().asSTLVector();
        //   // Sort them by pt (using lambda function for sorting)
        //   std::sort(clusters.begin(), clusters.end(),
        //     [](const xAOD::JetConstituent & a, const xAOD::JetConstituent & b) -> bool
        //     {
        //       return a.pt() > b.pt();
        //     });

        //   float cluster_pt = clusters[9]->pt();
        //   l_jet_fractional_pt->Fill(cluster_pt/large_jet->pt());
        // }catch(...){
        //   // l_jet_fractional_pt->Fill(-1);
        // }

    //   }
      
    //   // printf("Large-R Jet nclusters %d\n", large_jet->numConstituents());
    // }

    // for (auto small_jet : event.m_jets){
    //   // printf("small pt: %f\n", small_jet->pt());
    //   if (small_jet->pt() > pt_cut){
    //     s_jet_nClusters_hist->Fill(small_jet->numConstituents());
    //   }
    //   // printf("Small-R Jet nclusters %d\n", small_jet->numConstituents());
    // }
  


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


    for (auto jet : event.m_largeJets) rljets.push_back(jet);
    if (rljets.empty()) return;

    auto pt_sort_func = [](const xAOD::IParticle* p1, const xAOD::IParticle* p2){
      return p1->pt() > p2->pt();
    };
    auto mass_sort_func = [](const xAOD::IParticle* p1, const xAOD::IParticle* p2){
      return p1->m() > p2->m();
    };

    if (m_massOrderJets) {
      std::sort(rljets.begin(), rljets.end(), mass_sort_func);
    } else {
      std::sort(rljets.begin(), rljets.end(), pt_sort_func);
    }

    // so that we never waste time looping over or truth-matching un-kept jets
    if (rljets.size() > m_num_fatjets_keep) rljets.resize(m_num_fatjets_keep);

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

        // Storing TAccept objects
        if (m_runSmoothToptag) {

          m_rljet_smooth16Top_Tau32Split23Tag50eff[i] = static_cast<int>(convertSmoothedTopTaggerResult(
            topTagger16_Tau32Split23_50eff->tag(*rljets[i]), smoothedTopTaggerConfig::Tau32Split23));

          m_rljet_smooth16Top_Tau32Split23Tag80eff[i] = static_cast<int>(convertSmoothedTopTaggerResult(
          	topTagger16_Tau32Split23_80eff->tag(*rljets[i]), smoothedTopTaggerConfig::Tau32Split23));

          m_rljet_smooth16Top_MassTau32Tag50eff[i]    = static_cast<int>(convertSmoothedTopTaggerResult(
           topTagger16_MassTau32_50eff->tag(*rljets[i]), smoothedTopTaggerConfig::MassTau32));

          m_rljet_smooth16Top_MassTau32Tag80eff[i]    = static_cast<int>(convertSmoothedTopTaggerResult(
          	topTagger16_MassTau32_80eff->tag(*rljets[i]), smoothedTopTaggerConfig::MassTau32));

          m_rljet_smooth16Top_QwTau32Tag50eff[i]      = static_cast<int>(convertSmoothedTopTaggerResult(
          	topTagger16_QwTau32_50eff->tag(*rljets[i]), smoothedTopTaggerConfig::QwTau32));

          m_rljet_smooth16Top_QwTau32Tag80eff[i]      = static_cast<int>(convertSmoothedTopTaggerResult(
          	topTagger16_QwTau32_80eff->tag(*rljets[i]), smoothedTopTaggerConfig::QwTau32));
        }
        
        if(m_runMVAtag) {
          // BDT
          // output information from BDT tool similar to preRecWTagger, reuse the conversion method
          // top-tag (qqb containment top definition)

          m_rljet_topTag_BDT_qqb[i] = static_cast<int>(convertMVATaggerResult(
                      m_topTagger_BDT_qqb->tag(*rljets[i])));
          m_rljet_topTag_BDT_qqb_score[i] = rljets[i]->auxdata<float>("BDTTaggerTopQuark80_Score");

          // W-tag (qq containment W definition)
          m_rljet_wTag_BDT_qq[i] = static_cast<int>(convertMVATaggerResult(
                      m_wTagger_BDT_qq->tag(*rljets[i])));
          m_rljet_wTag_BDT_qq_score[i] = rljets[i]->auxdata<float>("BDTTaggerWBoson50_Score");

          // DNN
          // top-tag (qqb contiainment top definition)
          m_rljet_topTag_DNN_qqb[i] = static_cast<int>(convertMVATaggerResult(
                      m_topTagger_DNN_qqb->tag(*rljets[i])));
          m_rljet_topTag_DNN_qqb_score[i] = rljets[i]->auxdata<float>("DNNTaggerTopQuark80_Score");

          // W-tag (qq contiainment top definition)
          m_rljet_wTag_DNN_qq[i] = static_cast<int>(convertMVATaggerResult(
                      m_wTagger_DNN_qq->tag(*rljets[i])));
          m_rljet_wTag_DNN_qq_score[i] = rljets[i]->auxdata<float>("DNNTaggerWBoson50_Score");

          // TopoTagger
          Root::TAccept topoTaggerResult = m_topTagger_TopoTagger->tag(*rljets[i]);
          m_rljet_topTag_TopoTagger_20wp[i] = static_cast<int>(convertMVATaggerResult(topoTaggerResult, "PassScore20"));
          m_rljet_topTag_TopoTagger_50wp[i] = static_cast<int>(convertMVATaggerResult(topoTaggerResult, "PassScore50"));
          m_rljet_topTag_TopoTagger_80wp[i] = static_cast<int>(convertMVATaggerResult(topoTaggerResult, "PassScore80"));
          m_rljet_topTag_TopoTagger_score[i] = rljets[i]->auxdata<float>("TopotaggerTopQuark_Score");

        }

        if (m_runSmoothToptag && m_runSmoothUncontained) {
          m_rljet_smooth16Top_MassTau32Tag50eff_nocontain[i]    = topTagger16_MassTau32_50eff_nocontain->tag(*rljets[i]);
          m_rljet_smooth16Top_MassTau32Tag80eff_nocontain[i]    = topTagger16_MassTau32_80eff_nocontain->tag(*rljets[i]);
        }

        if (m_runSmoothWZtag) {
          m_rljet_smooth16W_Tag50eff[i] = wTagger16_50eff->tag(*rljets[i]);
          m_rljet_smooth16W_Tag80eff[i] = wTagger16_80eff->tag(*rljets[i]);

          m_rljet_smooth16Z_Tag50eff[i] = zTagger16_50eff->tag(*rljets[i]);
          m_rljet_smooth16Z_Tag80eff[i] = zTagger16_80eff->tag(*rljets[i]);
        }

        if (m_runSmoothWZtag && m_runSmoothUncontained) {
          m_rljet_smooth16W_Tag50eff_nocontain[i] = wTagger16_50eff_nocontain->tag(*rljets[i]);
          m_rljet_smooth16W_Tag80eff_nocontain[i] = wTagger16_80eff_nocontain->tag(*rljets[i]);

          m_rljet_smooth16Z_Tag50eff_nocontain[i] = zTagger16_50eff_nocontain->tag(*rljets[i]);
          m_rljet_smooth16Z_Tag80eff_nocontain[i] = zTagger16_80eff_nocontain->tag(*rljets[i]);
        }

        if (m_savePhoton) {
          const xAOD::Photon* lead_photon = *std::max_element(event.m_photons.begin(), event.m_photons.end(), pt_sort_func);

          if (lead_photon) {
            m_photon0_pt           = lead_photon->pt();
            m_photon0_eta          = lead_photon->eta();
            m_photon0_phi          = lead_photon->phi();
            m_photon0_ptcone20     = lead_photon->auxdata<float>("ptvarcone20");
            m_photon0_ptcone40     = lead_photon->auxdata<float>("ptvarcone40");
            m_photon0_topoetcone20 = lead_photon->auxdata<float>("topoetcone20");
            m_photon0_topoetcone40 = lead_photon->auxdata<float>("topoetcone40");
          }
        }

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

            m_rljet_m_ta[i]    = calibP4_TA.M();
            m_rljet_pt_ta[i]   = calibP4_TA.Pt();
            m_rljet_m_calo[i]  = calibP4_calo.M();
            m_rljet_pt_calo[i] = calibP4_calo.Pt();

            if(rljets[i]->isAvailable<float>("C2")) {
              m_rljet_C2[i] = rljets[i]->auxdata<float>("C2");
            } else {
              m_rljet_C2[i] = (ecf2 != 0 ? ecf3 * ecf1 / TMath::Power(ecf2, 2) : -1);
            }

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

            m_rljet_n_constituents[i] = rljets[i]->numConstituents();


            /*******************/
            /* NCLUSTERS STUDY */
            /*******************/

            try{
              // Extract jet constituents from jet
              std::vector<xAOD::JetConstituent> clusters = rljets[i]->getConstituents().asSTLVector();
              // Sort them by pt (using lambda function for sorting)
              std::sort(clusters.begin(), clusters.end(),
                [](const xAOD::JetConstituent & a, const xAOD::JetConstituent & b) -> bool
                {
                  return a.pt() > b.pt();
                });

              if (clusters.size() >= 0) m_rljet_fractional_pt_0[i] = clusters[0]->pt()/rljets[i]->pt();
              if (clusters.size() >= 1) m_rljet_fractional_pt_1[i] = clusters[1]->pt()/rljets[i]->pt();
              if (clusters.size() >= 2) m_rljet_fractional_pt_2[i] = clusters[2]->pt()/rljets[i]->pt();
              if (clusters.size() >= 3) m_rljet_fractional_pt_3[i] = clusters[3]->pt()/rljets[i]->pt();
              if (clusters.size() >= 4) m_rljet_fractional_pt_4[i] = clusters[4]->pt()/rljets[i]->pt();
              if (clusters.size() >= 5) m_rljet_fractional_pt_5[i] = clusters[5]->pt()/rljets[i]->pt();
              if (clusters.size() >= 6) m_rljet_fractional_pt_6[i] = clusters[6]->pt()/rljets[i]->pt();
              if (clusters.size() >= 7) m_rljet_fractional_pt_7[i] = clusters[7]->pt()/rljets[i]->pt();
              if (clusters.size() >= 8) m_rljet_fractional_pt_8[i] = clusters[8]->pt()/rljets[i]->pt();
              if (clusters.size() >= 9) m_rljet_fractional_pt_9[i] = clusters[9]->pt()/rljets[i]->pt();

            }catch(...){
              // l_jet_fractional_pt->Fill(-1);
            }

            /*****************************/
            /* PDGID FROM TRUTH MATCHING */
            /*****************************/

            if (m_config->isMC())
            { // save corresponding truth jet variables
              const xAOD::JetContainer* truth_large_jets = nullptr;
              top::check( evtStore()->retrieve(truth_large_jets, m_config->sgKeyTruthLargeRJets()), "FAILURE");

              const xAOD::Jet* matched_tljet = get_nearest_jet_in_collection(rljets[i], truth_large_jets);

              if (matched_tljet != nullptr) {
                const float dR = top::deltaR(*rljets[i], *matched_tljet);
                if (dR < 0.75) {
                  m_tljet_pt[i] = matched_tljet->pt();
                  m_tljet_eta[i] = matched_tljet->eta();
                  m_tljet_phi[i] = matched_tljet->phi();
                  m_tljet_m[i] = matched_tljet->m();
                  const xAOD::TruthParticle* matched_parton = match_jet_to_parton(event.m_truth, matched_tljet, 0.75);
                  if (matched_parton) {
                    m_rljet_pdgid[i]              = matched_parton->pdgId();
                    m_rljet_matched_parton_pt[i]  = matched_parton->pt();
                    m_rljet_matched_parton_eta[i] = matched_parton->eta();
                    m_rljet_matched_parton_phi[i] = matched_parton->phi();
                    m_rljet_matched_parton_m[i]   = matched_parton->m();
                  }
                }
              }

              /*
              for (const xAOD::TruthParticle* p : *(event.m_truth)) {
                Int_t pdgid = p->pdgId();

                if (p->m()/1000. > 10) {
                  std::cout << "truth mass: " << p->m()/1000. << " GeV" << std::endl;
                  std::cout << "truth pdgid: " << pdgid << std::endl;
                  std::cout << "has prod vtx: " << p->hasProdVtx() << std::endl;
                  std::cout << "has decay vtx: " << p->hasDecayVtx() << std::endl;
                  std::cout << "nParents: " << p->nParents() << std::endl;
                  std::cout << "nChildren: " << p->nChildren() << std::endl;
                }

                if (p->pdgId() > 1e6 && p->hasProdVtx() && p->hasDecayVtx()) {

                  const xAOD::TruthVertex* prod_vtx = p->prodVtx();
                  const xAOD::TruthVertex* decay_vtx = p->decayVtx();

                  if (prod_vtx && decay_vtx && prod_vtx->nIncomingParticles() == 2 && decay_vtx->nOutgoingParticles() == 2)
                  { // gg or qq -> X -> WW/WZ/ZZ resonance

                    const xAOD::TruthParticle* ip1 = prod_vtx->incomingParticle(0);
                    const xAOD::TruthParticle* ip2 = prod_vtx->incomingParticle(1);
                    const xAOD::TruthParticle* op1 = decay_vtx->outgoingParticle(0);
                    const xAOD::TruthParticle* op2 = decay_vtx->outgoingParticle(1);

                    const Int_t i1_pdg = ip1->pdgId();
                    const Int_t i2_pdg = ip2->pdgId();
                    const Int_t o1_pdg = op1->pdgId();
                    const Int_t o2_pdg = op2->pdgId();

                    const bool quark_produced = fabs(i1_pdg) <= 6 && fabs(i2_pdg) <=6;
                    const bool gluon_produced = i1_pdg == 21 && i2_pdg == 21;

                    const bool o1_isWZ = fabs(o1_pdg) == 24 || fabs(o1_pdg) == 23;
                    const bool o2_isWZ = fabs(o2_pdg) == 24 || fabs(o2_pdg) == 23;

                    if ((quark_produced || gluon_produced) && o1_isWZ && o2_isWZ) {
                      std::cout << "truth resonance mass: " << p->m()/1000. << " GeV" << std::endl;
                      std::cout << "truth resonance pdgid: " << pdgid << std::endl;
                      std::cout << "initial parton 1 pdgid: " << i1_pdg << std::endl;
                      std::cout << "initial parton 2 pdgid: " << i2_pdg << std::endl;
                      std::cout << "decay product 1 pdgid: " << o1_pdg << std::endl;
                      std::cout << "decay product 2 pdgid: " << o2_pdg << std::endl;
                    }
                  }
                }
              } // end of saving truth information for BSM resonance
              */
            } // end of saving truth jet variables

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

          m_rljet_ptasym    = ( fabs(rljets[0]->pt() - rljets[1]->pt()) ) / ( rljets[0]->pt() + rljets[1]->pt() );
          m_rljet_mass_asym = ( fabs(rljets[0]->m() - rljets[1]->m()) ) / ( rljets[0]->m() + rljets[1]->m() );
          m_rljet_dy        = fabs(rljets[0]->rapidity() - rljets[1]->rapidity());
          m_rljet_dR        = top::deltaR(*rljets[0], *rljets[1]);
          m_rljet_dphi      = top::deltaPhi(*rljets[0], *rljets[1]);
          m_rljet_deta      = fabs(rljets[0]->eta() - rljets[1]->eta());

          if (m_config->isMC()) {
            if (m_tljet_pt[0] > 0 && m_tljet_pt[1] > 0) {
              v_jet0.SetPtEtaPhiM(m_tljet_pt[0], m_tljet_eta[0], m_tljet_phi[0], m_tljet_m[0]);
              v_jet1.SetPtEtaPhiM(m_tljet_pt[1], m_tljet_eta[1], m_tljet_phi[1], m_tljet_m[1]);
              m_tljet_mjj = (v_jet0 + v_jet1).M();
            }

            if (m_rljet_matched_parton_pt[0] > 0 && m_rljet_matched_parton_pt[1] > 0) {
              v_jet0.SetPtEtaPhiM(m_rljet_matched_parton_pt[0], m_rljet_matched_parton_eta[0], m_rljet_matched_parton_phi[0], m_rljet_matched_parton_m[0]);
              v_jet1.SetPtEtaPhiM(m_rljet_matched_parton_pt[1], m_rljet_matched_parton_eta[1], m_rljet_matched_parton_phi[1], m_rljet_matched_parton_m[1]);
              m_parton_mjj = (v_jet0 + v_jet1).M();
            }

            if (event.m_truthEvent && event.m_truthEvent->size() > 0) {
              auto truth_evt = event.m_truthEvent->at(0);

              if (truth_evt) {
                m_pid1 = truth_evt->pdfInfo().pdgId1;
                m_pid2 = truth_evt->pdfInfo().pdgId2;
              }
            }

          }
        }

        // auto mcChannelNumber = event.info->mcChanelNumber();

        // const bool is_highpT_dijet_slice =
        //   (mcChannelNumber >= 361029 && mcChannelNumber <= 361032)
        //   || (mcChannelNumber >= 426049 && mcChannelNumber <= 426052);

        if (m_runHTT && !event.m_isLoose)
            this->runHTTAndFillTree();

        if (m_runSD && !event.m_isLoose)
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

    if (pt_sorted_ca15jets.size() < 2) return;

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
        std::vector<fastjet::PseudoJet> constituents;
        
        // also get groomed constituents 
        const xAOD::JetConstituentVector groomed_clusters = rljets[i]->getConstituents();
        std::vector<fastjet::PseudoJet> groomed_constituents;
        if (groomed_clusters.empty() || AssociatedClusters_forSD.empty())
            return;
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
        std::vector<fastjet::PseudoJet> calib_subjets;

        // Get trackjets and their B tag status
        std::vector<int> btag_uncalib(uncalib_subjets.size(), 0);
        //std::vector<int> btag_special2(new_subjets_2.size(), 0);
        std::vector<int> btag_special3(new_subjets_3.size(), 0);
        std::vector<int> btag(calib_subjets.size(), 0);
        std::vector<int> tracktag;    
        std::vector<int> btag_special5(dcut2_subjets.size(),0);
        std::vector<int> btag_special6(dcut3_subjets.size(),0);

        std::vector<fastjet::PseudoJet> uncalib_subjets_UP;
        std::vector<fastjet::PseudoJet> uncalib_subjets_DOWN;

        for ( unsigned int i = 0; i < uncalib_subjets.size(); i++ ) {
            uncalib_subjets_UP.push_back( uncalib_subjets[i] * 1.03 );
            uncalib_subjets_DOWN.push_back( uncalib_subjets[i] * 0.97 );
        }

        std::vector<fastjet::PseudoJet> dcut2_subjets_UP;
        std::vector<fastjet::PseudoJet> dcut2_subjets_DOWN;

        for ( unsigned int i = 0; i < dcut2_subjets.size(); i++ ) {
            dcut2_subjets_UP.push_back( dcut2_subjets[i] * 1.03 );
            dcut2_subjets_DOWN.push_back( dcut2_subjets[i] * 0.97 );
        }

        std::vector<fastjet::PseudoJet> new_subjets_3_UP;
        std::vector<fastjet::PseudoJet> new_subjets_3_DOWN;

        for ( unsigned int i = 0; i < new_subjets_3.size(); i++ ) {
            new_subjets_3_UP.push_back( new_subjets_3[i] * 1.03 );
            new_subjets_3_DOWN.push_back( new_subjets_3[i] * 0.97 );
        }

        m_rljet_SDt_dcut[i] = tagger_SDt_win50_btag1->tagJetFromSubjets(dcut2_subjets,btag_special5);
        m_rljet_SDw_dcut[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(dcut2_subjets,btag_special5);

        if(doSystShifts) {
            m_rljet_SDt_dcut_UP[i] = tagger_SDt_win50_btag1->tagJetFromSubjets(dcut2_subjets_UP,btag_special5);
            m_rljet_SDt_dcut_DOWN[i] = tagger_SDt_win50_btag1->tagJetFromSubjets(dcut2_subjets_DOWN,btag_special5);

            m_rljet_SDw_dcut_UP[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(dcut2_subjets_UP,btag_special5);
            m_rljet_SDw_dcut_DOWN[i] = tagger_SDw_win20_btag0->tagJetFromSubjets(dcut2_subjets_DOWN,btag_special5);
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

std::vector<fastjet::PseudoJet>
DataMCbackgroundEventSaver::JetconVecToPseudojet (xAOD::JetConstituentVector input)
{
  std::vector<fastjet::PseudoJet> constituents;

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

int DataMCbackgroundEventSaver::getBranchStatus(top::TreeManager const * treeManager, std::string const & variableName)
{
    // do not store any branches in the truth tree, we do not need it at all
    if(treeManager->name().compare("truth") == 0) {
        return 0;
    }
    /* Remove branches from output n-tuple using pattern matching */
    // small-R jets -- named "jet_blabla"
    if (variableName.find("jet_") != std::string::npos
            && variableName.find("rljet_") == std::string::npos)
        return 0;

    // b-tagging scale factors
    // calo b-tagging scale factors
    if (variableName.find("weight_bTagSF") != std::string::npos)
        return 0;

    // lepton indiv SF weights (these are not normally used in analysis)
    if (variableName.find("weight_indiv_SF_EL")!=std::string::npos || variableName.find("weight_indiv_SF_MU")!=std::string::npos)
        return 0;

    // unused large-R jet variables -- "ljet_blabla"
    if (variableName.find("ljet_") != std::string::npos
            && variableName.find("rljet_") == std::string::npos)
      return 0;

    return -1;
}

}

