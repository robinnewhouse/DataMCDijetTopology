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
    HistoPack(void);

    virtual ~HistoPack(void) { };

    vector<unique_ptr<TH1Tagged>> h_rljet_pt;
    vector<unique_ptr<TH1Tagged>> h_rljet_eta;
    vector<unique_ptr<TH1Tagged>> h_rljet_phi;
    vector<unique_ptr<TH1Tagged>> h_rljet_m;

    // delta R matched truth jets
    vector<unique_ptr<TH1Tagged>> h_tljet_pt;
    vector<unique_ptr<TH1Tagged>> h_tljet_eta;
    vector<unique_ptr<TH1Tagged>> h_tljet_phi;
    vector<unique_ptr<TH1Tagged>> h_tljet_m;

    // mass/energy response
    vector<unique_ptr<TH1Tagged>> h_rljet_Rm;
    vector<unique_ptr<TH1Tagged>> h_rljet_RpT;

    // track-assisted mass
    vector<unique_ptr<TH1Tagged>> h_rljet_m_ta;
    vector<unique_ptr<TH1Tagged>> h_rljet_m_ta_nocalib;

    vector<unique_ptr<TH1Tagged>> h_rljet_Split12;
    vector<unique_ptr<TH1Tagged>> h_rljet_Split23;
    vector<unique_ptr<TH1Tagged>> h_rljet_Split34;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau1;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau2;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau3;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau21;
    vector<unique_ptr<TH1Tagged>> h_rljet_Tau32;
    vector<unique_ptr<TH1Tagged>> h_rljet_ECF1;
    vector<unique_ptr<TH1Tagged>> h_rljet_ECF2;
    vector<unique_ptr<TH1Tagged>> h_rljet_ECF3;
    vector<unique_ptr<TH1Tagged>> h_rljet_D2;

    vector<unique_ptr<TH1Tagged>> h_rljet_NTrimSubjets;
    vector<unique_ptr<TH1Tagged>> h_rljet_Width;
    vector<unique_ptr<TH1Tagged>> h_rljet_ungroomed_ntrk500;

    // truth/trigger jet separation
    vector<unique_ptr<TH1Tagged>> h_hltjet_dR;
    vector<unique_ptr<TH1Tagged>> h_tljet_dR;

    vector<unique_ptr<TH1Tagged>> h_rljet_SDw_win20_btag0_logchi;
    vector<unique_ptr<TH1Tagged>> h_rljet_SDw_win25_btag0_logchi;
    vector<unique_ptr<TH1Tagged>> h_rljet_SDt_win50_btag0_logchi;
    vector<unique_ptr<TH1Tagged>> h_rljet_SDt_win55_btag0_logchi;

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

    unique_ptr<TH1Tagged> h_rljet_mjj;
    unique_ptr<TH1Tagged> h_mu;

    unique_ptr<TH1Tagged> h_BDT_TopTag;

    void WriteCommonHistograms() const;
    void WriteNominalOnlyHistograms() const;

    ClassDef(HistoPack, 0);
};

#endif // #ifdef HistoPack_h
