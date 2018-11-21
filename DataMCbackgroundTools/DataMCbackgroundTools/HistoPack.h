#ifndef HistoPack_h
#define HistoPack_h

#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>

#include "DataMCbackgroundTools/TH1Tagged.h"
#include "DataMCbackgroundTools/LocalTools.h"

#include <vector>
using std::vector;
#include <memory>
using std::unique_ptr;

class HistoPack {
public:
    HistoPack(unsigned num_fatjets_keep);

    virtual ~HistoPack(void) { };
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_0;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_1;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_2;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_3;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_4;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_5;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_6;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_7;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_8;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_9;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_10;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_11;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_12;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_13;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_14;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_15;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_16;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_17;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_18;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_19;
    vector<unique_ptr<TH1Tagged>> h_rljet_fractional_pt_rest;

    vector<unique_ptr<TH1Tagged>> h_rljet_pt_comb;
    vector<unique_ptr<TH1Tagged>> h_rljet_m_comb;
    vector<unique_ptr<TH1Tagged>> h_rljet_pt_calo;
    vector<unique_ptr<TH1Tagged>> h_rljet_m_calo;
    vector<unique_ptr<TH1Tagged>> h_rljet_pt_ta;
    vector<unique_ptr<TH1Tagged>> h_rljet_m_ta;

    vector<unique_ptr<TH1Tagged>> h_rljet_eta;
    vector<unique_ptr<TH1Tagged>> h_rljet_phi;

    // lead/sublead jet variables
    unique_ptr<TH1Tagged> h_rljet_mjj;
    unique_ptr<TH1Tagged> h_rljet_ptasym;
    unique_ptr<TH1Tagged> h_rljet_dy;
    unique_ptr<TH1Tagged> h_rljet_dR;
    unique_ptr<TH1Tagged> h_rljet_deta;
    unique_ptr<TH1Tagged> h_rljet_dphi;

    unique_ptr<TH1Tagged> h_mu;
    unique_ptr<TH1Tagged> h_NPV;

    unique_ptr<TH1Tagged> h_topoetcone40_over_pt;
    unique_ptr<TH1Tagged> h_topoetcone20_over_pt;
    unique_ptr<TH1Tagged> h_ptcone40_over_pt;
    unique_ptr<TH1Tagged> h_ptcone20_over_pt;

    // mass/energy response
    vector<unique_ptr<TH1Tagged>> h_rljet_RES_m;
    vector<unique_ptr<TH1Tagged>> h_rljet_RES_pT;
    vector<unique_ptr<TH1Tagged>> h_rljet_RES_D2;
    vector<unique_ptr<TH1Tagged>> h_rljet_RES_Tau32_wta;

    vector<unique_ptr<TH1Tagged>> h_rljet_pdgid;

    vector<unique_ptr<TH1Tagged>> h_rljet_Tau1_wta;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau2_wta;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau3_wta;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau21_wta;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau32_wta;
    vector<unique_ptr<TH1Tagged>> h_rljet_ECF1;
    vector<unique_ptr<TH1Tagged>> h_rljet_ECF2;
    vector<unique_ptr<TH1Tagged>> h_rljet_ECF3;
    vector<unique_ptr<TH1Tagged>> h_rljet_C2;
    vector<unique_ptr<TH1Tagged>> h_rljet_D2;
    vector<unique_ptr<TH1Tagged>> h_rljet_FoxWolfram0;
    vector<unique_ptr<TH1Tagged>> h_rljet_FoxWolfram2;
    vector<unique_ptr<TH1Tagged>> h_rljet_FoxWolfram20;
    vector<unique_ptr<TH1Tagged>> h_rljet_Qw;
    vector<unique_ptr<TH1Tagged>> h_rljet_Angularity;
    vector<unique_ptr<TH1Tagged>> h_rljet_Aplanarity;
    vector<unique_ptr<TH1Tagged>> h_rljet_Dip12;
    vector<unique_ptr<TH1Tagged>> h_rljet_KtDR;
    vector<unique_ptr<TH1Tagged>> h_rljet_Mu12;
    vector<unique_ptr<TH1Tagged>> h_rljet_PlanarFlow;
    vector<unique_ptr<TH1Tagged>> h_rljet_Sphericity;
    vector<unique_ptr<TH1Tagged>> h_rljet_Split12;
    vector<unique_ptr<TH1Tagged>> h_rljet_Split23;
    vector<unique_ptr<TH1Tagged>> h_rljet_Split34;
    vector<unique_ptr<TH1Tagged>> h_rljet_ThrustMaj;
    vector<unique_ptr<TH1Tagged>> h_rljet_ThrustMin;
    vector<unique_ptr<TH1Tagged>> h_rljet_ZCut12;

    vector<unique_ptr<TH1Tagged>> h_rljet_NTrimSubjets;
    vector<unique_ptr<TH1Tagged>> h_rljet_ungroomed_ntrk500;

    vector<unique_ptr<TH1Tagged>> h_htt_caJet_pt;
    vector<unique_ptr<TH1Tagged>> h_htt_caJet_eta;
    vector<unique_ptr<TH1Tagged>> h_htt_caJet_phi;
    vector<unique_ptr<TH1Tagged>> h_htt_caJet_m;

    vector<unique_ptr<TH1Tagged>> h_htt_caGroomJet_pt;
    vector<unique_ptr<TH1Tagged>> h_htt_caGroomJet_eta;
    vector<unique_ptr<TH1Tagged>> h_htt_caGroomJet_phi;
    vector<unique_ptr<TH1Tagged>> h_htt_caGroomJet_m;

    vector<unique_ptr<TH1Tagged>> h_htt_pt;
    vector<unique_ptr<TH1Tagged>> h_htt_eta;
    vector<unique_ptr<TH1Tagged>> h_htt_phi;
    vector<unique_ptr<TH1Tagged>> h_htt_m;
    vector<unique_ptr<TH1Tagged>> h_htt_m23m123;
    vector<unique_ptr<TH1Tagged>> h_htt_atan1312;
    vector<unique_ptr<TH1Tagged>> h_htt_pts1;
    vector<unique_ptr<TH1Tagged>> h_htt_pts2;
    vector<unique_ptr<TH1Tagged>> h_htt_pts3;

    vector<unique_ptr<TH1Tagged>> h_rljet_SD_logchi;

    // vector<unique_ptr<TH1Tagged>> h_rljet_BDT_score;
    // vector<unique_ptr<TH1Tagged>> h_rljet_DNN_score;

    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_BDT_qqb;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_BDT_qqb_score;
    vector<unique_ptr<TH1Tagged>> h_rljet_wTag_BDT_qq;
    vector<unique_ptr<TH1Tagged>> h_rljet_wTag_BDT_qq_score;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_DNN_qqb_score;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_DNN_qqb;
    vector<unique_ptr<TH1Tagged>> h_rljet_wTag_DNN_qq_score;
    vector<unique_ptr<TH1Tagged>> h_rljet_wTag_DNN_qq;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_TopoTagger_20wp;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_TopoTagger_50wp;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_TopoTagger_80wp;
    vector<unique_ptr<TH1Tagged>> h_rljet_topTag_TopoTagger_score;

    void WriteCommonHistograms() const;
    void WriteNominalOnlyHistograms() const;

    ClassDef(HistoPack, 0);
};

#endif // #ifdef HistoPack_h
