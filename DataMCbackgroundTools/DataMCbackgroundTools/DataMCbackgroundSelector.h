#ifndef DataMCbackgroundSelector_h
#define DataMCbackgroundSelector_h

// Local Package Imports
#include "DataMCbackgroundTools/HistoPack.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "DataMCbackgroundTools/WeightTool.h"

// ROOTCORE/Athena/Framework imports
#include "TopExamples/AnalysisTools.h"

// ROOT imports
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TNamed.h>
#include <TROOT.h>
#include <TSelector.h>
#include <TStyle.h>
#include <TF1.h>

// STL imports
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <math.h>

class DataMCbackgroundSelector;

typedef bool (DataMCbackgroundSelector::*EventSelector)(void);

class DataMCbackgroundSelector : public TSelector {
    public :
        TTree* fChain;   //!pointer to the analyzed TTree or TChain

        HistoPack* hp;
        TH2F* h_bdt_top_vs_mu;
        TH2F* h_bdt_w_vs_mu;
        TH2F* h_dnn_top_vs_mu;
        TH2F* h_dnn_w_vs_mu;

        static const std::unordered_map<std::string, EventSelector> available_event_selectors;
        EventSelector chosen_event_selector;

        bool NO_SELECTION(void) { return true; };

        bool GAMMAJET_LOOSE(void) {
          float jet_pt = rljet_pt_comb->at(0) / 1000;
          return jet_pt > 200;
        };

        bool GAMMAJET_TIGHT(void) {
          float jet_pt = rljet_pt_comb->at(0) / 1000;
          return jet_pt > 200;
        };

        std::ofstream output_log;

        // CONFIGURATION OPTIONS
        const std::string output_filepath;
        const std::string root_dir_str;
        const std::string sub_dir_str;
        const std::string data_trigger_str;

        const bool operating_on_mc;
        const bool on_nominal_branch;
        bool processing_dijet_slice;
        int dijet_slice_number;
        const float luminosity;
        float SF_lumi_Fb;

        // keep track of highest weight event in file for finding pesky high-weight events
        float max_weight;

        bool keptPhotons;
        bool default_photon_vars;
        bool ranSD;

        // // BDT's for top and W tagging -- contained top/W def
        // unique_ptr<BDTWTopTagger> BDT_topTag;
        // unique_ptr<BDTWTopTagger> BDT_WTag;
        // // DNN for top tagging -- contained top/W def
        // unique_ptr<DNNWTopTagger> DNN_topTag;
        // unique_ptr<DNNWTopTagger> DNN_WTag;

        std::map<std::string, double> jetMoments;

        TF1* f_sdw;
        TF1* f_sdtop;

        /******************/
        /***** TOOLS ******/
        /******************/

        std::unique_ptr<WeightTool> weight_tool;

        // various maps used to store tag pass/fail status
        std::unordered_map<std::string, bool> smooth_tag_map;
        std::unordered_map<std::string, bool> smooth_tag_map_nominal;
        std::unordered_map<std::string, bool> ntrk_prerec_tag_map;
        std::unordered_map<std::string, bool> SD_nominal_tag_map;
        std::unordered_map<std::string, bool> SD_systematic_tag_map;
        std::unordered_map<std::string, bool> mva_tag_map;

        std::unordered_map<std::string, Float_t> fvars;
        std::unordered_map<std::string, UInt_t> uvars;
        std::unordered_map<std::string, ULong64_t> lvars;
        std::unordered_map<std::string, std::vector<float>> vvars;

        std::unordered_map<std::string, TBranch*> branches;

        /***********************************/
        /***** CONNECT LEAFS/BRANCHES ******/
        /***********************************/

        // Declaration of leaf types
        Char_t          HLT_trigger;
        Float_t         weight_mc;
        Float_t         weight_pileup;

        Float_t         weight_photonSF;
        Float_t         weight_photonSF_effIso;

        Float_t         weight_photonSF_ID_UP;
        Float_t         weight_photonSF_ID_DOWN;
        Float_t         weight_photonSF_effTrkIso_UP;
        Float_t         weight_photonSF_effTrkIso_DOWN;

        ULong64_t       eventNumber;
        UInt_t          runNumber;
        UInt_t          randomRunNumber;
        UInt_t          mcChannelNumber;
        Float_t         mu;
        UInt_t          backgroundFlags;

        Float_t         photon_ptcone20;
        Float_t         photon_ptcone40;
        Float_t         photon_topoetcone20;
        Float_t         photon_topoetcone40;

        std::vector<float>   *ph_pt;
        std::vector<float>   *ph_phi;
        std::vector<float>   *ph_eta;
        std::vector<float>   *ph_e;

        Float_t         photon0_phi;

        std::vector<float>   *rljet_eta;
        std::vector<float>   *rljet_phi;
        std::vector<float>   *rljet_m_comb;
        std::vector<float>   *rljet_pt_comb;
        std::vector<float>   *rljet_D2;
        std::vector<float>   *rljet_Tau32_wta;
        std::vector<int>     *rljet_smooth16Top_Tau32Split23Tag50eff;
        std::vector<int>     *rljet_smooth16Top_Tau32Split23Tag80eff;
        std::vector<int>     *rljet_smooth16Top_MassTau32Tag50eff;
        std::vector<int>     *rljet_smooth16Top_MassTau32Tag80eff;
        std::vector<int>     *rljet_smooth16Top_QwTau32Tag50eff;
        std::vector<int>     *rljet_smooth16Top_QwTau32Tag80eff;
        std::vector<int>     *rljet_smooth16WTag_50eff;
        std::vector<int>     *rljet_smooth16WTag_80eff;
        std::vector<int>     *rljet_smooth16ZTag_50eff;
        std::vector<int>     *rljet_smooth16ZTag_80eff;

        std::vector<int>     *rljet_pdgid;

        // std::vector<int> *rljet_smooth16Top_MassTau32Tag50eff_nocontain;
        // std::vector<int> *rljet_smooth16Top_MassTau32Tag80eff_nocontain;
        // std::vector<int> *rljet_smooth16WTag_50eff_nocontain;
        // std::vector<int> *rljet_smooth16WTag_80eff_nocontain;
        // std::vector<int> *rljet_smooth16ZTag_50eff_nocontain;
        // std::vector<int> *rljet_smooth16ZTag_80eff_nocontain;

        // 2017 MVA
        // std::vector<float> *rljet_BDT_score_top_qqb;
        // std::vector<float> *rljet_BDT_score_top_inclusive;
        // std::vector<float> *rljet_BDT_score_w;
        // std::vector<float> *rljet_DNN_score_top;
        // std::vector<float> *rljet_DNN_score_w;

        // BDT's
        std::vector<int> *rljet_topTag_BDT_qqb;
        std::vector<float> *rljet_topTag_BDT_qqb_score;
        std::vector<int> *rljet_wTag_BDT_qq;
        std::vector<float> *rljet_wTag_BDT_qq_score;
        // DNN's
        std::vector<int> *rljet_topTag_DNN_qqb;
        std::vector<float> *rljet_topTag_DNN_qqb_score;
        std::vector<int> *rljet_wTag_DNN_qq;
        std::vector<float> *rljet_wTag_DNN_qq_score;
        // Topocluster DNN's
        std::vector<int> *rljet_topTag_TopoTagger_20wp;
        std::vector<int> *rljet_topTag_TopoTagger_50wp;
        std::vector<int> *rljet_topTag_TopoTagger_80wp;
        std::vector<float> *rljet_topTag_TopoTagger_score;

