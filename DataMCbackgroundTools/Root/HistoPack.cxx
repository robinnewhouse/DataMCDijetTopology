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

        this->h_rljet_split12.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_split12", 0. , 180., 5.));

        this->h_rljet_split23.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_split23", 0. , 50., 1.));

        this->h_rljet_split34.push_back ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_split34", 0., 30., 0.5));

        this->h_rljet_tau1.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_tau1", 0., 1., 0.02));

        this->h_rljet_tau2.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_tau2", 0., 1., 0.02));

        this->h_rljet_tau3.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_tau3" , 0. , 1., 0.02));

        this->h_rljet_tau21.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_tau21", 0. , 1., 0.02));

        this->h_rljet_tau32.push_back   ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_tau32", 0., 1., 0.02));

        this->h_rljet_ecf1.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ecf1", 0. , 1.5e6, 3.0e4));

        this->h_rljet_ecf2.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ecf2", 0. , 20e9, 2.5e8));

        this->h_rljet_ecf3.push_back    ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ecf3", 0. , 1.5e12, 1.5e10));

        this->h_rljet_d2.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_d2", 0. , 5., 0.15));

        this->h_rljet_ntrimsubjets.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ntrimsubjets", 1. , 20., 1.0));

        this->h_rljet_width.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_width", 0. , 3., 0.02));

        this->h_rljet_area.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_area", 0. , 3., 0.02));

        this->h_rljet_ungroomed_pdgid.push_back      ( make_unique<TH1Tagged>(
                    "h_rljet" + std::to_string (i) + "_ungroomed_pdgid", 0. , 40., 1.0));

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

    this->h_rl_dijet_m = make_unique<TH1Tagged>("h_rl_dijet_m", 0., 8000., 25.);

    this->h_mu = make_unique<TH1Tagged>("h_mu", 0.5, 80.5, 1.0);

    this->h_BDT_TopTag = make_unique<TH1Tagged>("h_BDT_TopTag", -1.0, 1.0, 0.01);
}

void HistoPack::WriteCommonHistograms() const
{
    for (auto const &h : h_rljet_pt) h->write_all();
    for (auto const &h : h_rljet_eta) h->write_all();
    for (auto const &h : h_rljet_phi) h->write_all();
    for (auto const &h : h_rljet_m) h->write_all();

    for (auto const &h : h_rljet_d2) h->write_all();
    for (auto const &h : h_rljet_tau32) h->write_all();
}

void HistoPack::WriteNominalOnlyHistograms() const
{
    for (auto const &h : h_rljet_split12) h->write_all();
    for (auto const &h : h_rljet_split23) h->write_all();
    for (auto const &h : h_rljet_split34) h->write_all();
    for (auto const &h : h_rljet_tau1) h->write_all();
    for (auto const &h : h_rljet_tau2) h->write_all();
    for (auto const &h : h_rljet_tau3) h->write_all();
    for (auto const &h : h_rljet_tau21) h->write_all();
    for (auto const &h : h_rljet_ecf1) h->write_all();
    for (auto const &h : h_rljet_ecf2) h->write_all();
    for (auto const &h : h_rljet_ecf3) h->write_all();

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

    for (auto const &h : h_rljet_ntrimsubjets) h->write_all();
    for (auto const &h : h_rljet_width) h->write_all();
    for (auto const &h : h_rljet_area) h->write_all();
    for (auto const &h : h_rljet_ungroomed_pdgid) h->write_all();
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

    h_rl_dijet_m->write_all();
    h_mu->write_all();
    h_BDT_TopTag->write_all();
}

