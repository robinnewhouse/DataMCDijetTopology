#ifndef DataMCbackgroundSelector_h
#define DataMCbackgroundSelector_h

#include "DataMCbackgroundTools/HistoPack.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "DataMCbackgroundTools/WeightTool.h"

#include "TopExamples/AnalysisTools.h"

#include <TROOT.h>
#include <TH1F.h>
#include <TNamed.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TLorentzVector.h>

#include <vector>
using std::vector;
#include <array>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>

class DataMCbackgroundSelector;

typedef std::function<bool(const DataMCbackgroundSelector*)> EventSelector;

class DataMCbackgroundSelector : public TSelector {
    public :
        TTree* fChain;   //!pointer to the analyzed TTree or TChain

        HistoPack* hp;

        const std::string output_filepath;
        const std::string root_dir_str;
        const std::string sub_dir_str;
        const std::string data_trigger_str;

        WeightTool weight_tool;

        const bool operating_on_mc;
        const bool on_nominal_branch;
        bool compute_Tau32;
        bool compute_D2;

        const float luminosity;

        static const std::unordered_map<std::string, EventSelector> available_event_selectors;
        EventSelector chosen_event_selector;

        float max_weight;
        float SF_lumi_Fb;

        vector<std::string> htt_config_strings;

        std::ofstream output_log;

        std::unordered_map<std::string, bool> prerec_tag_map;
        std::unordered_map<std::string, bool> ntrk_prerec_tag_map;
        std::unordered_map<std::string, bool> SD_nominal_tag_map;
        std::unordered_map<std::string, bool> SD_systematic_tag_map;
        // std::unordered_map<std::string, bool> nominal_tag_map;

        // Declaration of leaf types
        Float_t   weight_mc;
        Float_t   weight_pileup;
        Float_t   weight_jvt;
        Float_t   weight_bTagSF_70;

        ULong64_t eventNumber;
        UInt_t    runNumber;
        UInt_t    mcChannelNumber;
        Float_t   mu;
        Float_t   mu_original_xAOD;
        UInt_t    backgroundFlags;

        Float_t         met_met;
        Float_t         met_phi;

        Char_t          HLT_jet_trigger;

        Int_t           rljet_count;
        vector<float>   *rljet_pt;
        vector<float>   *rljet_eta;
        vector<float>   *rljet_phi;
        vector<float>   *rljet_m;

        Float_t rljet_mjj;
        Float_t rljet_ptasym;
        Float_t rljet_dy;
        Float_t rljet_dR;

        vector<float> *rljet_Tau1_wta;
        vector<float> *rljet_Tau2_wta;
        vector<float> *rljet_Tau3_wta;
        vector<float> *rljet_Tau32_wta;
        vector<float> *rljet_ECF1;
        vector<float> *rljet_ECF2;
        vector<float> *rljet_ECF3;
        vector<float> *rljet_D2;
        vector<float> *rljet_FoxWolfram0;
        vector<float> *rljet_FoxWolfram2;
        vector<float> *rljet_Qw;
        vector<float> *rljet_Angularity;
        vector<float> *rljet_Aplanarity;
        vector<float> *rljet_Dip12;
        vector<float> *rljet_KtDR;
        vector<float> *rljet_Mu12;
        vector<float> *rljet_PlanarFlow;
        vector<float> *rljet_Sphericity;
        vector<float> *rljet_Split12;
        vector<float> *rljet_Split23;
        vector<float> *rljet_Split34;
        vector<float> *rljet_ThrustMaj;
        vector<float> *rljet_ThrustMin;
        vector<float> *rljet_ZCut12;

		vector<int> *rljet_NTrimSubjets;
		vector<int> *rljet_ungroomed_ntrk500;

        vector<float>   *tljet_pt;
        vector<float>   *tljet_eta;
        vector<float>   *tljet_phi;
        vector<float>   *tljet_m;
        vector<float>   *tljet_dR;
        vector<float>   *tljet_D2;
        vector<float>   *tljet_Tau32_wta;

        vector<int> *rljet_dRmatched_reco_truth;
        vector<int> *rljet_dRmatched_particle_flavor;
        vector<int> *rljet_dRmatched_maxEParton_flavor;
        vector<int> *rljet_dRmatched_topBChild;
        vector<int> *rljet_dRmatched_nQuarkChildren;

