#define DataMCbackgroundSelector_cxx
// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.

#include "DataMCbackgroundTools/DataMCbackgroundSelector.h"
#include "DataMCbackgroundTools/HistoPack.h"

#include <TStyle.h>
#include <TMath.h>
#include <TLorentzVector.h>

#include <iostream>
#include <vector>
#include <unordered_map>

#include "TopExamples/AnalysisTools.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

DataMCbackgroundSelector::DataMCbackgroundSelector(
        std::string output_filepath_,
        std::string root_dir_str_,
        std::string sub_dir_str_,
        std::string data_trigger_str_,
        float luminosity_
        )
    : fChain(nullptr),
    hp(nullptr),
    output_filepath(output_filepath_),
    root_dir_str(root_dir_str_),
    sub_dir_str(sub_dir_str_),
    data_trigger_str(data_trigger_str_),
    operating_on_mc(root_dir_str_ != "data"),
    on_nominal_branch(sub_dir_str_ == "nominal"),
    compute_tau32(false),
    compute_d2(false),
    luminosity(luminosity_)
{
    const char* const rc = getenv("ROOTCOREBIN");
    std::string xsection_file = std::string(rc) + "/data/DataMCbackgroundTools/sample_weights.txt";
    if (!weight_tool.read_weights_file(xsection_file))
        exit(1);

    std::string bugged_events_file = std::string(rc) + "/data/DataMCbackgroundTools/bugged_events.txt";
    if (!weight_tool.read_bugged_events_file(bugged_events_file))
        exit(1);

    TMVA::Tools::Instance();
    readerTOP = new TMVA::Reader( "!Color:!Silent" );
    readerW = nullptr;
}

void DataMCbackgroundSelector::log(const std::string& line)
{
    std::cout << std::scientific << line << std::endl;
    this->output_log << std::scientific << line << std::endl;
}

void DataMCbackgroundSelector::Begin(TTree * /*tree*/)
{
    // The Begin() function is called at the start of the query.
    // When running with PROOF Begin() is only called on the client.
    // The tree argument is deprecated (on PROOF 0 is passed).
    TString option = GetOption();

    this->output_log.open(output_filepath + ".log", std::ios::app);
}

void DataMCbackgroundSelector::SlaveBegin(TTree * /*tree*/)
{
    // The SlaveBegin() function is called after the Begin() function.
    // When running with PROOF SlaveBegin() is called on each slave server.
    // The tree argument is deprecated (on PROOF 0 is passed).
    TString option = GetOption();

    this->hp = new HistoPack();
}