        Int_t           NPV;
        Int_t           rljet_count;
        Float_t         rljet_mjj;
        Float_t         rljet_ptasym;
        Float_t         rljet_dy;
        Float_t         rljet_dR;
        Float_t         rljet_dphi;
        Float_t         rljet_deta;
        std::vector<float>   *rljet_m_calo;
        std::vector<float>   *rljet_pt_calo;
        std::vector<float>   *rljet_m_ta;
        std::vector<float>   *rljet_pt_ta;
        std::vector<int>     *rljet_smooth15Top_MassTau32Tag50eff;
        std::vector<int>     *rljet_smooth15Top_MassTau32Tag80eff;
        std::vector<int>     *rljet_smooth15WTag_50eff;
        std::vector<int>     *rljet_smooth15WTag_25eff;
        std::vector<int>     *rljet_smooth15ZTag_50eff;
        std::vector<int>     *rljet_smooth15ZTag_25eff;
        std::vector<float>   *rljet_Tau1_wta;
        std::vector<float>   *rljet_Tau2_wta;
        std::vector<float>   *rljet_Tau3_wta;
        std::vector<float>   *rljet_ECF1;
        std::vector<float>   *rljet_ECF2;
        std::vector<float>   *rljet_ECF3;
        std::vector<float>   *rljet_FoxWolfram0;
        std::vector<float>   *rljet_FoxWolfram2;
        std::vector<float>   *rljet_Qw;
        std::vector<float>   *rljet_Angularity;
        std::vector<float>   *rljet_Aplanarity;
        std::vector<float>   *rljet_Dip12;
        std::vector<float>   *rljet_KtDR;
        std::vector<float>   *rljet_Mu12;
        std::vector<float>   *rljet_PlanarFlow;
        std::vector<float>   *rljet_Sphericity;
        std::vector<float>   *rljet_Split12;
        std::vector<float>   *rljet_Split23;
        std::vector<float>   *rljet_Split34;
        std::vector<float>   *rljet_ThrustMaj;
        std::vector<float>   *rljet_ThrustMin;
        std::vector<float>   *rljet_ZCut12;
        std::vector<int>     *rljet_NTrimSubjets;
        std::vector<int>     *rljet_ungroomed_ntrk500;
        std::vector<int>     *rljet_n_constituents;
        std::vector<float>   *htt_pt_def;
        std::vector<float>   *htt_eta_def;
        std::vector<float>   *htt_phi_def;
        std::vector<float>   *htt_m_def;
        std::vector<float>   *htt_m123_def;
        std::vector<float>   *htt_m23m123_def;
        std::vector<float>   *htt_atan1312_def;
        std::vector<int>     *htt_nTagCands_def;
        std::vector<int>     *htt_tag_def;
        std::vector<float>   *htt_pts1_def;
        std::vector<float>   *htt_pts2_def;
        std::vector<float>   *htt_pts3_def;
        std::vector<float>   *htt_pt_sjcalib1030;
        std::vector<float>   *htt_eta_sjcalib1030;
        std::vector<float>   *htt_phi_sjcalib1030;
        std::vector<float>   *htt_m_sjcalib1030;
        std::vector<float>   *htt_m123_sjcalib1030;
        std::vector<float>   *htt_m23m123_sjcalib1030;
        std::vector<float>   *htt_atan1312_sjcalib1030;
        std::vector<int>     *htt_nTagCands_sjcalib1030;
        std::vector<int>     *htt_tag_sjcalib1030;
        std::vector<float>   *htt_pts1_sjcalib1030;
        std::vector<float>   *htt_pts2_sjcalib1030;
        std::vector<float>   *htt_pts3_sjcalib1030;
        std::vector<float>   *htt_pt_sjcalib0970;
        std::vector<float>   *htt_eta_sjcalib0970;
        std::vector<float>   *htt_phi_sjcalib0970;
        std::vector<float>   *htt_m_sjcalib0970;
        std::vector<float>   *htt_m123_sjcalib0970;
        std::vector<float>   *htt_m23m123_sjcalib0970;
        std::vector<float>   *htt_atan1312_sjcalib0970;
        std::vector<int>     *htt_nTagCands_sjcalib0970;
        std::vector<int>     *htt_tag_sjcalib0970;
        std::vector<float>   *htt_pts1_sjcalib0970;
        std::vector<float>   *htt_pts2_sjcalib0970;
        std::vector<float>   *htt_pts3_sjcalib0970;
        std::vector<float>   *htt_caJet_pt;
        std::vector<float>   *htt_caJet_eta;
        std::vector<float>   *htt_caJet_phi;
        std::vector<float>   *htt_caJet_m;
        Int_t           caJet_count;
        std::vector<float>   *htt_caGroomJet_pt_def;
        std::vector<float>   *htt_caGroomJet_eta_def;
        std::vector<float>   *htt_caGroomJet_phi_def;
        std::vector<float>   *htt_caGroomJet_m_def;
        std::vector<float>   *htt_caGroomJet_pt_sjcalib1030;
        std::vector<float>   *htt_caGroomJet_eta_sjcalib1030;
        std::vector<float>   *htt_caGroomJet_phi_sjcalib1030;
        std::vector<float>   *htt_caGroomJet_m_sjcalib1030;
        std::vector<float>   *htt_caGroomJet_pt_sjcalib0970;
        std::vector<float>   *htt_caGroomJet_eta_sjcalib0970;
        std::vector<float>   *htt_caGroomJet_phi_sjcalib0970;
        std::vector<float>   *htt_caGroomJet_m_sjcalib0970;

        std::vector<float>* rljet_SDw_calib;
        std::vector<float>* rljet_SDw_uncalib;
        std::vector<float>* rljet_SDw_combined;
        std::vector<float>* rljet_SDw_dcut;
        std::vector<float>* rljet_SDt_calib;
        std::vector<float>* rljet_SDt_uncalib;
        std::vector<float>* rljet_SDt_combined;
        std::vector<float>* rljet_SDt_dcut;
        std::vector<float>* rljet_SDw_calib_DOWN;
        std::vector<float>* rljet_SDw_uncalib_DOWN;
        std::vector<float>* rljet_SDw_combined_DOWN;
        std::vector<float>* rljet_SDw_dcut_DOWN;
        std::vector<float>* rljet_SDt_calib_DOWN;
        std::vector<float>* rljet_SDt_uncalib_DOWN;
        std::vector<float>* rljet_SDt_combined_DOWN;
        std::vector<float>* rljet_SDt_dcut_DOWN;
        std::vector<float>* rljet_SDw_calib_UP;
        std::vector<float>* rljet_SDw_uncalib_UP;
        std::vector<float>* rljet_SDw_combined_UP;
        std::vector<float>* rljet_SDw_dcut_UP;
        std::vector<float>* rljet_SDt_calib_UP;
        std::vector<float>* rljet_SDt_uncalib_UP;
        std::vector<float>* rljet_SDt_combined_UP;
        std::vector<float>* rljet_SDt_dcut_UP;

        // List of branches
        TBranch        *b_HLT_trigger;  //!
        TBranch        *b_weight_mc;   //!
        TBranch        *b_weight_pileup;   //!

        TBranch        *b_weight_photonSF; //!
        TBranch        *b_weight_photonSF_effIso; //!
        TBranch        *b_weight_photonSF_ID_UP; //!
        TBranch        *b_weight_photonSF_ID_DOWN; //!
        TBranch        *b_weight_photonSF_effTrkIso_UP; //!
        TBranch        *b_weight_photonSF_effTrkIso_DOWN; //!

        TBranch        *b_eventNumber;   //!
        TBranch        *b_runNumber;   //!
        TBranch        *b_randomRunNumber;   //!
        TBranch        *b_mcChannelNumber;   //!
        TBranch        *b_mu;   //!
        TBranch        *b_backgroundFlags;   //!

        TBranch* b_photon_ptcone20; //!
        TBranch* b_photon_ptcone40; //!
        TBranch* b_photon_topoetcone20; //!
        TBranch* b_photon_topoetcone40; //!

        TBranch* b_ph_pt; //!
        TBranch* b_ph_phi; //!
        TBranch* b_ph_eta; //!
        TBranch* b_ph_e; //!

        TBranch* b_photon0_phi; //!

