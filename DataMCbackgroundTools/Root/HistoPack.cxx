#include "DataMCbackgroundTools/HistoPack.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "DataMCbackgroundTools/TH1Tagged.h"

#include <string>
#include <iostream>

HistoPack::HistoPack(unsigned num_fatjets_keep)
{
    UInt_t i;

    /* LARGE-R ANTIKT10 JETS */
    for (i = 0; i < num_fatjets_keep; i++) {

        this->h_rljet_fractional_pt_0.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_0" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_1.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_1" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_2.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_2" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_3.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_3" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_4.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_4" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_5.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_5" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_6.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_6" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_7.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_7" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_8.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_8" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_9.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_9" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_10.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_10" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_11.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_11" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_12.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_12" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_13.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_13" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_14.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_14" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_15.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_15" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_16.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_16" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_17.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_17" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_18.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_18" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_19.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_19" ,0.,  1,  0.01));
        this->h_rljet_fractional_pt_rest.push_back  (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_fractional_pt_rest" ,0.,  1,  0.01));

        this->h_rljet_pt_comb.push_back  ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_pt_comb", 0., 4000., 10.));
        this->h_rljet_m_comb.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m_comb", 0., 1000, 1.0));
        this->h_rljet_pt_calo.push_back  ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_pt_calo", 0., 4000., 10.));
        this->h_rljet_m_calo.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m_calo", 0., 1000, 1.0));
        this->h_rljet_pt_ta.push_back  ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_pt_ta", 0., 4000., 10.));
        this->h_rljet_m_ta.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m_ta", 0., 1000, 1.0));

        this->h_rljet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.05));

        this->h_rljet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.025));


        this->h_rljet_Tau1_wta.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau1_wta", 0., 1., 0.005));

        this->h_rljet_Tau2_wta.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau2_wta", 0., 0.7, 0.0025));

        this->h_rljet_Tau3_wta.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau3_wta" , 0. , 0.4, 0.002));

        this->h_rljet_Tau21_wta.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau21_wta", 0. , 1., 0.005));

        this->h_rljet_Tau32_wta.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau32_wta", 0., 1., 0.005));

        this->h_rljet_ECF1.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF1", 0. , 4.0e6, 3.0e4));

        this->h_rljet_ECF2.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF2", 0. , 1000e9, 5.0e9));

        this->h_rljet_ECF3.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF3", 0. , 150.0e14, 40e12));

        this->h_rljet_C2.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_C2", 0. , 1., 0.005));

        this->h_rljet_D2.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_D2", 0. , 20., 0.025));

        this->h_rljet_FoxWolfram0.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_FoxWolfram0", 0.2 , 0.6, 0.002));

        this->h_rljet_FoxWolfram2.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_FoxWolfram2", -0.2 , 0.6, 0.002));

        this->h_rljet_FoxWolfram20.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_FoxWolfram20", -0.5 , 2., 0.01));

        this->h_rljet_Qw.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Qw", 0. , 1000, 2));

        this->h_rljet_Angularity.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Angularity", 0. , 0.1, 0.0005));

        this->h_rljet_Aplanarity.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Aplanarity", 0. , 0.5, 0.005));

        this->h_rljet_Dip12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Dip12", 0. , 3.0, 0.005));

        this->h_rljet_KtDR.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_KtDR", 0. , 7.0, 0.025));

        this->h_rljet_Mu12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Mu12", 0. , 1.0, 0.01));

        this->h_rljet_PlanarFlow.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_PlanarFlow", 0. , 1., 0.01));

        this->h_rljet_Sphericity.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Sphericity", 0. , 1.0, 0.01));

        this->h_rljet_Split12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split12", 0. , 2500e3, 1e4));

        this->h_rljet_Split23.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split23", 0., 1000, 2));

        this->h_rljet_Split34.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split34", 0., 500e3, 0.5e4));

        this->h_rljet_ThrustMaj.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ThrustMaj", 0., 1., 0.005));

        this->h_rljet_ThrustMin.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ThrustMin", 0., 1., 0.005));

        this->h_rljet_ZCut12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ZCut12", 0., 1., 0.005));

        this->h_rljet_NTrimSubjets.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_NTrimSubjets", 1. , 20., 1.0));

        this->h_rljet_ungroomed_ntrk500.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ungroomed_ntrk500", 0. , 150., 1.0));

        this->h_rljet_RES_m.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_m", 0. , 10., 0.025));

        this->h_rljet_RES_pT.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_pT", 0. , 10., 0.025));

        this->h_rljet_RES_D2.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_D2", 0. , 10., 0.025));

        this->h_rljet_RES_Tau32_wta.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_Tau32_wta", 0. , 10., 0.025));

        this->h_rljet_pdgid.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_pdgid", -1. , 40., 1.0));

        this->h_rljet_SD_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SD_logchi", -25., 25., 0.1));

        this->h_htt_caJet_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_pt", 0., 4000., 10.));

        this->h_htt_caJet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.025));

        this->h_htt_caJet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.025));

        this->h_htt_caJet_m.push_back   ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_m", 0., 1000, 1.0));

        this->h_htt_caGroomJet_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_pt", 0., 4000., 10.));

        this->h_htt_caGroomJet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.025));

        this->h_htt_caGroomJet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.025));

        this->h_htt_caGroomJet_m.push_back   ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_m", 0., 1000, 1.0));

        this->h_htt_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_pt", 0., 4000., 10.));

        this->h_htt_eta.push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.025));

        this->h_htt_phi.push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.025));

        this->h_htt_m.push_back   ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_m", 0., 300, 1.0));

        this->h_htt_m23m123 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_m23m123", 0.35, 0.8, 0.001));

        this->h_htt_atan1312 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_atan1312", 0.2, 1.3, 0.0025));

        this->h_htt_pts1 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_pts1", 0., 2000., 10));

        this->h_htt_pts2 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_pts2", 0., 2000., 10));

        this->h_htt_pts3 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_pts3", 0., 2000., 10));

        // this->h_rljet_BDT_score . push_back( make_unique<TH1Tagged>(
        //             "h_rljet" + std::to_string(i) + "_BDT_score", -1.0, 1.0, 0.01));

        // this->h_rljet_DNN_score . push_back( make_unique<TH1Tagged>(
        //             "h_rljet" + std::to_string(i) + "_DNN_score", 0., 1.0, 0.005));

        // MVA taggers
        this->h_rljet_topTag_BDT_qqb . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_BDT_qqb", 0., 4.0, 1.0));
        
        this->h_rljet_topTag_BDT_qqb_score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_BDT_qqb_score", -1.0, 1.0, 0.01));
        
        this->h_rljet_wTag_BDT_qq . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_wTag_BDT_qq", 0., 4.0, 1.0));
        
        this->h_rljet_wTag_BDT_qq_score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_wTag_BDT_qq_score", -1.0, 1.0, 0.01));
        
        this->h_rljet_topTag_DNN_qqb_score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_DNN_qqb_score", 0., 1.0, 0.005));
        
        this->h_rljet_topTag_DNN_qqb . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_DNN_qqb", 0., 4.0, 1.0));
        
        this->h_rljet_wTag_DNN_qq_score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_wTag_DNN_qq_score", 0., 1.0, 0.005));
        
        this->h_rljet_wTag_DNN_qq . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_wTag_DNN_qq", 0., 4.0, 1.0));
        
        this->h_rljet_topTag_TopoTagger_20wp . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_TopoTagger_20wp", 0., 4.0, 1.0));
        
        this->h_rljet_topTag_TopoTagger_50wp . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_TopoTagger_50wp", 0., 4.0, 1.0));
        
        this->h_rljet_topTag_TopoTagger_80wp . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_TopoTagger_80wp", 0., 4.0, 1.0));
        
        this->h_rljet_topTag_TopoTagger_score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_topTag_TopoTagger_score", 0., 1.0, 0.005));
        


    }

    this->h_topoetcone40_over_pt = make_unique<TH1Tagged>("h_topoetcone40_over_pt", -12, 2, 1e-3);
    this->h_topoetcone20_over_pt = make_unique<TH1Tagged>("h_topoetcone20_over_pt", -0.1, 0.1, 1e-4);
    this->h_ptcone40_over_pt = make_unique<TH1Tagged>("h_ptcone40_over_pt", -0.01, 0.01, 1e-6);
    this->h_ptcone20_over_pt = make_unique<TH1Tagged>("h_ptcone20_over_pt", -0.01, 0.01, 1e-6);

    this->h_rljet_mjj    = make_unique<TH1Tagged>("h_rljet_mjj"    , 0.  , 10000. , 10.);
    this->h_rljet_ptasym = make_unique<TH1Tagged>("h_rljet_ptasym" , 0.  , 1.0    , 0.005);
    this->h_rljet_dy     = make_unique<TH1Tagged>("h_rljet_dy"     , -5. , 5.     , 0.05);
    this->h_rljet_dR     = make_unique<TH1Tagged>("h_rljet_dR"     , 0.  , 6.4    , 0.02);
    this->h_rljet_deta   = make_unique<TH1Tagged>("h_rljet_deta"   , -5. , 5.     , 0.05);
    this->h_rljet_dphi   = make_unique<TH1Tagged>("h_rljet_dphi"   , 0.  , 6.4    , 0.02);

    this->h_mu  = make_unique<TH1Tagged>("h_mu"  , 0.0 , 120.0 , 1.);
    this->h_NPV = make_unique<TH1Tagged>("h_NPV" , -0.5  , 120.5 , 1.);
}