        Int_t           hltjet_count;
        vector<float>   *hltjet_m;
        vector<float>   *hltjet_pt;
        vector<float>   *hltjet_eta;
        vector<float>   *hltjet_phi;
        vector<float>   *hltjet_dR;

        Int_t           caJet_count;
        vector<float>   *htt_caJet_pt;
        vector<float>   *htt_caJet_eta;
        vector<float>   *htt_caJet_phi;
        vector<float>   *htt_caJet_m;

        vector<vector<float>*>   htt_pt;
        vector<vector<float>*>   htt_eta;
        vector<vector<float>*>   htt_phi;
        vector<vector<float>*>   htt_m;
        vector<vector<float>*>   htt_m23m123;
        vector<vector<float>*>   htt_atan1312;
        vector<vector<float>*>   htt_pts1;
        vector<vector<float>*>   htt_pts2;
        vector<vector<float>*>   htt_pts3;
        vector<vector<float>*>   htt_nTagCands;
        vector<vector<bool>*>    htt_tag;

        vector<vector<float>*>   htt_caGroomJet_pt;
        vector<vector<float>*>   htt_caGroomJet_eta;
        vector<vector<float>*>   htt_caGroomJet_phi;
        vector<vector<float>*>   htt_caGroomJet_m;

        vector<bool>    *rljet_smoothMassTag50eff;
        vector<bool>    *rljet_smoothMassTag80eff;
        vector<bool>    *rljet_smoothTau32Tag50eff;
        vector<bool>    *rljet_smoothTau32Tag80eff;
        vector<bool>    *rljet_smoothMassTau32Tag50eff;
        vector<bool>    *rljet_smoothMassTau32Tag80eff;
        vector<int>     *rljet_smoothWTag50eff;
        vector<int>     *rljet_smoothWTag25eff;
        vector<int>     *rljet_smoothZTag50eff;
        vector<int>     *rljet_smoothZTag25eff;

        vector<float>   *rljet_BDT_Top_Score;
        vector<float>   *rljet_BDT_W_Score;

        vector<bool>    *rljet_btag_double;
        vector<bool>    *rljet_btag_single;
        vector<bool>    *rljet_btag_leading_pt;

        vector<double>  *rljet_SDw_win20_btag0;
        vector<double>  *rljet_SDz_win20_btag0;
        vector<double>  *rljet_SDt_win50_btag0;
        vector<double>  *rljet_SDw_win20_btag0_UP;
        vector<double>  *rljet_SDz_win20_btag0_UP;
        vector<double>  *rljet_SDt_win50_btag0_UP;
        vector<double>  *rljet_SDw_win20_btag0_DOWN;
        vector<double>  *rljet_SDz_win20_btag0_DOWN;
        vector<double>  *rljet_SDt_win50_btag0_DOWN;

        // List of branches
        TBranch        *b_weight_mc;   //!
        TBranch        *b_weight_pileup;   //!
        TBranch        *b_weight_jvt;   //!
        TBranch        *b_weight_bTagSF_70;   //!
        TBranch        *b_eventNumber;   //!
        TBranch        *b_runNumber;   //!
        TBranch        *b_mcChannelNumber;   //!
        TBranch        *b_mu;   //!
        TBranch        *b_mu_original_xAOD;   //!
        TBranch        *b_backgroundFlags;   //!

        TBranch        *b_met_met;   //!
        TBranch        *b_met_phi;   //!

        TBranch        *b_HLT_jet_trigger;  //!

        TBranch        *b_rljet_count;   //!
        TBranch        *b_rljet_pt;   //!
        TBranch        *b_rljet_eta;   //!
        TBranch        *b_rljet_phi;   //!
        TBranch        *b_rljet_m;   //!

        TBranch        *b_rljet_mjj;   //!
        TBranch        *b_rljet_ptasym;   //!
        TBranch        *b_rljet_dy;   //!
        TBranch        *b_rljet_dR;   //!

        TBranch        *b_tljet_pt;   //!
        TBranch        *b_tljet_eta;   //!
        TBranch        *b_tljet_phi;   //!
        TBranch        *b_tljet_m;   //!
        TBranch        *b_tljet_dR;   //!
        TBranch        *b_tljet_D2;   //!
        TBranch        *b_tljet_Tau32_wta;   //!

