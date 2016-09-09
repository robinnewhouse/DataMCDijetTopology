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

        this->h_rljet_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_pt", 0., 4000., 50.));

        this->h_rljet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.1));

        this->h_rljet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.1));

        this->h_rljet_m.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m", 0., 800, 2.5));

        this->h_rljet_Tau1_wta.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau1_wta", 0., 1., 0.02));

        this->h_rljet_Tau2_wta.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau2_wta", 0., 1., 0.02));

        this->h_rljet_Tau3_wta.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau3_wta" , 0. , 1., 0.02));

        this->h_rljet_Tau21_wta.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau21_wta", 0. , 1., 0.02));

        this->h_rljet_Tau32_wta.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau32_wta", 0., 1., 0.02));

        this->h_rljet_ECF1.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF1", 0. , 1.5e6, 3.0e4));

        this->h_rljet_ECF2.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF2", 0. , 20e9, 2.5e8));

        this->h_rljet_ECF3.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF3", 0. , 1.5e12, 1.5e10));

        this->h_rljet_C2.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_C2", 0. , 5., 0.15));

        this->h_rljet_D2.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_D2", 0. , 5., 0.15));

        this->h_rljet_FoxWolfram0.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_FoxWolfram0", 0. , 1., 0.01));

        this->h_rljet_FoxWolfram2.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_FoxWolfram2", -1. , 1., 0.02));

        this->h_rljet_FoxWolfram20.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_FoxWolfram20", -1. , 1., 0.02));

        this->h_rljet_Qw.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Qw", 0. , 200e3, 2e3));

        this->h_rljet_Angularity.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Angularity", 0. , 0.1, 0.001));

        this->h_rljet_Aplanarity.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Aplanarity", 0. , 1.0, 0.01));

        this->h_rljet_Dip12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Dip12", 0. , 2.0, 0.02));

        this->h_rljet_KtDR.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_KtDR", 0. , 10.0, 0.1));

        this->h_rljet_Mu12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Mu12", 0. , 1.0, 0.01));

        this->h_rljet_PlanarFlow.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_PlanarFlow", 0. , 1.0, 0.01));

        this->h_rljet_Sphericity.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Sphericity", 0. , 1.0, 0.01));

        this->h_rljet_Split12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split12", 0. , 180., 5.));

        this->h_rljet_Split23.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split23", 0. , 50., 1.));

        this->h_rljet_Split34.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split34", 0., 30., 0.5));

        this->h_rljet_ThrustMaj.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ThrustMaj", 0., 1., 0.01));

        this->h_rljet_ThrustMin.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ThrustMin", 0., 1., 0.01));

        this->h_rljet_ZCut12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ZCut12", 0., 1., 0.01));

        this->h_rljet_NTrimSubjets.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_NTrimSubjets", 1. , 20., 1.0));

        this->h_rljet_ungroomed_ntrk500.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ungroomed_ntrk500", 0. , 120., 1.0));

        this->h_rljet_RES_m.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_m", -10. , 10., 0.05));

        this->h_rljet_RES_pT.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_pT", -10. , 10., 0.05));

        this->h_rljet_RES_D2.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_D2", -10. , 10., 0.05));

        this->h_rljet_RES_Tau32_wta.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RES_Tau32_wta", -10. , 10., 0.05));

        this->h_rljet_SDw_win20_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDw_win20_btag0_logchi", -6., 10., 0.25));

        this->h_rljet_SDz_win20_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDz_win20_btag0_logchi", -6., 10., 0.25));

        this->h_rljet_SDt_win50_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDt_win50_btag0_logchi", -10., 10., 0.5));

        this->h_htt_caJet_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_pt", 0., 4000., 50.));

        this->h_htt_caJet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.1));

        this->h_htt_caJet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.1));

        this->h_htt_caJet_m.push_back   ( make_unique<TH1Tagged>(
                    "h_htt_caJet" + std::to_string (i) + "_m", 0., 800, 2.5));

        this->h_htt_caGroomJet_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_pt", 0., 4000., 50.));

        this->h_htt_caGroomJet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.1));

        this->h_htt_caGroomJet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.1));

        this->h_htt_caGroomJet_m.push_back   ( make_unique<TH1Tagged>(
                    "h_htt_caGroomJet" + std::to_string (i) + "_m", 0., 800, 2.5));

        this->h_htt_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_pt", 0., 4000., 50.));

        this->h_htt_eta.push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.1));

        this->h_htt_phi.push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.1));

        this->h_htt_m.push_back   ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string (i) + "_m", 0., 800, 2.5));

        this->h_htt_m23m123 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_m23m123", 0, 1.2, 0.02));

        this->h_htt_atan1312 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_atan1312", 0.0, 1.5, 0.02));

        this->h_htt_pts1 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_pts1", 0., 2000., 25));

        this->h_htt_pts2 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_pts2", 0., 2000., 25));

        this->h_htt_pts3 . push_back ( make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_pts3", 0., 2000., 25));

        this->h_rljet_BDT_Top_Score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_BDT_Top_Score", -1.0, 1.0, 0.01));

        this->h_rljet_BDT_W_Score . push_back( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_BDT_W_Score", -1.0, 1.0, 0.01));
    }

    this->h_rljet_mjj = make_unique<TH1Tagged>("h_rljet_mjj", 0., 8000., 25.);
    this->h_rljet_ptasym = make_unique<TH1Tagged>("h_rljet_ptasym", 0., 1.0, 0.01);
    this->h_rljet_dy = make_unique<TH1Tagged>("h_rljet_dy", -5., 5., 0.1);
    this->h_rljet_dR = make_unique<TH1Tagged>("h_rljet_dR", 0., 6.4, 0.05);
}