        // TBranch        *b_met_met;   //!
        // TBranch        *b_met_phi;   //!
        TBranch        *b_rljet_eta;   //!
        TBranch        *b_rljet_phi;   //!
        TBranch        *b_rljet_m_comb;   //!
        TBranch        *b_rljet_pt_comb;   //!
        TBranch        *b_rljet_D2;   //!
        TBranch        *b_rljet_Tau32_wta;   //!
        TBranch        *b_m_rljet_smooth16Top_Tau32Split23Tag50eff;   //!
        TBranch        *b_m_rljet_smooth16Top_Tau32Split23Tag80eff;   //!
        TBranch        *b_m_rljet_smooth16Top_MassTau32Tag50eff;   //!
        TBranch        *b_m_rljet_smooth16Top_MassTau32Tag80eff;   //!
        TBranch        *b_m_rljet_smooth16Top_QwTau32Tag50eff;   //!
        TBranch        *b_m_rljet_smooth16Top_QwTau32Tag80eff;   //!
        TBranch        *b_rljet_smooth16WTag_50eff;   //!
        TBranch        *b_rljet_smooth16WTag_80eff;   //!
        TBranch        *b_rljet_smooth16ZTag_50eff;   //!
        TBranch        *b_rljet_smooth16ZTag_80eff;   //!

        TBranch        *b_rljet_pdgid;   //!

        // TBranch *b_rljet_smooth16Top_MassTau32Tag50eff_nocontain; //!
        // TBranch *b_rljet_smooth16Top_MassTau32Tag80eff_nocontain; //!
        // TBranch *b_rljet_smooth16WTag_50eff_nocontain; //!
        // TBranch *b_rljet_smooth16WTag_80eff_nocontain; //!
        // TBranch *b_rljet_smooth16ZTag_50eff_nocontain; //!
        // TBranch *b_rljet_smooth16ZTag_80eff_nocontain; //!

        // 2017 MVA
        // TBranch* b_rljet_BDT_score_top_qqb; //!
        // TBranch* b_rljet_BDT_score_top_inclusive; //!
        // TBranch* b_rljet_BDT_score_w; //!
        // TBranch* b_rljet_DNN_score_top; //!
        // TBranch* b_rljet_DNN_score_w; //!

        TBranch        *b_rljet_topTag_BDT_qqb; //!
        TBranch        *b_rljet_topTag_BDT_qqb_score; //!
        TBranch        *b_rljet_wTag_BDT_qq; //!
        TBranch        *b_rljet_wTag_BDT_qq_score; //!
        TBranch        *b_rljet_topTag_DNN_qqb_score; //!
        TBranch        *b_rljet_topTag_DNN_qqb; //!
        TBranch        *b_rljet_wTag_DNN_qq_score; //!
        TBranch        *b_rljet_wTag_DNN_qq; //!
        TBranch        *b_rljet_topTag_TopoTagger_20wp; //!
        TBranch        *b_rljet_topTag_TopoTagger_50wp; //!
        TBranch        *b_rljet_topTag_TopoTagger_80wp; //!
        TBranch        *b_rljet_topTag_TopoTagger_score; //!


        TBranch        *b_NPV;   //!
        TBranch        *b_rljet_count;   //!
        TBranch        *b_rljet_mjj;   //!
        TBranch        *b_rljet_ptasym;   //!
        TBranch        *b_rljet_dy;   //!
        TBranch        *b_rljet_dR;   //!
        TBranch        *b_rljet_dphi;   //!
        TBranch        *b_rljet_deta;   //!
        TBranch        *b_rljet_m_calo;   //!
        TBranch        *b_rljet_pt_calo;   //!
        TBranch        *b_rljet_m_ta;   //!
        TBranch        *b_rljet_pt_ta;   //!
        TBranch        *b_m_rljet_smooth15Top_MassTau32Tag50eff;   //!
        TBranch        *b_m_rljet_smooth15Top_MassTau32Tag80eff;   //!
        TBranch        *b_rljet_smooth15WTag_50eff;   //!
        TBranch        *b_rljet_smooth15WTag_25eff;   //!
        TBranch        *b_rljet_smooth15ZTag_50eff;   //!
        TBranch        *b_rljet_smooth15ZTag_25eff;   //!
        TBranch        *b_rljet_Tau1_wta;   //!
        TBranch        *b_rljet_Tau2_wta;   //!
        TBranch        *b_rljet_Tau3_wta;   //!
        TBranch        *b_rljet_ECF1;   //!
        TBranch        *b_rljet_ECF2;   //!
        TBranch        *b_rljet_ECF3;   //!
        TBranch        *b_rljet_FoxWolfram0;   //!
        TBranch        *b_rljet_FoxWolfram2;   //!
        TBranch        *b_rljet_Qw;   //!
        TBranch        *b_rljet_Angularity;   //!
        TBranch        *b_rljet_Aplanarity;   //!
        TBranch        *b_rljet_Dip12;   //!
        TBranch        *b_rljet_KtDR;   //!
        TBranch        *b_rljet_Mu12;   //!
        TBranch        *b_rljet_PlanarFlow;   //!
        TBranch        *b_rljet_Sphericity;   //!
        TBranch        *b_rljet_Split12;   //!
        TBranch        *b_rljet_Split23;   //!
        TBranch        *b_rljet_Split34;   //!
        TBranch        *b_rljet_ThrustMaj;   //!
        TBranch        *b_rljet_ThrustMin;   //!
        TBranch        *b_rljet_ZCut12;   //!
        TBranch        *b_rljet_NTrimSubjets;   //!
        TBranch        *b_rljet_ungroomed_ntrk500;   //!
        TBranch        *b_rljet_n_constituents;   //!
        TBranch        *b_htt_pt_def;   //!
        TBranch        *b_htt_eta_def;   //!
        TBranch        *b_htt_phi_def;   //!
        TBranch        *b_htt_m_def;   //!
        TBranch        *b_htt_m123_def;   //!
        TBranch        *b_htt_m23m123_def;   //!
        TBranch        *b_htt_atan1312_def;   //!
        TBranch        *b_htt_nTagCands_def;   //!
        TBranch        *b_htt_tag_def;   //!
        TBranch        *b_htt_pts1_def;   //!
        TBranch        *b_htt_pts2_def;   //!
        TBranch        *b_htt_pts3_def;   //!
        TBranch        *b_htt_pt_sjcalib1030;   //!
        TBranch        *b_htt_eta_sjcalib1030;   //!
        TBranch        *b_htt_phi_sjcalib1030;   //!
        TBranch        *b_htt_m_sjcalib1030;   //!
        TBranch        *b_htt_m123_sjcalib1030;   //!
        TBranch        *b_htt_m23m123_sjcalib1030;   //!
        TBranch        *b_htt_atan1312_sjcalib1030;   //!
        TBranch        *b_htt_nTagCands_sjcalib1030;   //!
        TBranch        *b_htt_tag_sjcalib1030;   //!
        TBranch        *b_htt_pts1_sjcalib1030;   //!
        TBranch        *b_htt_pts2_sjcalib1030;   //!
        TBranch        *b_htt_pts3_sjcalib1030;   //!
        TBranch        *b_htt_pt_sjcalib0970;   //!
        TBranch        *b_htt_eta_sjcalib0970;   //!
        TBranch        *b_htt_phi_sjcalib0970;   //!
        TBranch        *b_htt_m_sjcalib0970;   //!
        TBranch        *b_htt_m123_sjcalib0970;   //!
        TBranch        *b_htt_m23m123_sjcalib0970;   //!
        TBranch        *b_htt_atan1312_sjcalib0970;   //!
        TBranch        *b_htt_nTagCands_sjcalib0970;   //!
        TBranch        *b_htt_tag_sjcalib0970;   //!
        TBranch        *b_htt_pts1_sjcalib0970;   //!
        TBranch        *b_htt_pts2_sjcalib0970;   //!
        TBranch        *b_htt_pts3_sjcalib0970;   //!
        TBranch        *b_htt_caJet_pt;   //!
        TBranch        *b_htt_caJet_eta;   //!
        TBranch        *b_htt_caJet_phi;   //!
        TBranch        *b_htt_caJet_m;   //!
        TBranch        *b_caJet_count;   //!
        TBranch        *b_htt_caGroomJet_pt_def;   //!
        TBranch        *b_htt_caGroomJet_eta_def;   //!
        TBranch        *b_htt_caGroomJet_phi_def;   //!
        TBranch        *b_htt_caGroomJet_m_def;   //!
        TBranch        *b_htt_caGroomJet_pt_sjcalib1030;   //!
        TBranch        *b_htt_caGroomJet_eta_sjcalib1030;   //!
        TBranch        *b_htt_caGroomJet_phi_sjcalib1030;   //!
        TBranch        *b_htt_caGroomJet_m_sjcalib1030;   //!
        TBranch        *b_htt_caGroomJet_pt_sjcalib0970;   //!
        TBranch        *b_htt_caGroomJet_eta_sjcalib0970;   //!
        TBranch        *b_htt_caGroomJet_phi_sjcalib0970;   //!
        TBranch        *b_htt_caGroomJet_m_sjcalib0970;   //!