        TBranch        *b_rljet_dRmatched_reco_truth; //!
        TBranch        *b_rljet_dRmatched_particle_flavor; //!
        TBranch        *b_rljet_dRmatched_maxEParton_flavor; //!
        TBranch        *b_rljet_dRmatched_topBChild; //!
        TBranch        *b_rljet_dRmatched_nQuarkChildren; //!

        TBranch        *b_hltjet_count;   //!
        TBranch        *b_hltjet_m;   //!
        TBranch        *b_hltjet_pt;   //!
        TBranch        *b_hltjet_eta;   //!
        TBranch        *b_hltjet_phi;   //!
        TBranch        *b_hltjet_dR;   //!

        TBranch        *b_htt_caJet_pt; //!
        TBranch        *b_htt_caJet_eta; //!
        TBranch        *b_htt_caJet_phi; //!
        TBranch        *b_htt_caJet_m; //!

        vector<TBranch*> b_htt_pt; //!
        vector<TBranch*> b_htt_eta; //!
        vector<TBranch*> b_htt_phi; //!
        vector<TBranch*> b_htt_m; //!
        vector<TBranch*> b_htt_m23m123; //!
        vector<TBranch*> b_htt_atan1312; //!
        vector<TBranch*> b_htt_pts1; //!
        vector<TBranch*> b_htt_pts2; //!
        vector<TBranch*> b_htt_pts3; //!
        vector<TBranch*> b_htt_nTagCands; //!
        vector<TBranch*> b_htt_tag; //!

        TBranch        *b_caJet_count;   //!
        vector<TBranch*> b_htt_caGroomJet_pt; //!
        vector<TBranch*> b_htt_caGroomJet_eta; //!
        vector<TBranch*> b_htt_caGroomJet_phi; //!
        vector<TBranch*> b_htt_caGroomJet_m; //!

        TBranch        *b_rljet_Tau1_wta;
        TBranch        *b_rljet_Tau2_wta;
        TBranch        *b_rljet_Tau3_wta;
        TBranch        *b_rljet_Tau32_wta;
        TBranch        *b_rljet_ECF1;
        TBranch        *b_rljet_ECF2;
        TBranch        *b_rljet_ECF3;
        TBranch        *b_rljet_D2;
        TBranch        *b_rljet_FoxWolfram0;
        TBranch        *b_rljet_FoxWolfram2;
        TBranch        *b_rljet_Qw;
        TBranch        *b_rljet_Angularity;
        TBranch        *b_rljet_Aplanarity;
        TBranch        *b_rljet_Dip12;
        TBranch        *b_rljet_KtDR;
        TBranch        *b_rljet_Mu12;
        TBranch        *b_rljet_PlanarFlow;
        TBranch        *b_rljet_Sphericity;
        TBranch        *b_rljet_Split12;
        TBranch        *b_rljet_Split23;
        TBranch        *b_rljet_Split34;
        TBranch        *b_rljet_ThrustMaj;
        TBranch        *b_rljet_ThrustMin;
        TBranch        *b_rljet_ZCut12;

		TBranch        *b_rljet_NTrimSubjets; //!
		TBranch        *b_rljet_ungroomed_ntrk500; //!

        TBranch        *b_rljet_smoothMassTag50eff;   //!
        TBranch        *b_rljet_smoothMassTag80eff;   //!
        TBranch        *b_rljet_smoothTau32Tag50eff;   //!
        TBranch        *b_rljet_smoothTau32Tag80eff;   //!
        TBranch        *b_rljet_smoothMassTau32Tag50eff;   //!
        TBranch        *b_rljet_smoothMassTau32Tag80eff;   //!
        TBranch        *b_rljet_smoothWTag50eff;   //!
        TBranch        *b_rljet_smoothWTag25eff;   //!
        TBranch        *b_rljet_smoothZTag50eff;   //!
        TBranch        *b_rljet_smoothZTag25eff;   //!

        TBranch        *b_rljet_BDT_Top_Score;   //!
        TBranch        *b_rljet_BDT_W_Score;   //!

        TBranch        *b_rljet_btag_double;   //!
        TBranch        *b_rljet_btag_single;   //!
        TBranch        *b_rljet_btag_leading_pt;   //!