Bool_t DataMCbackgroundSelector::Process(Long64_t entry)
{
    // The Process() function is called for each entry in the tree (or possibly
    // keyed object in the case of PROOF) to be processed. The entry argument
    // specifies which entry in the currently loaded tree is to be processed.
    // It can be passed to either DataMCbackgroundSelector::GetEntry() or TBranch::GetEntry()
    // to read either all or the required parts of the data. When processing
    // keyed objects with PROOF, the object is already loaded and is available
    // via the fObject pointer.
    //
    // This function should contain the "body" of the analysis. It can contain
    // simple or elaborate selection criteria, run algorithms on the data
    // of the event and typically fill histograms.
    //
    // The processing can be stopped by calling Abort().
    //
    // Use fStatus to set the return value of TTree::Process().
    //
    // The return value is currently not used.

    b_rljet_pt->GetEntry(entry);
    b_rljet_eta->GetEntry(entry);
    b_rljet_phi->GetEntry(entry);
    b_rljet_m->GetEntry(entry);

    b_rljet_smoothMassTag50eff->GetEntry(entry);
    b_rljet_smoothMassTag80eff->GetEntry(entry);
    b_rljet_smoothTau32Tag50eff->GetEntry(entry);
    b_rljet_smoothTau32Tag80eff->GetEntry(entry);
    b_rljet_smoothMassTau32Tag50eff->GetEntry(entry);
    b_rljet_smoothMassTau32Tag80eff->GetEntry(entry);

    b_rljet_smoothWTag50eff->GetEntry(entry);
    b_rljet_smoothWTag25eff->GetEntry(entry);
    b_rljet_smoothZTag50eff->GetEntry(entry);
    b_rljet_smoothZTag25eff->GetEntry(entry);

    if (!compute_tau32)
        b_rljet_tau32_wta->GetEntry(entry);

    if (!compute_d2)
        b_rljet_d2->GetEntry(entry);

    if (this->operating_on_mc) {
        b_weight_mc->GetEntry(entry);
        b_weight_pileup->GetEntry(entry);
        b_weight_jvt->GetEntry(entry);
        b_weight_bTagSF_70->GetEntry(entry);
    }

    b_mcChannelNumber->GetEntry(entry);
    b_eventNumber->GetEntry(entry);

    if (this->on_nominal_branch) {
        if (this->operating_on_mc) {
            b_mu->GetEntry(entry);
        } else {
            b_mu_original_xAOD->GetEntry(entry);
        }

        b_rl_dijet_m->GetEntry(entry);
        b_rljet_N->GetEntry(entry);
        b_rljet_m_ta->GetEntry(entry);
        b_rljet_m_ta_nocalib->GetEntry(entry);
        b_met_met->GetEntry(entry);

        b_rljet_split12->GetEntry(entry);
        b_rljet_split23->GetEntry(entry);
        b_rljet_split34->GetEntry(entry);
        b_rljet_tau1_wta->GetEntry(entry);
        b_rljet_tau2_wta->GetEntry(entry);
        b_rljet_tau3_wta->GetEntry(entry);
        b_rljet_ecf1->GetEntry(entry);
        b_rljet_ecf2->GetEntry(entry);
        b_rljet_ecf3->GetEntry(entry);
        b_rljet_qw->GetEntry(entry);

		b_rljet_ntrimsubjets->GetEntry(entry);
		b_rljet_width->GetEntry(entry);
		b_rljet_area->GetEntry(entry);
		b_rljet_ungroomed_ntrk500->GetEntry(entry);

        if (this->operating_on_mc) {
            b_tljet_pt->GetEntry(entry);
            b_tljet_eta->GetEntry(entry);
            b_tljet_phi->GetEntry(entry);
            b_tljet_m->GetEntry(entry);
            b_tljet_dR->GetEntry(entry);
        }

        b_HLT_jet_trigger->GetEntry(entry);

        b_hltjet_N->GetEntry(entry);
        b_hltjet_m->GetEntry(entry);
        b_hltjet_pt->GetEntry(entry);
        b_hltjet_eta->GetEntry(entry);
        b_hltjet_phi->GetEntry(entry);
        b_hltjet_dR->GetEntry(entry);

        b_caJet_N->GetEntry(entry);
        b_htt_caJet_pt->GetEntry(entry);
        b_htt_caJet_eta->GetEntry(entry);
        b_htt_caJet_phi->GetEntry(entry);
        b_htt_caJet_m->GetEntry(entry);

        for (UInt_t i = 0; i < this->htt_config_strings.size(); i++)
        {
            b_htt_pt[i]->GetEntry(entry);
            b_htt_eta[i]->GetEntry(entry);
            b_htt_phi[i]->GetEntry(entry);
            b_htt_m[i]->GetEntry(entry);
            b_htt_m23m123[i]->GetEntry(entry);
            b_htt_atan1312[i]->GetEntry(entry);
            b_htt_nTagCands[i]->GetEntry(entry);
            b_htt_tag[i]->GetEntry(entry);

            b_htt_caGroomJet_pt[i]->GetEntry(entry);
            b_htt_caGroomJet_eta[i]->GetEntry(entry);
            b_htt_caGroomJet_phi[i]->GetEntry(entry);
            b_htt_caGroomJet_m[i]->GetEntry(entry);
        }

        b_rljet_btag_double->GetEntry(entry);
        b_rljet_btag_single->GetEntry(entry);
        b_rljet_btag_leading_pt->GetEntry(entry);

        b_rljet_SDw_win20_btag0->GetEntry(entry);
        b_rljet_SDw_win25_btag0->GetEntry(entry);
        b_rljet_SDz_win20_btag0->GetEntry(entry);
        b_rljet_SDz_win25_btag0->GetEntry(entry);
        b_rljet_SDt_win50_btag0->GetEntry(entry);
        b_rljet_SDt_win55_btag0->GetEntry(entry);

        if (this->operating_on_mc) {
            b_rljet_SDw_win20_btag0_UP->GetEntry(entry);
            b_rljet_SDw_win25_btag0_UP->GetEntry(entry);
            b_rljet_SDz_win20_btag0_UP->GetEntry(entry);
            b_rljet_SDz_win25_btag0_UP->GetEntry(entry);
            b_rljet_SDt_win50_btag0_UP->GetEntry(entry);
            b_rljet_SDt_win55_btag0_UP->GetEntry(entry);

            b_rljet_SDw_win20_btag0_DOWN->GetEntry(entry);
            b_rljet_SDw_win25_btag0_DOWN->GetEntry(entry);
            b_rljet_SDz_win20_btag0_DOWN->GetEntry(entry);
            b_rljet_SDz_win25_btag0_DOWN->GetEntry(entry);
            b_rljet_SDt_win50_btag0_DOWN->GetEntry(entry);
            b_rljet_SDt_win55_btag0_DOWN->GetEntry(entry);
        }
    }

    const bool is_ttbar_slice = mcChannelNumber >= 303722 && mcChannelNumber <= 303726;

    // calculate luminosity SF only once per file
    if (entry == 0) {
        TFile* current_file = this->fChain->GetCurrentFile();

        this->log("PROCESSING FILE: " + std::string(current_file->GetName()));
        this->log("BRANCH: " + std::string(fChain->GetName()));

        if (is_ttbar_slice) {
            std::cout << "working on ttbar slice..." << std::endl;
        }

        this->SF_lumi_Fb = 1.0;
        this->max_weight = 0.;

        num_bdt_rejects = 0;

        if (this->operating_on_mc) {
            const ULong64_t nevents_skimmed = getTotalEventsSample(current_file);
            const float sum_weights = get_sum_weights_sample(current_file);
            float xsection  = weight_tool.get_xsection(mcChannelNumber);
            float nevents   = weight_tool.get_nevents(mcChannelNumber);
            float filtereff = weight_tool.get_filtereff(mcChannelNumber);

            this->log("TOTAL EVENTS (SKIMMED): "   + std::to_string(nevents_skimmed));
            this->log("TOTAL EVENTS (UNSKIMMED): " + std::to_string(nevents));
            this->log("SUM WEIGHTS (UNSKIMMED): "  + std::to_string(sum_weights));

            const bool is_pythia_JZXW_slice = mcChannelNumber >= 361020 && mcChannelNumber <= 361032;
            const bool is_herwig_JZXW_slice = mcChannelNumber >= 426040 && mcChannelNumber <= 426052;

            if (is_pythia_JZXW_slice || is_herwig_JZXW_slice || is_ttbar_slice) {
                this->SF_lumi_Fb = xsection * filtereff / nevents;
            } else {
                this->SF_lumi_Fb = xsection * filtereff / sum_weights;
            }
        }

        current_file = nullptr; // because ROOT ownership semantics are weird
    }

    // events in data must pass the specified trigger
    if (!this->operating_on_mc && HLT_jet_trigger == 0)
        return kFALSE;

    float weight;
    if (this->operating_on_mc) {

        if (!is_ttbar_slice) {
            weight = weight_mc * weight_pileup * weight_jvt * weight_bTagSF_70 * this->SF_lumi_Fb * this->luminosity;
        } else { // temporary sloppy workaround. TODO: fixme
            weight = 1.0 * weight_pileup * weight_jvt * weight_bTagSF_70 * this->SF_lumi_Fb * this->luminosity;
            if (weight_mc < 0) weight *= -1.0;
        }

        if (weight > max_weight) {
            max_weight = weight;

            std::stringstream ss;
            ss << std::fixed;
            ss << "FOUND HIGH WEIGHT -- ENTRY NUMBER: " << entry << " ";
            ss << "EVENT NUMBER: " << eventNumber << " ";
            ss << std::scientific;
            ss << "WEIGHT: " << weight << " ";
            ss << "LEAD JET pT: " << rljet_pt->at(0) / 1000. << " GeV";
            this->log(ss.str());
        }

        if (weight_tool.is_bugged_event(mcChannelNumber, eventNumber)) {
            std::stringstream ss;
            ss << std::fixed;
            ss << "REJECTING EVENT FROM BUGGED EVENT LIST: ";
            ss << "DSID: " << mcChannelNumber << " ";
            ss << "EVENT NUMBER: " << eventNumber;
            this->log(ss.str());
            return kFALSE;
        }

        //if (rljet_pt->at(0) / 1000. > 2400 && weight > 20) {
        //    std::stringstream ss;
        //    ss << "SKIPPING EVENT WITH WEIGHT: " << weight << " ";
        //    ss << std::fixed;
        //    ss << "EVENT NUMBER: " << eventNumber << " ";
        //    ss << std::scientific;
        //    ss << "AND LEAD JET pT: " << rljet_pt->at(0);
        //    this->log(ss.str());
        //    return kFALSE;
        //}

    } else { // if event is data
        weight = 1.;
    }

    /***************************************************************/
    /* VARIABLES TO RECORD FOR ALL BRANCHES (NOMINAL + SYSTEMATIC) */
    /***************************************************************/

    const UInt_t n_rljets_recorded = rljet_pt->size();
    for (UInt_t i = 0; i < 3 && i < n_rljets_recorded; i++) {
        // kinematics
        hp->h_rljet_pt.at(i)->fill(rljet_pt->at(i)/1000., weight);
        hp->h_rljet_eta.at(i)->fill(rljet_eta->at(i), weight);
        hp->h_rljet_phi.at(i)->fill(rljet_phi->at(i), weight);
        hp->h_rljet_m.at(i)->fill(rljet_m->at(i)/1000., weight);

        // we only want to look at distributions of these substructure tagging variables
        // for more 'signal-like' jets, so impose a mass cut on the d2/tau32 plots.
        if (rljet_m->at(i) / 1000. > 50.) {
            if (compute_d2 && on_nominal_branch) {
                float ecf1 = rljet_ecf1->at(i);
                float ecf2 = rljet_ecf2->at(i);
                float ecf3 = rljet_ecf3->at(i);
                float d2 = ecf3 * TMath::Power(ecf1, 3) / TMath::Power(ecf2, 3);
                hp->h_rljet_d2.at(i)->fill(d2, weight);
            } else {
                hp->h_rljet_d2.at(i)->fill(rljet_d2->at(i), weight);
            }
        }

        if (rljet_m->at(i) / 1000. > 100.) {
            if (compute_tau32 && on_nominal_branch) {
                float tau32_wta =  rljet_tau3_wta->at(i) / rljet_tau2_wta->at(i);
                hp->h_rljet_tau32.at(i)->fill(tau32_wta, weight);
            } else {
                hp->h_rljet_tau32.at(i)->fill(rljet_tau32_wta->at(i), weight);
            }
        }

        // SMOOTHED SUBSTRUCTURE TAGGERS
        prerec_tag_map["w_prerec_25eff_d2cut"        ] = rljet_smoothWTag25eff->at(i) == 1 || rljet_smoothWTag25eff->at(i) == 3;
        prerec_tag_map["w_prerec_25eff_masscut"      ] = rljet_smoothWTag25eff->at(i) >= 2;
        prerec_tag_map["w_prerec_25eff_d2masscut"    ] = rljet_smoothWTag25eff->at(i) == 3;
        prerec_tag_map["w_prerec_50eff_d2cut"        ] = rljet_smoothWTag50eff->at(i) == 1 || rljet_smoothWTag50eff->at(i) == 3;
        prerec_tag_map["w_prerec_50eff_masscut"      ] = rljet_smoothWTag50eff->at(i) >= 2;
        prerec_tag_map["w_prerec_50eff_d2masscut"    ] = rljet_smoothWTag50eff->at(i) == 3;
        prerec_tag_map["t_prerec_50eff_tau32cut"     ] = rljet_smoothTau32Tag50eff->at(i);
        prerec_tag_map["t_prerec_50eff_masscut"      ] = rljet_smoothMassTag50eff->at(i);
        prerec_tag_map["t_prerec_50eff_tau32masscut" ] = rljet_smoothMassTau32Tag50eff->at(i);
        prerec_tag_map["t_prerec_80eff_tau32cut"     ] = rljet_smoothTau32Tag80eff->at(i);
        prerec_tag_map["t_prerec_80eff_masscut"      ] = rljet_smoothMassTag80eff->at(i);
        prerec_tag_map["t_prerec_80eff_tau32masscut" ] = rljet_smoothMassTau32Tag80eff->at(i);

        for (const auto& itag : prerec_tag_map) {
            hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
            hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
        }
    }

    /******************************************************************/
    /* BELOW HERE, ONLY SAVING VARIABLES RESTRICTED TO NOMINAL BRANCH */
    /******************************************************************/

    if (!on_nominal_branch)
        return kTRUE;

    /***************************/
    /* GENERAL EVENT VARIABLES */
    /***************************/

    if (this->operating_on_mc) {
        hp->h_mu->fill(mu, weight);
    } else {
        hp->h_mu->fill(mu_original_xAOD, weight);
    }

    const bool ranSD = rljet_SDw_win20_btag0->size() > 0;
    const bool keptTAMass = rljet_m_ta->size() > 0;

    /*******************/
    /* ANTI-KT 10 JETS */
    /*******************/

    for (UInt_t i = 0; i < 3 && i < n_rljets_recorded; i++) {
        // other substructure variables
        hp->h_rljet_split12.at(i)->fill(rljet_split12->at(i)/1000., weight);
        hp->h_rljet_split23.at(i)->fill(rljet_split23->at(i)/1000., weight);
        hp->h_rljet_split34.at(i)->fill(rljet_split34->at(i)/1000., weight);
        hp->h_rljet_tau1.at(i)->fill(rljet_tau1_wta->at(i), weight);
        hp->h_rljet_tau2.at(i)->fill(rljet_tau2_wta->at(i), weight);
        hp->h_rljet_tau3.at(i)->fill(rljet_tau3_wta->at(i), weight);
        hp->h_rljet_tau21.at(i)->fill(rljet_tau2_wta->at(i) / rljet_tau1_wta->at(i), weight);

        float ecf1 = rljet_ecf1->at(i);
        float ecf2 = rljet_ecf2->at(i);
        float ecf3 = rljet_ecf3->at(i);

        hp->h_rljet_ecf1.at(i)->fill(ecf1, weight);
        hp->h_rljet_ecf2.at(i)->fill(ecf2, weight);
        hp->h_rljet_ecf3.at(i)->fill(ecf3, weight);

        if (keptTAMass) {
            hp->h_rljet_m_ta.at(i)->fill(rljet_m_ta->at(i)/1000., weight);
            hp->h_rljet_m_ta_nocalib.at(i)->fill(rljet_m_ta_nocalib->at(i)/1000., weight);
        }

        hp->h_rljet_ntrimsubjets.at(i)->fill(rljet_ntrimsubjets->at(i), weight);
        hp->h_rljet_width.at(i)->fill(rljet_width->at(i), weight);
        hp->h_rljet_area.at(i)->fill(rljet_area->at(i), weight);
        hp->h_rljet_ungroomed_ntrk500.at(i)->fill(rljet_ungroomed_ntrk500->at(i), weight);

        const bool D2W50wp = rljet_smoothWTag50eff->at(i) == 1
            || rljet_smoothWTag50eff->at(i) == 3;

        const bool D2Z50wp = rljet_smoothZTag50eff->at(i) == 1
            || rljet_smoothZTag50eff->at(i) == 3;

        const bool massD2W50wp = rljet_smoothWTag50eff->at(i) == 3;
        const bool massD2Z50wp = rljet_smoothZTag50eff->at(i) == 3;

        const bool massTAU3250wp = rljet_smoothMassTau32Tag50eff->at(i);

        nominal_tag_map["massD2W50wp_ntrk"]           = massD2W50wp && rljet_ungroomed_ntrk500->at(i) < 20;
        nominal_tag_map["massD2W50wp_ntrimsubjets"]   = massD2W50wp && rljet_ntrimsubjets->at(i) < 2;
        nominal_tag_map["massD2W50wp_width"]          = massD2W50wp && rljet_width->at(i) < 0.13;
        nominal_tag_map["massTAU3250wp_ntrk"]         = massTAU3250wp && rljet_ungroomed_ntrk500->at(i) < 20;
        nominal_tag_map["massTAU3250wp_ntrimsubjets"] = massTAU3250wp && rljet_ntrimsubjets->at(i) < 2;
        nominal_tag_map["massTAU3250wp_width"]        = massTAU3250wp && rljet_width->at(i) < 0.2;

        nominal_tag_map["massD2W50wp_ANTI_ntrk"]           = massD2W50wp && !(rljet_ungroomed_ntrk500->at(i) < 20);
        nominal_tag_map["massD2W50wp_ANTI_ntrimsubjets"]   = massD2W50wp && !(rljet_ntrimsubjets->at(i) < 2);
        nominal_tag_map["massD2W50wp_ANTI_width"]          = massD2W50wp && !(rljet_width->at(i) < 0.13);
        nominal_tag_map["massTAU3250wp_ANTI_ntrk"]         = massTAU3250wp && !(rljet_ungroomed_ntrk500->at(i) < 20);
        nominal_tag_map["massTAU3250wp_ANTI_ntrimsubjets"] = massTAU3250wp && !(rljet_ntrimsubjets->at(i) < 2);
        nominal_tag_map["massTAU3250wp_ANTI_width"]        = massTAU3250wp && !(rljet_width->at(i) < 0.2);

        // for in-situ JMR/JMS study with W+jets from dijets
        nominal_tag_map["D2W50wp_btag_double"]      = D2W50wp && rljet_btag_double->at(i);
        nominal_tag_map["D2W50wp_btag_single"]      = D2W50wp && rljet_btag_single->at(i);
        nominal_tag_map["D2W50wp_btag_leading_pt"]  = D2W50wp && rljet_btag_leading_pt->at(i);
        nominal_tag_map["D2Z50wp_btag_double"]      = D2Z50wp && rljet_btag_double->at(i);
        nominal_tag_map["D2Z50wp_btag_single"]      = D2Z50wp && rljet_btag_single->at(i);
        nominal_tag_map["D2Z50wp_btag_leading_pt"]  = D2Z50wp && rljet_btag_leading_pt->at(i);

        // see https://indico.cern.ch/event/368266/contributions/870965/attachments/732236/1004635/JetEtmissPP_toptag.pdf
        bool away_from_trigger = hltjet_dR->at(i) > 0.01;

        nominal_tag_map["massD2W50wp_near_trigger"]   = massD2W50wp && !away_from_trigger;
        nominal_tag_map["massD2W50wp_away_trigger"]   = massD2W50wp && away_from_trigger;
        nominal_tag_map["massTAU3250wp_near_trigger"] = massTAU3250wp && !away_from_trigger;
        nominal_tag_map["massTAU3250wp_away_trigger"] = massTAU3250wp && away_from_trigger;

        if (this->operating_on_mc)
        { // since we haven't yet required the MC event passed the same trigger as the data
            nominal_tag_map["massD2W50wp_near_trigger"]   &= HLT_jet_trigger;
            nominal_tag_map["massD2W50wp_away_trigger"]   &= HLT_jet_trigger;
            nominal_tag_map["massTAU3250wp_near_trigger"] &= HLT_jet_trigger;
            nominal_tag_map["massTAU3250wp_away_trigger"] &= HLT_jet_trigger;
        }

        for (const auto& itag : nominal_tag_map) {
            hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
            hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
        }

        // SD log(chi) variables
        if (ranSD) {
            hp->h_rljet_SDw_win20_btag0_logchi.at(i)->fill(rljet_SDw_win20_btag0->at(i), weight);
            hp->h_rljet_SDw_win25_btag0_logchi.at(i)->fill(rljet_SDw_win25_btag0->at(i), weight);
            hp->h_rljet_SDt_win50_btag0_logchi.at(i)->fill(rljet_SDt_win50_btag0->at(i), weight);
            hp->h_rljet_SDt_win55_btag0_logchi.at(i)->fill(rljet_SDt_win55_btag0->at(i), weight);
        }

        // SD-tagged distributions
        if (ranSD) {
            SD_nominal_tag_map["SDw_win20_btag0"] = rljet_SDw_win20_btag0->at(i) > 2.5 && rljet_m->at(i) / 1000. < 95 && rljet_m->at(i) / 1000. > 65;
            SD_nominal_tag_map["SDw_win25_btag0"] = rljet_SDw_win25_btag0->at(i) > 2.5 && rljet_m->at(i) / 1000. < 95 && rljet_m->at(i) / 1000. > 65;
            SD_nominal_tag_map["SDt_win50_btag0"] = rljet_SDt_win50_btag0->at(i) > 1.0 && rljet_m->at(i) / 1000. < 210 && rljet_m->at(i) / 1000. > 140;
            SD_nominal_tag_map["SDt_win55_btag0"] = rljet_SDt_win55_btag0->at(i) > 1.0 && rljet_m->at(i) / 1000. < 210 && rljet_m->at(i) / 1000. > 140;

            for (const auto& itag : SD_nominal_tag_map) {
                hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
                hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);

                if (keptTAMass) {
                    hp->h_rljet_m_ta.at(i)->fill_tagged(itag.first, rljet_m_ta->at(i)/1000., weight, itag.second);
                    hp->h_rljet_m_ta_nocalib.at(i)->fill_tagged(itag.first, rljet_m_ta_nocalib->at(i)/1000., weight, itag.second);
                }
            }
        }

        if (ranSD && this->operating_on_mc) {
            SD_systematic_tag_map["SDw_win20_btag0_UP"] = rljet_SDw_win20_btag0_UP->at(i) > 2.5 && rljet_m->at(i) / 1000. < 95 && rljet_m->at(i) / 1000. > 65;
            SD_systematic_tag_map["SDw_win25_btag0_UP"] = rljet_SDw_win25_btag0_UP->at(i) > 2.5 && rljet_m->at(i) / 1000. < 95 && rljet_m->at(i) / 1000. > 65;
            SD_systematic_tag_map["SDt_win50_btag0_UP"] = rljet_SDt_win50_btag0_UP->at(i) > 1.0 && rljet_m->at(i) / 1000. < 210 && rljet_m->at(i) / 1000. > 140;
            SD_systematic_tag_map["SDt_win55_btag0_UP"] = rljet_SDt_win55_btag0_UP->at(i) > 1.0 && rljet_m->at(i) / 1000. < 210 && rljet_m->at(i) / 1000. > 140;
            SD_systematic_tag_map["SDw_win20_btag0_DOWN"] = rljet_SDw_win20_btag0_DOWN->at(i) > 2.5 && rljet_m->at(i) / 1000. < 95 && rljet_m->at(i) / 1000. > 65;
            SD_systematic_tag_map["SDw_win25_btag0_DOWN"] = rljet_SDw_win25_btag0_DOWN->at(i) > 2.5 && rljet_m->at(i) / 1000. < 95 && rljet_m->at(i) / 1000. > 65;
            SD_systematic_tag_map["SDt_win50_btag0_DOWN"] = rljet_SDt_win50_btag0_DOWN->at(i) > 1.0 && rljet_m->at(i) / 1000. < 210 && rljet_m->at(i) / 1000. > 140;
            SD_systematic_tag_map["SDt_win55_btag0_DOWN"] = rljet_SDt_win55_btag0_DOWN->at(i) > 1.0 && rljet_m->at(i) / 1000. < 210 && rljet_m->at(i) / 1000. > 140;

            for (const auto& itag : SD_systematic_tag_map) {
                hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
                hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);

                if (keptTAMass) {
                    hp->h_rljet_m_ta.at(i)->fill_tagged(itag.first, rljet_m_ta->at(i)/1000., weight, itag.second);
                    hp->h_rljet_m_ta_nocalib.at(i)->fill_tagged(itag.first, rljet_m_ta_nocalib->at(i)/1000., weight, itag.second);
                }
            }
        } // end of saving systematic branch SD-tagged variables

        /*******/
        /* BDT */
        /*******/

        rljet_tau21_wta = rljet_tau2_wta->at(i) / rljet_tau1_wta->at(i);

        bool is_good_jet_for_BDT =
            rljet_d2->at(i) > 0
            && rljet_tau32_wta->at(i) > 0
            && rljet_qw->at(i) > 0
            && rljet_ecf3->at(i) > 0
            && rljet_tau21_wta > 0
            && rljet_ecf1->at(i) > 0
            && rljet_split12->at(i) > 0;

        if (i == 0 && is_good_jet_for_BDT) {
            Float_t BDT_TopTag = this->readerTOP->EvaluateMVA( "BDTG method" );

            hp->h_BDT_TopTag->fill(BDT_TopTag, weight);

            hp->h_BDT_TopTag->fill_tagged("mass_gt100", BDT_TopTag, weight,
                    (rljet_m->at(i) / 1000.) > 100.);

        } else {
            num_bdt_rejects++;
        }
    } // end of saving all anti-kt R = 1.0 distributions

    if (n_rljets_recorded >= 2)
        hp->h_rl_dijet_m->fill(rl_dijet_m/1000., weight);

    /**************************/
    /* C/A 15 JETS (FROM HTT) */
    /**************************/

    const UInt_t n_cajets_recorded = htt_caJet_pt->size();
    for (UInt_t ijet = 0; ijet < 3 && ijet < n_cajets_recorded; ijet++)
    {
        hp->h_htt_caJet_pt.at(ijet)->fill(htt_caJet_pt->at(ijet), weight);
        hp->h_htt_caJet_eta.at(ijet)->fill(htt_caJet_eta->at(ijet), weight);
        hp->h_htt_caJet_phi.at(ijet)->fill(htt_caJet_phi->at(ijet), weight);
        hp->h_htt_caJet_m.at(ijet)->fill(htt_caJet_m->at(ijet), weight);

        for (UInt_t ihtt = 0; ihtt < htt_config_strings.size(); ihtt++)
        {
            hp->h_htt_pt.at(ijet)->fill_tagged(htt_config_strings[ihtt]       , htt_pt[ihtt]->at(ijet) / 1000. , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_eta.at(ijet)->fill_tagged(htt_config_strings[ihtt]      , htt_eta[ihtt]->at(ijet)        , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_phi.at(ijet)->fill_tagged(htt_config_strings[ihtt]      , htt_phi[ihtt]->at(ijet)        , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_m.at(ijet)->fill_tagged(htt_config_strings[ihtt]        , htt_m[ihtt]->at(ijet) / 1000.  , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_atan1312.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_atan1312[ihtt]->at(ijet)   , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_m23m123.at(ijet)->fill_tagged(htt_config_strings[ihtt]  , htt_m23m123[ihtt]->at(ijet)    , weight , htt_tag[ihtt]->at(ijet));

            hp->h_htt_caGroomJet_pt.at(ijet)->fill_tagged(htt_config_strings[ihtt]  , htt_caGroomJet_pt[ihtt]->at(ijet) / 1000. , weight , htt_caGroomJet_pt[ihtt]->at(ijet) > -1000);
            hp->h_htt_caGroomJet_eta.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_caGroomJet_eta[ihtt]->at(ijet)        , weight , htt_caGroomJet_eta[ihtt]->at(ijet) > -1000);
            hp->h_htt_caGroomJet_phi.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_caGroomJet_phi[ihtt]->at(ijet)        , weight , htt_caGroomJet_phi[ihtt]->at(ijet) > -1000);
            hp->h_htt_caGroomJet_m.at(ijet)->fill_tagged(htt_config_strings[ihtt]   , htt_caGroomJet_m[ihtt]->at(ijet) / 1000.  , weight , htt_caGroomJet_m[ihtt]->at(ijet) > -1000);
        }
    }

    return kTRUE;
}

void DataMCbackgroundSelector::SlaveTerminate()
{
    // The SlaveTerminate() function is called after all entries or objects
    // have been processed. When running with PROOF SlaveTerminate() is called
    // on each slave server.

}

void DataMCbackgroundSelector::Terminate()
{
    // The Terminate() function is the last function to be called during
    // a query. It always runs on the client, it can be used to present
    // the results graphically or save the results to file.

    TFile* output_file = new TFile(this->output_filepath.c_str(), "UPDATE");

    TDirectory* root_tdir = output_file->GetDirectory(this->root_dir_str.c_str());
    if (root_tdir == 0) {
        root_tdir = output_file->mkdir(this->root_dir_str.c_str());
    }
    root_tdir->cd();

    TDirectory* sub_tdir = root_tdir->GetDirectory(this->sub_dir_str.c_str());
    if (sub_tdir == 0) {
        sub_tdir = root_tdir->mkdir(this->sub_dir_str.c_str());
    } else {
        output_file->Close();
        throw std::runtime_error(
            "directory " + root_dir_str + "/" + sub_dir_str + " should not exist yet!");
    }
    sub_tdir->cd();

    hp->WriteCommonHistograms();
    if (sub_dir_str == "nominal")
        hp->WriteNominalOnlyHistograms();

    output_file->Close();

    std::stringstream ss;
    ss << "FINISHED: " << root_dir_str << "/" << sub_dir_str;

    this->log(ss.str());

    delete hp;

    TMVA::Tools::DestroyInstance();

    std::cout << "BDT REJECTS: " << num_bdt_rejects << std::endl;
}