        TBranch*  b_rljet_SDw_calib; //!
        TBranch*  b_rljet_SDw_uncalib; //!
        TBranch*  b_rljet_SDw_combined; //!
        TBranch*  b_rljet_SDw_dcut; //!
        TBranch*  b_rljet_SDt_calib; //!
        TBranch*  b_rljet_SDt_uncalib; //!
        TBranch*  b_rljet_SDt_combined; //!
        TBranch*  b_rljet_SDt_dcut; //!

        TBranch*  b_rljet_SDw_calib_DOWN; //!
        TBranch*  b_rljet_SDw_uncalib_DOWN; //!
        TBranch*  b_rljet_SDw_combined_DOWN; //!
        TBranch*  b_rljet_SDw_dcut_DOWN; //!
        TBranch*  b_rljet_SDt_calib_DOWN; //!
        TBranch*  b_rljet_SDt_uncalib_DOWN; //!
        TBranch*  b_rljet_SDt_combined_DOWN; //!
        TBranch*  b_rljet_SDt_dcut_DOWN; //!
        TBranch*  b_rljet_SDw_calib_UP; //!
        TBranch*  b_rljet_SDw_uncalib_UP; //!
        TBranch*  b_rljet_SDw_combined_UP; //!
        TBranch*  b_rljet_SDw_dcut_UP; //!
        TBranch*  b_rljet_SDt_calib_UP; //!
        TBranch*  b_rljet_SDt_uncalib_UP; //!
        TBranch*  b_rljet_SDt_combined_UP; //!
        TBranch*  b_rljet_SDt_dcut_UP; //!

        DataMCbackgroundSelector(
                std::string output_filepath_,
                std::string root_dir_str_,
                std::string sub_dir_str_,
                std::string data_trigger_str_,
                std::string event_selector_str_,
                float luminosity_
                );

        virtual ~DataMCbackgroundSelector() { }

        void log(const std::string& line);

        virtual Int_t   Version() const { return 2; }
        virtual void    Begin(TTree *tree);
        virtual void    SlaveBegin(TTree *tree);
        virtual void    Init(TTree *tree);
        virtual Bool_t  Notify();
        virtual Bool_t  Process(Long64_t entry);
        virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
        virtual void    SetOption(const char *option) { fOption = option; }
        virtual void    SetObject(TObject *obj) { fObject = obj; }
        virtual void    SetInputList(TList *input) { fInput = input; }
        virtual TList  *GetOutputList() const { return fOutput; }
        virtual void    SlaveTerminate();
        virtual void    Terminate();

        ClassDef(DataMCbackgroundSelector,0);
};

#endif