void HistoPack::WriteCommonHistograms() const
{
    for (auto const &h : h_rljet_pt_comb) h->write_all();
    for (auto const &h : h_rljet_m_comb) h->write_all();
    for (auto const &h : h_rljet_eta) h->write_all();
    for (auto const &h : h_rljet_phi) h->write_all();

    for (auto const &h : h_htt_caGroomJet_pt) h->write_all();
    for (auto const &h : h_htt_caGroomJet_m) h->write_all();
    for (auto const &h : h_htt_m) h->write_all();
    for (auto const &h : h_htt_m23m123) h->write_all();
    for (auto const &h : h_htt_atan1312) h->write_all();

    for (auto const &h : h_rljet_D2) h->write_all();
    for (auto const &h : h_rljet_Tau32_wta) h->write_all();
    for (auto const &h : h_rljet_Qw) h->write_all();
    for (auto const &h : h_rljet_Split23) h->write_all();
    for (auto const &h : h_rljet_topTag_BDT_qqb) h->write_all();
    for (auto const &h : h_rljet_topTag_BDT_qqb_score) h->write_all();
    for (auto const &h : h_rljet_wTag_BDT_qq) h->write_all();
    for (auto const &h : h_rljet_wTag_BDT_qq_score) h->write_all();
    for (auto const &h : h_rljet_topTag_DNN_qqb_score) h->write_all();
    for (auto const &h : h_rljet_topTag_DNN_qqb) h->write_all();
    for (auto const &h : h_rljet_wTag_DNN_qq_score) h->write_all();
    for (auto const &h : h_rljet_wTag_DNN_qq) h->write_all();
    for (auto const &h : h_rljet_topTag_TopoTagger_20wp) h->write_all();
    for (auto const &h : h_rljet_topTag_TopoTagger_50wp) h->write_all();
    for (auto const &h : h_rljet_topTag_TopoTagger_80wp) h->write_all();
    for (auto const &h : h_rljet_topTag_TopoTagger_score) h->write_all();
    for (auto const &h : h_rljet_SD_logchi) h->write_all();
    h_mu->write_all();
}