        TBranch        *b_rljet_SDw_win20_btag0;   //!
        TBranch        *b_rljet_SDz_win20_btag0;   //!
        TBranch        *b_rljet_SDt_win50_btag0;   //!
        TBranch        *b_rljet_SDw_win20_btag0_UP;   //!
        TBranch        *b_rljet_SDz_win20_btag0_UP;   //!
        TBranch        *b_rljet_SDt_win50_btag0_UP;   //!
        TBranch        *b_rljet_SDw_win20_btag0_DOWN;   //!
        TBranch        *b_rljet_SDz_win20_btag0_DOWN;   //!
        TBranch        *b_rljet_SDt_win50_btag0_DOWN;   //!

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

    rljet_pt = 0;
    rljet_eta = 0;
    rljet_phi = 0;
    rljet_m = 0;

    rljet_Tau1_wta = 0;
    rljet_Tau2_wta = 0;
    rljet_Tau3_wta = 0;
    rljet_Tau32_wta = 0;
    rljet_ECF1 = 0;
    rljet_ECF2 = 0;
    rljet_ECF3 = 0;
    rljet_D2 = 0;
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

    tljet_pt = 0;
    tljet_eta = 0;
    tljet_phi = 0;
    tljet_m = 0;
    tljet_dR = 0;
    tljet_D2 = 0;
    tljet_Tau32_wta = 0;

    rljet_dRmatched_reco_truth = 0;
    rljet_dRmatched_particle_flavor = 0;
    rljet_dRmatched_maxEParton_flavor = 0;
    rljet_dRmatched_topBChild = 0;
    rljet_dRmatched_nQuarkChildren = 0;

    hltjet_m = 0;
    hltjet_pt = 0;
    hltjet_eta = 0;
    hltjet_phi = 0;
    hltjet_dR = 0;

    htt_caJet_pt = 0;
    htt_caJet_eta = 0;
    htt_caJet_phi = 0;
    htt_caJet_m = 0;

    rljet_smoothMassTag50eff = 0;
    rljet_smoothMassTag80eff = 0;
    rljet_smoothTau32Tag50eff = 0;
    rljet_smoothTau32Tag80eff = 0;
    rljet_smoothMassTau32Tag50eff = 0;
    rljet_smoothMassTau32Tag80eff = 0;
    rljet_smoothWTag50eff = 0;
    rljet_smoothWTag25eff = 0;
    rljet_smoothZTag50eff = 0;
    rljet_smoothZTag25eff = 0;

    rljet_BDT_Top_Score = 0;
    rljet_BDT_W_Score = 0;

    rljet_btag_double = 0;
    rljet_btag_single = 0;
    rljet_btag_leading_pt = 0;

    rljet_SDw_win20_btag0 = 0;
    rljet_SDz_win20_btag0 = 0;
    rljet_SDt_win50_btag0 = 0;
    rljet_SDw_win20_btag0_UP = 0;
    rljet_SDz_win20_btag0_UP = 0;
    rljet_SDt_win50_btag0_UP = 0;
    rljet_SDw_win20_btag0_DOWN = 0;
    rljet_SDz_win20_btag0_DOWN = 0;
    rljet_SDt_win50_btag0_DOWN = 0;

    // Set branch addresses and branch pointers
    if (!tree) return;
    fChain = tree;
    fChain->SetMakeClass(1);

    if (this->operating_on_mc) {
        fChain->SetBranchAddress("weight_mc"        , &weight_mc        , &b_weight_mc);
        fChain->SetBranchAddress("weight_pileup"    , &weight_pileup    , &b_weight_pileup);
        fChain->SetBranchAddress("weight_jvt"       , &weight_jvt       , &b_weight_jvt);
        fChain->SetBranchAddress("weight_bTagSF_70" , &weight_bTagSF_70 , &b_weight_bTagSF_70);
    }

    fChain->SetBranchAddress("mcChannelNumber" , &mcChannelNumber , &b_mcChannelNumber);
    fChain->SetBranchAddress("eventNumber"     , &eventNumber     , &b_eventNumber);
    fChain->SetBranchAddress("runNumber"       , &runNumber       , &b_runNumber);
    fChain->SetBranchAddress("backgroundFlags" , &backgroundFlags , &b_backgroundFlags);

