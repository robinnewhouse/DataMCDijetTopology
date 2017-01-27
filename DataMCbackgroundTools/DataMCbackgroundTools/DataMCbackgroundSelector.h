#ifndef DataMCbackgroundSelector_h
#define DataMCbackgroundSelector_h

#include "DataMCbackgroundTools/HistoPack.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "DataMCbackgroundTools/WeightTool.h"

#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

#include "TopExamples/AnalysisTools.h"

#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TNamed.h>
#include <TROOT.h>
#include <TSelector.h>
#include <TStyle.h>

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using std::vector;

class DataMCbackgroundSelector;

typedef std::function<bool(const DataMCbackgroundSelector*)> EventSelector;

class DataMCbackgroundSelector : public TSelector {
    public :
        TTree* fChain;   //!pointer to the analyzed TTree or TChain

        HistoPack* hp;

        static const std::unordered_map<std::string, EventSelector> available_event_selectors;
        EventSelector chosen_event_selector;

        std::ofstream output_log;

        const std::string output_filepath;
        const std::string root_dir_str;
        const std::string sub_dir_str;
        const std::string data_trigger_str;

        std::unique_ptr<WeightTool> weight_tool;

        const bool operating_on_mc;
        const bool on_nominal_branch;
        bool processing_dijet_slice;
        int dijet_slice_number;

        const float luminosity;
        float SF_lumi_Fb;

        // keep track of highest weight event in file for finding bugged events
        float max_weight;

        // MVA taggers
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_qqb;
        std::unique_ptr<JSSWTopTaggerBDT> m_topTagger_BDT_inclusive;
        std::unique_ptr<JSSWTopTaggerBDT> m_wTagger_BDT;
        std::unique_ptr<JSSWTopTaggerDNN> m_topTagger_DNN_qqb;
        std::unique_ptr<JSSWTopTaggerDNN> m_topTagger_DNN_inclusive;

        // various maps used for creating tagged histograms
        std::unordered_map<std::string, bool> smooth_tag_map;
        std::unordered_map<std::string, bool> ntrk_prerec_tag_map;
        std::unordered_map<std::string, bool> SD_nominal_tag_map;
        std::unordered_map<std::string, bool> SD_systematic_tag_map;
        std::unordered_map<std::string, bool> mva_tag_map;