void HistoPack::WriteNominalOnlyHistograms() const
{

    for (auto const &h : h_rljet_fractional_pt_0) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_1) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_2) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_3) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_4) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_5) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_6) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_7) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_8) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_9) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_10) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_11) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_12) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_13) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_14) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_15) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_16) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_17) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_18) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_19) h->write_all();
    for (auto const &h : h_rljet_fractional_pt_rest) h->write_all();

    for (auto const &h : h_rljet_pt_calo) h->write_all();
    for (auto const &h : h_rljet_m_calo) h->write_all();
    for (auto const &h : h_rljet_pt_ta) h->write_all();
    for (auto const &h : h_rljet_m_ta) h->write_all();

    h_rljet_mjj->write_all();
    h_rljet_ptasym->write_all();
    h_rljet_dy->write_all();
    h_rljet_dR->write_all();
    h_rljet_deta->write_all();
    h_rljet_dphi->write_all();

    h_topoetcone40_over_pt->write_all();
    h_topoetcone20_over_pt->write_all();
    h_ptcone40_over_pt->write_all();
    h_ptcone20_over_pt->write_all();

    h_NPV->write_all();

    for (auto const &h : h_rljet_RES_m) h->write_all();
    for (auto const &h : h_rljet_RES_pT) h->write_all();
    for (auto const &h : h_rljet_RES_D2) h->write_all();
    for (auto const &h : h_rljet_RES_Tau32_wta) h->write_all();

    for (auto const &h : h_rljet_pdgid) h->write_all();

    for (auto const &h : h_rljet_Tau1_wta) h->write_all();
    for (auto const &h : h_rljet_Tau2_wta) h->write_all();
    for (auto const &h : h_rljet_Tau3_wta) h->write_all();
    for (auto const &h : h_rljet_Tau21_wta) h->write_all();
    for (auto const &h : h_rljet_ECF1) h->write_all();
    for (auto const &h : h_rljet_ECF2) h->write_all();
    for (auto const &h : h_rljet_ECF3) h->write_all();
    for (auto const &h : h_rljet_C2) h->write_all();
    for (auto const &h : h_rljet_FoxWolfram0) h->write_all();
    for (auto const &h : h_rljet_FoxWolfram2) h->write_all();
    for (auto const &h : h_rljet_FoxWolfram20) h->write_all();
    for (auto const &h : h_rljet_Angularity) h->write_all();
    for (auto const &h : h_rljet_Aplanarity) h->write_all();
    for (auto const &h : h_rljet_Dip12) h->write_all();
    for (auto const &h : h_rljet_KtDR) h->write_all();
    for (auto const &h : h_rljet_Mu12) h->write_all();
    for (auto const &h : h_rljet_PlanarFlow) h->write_all();
    for (auto const &h : h_rljet_Sphericity) h->write_all();
    for (auto const &h : h_rljet_Split12) h->write_all();
    for (auto const &h : h_rljet_Split34) h->write_all();
    for (auto const &h : h_rljet_ThrustMaj) h->write_all();
    for (auto const &h : h_rljet_ThrustMin) h->write_all();
    for (auto const &h : h_rljet_ZCut12) h->write_all();

    for (auto const &h : h_rljet_NTrimSubjets) h->write_all();
    for (auto const &h : h_rljet_ungroomed_ntrk500) h->write_all();

    for (auto const &h : h_htt_caJet_pt) h->write_all();
    for (auto const &h : h_htt_caJet_eta) h->write_all();
    for (auto const &h : h_htt_caJet_phi) h->write_all();
    for (auto const &h : h_htt_caJet_m) h->write_all();

    for (auto const &h : h_htt_caGroomJet_eta) h->write_all();
    for (auto const &h : h_htt_caGroomJet_phi) h->write_all();

    for (auto const &h : h_htt_pt) h->write_all();
    for (auto const &h : h_htt_eta) h->write_all();
    for (auto const &h : h_htt_phi) h->write_all();

    for (auto const &h : h_htt_pts1) h->write_all();
    for (auto const &h : h_htt_pts2) h->write_all();
    for (auto const &h : h_htt_pts3) h->write_all();

}