    fChain->SetBranchAddress("rljet_pt"  , &rljet_pt  , &b_rljet_pt);
    fChain->SetBranchAddress("rljet_eta" , &rljet_eta , &b_rljet_eta);
    fChain->SetBranchAddress("rljet_phi" , &rljet_phi , &b_rljet_phi);
    fChain->SetBranchAddress("rljet_m"   , &rljet_m   , &b_rljet_m);

    if (fChain->GetListOfBranches()->FindObject("rljet_Tau32_wta")) {
        fChain->SetBranchAddress("rljet_Tau32_wta", &rljet_Tau32_wta, &b_rljet_Tau32_wta);
    } else {
        compute_Tau32 = true;
    }

    if (fChain->GetListOfBranches()->FindObject("rljet_D2")) {
        fChain->SetBranchAddress("rljet_D2", &rljet_D2, &b_rljet_D2);
    } else {
        compute_D2 = true;
    }

    fChain->SetBranchAddress("rljet_smoothMassTag50eff"      , &rljet_smoothMassTag50eff      , &b_rljet_smoothMassTag50eff);
    fChain->SetBranchAddress("rljet_smoothMassTag80eff"      , &rljet_smoothMassTag80eff      , &b_rljet_smoothMassTag80eff);
    fChain->SetBranchAddress("rljet_smoothTau32Tag50eff"     , &rljet_smoothTau32Tag50eff     , &b_rljet_smoothTau32Tag50eff);
    fChain->SetBranchAddress("rljet_smoothTau32Tag80eff"     , &rljet_smoothTau32Tag80eff     , &b_rljet_smoothTau32Tag80eff);
    fChain->SetBranchAddress("rljet_smoothMassTau32Tag50eff" , &rljet_smoothMassTau32Tag50eff , &b_rljet_smoothMassTau32Tag50eff);
    fChain->SetBranchAddress("rljet_smoothMassTau32Tag80eff" , &rljet_smoothMassTau32Tag80eff , &b_rljet_smoothMassTau32Tag80eff);
    fChain->SetBranchAddress("rljet_smoothWTag50eff"         , &rljet_smoothWTag50eff         , &b_rljet_smoothWTag50eff);
    fChain->SetBranchAddress("rljet_smoothWTag25eff"         , &rljet_smoothWTag25eff         , &b_rljet_smoothWTag25eff);
    fChain->SetBranchAddress("rljet_smoothZTag50eff"         , &rljet_smoothZTag50eff         , &b_rljet_smoothZTag50eff);
    fChain->SetBranchAddress("rljet_smoothZTag25eff"         , &rljet_smoothZTag25eff         , &b_rljet_smoothZTag25eff);

    if (this->operating_on_mc) {
        fChain->SetBranchAddress("rljet_dRmatched_reco_truth"        , &rljet_dRmatched_reco_truth        , &b_rljet_dRmatched_reco_truth);
        fChain->SetBranchAddress("rljet_dRmatched_particle_flavor"   , &rljet_dRmatched_particle_flavor   , &b_rljet_dRmatched_particle_flavor);
        fChain->SetBranchAddress("rljet_dRmatched_maxEParton_flavor" , &rljet_dRmatched_maxEParton_flavor , &b_rljet_dRmatched_maxEParton_flavor);
        fChain->SetBranchAddress("rljet_dRmatched_topBChild"         , &rljet_dRmatched_topBChild         , &b_rljet_dRmatched_topBChild);
        fChain->SetBranchAddress("rljet_dRmatched_nQuarkChildren"    , &rljet_dRmatched_nQuarkChildren    , &b_rljet_dRmatched_nQuarkChildren);
    }