void HistoPack::WriteCommonHistograms() const
{
    for (auto const &h : h_rljet_pt) h->write_all();
    for (auto const &h : h_rljet_eta) h->write_all();
    for (auto const &h : h_rljet_phi) h->write_all();
    for (auto const &h : h_rljet_m) h->write_all();

    for (auto const &h : h_rljet_D2) h->write_all();
    for (auto const &h : h_rljet_Tau32_wta) h->write_all();
}

void HistoPack::WriteNominalOnlyHistograms() const
{
    h_rljet_mjj->write_all();
    h_rljet_ptasym->write_all();
    h_rljet_dy->write_all();
    h_rljet_dR->write_all();

    for (auto const &h : h_rljet_RES_m) h->write_all();
    for (auto const &h : h_rljet_RES_pT) h->write_all();
    for (auto const &h : h_rljet_RES_D2) h->write_all();
    for (auto const &h : h_rljet_RES_Tau32_wta) h->write_all();

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
    for (auto const &h : h_rljet_Qw) h->write_all();
    for (auto const &h : h_rljet_Angularity) h->write_all();
    for (auto const &h : h_rljet_Aplanarity) h->write_all();
    for (auto const &h : h_rljet_Dip12) h->write_all();
    for (auto const &h : h_rljet_KtDR) h->write_all();
    for (auto const &h : h_rljet_Mu12) h->write_all();
    for (auto const &h : h_rljet_PlanarFlow) h->write_all();
    for (auto const &h : h_rljet_Sphericity) h->write_all();
    for (auto const &h : h_rljet_Split12) h->write_all();
    for (auto const &h : h_rljet_Split23) h->write_all();
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

    for (auto const &h : h_htt_caGroomJet_pt) h->write_all();
    for (auto const &h : h_htt_caGroomJet_eta) h->write_all();
    for (auto const &h : h_htt_caGroomJet_phi) h->write_all();
    for (auto const &h : h_htt_caGroomJet_m) h->write_all();

    for (auto const &h : h_htt_pt) h->write_all();
    for (auto const &h : h_htt_eta) h->write_all();
    for (auto const &h : h_htt_phi) h->write_all();
    for (auto const &h : h_htt_m) h->write_all();
    for (auto const &h : h_htt_m23m123) h->write_all();
    for (auto const &h : h_htt_atan1312) h->write_all();
    for (auto const &h : h_htt_pts1) h->write_all();
    for (auto const &h : h_htt_pts2) h->write_all();
    for (auto const &h : h_htt_pts3) h->write_all();

    for (auto const &h : h_rljet_SDw_win20_btag0_logchi) h->write_all();
    for (auto const &h : h_rljet_SDz_win20_btag0_logchi) h->write_all();
    for (auto const &h : h_rljet_SDt_win50_btag0_logchi) h->write_all();

    for (auto const &h : h_rljet_BDT_Top_Score) h->write_all();
    for (auto const &h : h_rljet_BDT_W_Score) h->write_all();
}

