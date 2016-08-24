#include "DataMCbackgroundTools/HistoPack.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "DataMCbackgroundTools/TH1Tagged.h"

#include <string>
#include <iostream>

HistoPack::HistoPack()
{
    UInt_t i;

    /* LARGE-R ANTIKT10 JETS */
    for (i = 0; i < 3; i++) {

        this->h_rljet_pt.push_back  ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_pt", 0., 4000., 50.));

        this->h_rljet_eta.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_eta", -2.5 , 2.5, 0.1));

        this->h_rljet_phi.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_phi", -3.2 , 3.2, 0.1));

        this->h_rljet_m.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m", 0., 800, 2.5));

        this->h_rljet_m_ta.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m_ta", 0., 800, 2.5));

        this->h_rljet_m_ta_nocalib.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_m_ta_nocalib", 0, 800, 2.5));

        this->h_rljet_Split12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split12", 0. , 180., 5.));

        this->h_rljet_Split23.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split23", 0. , 50., 1.));

        this->h_rljet_Split34.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Split34", 0., 30., 0.5));

        this->h_rljet_Tau1.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau1", 0., 1., 0.02));

        this->h_rljet_Tau2.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau2", 0., 1., 0.02));

        this->h_rljet_Tau3.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau3" , 0. , 1., 0.02));

        this->h_rljet_Tau21.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau21", 0. , 1., 0.02));

        this->h_rljet_Tau32.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Tau32", 0., 1., 0.02));

        this->h_rljet_ECF1.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF1", 0. , 1.5e6, 3.0e4));

        this->h_rljet_ECF2.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF2", 0. , 20e9, 2.5e8));

        this->h_rljet_ECF3.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ECF3", 0. , 1.5e12, 1.5e10));

        this->h_rljet_D2.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_D2", 0. , 5., 0.15));

        this->h_rljet_NTrimSubjets.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_NTrimSubjets", 1. , 20., 1.0));

        this->h_rljet_Width.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Width", 0. , 3., 0.02));

        this->h_rljet_ungroomed_ntrk500.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ungroomed_ntrk500", 0. , 120., 1.0));

        this->h_rljet_Rm.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_Rm", -10. , 10., 0.05));

        this->h_rljet_RpT.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_RpT", -10. , 10., 0.05));

        this->h_hltjet_dR.push_back ( make_unique<TH1Tagged>(
                    "h_hltjet" + std::to_string (i) + "_dR", 0. , 7., 0.01));

        this->h_tljet_dR.push_back ( make_unique<TH1Tagged>(
                    "h_tljet" + std::to_string (i) + "_dR", 0. , 7., 0.01));

        this->h_rljet_SDw_win20_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDw_win20_btag0_logchi", -6., 10., 0.25));

        this->h_rljet_SDw_win25_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDw_win25_btag0_logchi", -6., 10., 0.25));

        this->h_rljet_SDt_win50_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDt_win50_btag0_logchi", -10., 10., 0.5));

        this->h_rljet_SDt_win55_btag0_logchi.push_back (make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string(i) + "_SDt_win55_btag0_logchi", -10., 10., 0.5));

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

        this->h_htt_m23m123 . push_back (make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_m23m123", 0, 1.2, 0.02));

        this->h_htt_atan1312 . push_back (make_unique<TH1Tagged>(
                    "h_htt" + std::to_string(i) + "_atan1312", 0.0, 1.5, 0.02));
    }

    this->h_rljet_mjj = make_unique<TH1Tagged>("h_rljet_mjj", 0., 8000., 25.);

    this->h_mu = make_unique<TH1Tagged>("h_mu", 0.5, 80.5, 1.0);

    this->h_BDT_TopTag = make_unique<TH1Tagged>("h_BDT_TopTag", -1.0, 1.0, 0.01);
}

void HistoPack::WriteCommonHistograms() const
{
    for (auto const &h : h_rljet_pt) h->write_all();
    for (auto const &h : h_rljet_eta) h->write_all();
    for (auto const &h : h_rljet_phi) h->write_all();
    for (auto const &h : h_rljet_m) h->write_all();

    for (auto const &h : h_rljet_D2) h->write_all();
    for (auto const &h : h_rljet_Tau32) h->write_all();
}

void HistoPack::WriteNominalOnlyHistograms() const
{
    for (auto const &h : h_rljet_Split12) h->write_all();
    for (auto const &h : h_rljet_Split23) h->write_all();
    for (auto const &h : h_rljet_Split34) h->write_all();
    for (auto const &h : h_rljet_Tau1) h->write_all();
    for (auto const &h : h_rljet_Tau2) h->write_all();
    for (auto const &h : h_rljet_Tau3) h->write_all();
    for (auto const &h : h_rljet_Tau21) h->write_all();
    for (auto const &h : h_rljet_ECF1) h->write_all();
    for (auto const &h : h_rljet_ECF2) h->write_all();
    for (auto const &h : h_rljet_ECF3) h->write_all();

    for (auto const &h : h_rljet_SDw_win20_btag0_logchi) h->write_all();
    for (auto const &h : h_rljet_SDw_win25_btag0_logchi) h->write_all();
    for (auto const &h : h_rljet_SDt_win50_btag0_logchi) h->write_all();
    for (auto const &h : h_rljet_SDt_win55_btag0_logchi) h->write_all();

    for (auto const &h : h_tljet_pt) h->write_all();
    for (auto const &h : h_tljet_eta) h->write_all();
    for (auto const &h : h_tljet_phi) h->write_all();
    for (auto const &h : h_tljet_m) h->write_all();

    for (auto const &h : h_hltjet_dR) h->write_all();
    for (auto const &h : h_tljet_dR) h->write_all();

    for (auto const &h : h_rljet_NTrimSubjets) h->write_all();
    for (auto const &h : h_rljet_Width) h->write_all();
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

    h_rljet_mjj->write_all();
    h_mu->write_all();
    h_BDT_TopTag->write_all();
}