        // Declaration of leaf types
        Char_t          HLT_jet_trigger;
        Float_t         weight_mc;
        Float_t         weight_pileup;
        Float_t         weight_leptonSF;
        Float_t         weight_jvt;
        ULong64_t       eventNumber;
        UInt_t          runNumber;
        UInt_t          randomRunNumber;
        UInt_t          mcChannelNumber;
        Float_t         mu;
        UInt_t          backgroundFlags;
        Float_t         met_met;
        Float_t         met_phi;
        Int_t           dijets;
        vector<float>   *rljet_eta;
        vector<float>   *rljet_phi;
        vector<float>   *rljet_m_comb;
        vector<float>   *rljet_pt_comb;
        vector<float>   *rljet_D2;
        vector<float>   *rljet_Tau32_wta;
        vector<int>     *rljet_smooth16Top_Tau32Split23Tag50eff;
        vector<int>     *rljet_smooth16Top_Tau32Split23Tag80eff;
        vector<int>     *rljet_smooth16Top_MassTau32Tag50eff;
        vector<int>     *rljet_smooth16Top_MassTau32Tag80eff;
        vector<int>     *rljet_smooth16Top_QwTau32Tag50eff;
        vector<int>     *rljet_smooth16Top_QwTau32Tag80eff;
        vector<int>     *rljet_smooth16WTag_50eff;
        vector<int>     *rljet_smooth16WTag_80eff;
        vector<int>     *rljet_smooth16ZTag_50eff;
        vector<int>     *rljet_smooth16ZTag_80eff;
        Int_t           NPV;
        Int_t           rljet_count;
        Float_t         rljet_mjj;
        Float_t         rljet_ptasym;
        Float_t         rljet_dy;
        Float_t         rljet_dR;
        Float_t         rljet_dphi;
        Float_t         rljet_deta;
        vector<float>   *rljet_m_calo;
        vector<float>   *rljet_pt_calo;
        vector<float>   *rljet_m_ta;
        vector<float>   *rljet_pt_ta;
        vector<int>     *rljet_smooth15Top_MassTau32Tag50eff;
        vector<int>     *rljet_smooth15Top_MassTau32Tag80eff;
        vector<int>     *rljet_smooth15WTag_50eff;
        vector<int>     *rljet_smooth15WTag_25eff;
        vector<int>     *rljet_smooth15ZTag_50eff;
        vector<int>     *rljet_smooth15ZTag_25eff;
        vector<float>   *rljet_Tau1_wta;
        vector<float>   *rljet_Tau2_wta;
        vector<float>   *rljet_Tau3_wta;
        vector<float>   *rljet_ECF1;
        vector<float>   *rljet_ECF2;
        vector<float>   *rljet_ECF3;
        vector<float>   *rljet_FoxWolfram0;
        vector<float>   *rljet_FoxWolfram2;
        vector<float>   *rljet_Qw;
        vector<float>   *rljet_Angularity;
        vector<float>   *rljet_Aplanarity;
        vector<float>   *rljet_Dip12;
        vector<float>   *rljet_KtDR;
        vector<float>   *rljet_Mu12;
        vector<float>   *rljet_PlanarFlow;
        vector<float>   *rljet_Sphericity;
        vector<float>   *rljet_Split12;
        vector<float>   *rljet_Split23;
        vector<float>   *rljet_Split34;
        vector<float>   *rljet_ThrustMaj;
        vector<float>   *rljet_ThrustMin;
        vector<float>   *rljet_ZCut12;
        vector<int>     *rljet_NTrimSubjets;
        vector<int>     *rljet_ungroomed_ntrk500;
        vector<float>   *htt_pt_def;
        vector<float>   *htt_eta_def;
        vector<float>   *htt_phi_def;
        vector<float>   *htt_m_def;
        vector<float>   *htt_m123_def;
        vector<float>   *htt_m23m123_def;
        vector<float>   *htt_atan1312_def;
        vector<int>     *htt_nTagCands_def;
        vector<int>     *htt_tag_def;
        vector<float>   *htt_pts1_def;
        vector<float>   *htt_pts2_def;
        vector<float>   *htt_pts3_def;
        vector<float>   *htt_pt_sjcalib1030;
        vector<float>   *htt_eta_sjcalib1030;
        vector<float>   *htt_phi_sjcalib1030;
        vector<float>   *htt_m_sjcalib1030;
        vector<float>   *htt_m123_sjcalib1030;
        vector<float>   *htt_m23m123_sjcalib1030;
        vector<float>   *htt_atan1312_sjcalib1030;
        vector<int>     *htt_nTagCands_sjcalib1030;
        vector<int>     *htt_tag_sjcalib1030;
        vector<float>   *htt_pts1_sjcalib1030;
        vector<float>   *htt_pts2_sjcalib1030;
        vector<float>   *htt_pts3_sjcalib1030;
        vector<float>   *htt_pt_sjcalib0970;
        vector<float>   *htt_eta_sjcalib0970;
        vector<float>   *htt_phi_sjcalib0970;
        vector<float>   *htt_m_sjcalib0970;
        vector<float>   *htt_m123_sjcalib0970;
        vector<float>   *htt_m23m123_sjcalib0970;
        vector<float>   *htt_atan1312_sjcalib0970;
        vector<int>     *htt_nTagCands_sjcalib0970;
        vector<int>     *htt_tag_sjcalib0970;
        vector<float>   *htt_pts1_sjcalib0970;
        vector<float>   *htt_pts2_sjcalib0970;
        vector<float>   *htt_pts3_sjcalib0970;
        vector<float>   *htt_caJet_pt;
        vector<float>   *htt_caJet_eta;
        vector<float>   *htt_caJet_phi;
        vector<float>   *htt_caJet_m;
        Int_t           caJet_count;
        vector<float>   *htt_caGroomJet_pt_def;
        vector<float>   *htt_caGroomJet_eta_def;
        vector<float>   *htt_caGroomJet_phi_def;
        vector<float>   *htt_caGroomJet_m_def;
        vector<float>   *htt_caGroomJet_pt_sjcalib1030;
        vector<float>   *htt_caGroomJet_eta_sjcalib1030;
        vector<float>   *htt_caGroomJet_phi_sjcalib1030;
        vector<float>   *htt_caGroomJet_m_sjcalib1030;
        vector<float>   *htt_caGroomJet_pt_sjcalib0970;
        vector<float>   *htt_caGroomJet_eta_sjcalib0970;
        vector<float>   *htt_caGroomJet_phi_sjcalib0970;
        vector<float>   *htt_caGroomJet_m_sjcalib0970;
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
        TBranch        *b_HLT_jet_trigger;  //!
        TBranch        *b_weight_mc;   //!
        TBranch        *b_weight_pileup;   //!
        TBranch        *b_weight_leptonSF;   //!
        TBranch        *b_weight_jvt;   //!
        TBranch        *b_eventNumber;   //!
        TBranch        *b_runNumber;   //!
        TBranch        *b_randomRunNumber;   //!
        TBranch        *b_mcChannelNumber;   //!
        TBranch        *b_mu;   //!
        TBranch        *b_backgroundFlags;   //!
        TBranch        *b_met_met;   //!
        TBranch        *b_met_phi;   //!
        TBranch        *b_dijets;   //!
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