#ifdef DataMCbackgroundSelector_cxx
void DataMCbackgroundSelector::Init(TTree *tree)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses and branch
    // pointers of the tree will be set.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    // Set object pointer
    ph_pt = 0;
    ph_phi = 0;
    ph_eta = 0;
    ph_e = 0;
    photon0_phi = 0;
    rljet_eta = 0;
    rljet_phi = 0;
    rljet_m_comb = 0;
    rljet_pt_comb = 0;
    rljet_D2 = 0;
    rljet_Tau32_wta = 0;
    rljet_smooth16Top_Tau32Split23Tag50eff = 0;
    rljet_smooth16Top_Tau32Split23Tag80eff = 0;
    rljet_smooth16Top_MassTau32Tag50eff = 0;
    rljet_smooth16Top_MassTau32Tag80eff = 0;
    rljet_smooth16Top_QwTau32Tag50eff = 0;
    rljet_smooth16Top_QwTau32Tag80eff = 0;
    rljet_smooth16WTag_50eff = 0;
    rljet_smooth16WTag_80eff = 0;
    rljet_smooth16ZTag_50eff = 0;
    rljet_smooth16ZTag_80eff = 0;

    rljet_pdgid = 0;

    //rljet_smooth16Top_MassTau32Tag50eff_nocontain = 0;
    //rljet_smooth16Top_MassTau32Tag80eff_nocontain = 0;
    //rljet_smooth16WTag_50eff_nocontain = 0;
    //rljet_smooth16WTag_80eff_nocontain = 0;
    //rljet_smooth16ZTag_50eff_nocontain = 0;
    //rljet_smooth16ZTag_80eff_nocontain = 0;

    // rljet_BDT_score_top_qqb = 0;
    // rljet_BDT_score_top_inclusive = 0;
    // rljet_BDT_score_w = 0;
    // rljet_DNN_score_top = 0;
    // rljet_DNN_score_w = 0;

    rljet_topTag_BDT_qqb = 0;
    rljet_topTag_BDT_qqb_score = 0;
    rljet_wTag_BDT_qq = 0;
    rljet_wTag_BDT_qq_score = 0;
    rljet_topTag_DNN_qqb_score = 0;
    rljet_topTag_DNN_qqb = 0;
    rljet_wTag_DNN_qq_score = 0;
    rljet_wTag_DNN_qq = 0;
    rljet_topTag_TopoTagger_20wp = 0;
    rljet_topTag_TopoTagger_50wp = 0;
    rljet_topTag_TopoTagger_80wp = 0;
    rljet_topTag_TopoTagger_score = 0;

    rljet_m_calo = 0;
    rljet_pt_calo = 0;
    rljet_m_ta = 0;
    rljet_pt_ta = 0;
    rljet_smooth15Top_MassTau32Tag50eff = 0;
    rljet_smooth15Top_MassTau32Tag80eff = 0;
    rljet_smooth15WTag_50eff = 0;
    rljet_smooth15WTag_25eff = 0;
    rljet_smooth15ZTag_50eff = 0;
    rljet_smooth15ZTag_25eff = 0;
    rljet_Tau1_wta = 0;
    rljet_Tau2_wta = 0;
    rljet_Tau3_wta = 0;
    rljet_ECF1 = 0;
    rljet_ECF2 = 0;
    rljet_ECF3 = 0;
    rljet_FoxWolfram0 = 0;
    rljet_FoxWolfram2 = 0;
    rljet_Qw = 0;
    rljet_Angularity = 0;
    rljet_Aplanarity = 0;
    rljet_Dip12 = 0;
    rljet_KtDR = 0;
    rljet_Mu12 = 0;
    rljet_PlanarFlow = 0;
    rljet_Sphericity = 0;
    rljet_Split12 = 0;
    rljet_Split23 = 0;
    rljet_Split34 = 0;
    rljet_ThrustMaj = 0;
    rljet_ThrustMin = 0;
    rljet_ZCut12 = 0;
    rljet_NTrimSubjets = 0;
    rljet_ungroomed_ntrk500 = 0;
    rljet_n_constituents = 0;
    htt_pt_def = 0;
    htt_eta_def = 0;
    htt_phi_def = 0;
    htt_m_def = 0;
    htt_m123_def = 0;
    htt_m23m123_def = 0;
    htt_atan1312_def = 0;
    htt_nTagCands_def = 0;
    htt_tag_def = 0;
    htt_pts1_def = 0;
    htt_pts2_def = 0;
    htt_pts3_def = 0;
    htt_pt_sjcalib1030 = 0;
    htt_eta_sjcalib1030 = 0;
    htt_phi_sjcalib1030 = 0;
    htt_m_sjcalib1030 = 0;
    htt_m123_sjcalib1030 = 0;
    htt_m23m123_sjcalib1030 = 0;
    htt_atan1312_sjcalib1030 = 0;
    htt_nTagCands_sjcalib1030 = 0;
    htt_tag_sjcalib1030 = 0;
    htt_pts1_sjcalib1030 = 0;
    htt_pts2_sjcalib1030 = 0;
    htt_pts3_sjcalib1030 = 0;
    htt_pt_sjcalib0970 = 0;
    htt_eta_sjcalib0970 = 0;
    htt_phi_sjcalib0970 = 0;
    htt_m_sjcalib0970 = 0;
    htt_m123_sjcalib0970 = 0;
    htt_m23m123_sjcalib0970 = 0;
    htt_atan1312_sjcalib0970 = 0;
    htt_nTagCands_sjcalib0970 = 0;
    htt_tag_sjcalib0970 = 0;
    htt_pts1_sjcalib0970 = 0;
    htt_pts2_sjcalib0970 = 0;
    htt_pts3_sjcalib0970 = 0;
    htt_caJet_pt = 0;
    htt_caJet_eta = 0;
    htt_caJet_phi = 0;
    htt_caJet_m = 0;
    htt_caGroomJet_pt_def = 0;
    htt_caGroomJet_eta_def = 0;
    htt_caGroomJet_phi_def = 0;
    htt_caGroomJet_m_def = 0;
    htt_caGroomJet_pt_sjcalib1030 = 0;
    htt_caGroomJet_eta_sjcalib1030 = 0;
    htt_caGroomJet_phi_sjcalib1030 = 0;
    htt_caGroomJet_m_sjcalib1030 = 0;
    htt_caGroomJet_pt_sjcalib0970 = 0;
    htt_caGroomJet_eta_sjcalib0970 = 0;
    htt_caGroomJet_phi_sjcalib0970 = 0;
    htt_caGroomJet_m_sjcalib0970 = 0;

    rljet_SDw_calib = 0;
    rljet_SDw_uncalib = 0;
    rljet_SDw_combined = 0;
    rljet_SDw_dcut = 0;
    rljet_SDt_calib = 0;
    rljet_SDt_uncalib = 0;
    rljet_SDt_combined = 0;
    rljet_SDt_dcut = 0;
    rljet_SDw_calib_DOWN = 0;
    rljet_SDw_uncalib_DOWN = 0;
    rljet_SDw_combined_DOWN = 0;
    rljet_SDw_dcut_DOWN = 0;
    rljet_SDt_calib_DOWN = 0;
    rljet_SDt_uncalib_DOWN = 0;
    rljet_SDt_combined_DOWN = 0;
    rljet_SDt_dcut_DOWN = 0;
    rljet_SDw_calib_UP = 0;
    rljet_SDw_uncalib_UP = 0;
    rljet_SDw_combined_UP = 0;
    rljet_SDw_dcut_UP = 0;
    rljet_SDt_calib_UP = 0;
    rljet_SDt_uncalib_UP = 0;
    rljet_SDt_combined_UP = 0;
    rljet_SDt_dcut_UP = 0;

    // Set branch addresses and branch pointers
    if (!tree) return;
    fChain = tree;
    fChain->SetMakeClass(1);

    ranSD = false;
    keptPhotons = false;
    auto array = fChain->GetListOfBranches();
    for(int i = 0; i < array->GetEntries(); ++i) {
      if (std::string(array->At(i)->GetName()).find("photon") != std::string::npos) {
        keptPhotons = true;
      }
      if (std::string(array->At(i)->GetName()).find("SD") != std::string::npos) {
        ranSD = true;
      }
    }

    default_photon_vars = false;
    for(int i = 0; i < array->GetEntries(); ++i) {
      if (std::string(array->At(i)->GetName()).find("ph_pt") != std::string::npos) {
        default_photon_vars = true;
        keptPhotons = true;
      }
    }


    if (this->operating_on_mc) {
        fChain->SetBranchAddress("weight_mc", &weight_mc, &b_weight_mc);
        fChain->SetBranchAddress("weight_pileup", &weight_pileup, &b_weight_pileup);

        if (keptPhotons) {
          fChain->SetBranchAddress("weight_photonSF", &weight_photonSF, &b_weight_photonSF);
          // fChain->SetBranchAddress("weight_photonSF_effIso", &weight_photonSF_effIso, &b_weight_photonSF_effIso);
          if (sub_dir_str.find("photon") != std::string::npos) {
            fChain->SetBranchAddress("weight_photonSF_ID_UP", &weight_photonSF_ID_UP, &b_weight_photonSF_ID_UP);
            fChain->SetBranchAddress("weight_photonSF_ID_DOWN", &weight_photonSF_ID_DOWN, &b_weight_photonSF_ID_DOWN);
            fChain->SetBranchAddress("weight_photonSF_effTrkIso_UP", &weight_photonSF_effTrkIso_UP, &b_weight_photonSF_effTrkIso_UP);
            fChain->SetBranchAddress("weight_photonSF_effTrkIso_DOWN", &weight_photonSF_effTrkIso_DOWN, &b_weight_photonSF_effTrkIso_DOWN);
          }
        }

        fChain->SetBranchAddress("randomRunNumber", &randomRunNumber, &b_randomRunNumber);
        fChain->SetBranchAddress("mu", &mu, &b_mu);
    } else {
        fChain->SetBranchAddress(this->data_trigger_str.c_str(), &HLT_trigger, &b_HLT_trigger);
        fChain->SetBranchAddress("mu_original_xAOD", &mu, &b_mu);
    }

    fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
    fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
    fChain->SetBranchAddress("mcChannelNumber", &mcChannelNumber, &b_mcChannelNumber);
    fChain->SetBranchAddress("backgroundFlags", &backgroundFlags, &b_backgroundFlags);
    //fChain->SetBranchAddress("met_met", &met_met, &b_met_met);
    //fChain->SetBranchAddress("met_phi", &met_phi, &b_met_phi);
    fChain->SetBranchAddress("rljet_eta", &rljet_eta, &b_rljet_eta);
    fChain->SetBranchAddress("rljet_phi", &rljet_phi, &b_rljet_phi);
    fChain->SetBranchAddress("rljet_m_comb", &rljet_m_comb, &b_rljet_m_comb);
    fChain->SetBranchAddress("rljet_pt_comb", &rljet_pt_comb, &b_rljet_pt_comb);
    fChain->SetBranchAddress("rljet_D2", &rljet_D2, &b_rljet_D2);
    fChain->SetBranchAddress("rljet_Tau32_wta", &rljet_Tau32_wta, &b_rljet_Tau32_wta);
    fChain->SetBranchAddress("rljet_Qw", &rljet_Qw, &b_rljet_Qw);
    fChain->SetBranchAddress("rljet_Split23", &rljet_Split23, &b_rljet_Split23);
    fChain->SetBranchAddress("m_rljet_smooth16Top_Tau32Split23Tag50eff", &rljet_smooth16Top_Tau32Split23Tag50eff, &b_m_rljet_smooth16Top_Tau32Split23Tag50eff);
    fChain->SetBranchAddress("m_rljet_smooth16Top_Tau32Split23Tag80eff", &rljet_smooth16Top_Tau32Split23Tag80eff, &b_m_rljet_smooth16Top_Tau32Split23Tag80eff);
    fChain->SetBranchAddress("m_rljet_smooth16Top_MassTau32Tag50eff", &rljet_smooth16Top_MassTau32Tag50eff, &b_m_rljet_smooth16Top_MassTau32Tag50eff);
    fChain->SetBranchAddress("m_rljet_smooth16Top_MassTau32Tag80eff", &rljet_smooth16Top_MassTau32Tag80eff, &b_m_rljet_smooth16Top_MassTau32Tag80eff);
    fChain->SetBranchAddress("m_rljet_smooth16Top_QwTau32Tag50eff", &rljet_smooth16Top_QwTau32Tag50eff, &b_m_rljet_smooth16Top_QwTau32Tag50eff);
    fChain->SetBranchAddress("m_rljet_smooth16Top_QwTau32Tag80eff", &rljet_smooth16Top_QwTau32Tag80eff, &b_m_rljet_smooth16Top_QwTau32Tag80eff);
    fChain->SetBranchAddress("rljet_smooth16WTag_50eff", &rljet_smooth16WTag_50eff, &b_rljet_smooth16WTag_50eff);
    fChain->SetBranchAddress("rljet_smooth16WTag_80eff", &rljet_smooth16WTag_80eff, &b_rljet_smooth16WTag_80eff);
    fChain->SetBranchAddress("rljet_smooth16ZTag_50eff", &rljet_smooth16ZTag_50eff, &b_rljet_smooth16ZTag_50eff);
    fChain->SetBranchAddress("rljet_smooth16ZTag_80eff", &rljet_smooth16ZTag_80eff, &b_rljet_smooth16ZTag_80eff);


    // fChain->SetBranchAddress("m_rljet_smooth16Top_MassTau32Tag50eff_nocontain", &rljet_smooth16Top_MassTau32Tag50eff_nocontain, &b_rljet_smooth16Top_MassTau32Tag50eff_nocontain);
    // fChain->SetBranchAddress("m_rljet_smooth16Top_MassTau32Tag80eff_nocontain", &rljet_smooth16Top_MassTau32Tag80eff_nocontain, &b_rljet_smooth16Top_MassTau32Tag80eff_nocontain);
    // fChain->SetBranchAddress("rljet_smooth16WTag_50eff_nocontain", &rljet_smooth16WTag_50eff_nocontain, &b_rljet_smooth16WTag_50eff_nocontain);
    // fChain->SetBranchAddress("rljet_smooth16WTag_80eff_nocontain", &rljet_smooth16WTag_80eff_nocontain, &b_rljet_smooth16WTag_80eff_nocontain);
    // fChain->SetBranchAddress("rljet_smooth16ZTag_50eff_nocontain", &rljet_smooth16ZTag_50eff_nocontain, &b_rljet_smooth16ZTag_50eff_nocontain);
    // fChain->SetBranchAddress("rljet_smooth16ZTag_80eff_nocontain", &rljet_smooth16ZTag_80eff_nocontain, &b_rljet_smooth16ZTag_80eff_nocontain);

    fChain->SetBranchAddress("rljet_topTag_BDT_qqb", &rljet_topTag_BDT_qqb, &b_rljet_topTag_BDT_qqb);
    fChain->SetBranchAddress("rljet_topTag_BDT_qqb_score", &rljet_topTag_BDT_qqb_score, &b_rljet_topTag_BDT_qqb_score);
    fChain->SetBranchAddress("rljet_wTag_BDT_qq", &rljet_wTag_BDT_qq, &b_rljet_wTag_BDT_qq);
    fChain->SetBranchAddress("rljet_wTag_BDT_qq_score", &rljet_wTag_BDT_qq_score, &b_rljet_wTag_BDT_qq_score);
    fChain->SetBranchAddress("rljet_topTag_DNN_qqb_score", &rljet_topTag_DNN_qqb_score, &b_rljet_topTag_DNN_qqb_score);
    fChain->SetBranchAddress("rljet_topTag_DNN_qqb", &rljet_topTag_DNN_qqb, &b_rljet_topTag_DNN_qqb);
    fChain->SetBranchAddress("rljet_wTag_DNN_qq_score", &rljet_wTag_DNN_qq_score, &b_rljet_wTag_DNN_qq_score);
    fChain->SetBranchAddress("rljet_wTag_DNN_qq", &rljet_wTag_DNN_qq, &b_rljet_wTag_DNN_qq);
    fChain->SetBranchAddress("rljet_topTag_TopoTagger_20wp", &rljet_topTag_TopoTagger_20wp, &b_rljet_topTag_TopoTagger_20wp);
    fChain->SetBranchAddress("rljet_topTag_TopoTagger_50wp", &rljet_topTag_TopoTagger_50wp, &b_rljet_topTag_TopoTagger_50wp);
    fChain->SetBranchAddress("rljet_topTag_TopoTagger_80wp", &rljet_topTag_TopoTagger_80wp, &b_rljet_topTag_TopoTagger_80wp);
    fChain->SetBranchAddress("rljet_topTag_TopoTagger_score", &rljet_topTag_TopoTagger_score, &b_rljet_topTag_TopoTagger_score);



    if (sub_dir_str == "nominal") {
        fChain->SetBranchAddress("NPV", &NPV, &b_NPV);

        if (this->operating_on_mc) {
          fChain->SetBranchAddress("rljet_pdgid", &rljet_pdgid, &b_rljet_pdgid);
        }

        // fChain->SetBranchAddress("rljet_BDT_score_top_qqb"       , &rljet_BDT_score_top_qqb       , &b_rljet_BDT_score_top_qqb);
        // fChain->SetBranchAddress("rljet_BDT_score_top_inclusive" , &rljet_BDT_score_top_inclusive , &b_rljet_BDT_score_top_inclusive);
        // fChain->SetBranchAddress("rljet_BDT_score_w"             , &rljet_BDT_score_w             , &b_rljet_BDT_score_w);
        // fChain->SetBranchAddress("rljet_DNN_score_top"           , &rljet_DNN_score_top           , &b_rljet_DNN_score_top);
        // fChain->SetBranchAddress("rljet_DNN_score_w"             , &rljet_DNN_score_w             , &b_rljet_DNN_score_w);

        if (keptPhotons) {
          if (default_photon_vars) {
            fChain->SetBranchAddress("ph_pt"  , &ph_pt  , &b_ph_pt);
            fChain->SetBranchAddress("ph_phi" , &ph_phi  , &b_ph_phi);
            fChain->SetBranchAddress("ph_eta" , &ph_eta , &b_ph_eta);
            fChain->SetBranchAddress("ph_e"   , &ph_e   , &b_ph_e);
          } else {
            fChain->SetBranchAddress("photon0_phi"  , &photon0_phi  , &b_photon0_phi);
          }
          // fChain->SetBranchAddress("photon0_ptcone20", &photon_ptcone20, &b_photon_ptcone20);
          // fChain->SetBranchAddress("photon0_ptcone40", &photon_ptcone40, &b_photon_ptcone40);
          // fChain->SetBranchAddress("photon0_topoetcone20", &photon_topoetcone20, &b_photon_topoetcone20);
          // fChain->SetBranchAddress("photon0_topoetcone40", &photon_topoetcone40, &b_photon_topoetcone40);
        }

        fChain->SetBranchAddress("rljet_count", &rljet_count, &b_rljet_count);
        fChain->SetBranchAddress("rljet_mjj", &rljet_mjj, &b_rljet_mjj);
        fChain->SetBranchAddress("rljet_ptasym", &rljet_ptasym, &b_rljet_ptasym);
        fChain->SetBranchAddress("rljet_dy", &rljet_dy, &b_rljet_dy);
        fChain->SetBranchAddress("rljet_dR", &rljet_dR, &b_rljet_dR);
        fChain->SetBranchAddress("rljet_dphi", &rljet_dphi, &b_rljet_dphi);
        fChain->SetBranchAddress("rljet_deta", &rljet_deta, &b_rljet_deta);
        fChain->SetBranchAddress("rljet_m_calo", &rljet_m_calo, &b_rljet_m_calo);
        fChain->SetBranchAddress("rljet_pt_calo", &rljet_pt_calo, &b_rljet_pt_calo);
        fChain->SetBranchAddress("rljet_m_ta", &rljet_m_ta, &b_rljet_m_ta);
        fChain->SetBranchAddress("rljet_pt_ta", &rljet_pt_ta, &b_rljet_pt_ta);
        // fChain->SetBranchAddress("m_rljet_smooth15Top_MassTau32Tag50eff", &rljet_smooth15Top_MassTau32Tag50eff, &b_m_rljet_smooth15Top_MassTau32Tag50eff);
        // fChain->SetBranchAddress("m_rljet_smooth15Top_MassTau32Tag80eff", &rljet_smooth15Top_MassTau32Tag80eff, &b_m_rljet_smooth15Top_MassTau32Tag80eff);
        // fChain->SetBranchAddress("rljet_smooth15WTag_50eff", &rljet_smooth15WTag_50eff, &b_rljet_smooth15WTag_50eff);
        // fChain->SetBranchAddress("rljet_smooth15WTag_25eff", &rljet_smooth15WTag_25eff, &b_rljet_smooth15WTag_25eff);
        // fChain->SetBranchAddress("rljet_smooth15ZTag_50eff", &rljet_smooth15ZTag_50eff, &b_rljet_smooth15ZTag_50eff);
        // fChain->SetBranchAddress("rljet_smooth15ZTag_25eff", &rljet_smooth15ZTag_25eff, &b_rljet_smooth15ZTag_25eff);
        fChain->SetBranchAddress("rljet_Tau1_wta", &rljet_Tau1_wta, &b_rljet_Tau1_wta);
        fChain->SetBranchAddress("rljet_Tau2_wta", &rljet_Tau2_wta, &b_rljet_Tau2_wta);
        fChain->SetBranchAddress("rljet_Tau3_wta", &rljet_Tau3_wta, &b_rljet_Tau3_wta);
        fChain->SetBranchAddress("rljet_ECF1", &rljet_ECF1, &b_rljet_ECF1);
        fChain->SetBranchAddress("rljet_ECF2", &rljet_ECF2, &b_rljet_ECF2);
        fChain->SetBranchAddress("rljet_ECF3", &rljet_ECF3, &b_rljet_ECF3);
        fChain->SetBranchAddress("rljet_FoxWolfram0", &rljet_FoxWolfram0, &b_rljet_FoxWolfram0);
        fChain->SetBranchAddress("rljet_FoxWolfram2", &rljet_FoxWolfram2, &b_rljet_FoxWolfram2);
        fChain->SetBranchAddress("rljet_Angularity", &rljet_Angularity, &b_rljet_Angularity);
        fChain->SetBranchAddress("rljet_Aplanarity", &rljet_Aplanarity, &b_rljet_Aplanarity);
        fChain->SetBranchAddress("rljet_Dip12", &rljet_Dip12, &b_rljet_Dip12);
        fChain->SetBranchAddress("rljet_KtDR", &rljet_KtDR, &b_rljet_KtDR);
        fChain->SetBranchAddress("rljet_Mu12", &rljet_Mu12, &b_rljet_Mu12);
        fChain->SetBranchAddress("rljet_PlanarFlow", &rljet_PlanarFlow, &b_rljet_PlanarFlow);
        fChain->SetBranchAddress("rljet_Sphericity", &rljet_Sphericity, &b_rljet_Sphericity);
        fChain->SetBranchAddress("rljet_Split12", &rljet_Split12, &b_rljet_Split12);
        fChain->SetBranchAddress("rljet_Split34", &rljet_Split34, &b_rljet_Split34);
        fChain->SetBranchAddress("rljet_ThrustMaj", &rljet_ThrustMaj, &b_rljet_ThrustMaj);
        fChain->SetBranchAddress("rljet_ThrustMin", &rljet_ThrustMin, &b_rljet_ThrustMin);
        fChain->SetBranchAddress("rljet_ZCut12", &rljet_ZCut12, &b_rljet_ZCut12);
        fChain->SetBranchAddress("rljet_NTrimSubjets", &rljet_NTrimSubjets, &b_rljet_NTrimSubjets);
        fChain->SetBranchAddress("rljet_ungroomed_ntrk500", &rljet_ungroomed_ntrk500, &b_rljet_ungroomed_ntrk500);
        fChain->SetBranchAddress("rljet_n_constituents", &rljet_n_constituents, &b_rljet_n_constituents);
        fChain->SetBranchAddress("htt_pt_def", &htt_pt_def, &b_htt_pt_def);
        fChain->SetBranchAddress("htt_eta_def", &htt_eta_def, &b_htt_eta_def);
        fChain->SetBranchAddress("htt_phi_def", &htt_phi_def, &b_htt_phi_def);
        fChain->SetBranchAddress("htt_m_def", &htt_m_def, &b_htt_m_def);
        fChain->SetBranchAddress("htt_m123_def", &htt_m123_def, &b_htt_m123_def);
        fChain->SetBranchAddress("htt_m23m123_def", &htt_m23m123_def, &b_htt_m23m123_def);
        fChain->SetBranchAddress("htt_atan1312_def", &htt_atan1312_def, &b_htt_atan1312_def);
        fChain->SetBranchAddress("htt_nTagCands_def", &htt_nTagCands_def, &b_htt_nTagCands_def);
        fChain->SetBranchAddress("htt_tag_def", &htt_tag_def, &b_htt_tag_def);
        fChain->SetBranchAddress("htt_pts1_def", &htt_pts1_def, &b_htt_pts1_def);
        fChain->SetBranchAddress("htt_pts2_def", &htt_pts2_def, &b_htt_pts2_def);
        fChain->SetBranchAddress("htt_pts3_def", &htt_pts3_def, &b_htt_pts3_def);
        fChain->SetBranchAddress("htt_caJet_pt", &htt_caJet_pt, &b_htt_caJet_pt);
        fChain->SetBranchAddress("htt_caJet_eta", &htt_caJet_eta, &b_htt_caJet_eta);
        fChain->SetBranchAddress("htt_caJet_phi", &htt_caJet_phi, &b_htt_caJet_phi);
        fChain->SetBranchAddress("htt_caJet_m", &htt_caJet_m, &b_htt_caJet_m);
        fChain->SetBranchAddress("caJet_count", &caJet_count, &b_caJet_count);
        fChain->SetBranchAddress("htt_caGroomJet_pt_def", &htt_caGroomJet_pt_def, &b_htt_caGroomJet_pt_def);
        fChain->SetBranchAddress("htt_caGroomJet_eta_def", &htt_caGroomJet_eta_def, &b_htt_caGroomJet_eta_def);
        fChain->SetBranchAddress("htt_caGroomJet_phi_def", &htt_caGroomJet_phi_def, &b_htt_caGroomJet_phi_def);
        fChain->SetBranchAddress("htt_caGroomJet_m_def", &htt_caGroomJet_m_def, &b_htt_caGroomJet_m_def);

        if (ranSD) {
          fChain->SetBranchAddress("rljet_SDw_calib"         , &rljet_SDw_calib         , &b_rljet_SDw_calib);
          fChain->SetBranchAddress("rljet_SDw_uncalib"       , &rljet_SDw_uncalib       , &b_rljet_SDw_uncalib);
          fChain->SetBranchAddress("rljet_SDw_combined"      , &rljet_SDw_combined      , &b_rljet_SDw_combined);
          fChain->SetBranchAddress("rljet_SDw_dcut"          , &rljet_SDw_dcut          , &b_rljet_SDw_dcut);
          fChain->SetBranchAddress("rljet_SDt_calib"         , &rljet_SDt_calib         , &b_rljet_SDt_calib);
          fChain->SetBranchAddress("rljet_SDt_uncalib"       , &rljet_SDt_uncalib       , &b_rljet_SDt_uncalib);
          fChain->SetBranchAddress("rljet_SDt_combined"      , &rljet_SDt_combined      , &b_rljet_SDt_combined);
          fChain->SetBranchAddress("rljet_SDt_dcut"          , &rljet_SDt_dcut          , &b_rljet_SDt_dcut);
        }


        if (this->operating_on_mc) {
            fChain->SetBranchAddress("htt_pt_sjcalib1030", &htt_pt_sjcalib1030, &b_htt_pt_sjcalib1030);
            fChain->SetBranchAddress("htt_eta_sjcalib1030", &htt_eta_sjcalib1030, &b_htt_eta_sjcalib1030);
            fChain->SetBranchAddress("htt_phi_sjcalib1030", &htt_phi_sjcalib1030, &b_htt_phi_sjcalib1030);
            fChain->SetBranchAddress("htt_m_sjcalib1030", &htt_m_sjcalib1030, &b_htt_m_sjcalib1030);
            fChain->SetBranchAddress("htt_m123_sjcalib1030", &htt_m123_sjcalib1030, &b_htt_m123_sjcalib1030);
            fChain->SetBranchAddress("htt_m23m123_sjcalib1030", &htt_m23m123_sjcalib1030, &b_htt_m23m123_sjcalib1030);
            fChain->SetBranchAddress("htt_atan1312_sjcalib1030", &htt_atan1312_sjcalib1030, &b_htt_atan1312_sjcalib1030);
            fChain->SetBranchAddress("htt_nTagCands_sjcalib1030", &htt_nTagCands_sjcalib1030, &b_htt_nTagCands_sjcalib1030);
            fChain->SetBranchAddress("htt_tag_sjcalib1030", &htt_tag_sjcalib1030, &b_htt_tag_sjcalib1030);
            fChain->SetBranchAddress("htt_pts1_sjcalib1030", &htt_pts1_sjcalib1030, &b_htt_pts1_sjcalib1030);
            fChain->SetBranchAddress("htt_pts2_sjcalib1030", &htt_pts2_sjcalib1030, &b_htt_pts2_sjcalib1030);
            fChain->SetBranchAddress("htt_pts3_sjcalib1030", &htt_pts3_sjcalib1030, &b_htt_pts3_sjcalib1030);
            fChain->SetBranchAddress("htt_pt_sjcalib0970", &htt_pt_sjcalib0970, &b_htt_pt_sjcalib0970);
            fChain->SetBranchAddress("htt_eta_sjcalib0970", &htt_eta_sjcalib0970, &b_htt_eta_sjcalib0970);
            fChain->SetBranchAddress("htt_phi_sjcalib0970", &htt_phi_sjcalib0970, &b_htt_phi_sjcalib0970);
            fChain->SetBranchAddress("htt_m_sjcalib0970", &htt_m_sjcalib0970, &b_htt_m_sjcalib0970);
            fChain->SetBranchAddress("htt_m123_sjcalib0970", &htt_m123_sjcalib0970, &b_htt_m123_sjcalib0970);
            fChain->SetBranchAddress("htt_m23m123_sjcalib0970", &htt_m23m123_sjcalib0970, &b_htt_m23m123_sjcalib0970);
            fChain->SetBranchAddress("htt_atan1312_sjcalib0970", &htt_atan1312_sjcalib0970, &b_htt_atan1312_sjcalib0970);
            fChain->SetBranchAddress("htt_nTagCands_sjcalib0970", &htt_nTagCands_sjcalib0970, &b_htt_nTagCands_sjcalib0970);
            fChain->SetBranchAddress("htt_tag_sjcalib0970", &htt_tag_sjcalib0970, &b_htt_tag_sjcalib0970);
            fChain->SetBranchAddress("htt_pts1_sjcalib0970", &htt_pts1_sjcalib0970, &b_htt_pts1_sjcalib0970);
            fChain->SetBranchAddress("htt_pts2_sjcalib0970", &htt_pts2_sjcalib0970, &b_htt_pts2_sjcalib0970);
            fChain->SetBranchAddress("htt_pts3_sjcalib0970", &htt_pts3_sjcalib0970, &b_htt_pts3_sjcalib0970);
            fChain->SetBranchAddress("htt_caGroomJet_pt_sjcalib1030", &htt_caGroomJet_pt_sjcalib1030, &b_htt_caGroomJet_pt_sjcalib1030);
            fChain->SetBranchAddress("htt_caGroomJet_eta_sjcalib1030", &htt_caGroomJet_eta_sjcalib1030, &b_htt_caGroomJet_eta_sjcalib1030);
            fChain->SetBranchAddress("htt_caGroomJet_phi_sjcalib1030", &htt_caGroomJet_phi_sjcalib1030, &b_htt_caGroomJet_phi_sjcalib1030);
            fChain->SetBranchAddress("htt_caGroomJet_m_sjcalib1030", &htt_caGroomJet_m_sjcalib1030, &b_htt_caGroomJet_m_sjcalib1030);
            fChain->SetBranchAddress("htt_caGroomJet_pt_sjcalib0970", &htt_caGroomJet_pt_sjcalib0970, &b_htt_caGroomJet_pt_sjcalib0970);
            fChain->SetBranchAddress("htt_caGroomJet_eta_sjcalib0970", &htt_caGroomJet_eta_sjcalib0970, &b_htt_caGroomJet_eta_sjcalib0970);
            fChain->SetBranchAddress("htt_caGroomJet_phi_sjcalib0970", &htt_caGroomJet_phi_sjcalib0970, &b_htt_caGroomJet_phi_sjcalib0970);
            fChain->SetBranchAddress("htt_caGroomJet_m_sjcalib0970", &htt_caGroomJet_m_sjcalib0970, &b_htt_caGroomJet_m_sjcalib0970);

            if (ranSD) {
              fChain->SetBranchAddress("rljet_SDw_calib_DOWN"    , &rljet_SDw_calib_DOWN    , &b_rljet_SDw_calib_DOWN);
              fChain->SetBranchAddress("rljet_SDw_uncalib_DOWN"  , &rljet_SDw_uncalib_DOWN  , &b_rljet_SDw_uncalib_DOWN);
              fChain->SetBranchAddress("rljet_SDw_combined_DOWN" , &rljet_SDw_combined_DOWN , &b_rljet_SDw_combined_DOWN);
              fChain->SetBranchAddress("rljet_SDw_dcut_DOWN"     , &rljet_SDw_dcut_DOWN     , &b_rljet_SDw_dcut_DOWN);
              fChain->SetBranchAddress("rljet_SDt_calib_DOWN"    , &rljet_SDt_calib_DOWN    , &b_rljet_SDt_calib_DOWN);
              fChain->SetBranchAddress("rljet_SDt_uncalib_DOWN"  , &rljet_SDt_uncalib_DOWN  , &b_rljet_SDt_uncalib_DOWN);
              fChain->SetBranchAddress("rljet_SDt_combined_DOWN" , &rljet_SDt_combined_DOWN , &b_rljet_SDt_combined_DOWN);
              fChain->SetBranchAddress("rljet_SDt_dcut_DOWN"     , &rljet_SDt_dcut_DOWN     , &b_rljet_SDt_dcut_DOWN);
              fChain->SetBranchAddress("rljet_SDw_calib_UP"      , &rljet_SDw_calib_UP      , &b_rljet_SDw_calib_UP);
              fChain->SetBranchAddress("rljet_SDw_uncalib_UP"    , &rljet_SDw_uncalib_UP    , &b_rljet_SDw_uncalib_UP);
              fChain->SetBranchAddress("rljet_SDw_combined_UP"   , &rljet_SDw_combined_UP   , &b_rljet_SDw_combined_UP);
              fChain->SetBranchAddress("rljet_SDw_dcut_UP"       , &rljet_SDw_dcut_UP       , &b_rljet_SDw_dcut_UP);
              fChain->SetBranchAddress("rljet_SDt_calib_UP"      , &rljet_SDt_calib_UP      , &b_rljet_SDt_calib_UP);
              fChain->SetBranchAddress("rljet_SDt_uncalib_UP"    , &rljet_SDt_uncalib_UP    , &b_rljet_SDt_uncalib_UP);
              fChain->SetBranchAddress("rljet_SDt_combined_UP"   , &rljet_SDt_combined_UP   , &b_rljet_SDt_combined_UP);
              fChain->SetBranchAddress("rljet_SDt_dcut_UP"       , &rljet_SDt_dcut_UP       , &b_rljet_SDt_dcut_UP);
            }
        }
    }
}

Bool_t DataMCbackgroundSelector::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

#endif // #ifdef DataMCbackgroundSelector_cxx

