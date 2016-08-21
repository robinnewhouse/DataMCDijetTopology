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

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"


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
        bool compute_tau32;
        bool compute_d2;

        const float luminosity;

        static const std::unordered_map<std::string, EventSelector>
            available_event_selectors;
        EventSelector chosen_event_selector;

        float max_weight;
        float SF_lumi_Fb;

        int num_bdt_rejects;

        vector<std::string> htt_config_strings;

        std::ofstream output_log;

        std::unordered_map<std::string, bool> prerec_tag_map;
        std::unordered_map<std::string, bool> SD_nominal_tag_map;
        std::unordered_map<std::string, bool> SD_systematic_tag_map;
        std::unordered_map<std::string, bool> nominal_tag_map;

        TMVA::Reader *readerTOP;
        TMVA::Reader *readerW;

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

        Int_t           rljet_N;
        vector<float>   *rljet_pt;
        vector<float>   *rljet_eta;
        vector<float>   *rljet_phi;
        vector<float>   *rljet_e;
        vector<float>   *rljet_m;
        vector<float>   *rljet_m_ta;
        vector<float>   *rljet_m_ta_nocalib;
        Float_t         rl_dijet_m;

        vector<float>   *rljet_tau1;
        vector<float>   *rljet_tau2;
        vector<float>   *rljet_tau3;
        vector<float>   *rljet_tau1_wta;
        vector<float>   *rljet_tau2_wta;
        vector<float>   *rljet_tau3_wta;
        vector<float>   *rljet_tau32_wta;
        vector<float>   *rljet_split12;
        vector<float>   *rljet_split23;
        vector<float>   *rljet_split34;
        vector<float>   *rljet_qw;
        vector<float>   *rljet_ecf1;
        vector<float>   *rljet_ecf2;
        vector<float>   *rljet_ecf3;
        vector<float>   *rljet_d2;

        float rljet_tau21_wta;

		vector<int>     *rljet_ntrimsubjets;
		vector<float>   *rljet_width;
		vector<float>   *rljet_area;
		vector<int>     *rljet_ungroomed_ntrk500;

        vector<float>   *tljet_pt;
        vector<float>   *tljet_eta;
        vector<float>   *tljet_phi;
        vector<float>   *tljet_m;
        vector<float>   *tljet_dR;

        Int_t           hltjet_N;
        vector<float>   *hltjet_m;
        vector<float>   *hltjet_pt;
        vector<float>   *hltjet_eta;
        vector<float>   *hltjet_phi;
        vector<float>   *hltjet_dR;

        Int_t           caJet_N;
        // ungroomed C/A 1.5 jets
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

        vector<bool>    *rljet_btag_double;
        vector<bool>    *rljet_btag_single;
        vector<bool>    *rljet_btag_leading_pt;

        vector<double>  *rljet_SDw_win20_btag0;
        vector<double>  *rljet_SDw_win25_btag0;
        vector<double>  *rljet_SDz_win20_btag0;
        vector<double>  *rljet_SDz_win25_btag0;
        vector<double>  *rljet_SDt_win50_btag0;
        vector<double>  *rljet_SDt_win55_btag0;
        vector<double>  *rljet_SDw_win20_btag0_UP;
        vector<double>  *rljet_SDw_win25_btag0_UP;
        vector<double>  *rljet_SDz_win20_btag0_UP;
        vector<double>  *rljet_SDz_win25_btag0_UP;
        vector<double>  *rljet_SDt_win50_btag0_UP;
        vector<double>  *rljet_SDt_win55_btag0_UP;
        vector<double>  *rljet_SDw_win20_btag0_DOWN;
        vector<double>  *rljet_SDw_win25_btag0_DOWN;
        vector<double>  *rljet_SDz_win20_btag0_DOWN;
        vector<double>  *rljet_SDz_win25_btag0_DOWN;
        vector<double>  *rljet_SDt_win50_btag0_DOWN;
        vector<double>  *rljet_SDt_win55_btag0_DOWN;

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

        TBranch        *b_rljet_N;   //!
        TBranch        *b_rljet_pt;   //!
        TBranch        *b_rljet_eta;   //!
        TBranch        *b_rljet_phi;   //!
        TBranch        *b_rljet_m;   //!
        TBranch        *b_rljet_e;   //!

        TBranch        *b_rljet_m_ta;   //!
        TBranch        *b_rljet_m_ta_nocalib;   //!
        TBranch        *b_rl_dijet_m;   //!

        TBranch        *b_tljet_pt;   //!
        TBranch        *b_tljet_eta;   //!
        TBranch        *b_tljet_phi;   //!
        TBranch        *b_tljet_m;   //!
        TBranch        *b_tljet_dR;   //!

        TBranch        *b_hltjet_N;   //!
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
        vector<TBranch*> b_htt_nTagCands; //!
        vector<TBranch*> b_htt_tag; //!

        TBranch        *b_caJet_N;   //!
        vector<TBranch*> b_htt_caGroomJet_pt; //!
        vector<TBranch*> b_htt_caGroomJet_eta; //!
        vector<TBranch*> b_htt_caGroomJet_phi; //!
        vector<TBranch*> b_htt_caGroomJet_m; //!

        TBranch        *b_rljet_tau1;   //!
        TBranch        *b_rljet_tau2;   //!
        TBranch        *b_rljet_tau3;   //!
        TBranch        *b_rljet_tau1_wta;   //!
        TBranch        *b_rljet_tau2_wta;   //!
        TBranch        *b_rljet_tau3_wta;   //!
        TBranch        *b_rljet_tau32_wta;   //!
        TBranch        *b_rljet_split12;   //!
        TBranch        *b_rljet_split23;   //!
        TBranch        *b_rljet_split34;   //!
        TBranch        *b_rljet_qw;   //!
        TBranch        *b_rljet_ecf1;   //!
        TBranch        *b_rljet_ecf2;   //!
        TBranch        *b_rljet_ecf3;   //!
        TBranch        *b_rljet_d2;   //!

		TBranch        *b_rljet_ntrimsubjets; //!
		TBranch        *b_rljet_width; //!
		TBranch        *b_rljet_area; //!
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

        TBranch        *b_rljet_btag_double;   //!
        TBranch        *b_rljet_btag_single;   //!
        TBranch        *b_rljet_btag_leading_pt;   //!

        TBranch        *b_rljet_SDw_win20_btag0;   //!
        TBranch        *b_rljet_SDw_win25_btag0;   //!
        TBranch        *b_rljet_SDz_win20_btag0;   //!
        TBranch        *b_rljet_SDz_win25_btag0;   //!
        TBranch        *b_rljet_SDt_win50_btag0;   //!
        TBranch        *b_rljet_SDt_win55_btag0;   //!
        TBranch        *b_rljet_SDw_win20_btag0_UP;   //!
        TBranch        *b_rljet_SDw_win25_btag0_UP;   //!
        TBranch        *b_rljet_SDz_win20_btag0_UP;   //!
        TBranch        *b_rljet_SDz_win25_btag0_UP;   //!
        TBranch        *b_rljet_SDt_win50_btag0_UP;   //!
        TBranch        *b_rljet_SDt_win55_btag0_UP;   //!
        TBranch        *b_rljet_SDw_win20_btag0_DOWN;   //!
        TBranch        *b_rljet_SDw_win25_btag0_DOWN;   //!
        TBranch        *b_rljet_SDz_win20_btag0_DOWN;   //!
        TBranch        *b_rljet_SDz_win25_btag0_DOWN;   //!
        TBranch        *b_rljet_SDt_win50_btag0_DOWN;   //!
        TBranch        *b_rljet_SDt_win55_btag0_DOWN;   //!

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
    rljet_e = 0;
    rljet_m = 0;
    rljet_m_ta = 0;
    rljet_m_ta_nocalib = 0;

    rljet_tau1 = 0;
    rljet_tau2 = 0;
    rljet_tau3 = 0;
    rljet_tau1_wta = 0;
    rljet_tau2_wta = 0;
    rljet_tau3_wta = 0;
    rljet_tau32_wta = 0;
    rljet_tau21_wta = 0;
    rljet_split12 = 0;
    rljet_split23 = 0;
    rljet_split34 = 0;
    rljet_qw = 0;
    rljet_ecf1 = 0;
    rljet_ecf2 = 0;
    rljet_ecf3 = 0;
    rljet_d2 = 0;

    rljet_ntrimsubjets = 0;
    rljet_width = 0;
    rljet_area = 0;
    rljet_ungroomed_ntrk500 = 0;

    tljet_pt = 0;
    tljet_eta = 0;
    tljet_phi = 0;
    tljet_m = 0;
    tljet_dR = 0;

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

    rljet_btag_double = 0;
    rljet_btag_single = 0;
    rljet_btag_leading_pt = 0;

    rljet_SDw_win20_btag0 = 0;
    rljet_SDw_win25_btag0 = 0;
    rljet_SDz_win20_btag0 = 0;
    rljet_SDz_win25_btag0 = 0;
    rljet_SDt_win50_btag0 = 0;
    rljet_SDt_win55_btag0 = 0;
    rljet_SDw_win20_btag0_UP = 0;
    rljet_SDw_win25_btag0_UP = 0;
    rljet_SDz_win20_btag0_UP = 0;
    rljet_SDz_win25_btag0_UP = 0;
    rljet_SDt_win50_btag0_UP = 0;
    rljet_SDt_win55_btag0_UP = 0;
    rljet_SDw_win20_btag0_DOWN = 0;
    rljet_SDw_win25_btag0_DOWN = 0;
    rljet_SDz_win20_btag0_DOWN = 0;
    rljet_SDz_win25_btag0_DOWN = 0;
    rljet_SDt_win50_btag0_DOWN = 0;
    rljet_SDt_win55_btag0_DOWN = 0;

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

    if (fChain->GetListOfBranches()->FindObject("rljet_tau32_wta")) {
        fChain->SetBranchAddress("rljet_tau32_wta", &rljet_tau32_wta, &b_rljet_tau32_wta);
    } else {
        compute_tau32 = true;
    }

    if (fChain->GetListOfBranches()->FindObject("rljet_d2")) {
        fChain->SetBranchAddress("rljet_d2", &rljet_d2, &b_rljet_d2);
    } else {
        compute_d2 = true;
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

    if (sub_dir_str == "nominal") {
        if (!this->operating_on_mc) {
            fChain->SetBranchAddress("mu_original_xAOD", &mu_original_xAOD, &b_mu_original_xAOD);
        }

        fChain->SetBranchAddress("mu", &mu, &b_mu);

        fChain->SetBranchAddress("met_met" , &met_met , &b_met_met);
        fChain->SetBranchAddress("met_phi" , &met_phi , &b_met_phi);

        fChain->SetBranchAddress(this->data_trigger_str.c_str(), &HLT_jet_trigger, &b_HLT_jet_trigger);

        fChain->SetBranchAddress("rljet_N"    , &rljet_N    , &b_rljet_N);
        fChain->SetBranchAddress("rljet_e"    , &rljet_e    , &b_rljet_e);
        fChain->SetBranchAddress("rl_dijet_m" , &rl_dijet_m , &b_rl_dijet_m);

        fChain->SetBranchAddress("rljet_m_ta"         , &rljet_m_ta         , &b_rljet_m_ta);
        fChain->SetBranchAddress("rljet_m_ta_nocalib" , &rljet_m_ta_nocalib , &b_rljet_m_ta_nocalib);

        fChain->SetBranchAddress("rljet_tau1"     , &rljet_tau1     , &b_rljet_tau1);
        fChain->SetBranchAddress("rljet_tau2"     , &rljet_tau2     , &b_rljet_tau2);
        fChain->SetBranchAddress("rljet_tau3"     , &rljet_tau3     , &b_rljet_tau3);
        fChain->SetBranchAddress("rljet_tau1_wta" , &rljet_tau1_wta , &b_rljet_tau1_wta);
        fChain->SetBranchAddress("rljet_tau2_wta" , &rljet_tau2_wta , &b_rljet_tau2_wta);
        fChain->SetBranchAddress("rljet_tau3_wta" , &rljet_tau3_wta , &b_rljet_tau3_wta);

        fChain->SetBranchAddress("rljet_split12" , &rljet_split12 , &b_rljet_split12);
        fChain->SetBranchAddress("rljet_split23" , &rljet_split23 , &b_rljet_split23);
        fChain->SetBranchAddress("rljet_split34" , &rljet_split34 , &b_rljet_split34);
        fChain->SetBranchAddress("rljet_qw"      , &rljet_qw      , &b_rljet_qw);
        fChain->SetBranchAddress("rljet_ecf1"    , &rljet_ecf1    , &b_rljet_ecf1);
        fChain->SetBranchAddress("rljet_ecf2"    , &rljet_ecf2    , &b_rljet_ecf2);
        fChain->SetBranchAddress("rljet_ecf3"    , &rljet_ecf3    , &b_rljet_ecf3);

		fChain->SetBranchAddress("rljet_ntrimsubjets"      , &rljet_ntrimsubjets      , &b_rljet_ntrimsubjets);
		fChain->SetBranchAddress("rljet_width"             , &rljet_width             , &b_rljet_width);
		fChain->SetBranchAddress("rljet_area"              , &rljet_area              , &b_rljet_area);
		fChain->SetBranchAddress("rljet_ungroomed_ntrk500" , &rljet_ungroomed_ntrk500 , &b_rljet_ungroomed_ntrk500);

        if (this->operating_on_mc) {
            fChain->SetBranchAddress("tljet_pt"  , &tljet_pt  , &b_tljet_pt);
            fChain->SetBranchAddress("tljet_eta" , &tljet_eta , &b_tljet_eta);
            fChain->SetBranchAddress("tljet_phi" , &tljet_phi , &b_tljet_phi);
            fChain->SetBranchAddress("tljet_m"   , &tljet_m   , &b_tljet_m);
            fChain->SetBranchAddress("tljet_dR"  , &tljet_dR  , &b_tljet_dR);
        }

        fChain->SetBranchAddress("hltjet_N"   , &hltjet_N   , &b_hltjet_N);
        fChain->SetBranchAddress("hltjet_m"   , &hltjet_m   , &b_hltjet_m);
        fChain->SetBranchAddress("hltjet_pt"  , &hltjet_pt  , &b_hltjet_pt);
        fChain->SetBranchAddress("hltjet_eta" , &hltjet_eta , &b_hltjet_eta);
        fChain->SetBranchAddress("hltjet_phi" , &hltjet_phi , &b_hltjet_phi);
        fChain->SetBranchAddress("hltjet_dR"  , &hltjet_dR  , &b_hltjet_dR);

        fChain->SetBranchAddress("caJet_N"       , &caJet_N       , &b_caJet_N);
        fChain->SetBranchAddress("htt_caJet_pt"  , &htt_caJet_pt  , &b_htt_caJet_pt);
        fChain->SetBranchAddress("htt_caJet_eta" , &htt_caJet_eta , &b_htt_caJet_eta);
        fChain->SetBranchAddress("htt_caJet_phi" , &htt_caJet_phi , &b_htt_caJet_phi);
        fChain->SetBranchAddress("htt_caJet_m"   , &htt_caJet_m   , &b_htt_caJet_m);

        if (this->operating_on_mc) {
            htt_config_strings = {
                "def",
                "sjcalib0955",
                "sjcalib0970",
                "sjcalib0985",
                "sjcalib1015",
                "sjcalib1030",
                "sjcalib1045"
            };
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
        htt_nTagCands.resize(num_htt_configs);
        htt_tag.resize(num_htt_configs);

        b_htt_pt.resize(num_htt_configs);
        b_htt_eta.resize(num_htt_configs);
        b_htt_phi.resize(num_htt_configs);
        b_htt_m.resize(num_htt_configs);
        b_htt_m23m123.resize(num_htt_configs);
        b_htt_atan1312.resize(num_htt_configs);
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
            fChain->SetBranchAddress(getHTTBranchName("nTagCands" , htt_config_strings[i]).c_str() , &htt_nTagCands[i] , &b_htt_nTagCands[i]);
            fChain->SetBranchAddress(getHTTBranchName("tag"       , htt_config_strings[i]).c_str() , &htt_tag[i]       , &b_htt_tag[i]);

            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_pt"  , htt_config_strings[i]).c_str() , &htt_caGroomJet_pt[i]  , &b_htt_caGroomJet_pt[i]);
            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_eta" , htt_config_strings[i]).c_str() , &htt_caGroomJet_eta[i] , &b_htt_caGroomJet_eta[i]);
            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_phi" , htt_config_strings[i]).c_str() , &htt_caGroomJet_phi[i] , &b_htt_caGroomJet_phi[i]);
            fChain->SetBranchAddress(getHTTBranchName("caGroomJet_m"   , htt_config_strings[i]).c_str() , &htt_caGroomJet_m[i]   , &b_htt_caGroomJet_m[i]);
        }

        fChain->SetBranchAddress("rljet_btag_double"     , &rljet_btag_double     , &b_rljet_btag_double);
        fChain->SetBranchAddress("rljet_btag_single"     , &rljet_btag_single     , &b_rljet_btag_single);
        fChain->SetBranchAddress("rljet_btag_leading_pt" , &rljet_btag_leading_pt , &b_rljet_btag_leading_pt);

        fChain->SetBranchAddress("rljet_SDw_win20_btag0", &rljet_SDw_win20_btag0, &b_rljet_SDw_win20_btag0);
        fChain->SetBranchAddress("rljet_SDw_win25_btag0", &rljet_SDw_win25_btag0, &b_rljet_SDw_win25_btag0);
        fChain->SetBranchAddress("rljet_SDz_win20_btag0", &rljet_SDz_win20_btag0, &b_rljet_SDz_win20_btag0);
        fChain->SetBranchAddress("rljet_SDz_win25_btag0", &rljet_SDz_win25_btag0, &b_rljet_SDz_win25_btag0);
        fChain->SetBranchAddress("rljet_SDt_win50_btag0", &rljet_SDt_win50_btag0, &b_rljet_SDt_win50_btag0);
        fChain->SetBranchAddress("rljet_SDt_win55_btag0", &rljet_SDt_win55_btag0, &b_rljet_SDt_win55_btag0);

        if (this->operating_on_mc) {
            fChain->SetBranchAddress("rljet_SDw_win20_btag0_UP", &rljet_SDw_win20_btag0_UP, &b_rljet_SDw_win20_btag0_UP);
            fChain->SetBranchAddress("rljet_SDw_win25_btag0_UP", &rljet_SDw_win25_btag0_UP, &b_rljet_SDw_win25_btag0_UP);
            fChain->SetBranchAddress("rljet_SDz_win20_btag0_UP", &rljet_SDz_win20_btag0_UP, &b_rljet_SDz_win20_btag0_UP);
            fChain->SetBranchAddress("rljet_SDz_win25_btag0_UP", &rljet_SDz_win25_btag0_UP, &b_rljet_SDz_win25_btag0_UP);
            fChain->SetBranchAddress("rljet_SDt_win50_btag0_UP", &rljet_SDt_win50_btag0_UP, &b_rljet_SDt_win50_btag0_UP);
            fChain->SetBranchAddress("rljet_SDt_win55_btag0_UP", &rljet_SDt_win55_btag0_UP, &b_rljet_SDt_win55_btag0_UP);
            fChain->SetBranchAddress("rljet_SDw_win20_btag0_DOWN", &rljet_SDw_win20_btag0_DOWN, &b_rljet_SDw_win20_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDw_win25_btag0_DOWN", &rljet_SDw_win25_btag0_DOWN, &b_rljet_SDw_win25_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDz_win20_btag0_DOWN", &rljet_SDz_win20_btag0_DOWN, &b_rljet_SDz_win20_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDz_win25_btag0_DOWN", &rljet_SDz_win25_btag0_DOWN, &b_rljet_SDz_win25_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDt_win50_btag0_DOWN", &rljet_SDt_win50_btag0_DOWN, &b_rljet_SDt_win50_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDt_win55_btag0_DOWN", &rljet_SDt_win55_btag0_DOWN, &b_rljet_SDt_win55_btag0_DOWN);
        }


        b_rljet_tau32_wta->GetEntry(0);
        b_rljet_qw->GetEntry(0);
        b_rljet_ecf3->GetEntry(0);
        b_rljet_d2->GetEntry(0);
        b_rljet_ecf1->GetEntry(0);
        b_rljet_split12->GetEntry(0);

        readerTOP->AddVariable( "fjet_Tau32_wta" , &rljet_tau32_wta->at(0) );
        readerTOP->AddVariable( "fjet_Qw"        , &rljet_qw->at(0)        );
        readerTOP->AddVariable( "fjet_ECF3"      , &rljet_ecf3->at(0)      );
        readerTOP->AddVariable( "fjet_D2"        , &rljet_d2->at(0)        );
        readerTOP->AddVariable( "fjet_Tau21_wta" , &rljet_tau21_wta        );
        readerTOP->AddVariable( "fjet_ECF1"      , &rljet_ecf1->at(0)      );
        readerTOP->AddVariable( "fjet_Split12"   , &rljet_split12->at(0)   );

        const char* const rc = getenv("ROOTCOREBIN");
        readerTOP->BookMVA( "BDTG method", std::string(rc) +
                "/data/DataMCbackgroundTools/bdt_weights/TMVAClassification_BDTG.TOP_NvarM_pT200to2000GeV.weights.xml");
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