    // Set object pointer
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
        fChain->SetBranchAddress("weight_mc", &weight_mc, &b_weight_mc);
        fChain->SetBranchAddress("weight_pileup", &weight_pileup, &b_weight_pileup);
        fChain->SetBranchAddress("weight_leptonSF", &weight_leptonSF, &b_weight_leptonSF);
        fChain->SetBranchAddress("weight_jvt", &weight_jvt, &b_weight_jvt);
        fChain->SetBranchAddress("randomRunNumber", &randomRunNumber, &b_randomRunNumber);
    } else {
        fChain->SetBranchAddress(this->data_trigger_str.c_str(), &HLT_jet_trigger, &b_HLT_jet_trigger); 
    }

    fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
    fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
    fChain->SetBranchAddress("mcChannelNumber", &mcChannelNumber, &b_mcChannelNumber);
    fChain->SetBranchAddress("mu", &mu, &b_mu);
    fChain->SetBranchAddress("backgroundFlags", &backgroundFlags, &b_backgroundFlags);
    fChain->SetBranchAddress("met_met", &met_met, &b_met_met);
    fChain->SetBranchAddress("met_phi", &met_phi, &b_met_phi);
    fChain->SetBranchAddress("dijets", &dijets, &b_dijets);
    fChain->SetBranchAddress("rljet_eta", &rljet_eta, &b_rljet_eta);
    fChain->SetBranchAddress("rljet_phi", &rljet_phi, &b_rljet_phi);
    fChain->SetBranchAddress("rljet_m_comb", &rljet_m_comb, &b_rljet_m_comb);
    fChain->SetBranchAddress("rljet_pt_comb", &rljet_pt_comb, &b_rljet_pt_comb);
    fChain->SetBranchAddress("rljet_D2", &rljet_D2, &b_rljet_D2);
    fChain->SetBranchAddress("rljet_Tau32_wta", &rljet_Tau32_wta, &b_rljet_Tau32_wta);
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

    if (sub_dir_str == "nominal") {
        fChain->SetBranchAddress("NPV", &NPV, &b_NPV);
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
        fChain->SetBranchAddress("m_rljet_smooth15Top_MassTau32Tag50eff", &rljet_smooth15Top_MassTau32Tag50eff, &b_m_rljet_smooth15Top_MassTau32Tag50eff);
        fChain->SetBranchAddress("m_rljet_smooth15Top_MassTau32Tag80eff", &rljet_smooth15Top_MassTau32Tag80eff, &b_m_rljet_smooth15Top_MassTau32Tag80eff);
        fChain->SetBranchAddress("rljet_smooth15WTag_50eff", &rljet_smooth15WTag_50eff, &b_rljet_smooth15WTag_50eff);
        fChain->SetBranchAddress("rljet_smooth15WTag_25eff", &rljet_smooth15WTag_25eff, &b_rljet_smooth15WTag_25eff);
        fChain->SetBranchAddress("rljet_smooth15ZTag_50eff", &rljet_smooth15ZTag_50eff, &b_rljet_smooth15ZTag_50eff);
        fChain->SetBranchAddress("rljet_smooth15ZTag_25eff", &rljet_smooth15ZTag_25eff, &b_rljet_smooth15ZTag_25eff);
        fChain->SetBranchAddress("rljet_Tau1_wta", &rljet_Tau1_wta, &b_rljet_Tau1_wta);
        fChain->SetBranchAddress("rljet_Tau2_wta", &rljet_Tau2_wta, &b_rljet_Tau2_wta);
        fChain->SetBranchAddress("rljet_Tau3_wta", &rljet_Tau3_wta, &b_rljet_Tau3_wta);
        fChain->SetBranchAddress("rljet_ECF1", &rljet_ECF1, &b_rljet_ECF1);
        fChain->SetBranchAddress("rljet_ECF2", &rljet_ECF2, &b_rljet_ECF2);
        fChain->SetBranchAddress("rljet_ECF3", &rljet_ECF3, &b_rljet_ECF3);
        fChain->SetBranchAddress("rljet_FoxWolfram0", &rljet_FoxWolfram0, &b_rljet_FoxWolfram0);
        fChain->SetBranchAddress("rljet_FoxWolfram2", &rljet_FoxWolfram2, &b_rljet_FoxWolfram2);
        fChain->SetBranchAddress("rljet_Qw", &rljet_Qw, &b_rljet_Qw);
        fChain->SetBranchAddress("rljet_Angularity", &rljet_Angularity, &b_rljet_Angularity);
        fChain->SetBranchAddress("rljet_Aplanarity", &rljet_Aplanarity, &b_rljet_Aplanarity);
        fChain->SetBranchAddress("rljet_Dip12", &rljet_Dip12, &b_rljet_Dip12);
        fChain->SetBranchAddress("rljet_KtDR", &rljet_KtDR, &b_rljet_KtDR);
        fChain->SetBranchAddress("rljet_Mu12", &rljet_Mu12, &b_rljet_Mu12);
        fChain->SetBranchAddress("rljet_PlanarFlow", &rljet_PlanarFlow, &b_rljet_PlanarFlow);
        fChain->SetBranchAddress("rljet_Sphericity", &rljet_Sphericity, &b_rljet_Sphericity);
        fChain->SetBranchAddress("rljet_Split12", &rljet_Split12, &b_rljet_Split12);
        fChain->SetBranchAddress("rljet_Split23", &rljet_Split23, &b_rljet_Split23);
        fChain->SetBranchAddress("rljet_Split34", &rljet_Split34, &b_rljet_Split34);
        fChain->SetBranchAddress("rljet_ThrustMaj", &rljet_ThrustMaj, &b_rljet_ThrustMaj);
        fChain->SetBranchAddress("rljet_ThrustMin", &rljet_ThrustMin, &b_rljet_ThrustMin);
        fChain->SetBranchAddress("rljet_ZCut12", &rljet_ZCut12, &b_rljet_ZCut12);
        fChain->SetBranchAddress("rljet_NTrimSubjets", &rljet_NTrimSubjets, &b_rljet_NTrimSubjets);
        fChain->SetBranchAddress("rljet_ungroomed_ntrk500", &rljet_ungroomed_ntrk500, &b_rljet_ungroomed_ntrk500);
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
        fChain->SetBranchAddress("rljet_SDw_win20_btag0", &rljet_SDw_win20_btag0, &b_rljet_SDw_win20_btag0);
        fChain->SetBranchAddress("rljet_SDz_win20_btag0", &rljet_SDz_win20_btag0, &b_rljet_SDz_win20_btag0);
        fChain->SetBranchAddress("rljet_SDt_win50_btag0", &rljet_SDt_win50_btag0, &b_rljet_SDt_win50_btag0);

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
            fChain->SetBranchAddress("rljet_SDw_win20_btag0_UP", &rljet_SDw_win20_btag0_UP, &b_rljet_SDw_win20_btag0_UP);
            fChain->SetBranchAddress("rljet_SDz_win20_btag0_UP", &rljet_SDz_win20_btag0_UP, &b_rljet_SDz_win20_btag0_UP);
            fChain->SetBranchAddress("rljet_SDt_win50_btag0_UP", &rljet_SDt_win50_btag0_UP, &b_rljet_SDt_win50_btag0_UP);
            fChain->SetBranchAddress("rljet_SDw_win20_btag0_DOWN", &rljet_SDw_win20_btag0_DOWN, &b_rljet_SDw_win20_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDz_win20_btag0_DOWN", &rljet_SDz_win20_btag0_DOWN, &b_rljet_SDz_win20_btag0_DOWN);
            fChain->SetBranchAddress("rljet_SDt_win50_btag0_DOWN", &rljet_SDt_win50_btag0_DOWN, &b_rljet_SDt_win50_btag0_DOWN);
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