    if (sub_dir_str == "nominal") {
        if (!this->operating_on_mc) {
            fChain->SetBranchAddress("mu_original_xAOD", &mu_original_xAOD, &b_mu_original_xAOD);
        }

        fChain->SetBranchAddress("mu", &mu, &b_mu);

        fChain->SetBranchAddress("met_met" , &met_met , &b_met_met);
        fChain->SetBranchAddress("met_phi" , &met_phi , &b_met_phi);

        fChain->SetBranchAddress(this->data_trigger_str.c_str(), &HLT_jet_trigger, &b_HLT_jet_trigger);

        fChain->SetBranchAddress("rljet_count"  , &rljet_count  , &b_rljet_count);
        fChain->SetBranchAddress("rljet_mjj"    , &rljet_mjj    , &b_rljet_mjj);
        fChain->SetBranchAddress("rljet_ptasym" , &rljet_ptasym , &b_rljet_ptasym);
        fChain->SetBranchAddress("rljet_dy"     , &rljet_dy     , &b_rljet_dy);
        fChain->SetBranchAddress("rljet_dR"     , &rljet_dR     , &b_rljet_dR);

        fChain->SetBranchAddress("rljet_Tau1_wta"    , &rljet_Tau1_wta    , &b_rljet_Tau1_wta);
        fChain->SetBranchAddress("rljet_Tau2_wta"    , &rljet_Tau2_wta    , &b_rljet_Tau2_wta);
        fChain->SetBranchAddress("rljet_Tau3_wta"    , &rljet_Tau3_wta    , &b_rljet_Tau3_wta);
        fChain->SetBranchAddress("rljet_ECF1"        , &rljet_ECF1        , &b_rljet_ECF1);
        fChain->SetBranchAddress("rljet_ECF2"        , &rljet_ECF2        , &b_rljet_ECF2);
        fChain->SetBranchAddress("rljet_ECF3"        , &rljet_ECF3        , &b_rljet_ECF3);
        fChain->SetBranchAddress("rljet_FoxWolfram0" , &rljet_FoxWolfram0 , &b_rljet_FoxWolfram0);
        fChain->SetBranchAddress("rljet_FoxWolfram2" , &rljet_FoxWolfram2 , &b_rljet_FoxWolfram2);
        fChain->SetBranchAddress("rljet_Qw"          , &rljet_Qw          , &b_rljet_Qw);
        fChain->SetBranchAddress("rljet_Angularity"  , &rljet_Angularity  , &b_rljet_Angularity);
        fChain->SetBranchAddress("rljet_Aplanarity"  , &rljet_Aplanarity  , &b_rljet_Aplanarity);
        fChain->SetBranchAddress("rljet_Dip12"       , &rljet_Dip12       , &b_rljet_Dip12);
        fChain->SetBranchAddress("rljet_KtDR"        , &rljet_KtDR        , &b_rljet_KtDR);
        fChain->SetBranchAddress("rljet_Mu12"        , &rljet_Mu12        , &b_rljet_Mu12);
        fChain->SetBranchAddress("rljet_PlanarFlow"  , &rljet_PlanarFlow  , &b_rljet_PlanarFlow);
        fChain->SetBranchAddress("rljet_Sphericity"  , &rljet_Sphericity  , &b_rljet_Sphericity);
        fChain->SetBranchAddress("rljet_Split12"     , &rljet_Split12     , &b_rljet_Split12);
        fChain->SetBranchAddress("rljet_Split23"     , &rljet_Split23     , &b_rljet_Split23);
        fChain->SetBranchAddress("rljet_Split34"     , &rljet_Split34     , &b_rljet_Split34);
        fChain->SetBranchAddress("rljet_ThrustMaj"   , &rljet_ThrustMaj   , &b_rljet_ThrustMaj);
        fChain->SetBranchAddress("rljet_ThrustMin"   , &rljet_ThrustMin   , &b_rljet_ThrustMin);
        fChain->SetBranchAddress("rljet_ZCut12"      , &rljet_ZCut12      , &b_rljet_ZCut12);

		fChain->SetBranchAddress("rljet_NTrimSubjets"      , &rljet_NTrimSubjets      , &b_rljet_NTrimSubjets);
		fChain->SetBranchAddress("rljet_ungroomed_ntrk500" , &rljet_ungroomed_ntrk500 , &b_rljet_ungroomed_ntrk500);

        if (this->operating_on_mc) {
            fChain->SetBranchAddress("tljet_pt"        , &tljet_pt        , &b_tljet_pt);
            fChain->SetBranchAddress("tljet_eta"       , &tljet_eta       , &b_tljet_eta);
            fChain->SetBranchAddress("tljet_phi"       , &tljet_phi       , &b_tljet_phi);
            fChain->SetBranchAddress("tljet_m"         , &tljet_m         , &b_tljet_m);
            fChain->SetBranchAddress("tljet_dR"        , &tljet_dR        , &b_tljet_dR);
            fChain->SetBranchAddress("tljet_D2"        , &tljet_D2        , &b_tljet_D2);
            fChain->SetBranchAddress("tljet_Tau32_wta" , &tljet_Tau32_wta , &b_tljet_Tau32_wta);
        }

        fChain->SetBranchAddress("hltjet_count" , &hltjet_count , &b_hltjet_count);
        fChain->SetBranchAddress("hltjet_m"     , &hltjet_m     , &b_hltjet_m);
        fChain->SetBranchAddress("hltjet_pt"    , &hltjet_pt    , &b_hltjet_pt);
        fChain->SetBranchAddress("hltjet_eta"   , &hltjet_eta   , &b_hltjet_eta);
        fChain->SetBranchAddress("hltjet_phi"   , &hltjet_phi   , &b_hltjet_phi);
        fChain->SetBranchAddress("hltjet_dR"    , &hltjet_dR    , &b_hltjet_dR);

        fChain->SetBranchAddress("caJet_count"   , &caJet_count   , &b_caJet_count);
        fChain->SetBranchAddress("htt_caJet_pt"  , &htt_caJet_pt  , &b_htt_caJet_pt);
        fChain->SetBranchAddress("htt_caJet_eta" , &htt_caJet_eta , &b_htt_caJet_eta);
        fChain->SetBranchAddress("htt_caJet_phi" , &htt_caJet_phi , &b_htt_caJet_phi);
        fChain->SetBranchAddress("htt_caJet_m"   , &htt_caJet_m   , &b_htt_caJet_m);

        if (this->operating_on_mc) {
            htt_config_strings = { "def", "sjcalib0970", "sjcalib1030" };
        } else {
            htt_config_strings = { "def" };
        }

        const int num_htt_configs = htt_config_strings.size();

        htt_pt.resize(num_htt_configs);
        htt_eta.resize(num_htt_configs);
        htt_phi.resize(num_htt_configs);
        htt_m.resize(num_htt_configs);
        htt_m23m123.resize(num_htt_configs);
        htt_atan1312.resize(num_htt_configs);
        htt_pts1.resize(num_htt_configs);
        htt_pts2.resize(num_htt_configs);
        htt_pts3.resize(num_htt_configs);
        htt_nTagCands.resize(num_htt_configs);
        htt_tag.resize(num_htt_configs);

        b_htt_pt.resize(num_htt_configs);
        b_htt_eta.resize(num_htt_configs);
        b_htt_phi.resize(num_htt_configs);
        b_htt_m.resize(num_htt_configs);
        b_htt_m23m123.resize(num_htt_configs);
        b_htt_atan1312.resize(num_htt_configs);
        b_htt_pts1.resize(num_htt_configs);
        b_htt_pts2.resize(num_htt_configs);
        b_htt_pts3.resize(num_htt_configs);
        b_htt_nTagCands.resize(num_htt_configs);
        b_htt_tag.resize(num_htt_configs);

        htt_caGroomJet_pt.resize(num_htt_configs);
        htt_caGroomJet_eta.resize(num_htt_configs);
        htt_caGroomJet_phi.resize(num_htt_configs);
        htt_caGroomJet_m.resize(num_htt_configs);

        b_htt_caGroomJet_pt.resize(num_htt_configs);
        b_htt_caGroomJet_eta.resize(num_htt_configs);
        b_htt_caGroomJet_phi.resize(num_htt_configs);
        b_htt_caGroomJet_m.resize(num_htt_configs);

        for (int i = 0; i < num_htt_configs; i++)
        {
            fChain->SetBranchAddress(getHTTBranchName("pt"        , htt_config_strings[i]).c_str() , &htt_pt[i]        , &b_htt_pt[i]);
            fChain->SetBranchAddress(getHTTBranchName("eta"       , htt_config_strings[i]).c_str() , &htt_eta[i]       , &b_htt_eta[i]);
            fChain->SetBranchAddress(getHTTBranchName("phi"       , htt_config_strings[i]).c_str() , &htt_phi[i]       , &b_htt_phi[i]);
            fChain->SetBranchAddress(getHTTBranchName("m"         , htt_config_strings[i]).c_str() , &htt_m[i]         , &b_htt_m[i]);
            fChain->SetBranchAddress(getHTTBranchName("m23m123"   , htt_config_strings[i]).c_str() , &htt_m23m123[i]   , &b_htt_m23m123[i]);
            fChain->SetBranchAddress(getHTTBranchName("atan1312"  , htt_config_strings[i]).c_str() , &htt_atan1312[i]  , &b_htt_atan1312[i]);
            fChain->SetBranchAddress(getHTTBranchName("pts1"      , htt_config_strings[i]).c_str() , &htt_pts1[i]      , &b_htt_pts1[i]);
            fChain->SetBranchAddress(getHTTBranchName("pts2"      , htt_config_strings[i]).c_str() , &htt_pts2[i]      , &b_htt_pts2[i]);
            fChain->SetBranchAddress(getHTTBranchName("pts3"      , htt_config_strings[i]).c_str() , &htt_pts3[i]      , &b_htt_pts3[i]);
            fChain->SetBranchAddress(getHTTBranchName("nTagCands" , htt_config_strings[i]).c_str() , &htt_nTagCands[i] , &b_htt_nTagCands[i]);
            fChain->SetBranchAddress(getHTTBranchName("tag"       , htt_config_strings[i]).c_str() , &htt_tag[i]       , &b_htt_tag[i]);

            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_pt"  , htt_config_strings[i]).c_str() , &htt_caGroomJet_pt[i]  , &b_htt_caGroomJet_pt[i]);
            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_eta" , htt_config_strings[i]).c_str() , &htt_caGroomJet_eta[i] , &b_htt_caGroomJet_eta[i]);
            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_phi" , htt_config_strings[i]).c_str() , &htt_caGroomJet_phi[i] , &b_htt_caGroomJet_phi[i]);
            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_m"   , htt_config_strings[i]).c_str() , &htt_caGroomJet_m[i]   , &b_htt_caGroomJet_m[i]);
        }

        fChain->SetBranchAddress("rljet_BDT_Top_Score" , &rljet_BDT_Top_Score , &b_rljet_BDT_Top_Score);
        fChain->SetBranchAddress("rljet_BDT_W_Score"   , &rljet_BDT_W_Score   , &b_rljet_BDT_W_Score);

        fChain->SetBranchAddress("rljet_btag_double"     , &rljet_btag_double     , &b_rljet_btag_double);
        fChain->SetBranchAddress("rljet_btag_single"     , &rljet_btag_single     , &b_rljet_btag_single);
        fChain->SetBranchAddress("rljet_btag_leading_pt" , &rljet_btag_leading_pt , &b_rljet_btag_leading_pt);

        fChain->SetBranchAddress("rljet_SDw_win20_btag0", &rljet_SDw_win20_btag0, &b_rljet_SDw_win20_btag0);
        fChain->SetBranchAddress("rljet_SDz_win20_btag0", &rljet_SDz_win20_btag0, &b_rljet_SDz_win20_btag0);
        fChain->SetBranchAddress("rljet_SDt_win50_btag0", &rljet_SDt_win50_btag0, &b_rljet_SDt_win50_btag0);

        if (this->operating_on_mc) {
            fChain->SetBranchAddress("rljet_SDw_win20_btag0_UP"   , &rljet_SDw_win20_btag0_UP   , &b_rljet_SDw_win20_btag0_UP);
            fChain->SetBranchAddress("rljet_SDz_win20_btag0_UP"   , &rljet_SDz_win20_btag0_UP   , &b_rljet_SDz_win20_btag0_UP);
            fChain->SetBranchAddress("rljet_SDt_win50_btag0_UP"   , &rljet_SDt_win50_btag0_UP   , &b_rljet_SDt_win50_btag0_UP);
            fChain->SetBranchAddress("rljet_SDw_win20_btag0_DOWN" , &rljet_SDw_win20_btag0_DOWN , &b_rljet_SDw_win20_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDz_win20_btag0_DOWN" , &rljet_SDz_win20_btag0_DOWN , &b_rljet_SDz_win20_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDt_win50_btag0_DOWN" , &rljet_SDt_win50_btag0_DOWN , &b_rljet_SDt_win50_btag0_DOWN);
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

