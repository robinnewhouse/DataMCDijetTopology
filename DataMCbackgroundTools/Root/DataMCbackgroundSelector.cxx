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

EventSelector NO_SELECTION = [](const DataMCbackgroundSelector* /* sel */) {
    return true;
};

EventSelector DIJET_LOOSE = [](const DataMCbackgroundSelector* sel) {
    if (sel->rljet_pt->at(0)/1000. < 450.) return false;

    return true;
};

EventSelector DIJET_TIGHT = [](const DataMCbackgroundSelector* sel) {
    if (sel->rljet_pt->at(0)/1000. < 500.)
        return false;

    if (sel->rljet_D2->at(0) <= 0.)
        return false;

    return true;
};

EventSelector VVJJ_SELECTION = [](const DataMCbackgroundSelector* sel ) {
    if ( sel->rljet_pt->at(0) / 1000. < 450
            || std::abs(sel->rljet_eta->at(0)) > 2.0
            || std::abs(sel->rljet_dy) >= 1.2
            || std::abs(sel->rljet_ptasym) >= 0.15
            || sel->rljet_mjj / 1000. <= 1000
            ) return false;

    return true;
};

EventSelector GAMMAJET_LOOSE = [](const DataMCbackgroundSelector* /* sel */) {
    return true;
};

EventSelector GAMMAJET_TIGHT = [](const DataMCbackgroundSelector* /* sel */) {
    return true;
};

const std::unordered_map<std::string, EventSelector>
DataMCbackgroundSelector::available_event_selectors = {
    { "NO_SELECTION"   , NO_SELECTION   },
    { "DIJET_LOOSE"    , DIJET_LOOSE    },
    { "DIJET_TIGHT"    , DIJET_TIGHT    },
    { "VVJJ_SELECTION" , VVJJ_SELECTION },
    { "GAMMAJET_LOOSE" , GAMMAJET_LOOSE },
    { "GAMMAJET_TIGHT" , GAMMAJET_TIGHT }
};

DataMCbackgroundSelector::DataMCbackgroundSelector(
        std::string output_filepath_,
        std::string root_dir_str_,
        std::string sub_dir_str_,
        std::string data_trigger_str_,
        std::string event_selector_str_,
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
    compute_Tau32(false),
    compute_D2(false),
    luminosity(luminosity_),
    num_mva_rejected_jets(0)
{
    const std::string rc = std::string(getenv("ROOTCOREBIN"));

    weight_tool = make_unique<WeightTool>();

    std::string xsection_file = rc + "/data/DataMCbackgroundTools/sample_weights.txt";
    if (!weight_tool->read_weights_file(xsection_file))
        exit(1);

    std::string bugged_events_file = rc + "/data/DataMCbackgroundTools/bugged_events.txt";
    if (!weight_tool->read_bugged_events_file(bugged_events_file))
        exit(1);

    dnn_top_tagger = make_unique<DNNWTopTagger>(
                    rc + "/data/DataMCbackgroundTools/dnn_weights/keras_top_dnn_2.json");

    TMVA::Tools::Instance();

    try {
        std::cout << "OFFLINE EVENT SELECTION SPECIFIED: " << event_selector_str_ << std::endl;
        chosen_event_selector = available_event_selectors.at(event_selector_str_);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
        std::cout << "EventSelector: " << event_selector_str_ << " not available." << std::endl;
        std::cout << "see DataMCbackgroundSelector.cxx for more details." << std::endl;
    }



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

    bdt_input_map.clear();

    bdt_input_map["fjet_Angularity"]   = -1000.;
    bdt_input_map["fjet_Aplanarity"]   = -1000.;
    bdt_input_map["fjet_C2"]           = -1000.;
    bdt_input_map["fjet_D2"]           = -1000.;
    bdt_input_map["fjet_Dip12"]        = -1000.;
    bdt_input_map["fjet_ECF1"]         = -1000.;
    bdt_input_map["fjet_ECF2"]         = -1000.;
    bdt_input_map["fjet_ECF3"]         = -1000.;
    bdt_input_map["fjet_FoxWolfram20"] = -1000.;
    bdt_input_map["fjet_KtDR"]         = -1000.;
    bdt_input_map["fjet_PlanarFlow"]   = -1000.;
    bdt_input_map["fjet_Qw"]           = -1000.;
    bdt_input_map["fjet_Sphericity"]   = -1000.;
    bdt_input_map["fjet_Split12"]      = -1000.;
    bdt_input_map["fjet_Tau1_wta"]     = -1000.;
    bdt_input_map["fjet_Tau2_wta"]     = -1000.;
    bdt_input_map["fjet_Tau3_wta"]     = -1000.;
    bdt_input_map["fjet_Tau21_wta"]    = -1000.;
    bdt_input_map["fjet_Tau32_wta"]    = -1000.;
    bdt_input_map["fjet_ThrustMaj"]    = -1000.;
    bdt_input_map["fjet_ThrustMin"]    = -1000.;
    bdt_input_map["fjet_ZCut12"]       = -1000.;

    auto add_bdt_vars = [this](TMVA::Reader* reader,
                    const std::vector<std::string>& bdt_vars) {
            for (auto const& v : bdt_vars) reader->AddVariable(v, &this->bdt_input_map[v]);
    };

    // bdt_readers["W_NvarAll"] = make_unique<TMVA::Reader>("!Color:!Silent");
    bdt_readers["W_NvarL"]   = make_unique<TMVA::Reader>("!Color:!Silent");
    bdt_readers["W_NvarM"]   = make_unique<TMVA::Reader>("!Color:!Silent");
    bdt_readers["W_NvarS"]   = make_unique<TMVA::Reader>("!Color:!Silent");

    //bdt_readers["TOP3q_NvarAll"] = make_unique<TMVA::Reader>("!Color:!Silent");
    bdt_readers["TOP3q_NvarL"]   = make_unique<TMVA::Reader>("!Color:!Silent");
    bdt_readers["TOP3q_NvarM"]   = make_unique<TMVA::Reader>("!Color:!Silent");
    bdt_readers["TOP3q_NvarS"]   = make_unique<TMVA::Reader>("!Color:!Silent");

    bdt_readers["TOP_NvarM"] = make_unique<TMVA::Reader>("!Color:!Silent");

    // add_bdt_vars(bdt_readers["W_NvarAll"].get(), {
    //                 "fjet_D2",
    //                 "fjet_C2",
    //                 "fjet_ECF3",
    //                 "fjet_KtDR",
    //                 "fjet_ThrustMin",
    //                 "fjet_Angularity",
    //                 "fjet_PlanarFlow",
    //                 "fjet_Tau21_wta",
    //                 "fjet_Aplanarity",
    //                 "fjet_FoxWolfram20",
    //                 "fjet_Split12",
    //                 "fjet_ZCut12",
    //                 "fjet_ECF2",
    //                 "fjet_Tau1_wta",
    //                 "fjet_Sphericity",
    //                 "fjet_Dip12",
    //                 "fjet_ThrustMaj",
    //                 "fjet_Tau2_wta",
    //                 "fjet_ECF1"
    //                 });

    add_bdt_vars(bdt_readers["W_NvarL"].get(), {
                    "fjet_D2",
                    "fjet_ECF3",
                    "fjet_C2",
                    "fjet_ThrustMin",
                    "fjet_Angularity",
                    "fjet_PlanarFlow",
                    "fjet_Tau21_wta",
                    "fjet_Aplanarity",
                    "fjet_FoxWolfram20",
                    "fjet_Split12",
                    "fjet_ZCut12",
                    "fjet_KtDR",
                    "fjet_ECF2",
                    "fjet_Tau1_wta",
                    "fjet_Sphericity",
                    "fjet_Dip12"
                    });

    add_bdt_vars(bdt_readers["W_NvarM"].get(), {
                    "fjet_D2",
                    "fjet_ECF3",
                    "fjet_C2",
                    "fjet_ThrustMin",
                    "fjet_Angularity",
                    "fjet_PlanarFlow",
                    "fjet_Tau21_wta",
                    "fjet_Aplanarity",
                    "fjet_FoxWolfram20",
                    "fjet_Split12",
                    "fjet_ZCut12",
                    "fjet_KtDR",
                    "fjet_ECF2"
                    });

    add_bdt_vars(bdt_readers["W_NvarS"].get(), {
                    "fjet_D2",
                    "fjet_ECF3",
                    "fjet_C2",
                    "fjet_ThrustMin",
                    "fjet_Angularity",
                    "fjet_PlanarFlow",
                    "fjet_Tau21_wta"
                    });

    // add_bdt_vars(bdt_readers["TOP3q_NvarAll"].get(), {
    //                 "fjet_Tau32_wta",
    //                 "fjet_ECF3",
    //                 "fjet_Qw",
    //                 "fjet_D2",
    //                 "fjet_ECF2",
    //                 "fjet_Split23",
    //                 "fjet_Tau21_wta",
    //                 "fjet_Split12",
    //                 "fjet_Tau3_wta",
    //                 "fjet_ECF1",
    //                 "fjet_C2",
    //                 "fjet_Tau2_wta",
    //                 "fjet_Tau1_wta"
    //                 });

    add_bdt_vars(bdt_readers["TOP3q_NvarL"].get(), {
                    "fjet_Tau32_wta",
                    "fjet_ECF3",
                    "fjet_Qw",
                    "fjet_D2",
                    "fjet_ECF2",
                    "fjet_Split23",
                    "fjet_Tau21_wta",
                    "fjet_Split12",
                    "fjet_Tau3_wta"
                    });

    add_bdt_vars(bdt_readers["TOP3q_NvarM"].get(), {
                    "fjet_Tau32_wta",
                    "fjet_ECF3",
                    "fjet_Qw",
                    "fjet_D2",
                    "fjet_ECF2",
                    "fjet_Split23",
                    "fjet_Tau21_wta"
                    });

    add_bdt_vars(bdt_readers["TOP3q_NvarS"].get(), {
                    "fjet_Tau32_wta",
                    "fjet_ECF3",
                    "fjet_Qw",
                    "fjet_D2",
                    "fjet_ECF2",
                    "fjet_Split23",
                    "fjet_Tau21_wta"
                    });

    add_bdt_vars(bdt_readers["TOP_NvarM"].get(), {
                    "fjet_Tau32_wta",
                    "fjet_Qw",
                    "fjet_ECF3",
                    "fjet_D2",
                    "fjet_Tau21_wta",
                    "fjet_ECF1",
                    "fjet_Split12"
                    });

    const std::string rc = std::string(getenv("ROOTCOREBIN"));

    for (auto& r : bdt_readers) {
            std::string weight_filepath = rc + "/data/DataMCbackgroundTools/bdt_weights/TMVAClassification_BDTG." + r.first + "_pT200to2000GeV.weights.xml";
            r.second->BookMVA(r.first, rc +
                "/data/DataMCbackgroundTools/bdt_weights/TMVAClassification_BDTG." + r.first + "_pT200to2000GeV.weights.xml");
    }

    this->hp = new HistoPack(1);

    // save 'anti-tags' for the tagged mass spectra
    this->hp->h_rljet_m.at(0)->set_fill_veto(true);
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

    if (!compute_Tau32)
        b_rljet_Tau32_wta->GetEntry(entry);

    if (!compute_D2)
        b_rljet_D2->GetEntry(entry);

    if (this->operating_on_mc) {
        b_weight_mc->GetEntry(entry);
        b_weight_pileup->GetEntry(entry);
        b_weight_jvt->GetEntry(entry);
        b_weight_bTagSF_70->GetEntry(entry);
    }

    b_mcChannelNumber->GetEntry(entry);
    b_eventNumber->GetEntry(entry);

    if (this->on_nominal_branch) {
        // TODO: figure out what I'm supposed to be saving here...
        if (this->operating_on_mc) {
            b_mu->GetEntry(entry);
        } else {
            b_mu_original_xAOD->GetEntry(entry);
        }

        b_rljet_mjj->GetEntry(entry);
        b_rljet_ptasym->GetEntry(entry);
        b_rljet_dy->GetEntry(entry);
        b_rljet_dR->GetEntry(entry);
        b_rljet_count->GetEntry(entry);

        b_met_met->GetEntry(entry);

        b_rljet_Tau1_wta->GetEntry(entry);
        b_rljet_Tau2_wta->GetEntry(entry);
        b_rljet_Tau3_wta->GetEntry(entry);
        b_rljet_ECF1->GetEntry(entry);
        b_rljet_ECF2->GetEntry(entry);
        b_rljet_ECF3->GetEntry(entry);
        b_rljet_FoxWolfram0->GetEntry(entry);
        b_rljet_FoxWolfram2->GetEntry(entry);
        b_rljet_Qw->GetEntry(entry);
        b_rljet_Angularity->GetEntry(entry);
        b_rljet_Aplanarity->GetEntry(entry);
        b_rljet_Dip12->GetEntry(entry);
        b_rljet_KtDR->GetEntry(entry);
        b_rljet_Mu12->GetEntry(entry);
        b_rljet_PlanarFlow->GetEntry(entry);
        b_rljet_Sphericity->GetEntry(entry);
        b_rljet_Split12->GetEntry(entry);
        b_rljet_Split23->GetEntry(entry);
        b_rljet_Split34->GetEntry(entry);
        b_rljet_ThrustMaj->GetEntry(entry);
        b_rljet_ThrustMin->GetEntry(entry);
        b_rljet_ZCut12->GetEntry(entry);

        b_rljet_NTrimSubjets->GetEntry(entry);
        b_rljet_ungroomed_ntrk500->GetEntry(entry);

        if (this->operating_on_mc) {
            b_tljet_pt->GetEntry(entry);
            b_tljet_eta->GetEntry(entry);
            b_tljet_phi->GetEntry(entry);
            b_tljet_m->GetEntry(entry);
            b_tljet_dR->GetEntry(entry);
            b_tljet_D2->GetEntry(entry);
            b_tljet_Tau32_wta->GetEntry(entry);
        }

        b_HLT_jet_trigger->GetEntry(entry);

        b_hltjet_count->GetEntry(entry);
        b_hltjet_m->GetEntry(entry);
        b_hltjet_pt->GetEntry(entry);
        b_hltjet_eta->GetEntry(entry);
        b_hltjet_phi->GetEntry(entry);
        b_hltjet_dR->GetEntry(entry);

        b_caJet_count->GetEntry(entry);
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
            b_htt_pts1[i]->GetEntry(entry);
            b_htt_pts2[i]->GetEntry(entry);
            b_htt_pts3[i]->GetEntry(entry);
            b_htt_nTagCands[i]->GetEntry(entry);
            b_htt_tag[i]->GetEntry(entry);

            b_htt_caGroomJet_pt[i]->GetEntry(entry);
            b_htt_caGroomJet_eta[i]->GetEntry(entry);
            b_htt_caGroomJet_phi[i]->GetEntry(entry);
            b_htt_caGroomJet_m[i]->GetEntry(entry);
        }

        // b_rljet_btag_double->GetEntry(entry);
        // b_rljet_btag_single->GetEntry(entry);
        // b_rljet_btag_leading_pt->GetEntry(entry);

        b_rljet_SDw_win20_btag0->GetEntry(entry);
        b_rljet_SDz_win20_btag0->GetEntry(entry);
        b_rljet_SDt_win50_btag0->GetEntry(entry);

        if (this->operating_on_mc) {
            b_rljet_SDw_win20_btag0_UP->GetEntry(entry);
            b_rljet_SDz_win20_btag0_UP->GetEntry(entry);
            b_rljet_SDt_win50_btag0_UP->GetEntry(entry);

            b_rljet_SDw_win20_btag0_DOWN->GetEntry(entry);
            b_rljet_SDz_win20_btag0_DOWN->GetEntry(entry);
            b_rljet_SDt_win50_btag0_DOWN->GetEntry(entry);
        }
    }

    // calculate luminosity SF only once per file
    if (entry == 0) {
        TFile* current_file = this->fChain->GetCurrentFile();

        this->log("PROCESSING FILE: " + std::string(current_file->GetName()));
        this->log("BRANCH: " + std::string(fChain->GetName()));

        this->SF_lumi_Fb = 1.0;
        this->max_weight = 0.;

        if (this->operating_on_mc) {
            const ULong64_t nevents_skimmed = getTotalEventsSample(current_file);
            float sum_weights = get_sum_weights_sample(current_file);
            float xsection  = weight_tool->get_xsection(mcChannelNumber);
            float nevents   = weight_tool->get_nevents(mcChannelNumber);
            float filtereff = weight_tool->get_filtereff(mcChannelNumber);

            // TEMPORARY FIX FOR BUGGED ALL-HAD TTBAR WEIGHTS
            if (mcChannelNumber == 303722) {
                sum_weights = 3.396948e-01;
            } else if (mcChannelNumber == 303723) {
                sum_weights = 1.467807e-01;
            } else if (mcChannelNumber == 303724) {
                sum_weights = 6.405792e-02;
            } else if (mcChannelNumber == 303725) {
                sum_weights = 4.490201e-02;
            } else if (mcChannelNumber == 303726) {
                sum_weights = 2.736783e-02;
            }

            this->log("TOTAL EVENTS (SKIMMED): "   + std::to_string(nevents_skimmed));
            this->log("TOTAL EVENTS (UNSKIMMED): " + std::to_string(nevents));
            this->log("SUM WEIGHTS (UNSKIMMED): "  + std::to_string(sum_weights));

            const bool is_pythia_JZXW_slice = mcChannelNumber >= 361020
                && mcChannelNumber <= 361032;
            const bool is_herwig_JZXW_slice = mcChannelNumber >= 426040
                && mcChannelNumber <= 426052;

            // TODO: check if this is even needed
            if (is_pythia_JZXW_slice || is_herwig_JZXW_slice) {
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

    // and all events must pass the specified offline cuts
    if (!chosen_event_selector(this))
        return kFALSE;

    float weight;
    if (this->operating_on_mc) {
        weight = weight_mc * weight_pileup * weight_jvt * weight_bTagSF_70 * this->SF_lumi_Fb * this->luminosity;

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

        if (weight_tool->is_bugged_event(mcChannelNumber, eventNumber)) {
            std::stringstream ss;
            ss << std::fixed;
            ss << "REJECTING EVENT FROM BUGGED EVENT LIST: ";
            ss << "DSID: " << mcChannelNumber << " ";
            ss << "EVENT NUMBER: " << eventNumber;
            this->log(ss.str());
            return kFALSE;
        }
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
        // for more 'signal-like' jets, so impose a mass cut on the D2/Tau32 plots.
        if (rljet_m->at(i) / 1000. > 50.) {
            if (compute_D2 && on_nominal_branch) {
                float ECF1 = rljet_ECF1->at(i);
                float ECF2 = rljet_ECF2->at(i);
                float ECF3 = rljet_ECF3->at(i);
                float D2 = ECF3 * TMath::Power(ECF1, 3) / TMath::Power(ECF2, 3);
                hp->h_rljet_D2.at(i)->fill(D2, weight);
            } else {
                hp->h_rljet_D2.at(i)->fill(rljet_D2->at(i), weight);
            }
        }

        if (rljet_m->at(i) / 1000. > 100.) {
            if (compute_Tau32 && on_nominal_branch) {
                float Tau32_wta =  rljet_Tau3_wta->at(i) / rljet_Tau2_wta->at(i);
                hp->h_rljet_Tau32_wta.at(i)->fill(Tau32_wta, weight);
            } else {
                hp->h_rljet_Tau32_wta.at(i)->fill(rljet_Tau32_wta->at(i), weight);
            }
        }

        // SMOOTHED SUBSTRUCTURE TAGGERS
        prerec_tag_map["w_prerec_25eff_D2cut"        ] = rljet_smoothWTag25eff->at(i) == 1 || rljet_smoothWTag25eff->at(i) == 3;
        prerec_tag_map["w_prerec_25eff_masscut"      ] = rljet_smoothWTag25eff->at(i) >= 2;
        prerec_tag_map["w_prerec_25eff_D2masscut"    ] = rljet_smoothWTag25eff->at(i) == 3;
        prerec_tag_map["w_prerec_50eff_D2cut"        ] = rljet_smoothWTag50eff->at(i) == 1 || rljet_smoothWTag50eff->at(i) == 3;
        prerec_tag_map["w_prerec_50eff_masscut"      ] = rljet_smoothWTag50eff->at(i) >= 2;
        prerec_tag_map["w_prerec_50eff_D2masscut"    ] = rljet_smoothWTag50eff->at(i) == 3;
        prerec_tag_map["t_prerec_50eff_Tau32cut"     ] = rljet_smoothTau32Tag50eff->at(i);
        prerec_tag_map["t_prerec_50eff_masscut"      ] = rljet_smoothMassTag50eff->at(i);
        prerec_tag_map["t_prerec_50eff_Tau32masscut" ] = rljet_smoothMassTau32Tag50eff->at(i);
        prerec_tag_map["t_prerec_80eff_Tau32cut"     ] = rljet_smoothTau32Tag80eff->at(i);
        prerec_tag_map["t_prerec_80eff_masscut"      ] = rljet_smoothMassTag80eff->at(i);
        prerec_tag_map["t_prerec_80eff_Tau32masscut" ] = rljet_smoothMassTau32Tag80eff->at(i);

        if (this->operating_on_mc) {
            int pdgid = -1;

            hp->h_rljet_pdgid.at(i)->fill(pdgid, weight);

            if (pdgid == 24) {
                prerec_tag_map["w_prerec_25eff_D2cut_WMatch"    ] = rljet_smoothWTag25eff->at(i) == 1 || rljet_smoothWTag25eff->at(i) == 3;
                prerec_tag_map["w_prerec_25eff_masscut_WMatch"  ] = rljet_smoothWTag25eff->at(i) >= 2;
                prerec_tag_map["w_prerec_25eff_D2masscut_WMatch"] = rljet_smoothWTag25eff->at(i) == 3;
                prerec_tag_map["w_prerec_50eff_D2cut_WMatch"    ] = rljet_smoothWTag50eff->at(i) == 1 || rljet_smoothWTag50eff->at(i) == 3;
                prerec_tag_map["w_prerec_50eff_masscut_WMatch"  ] = rljet_smoothWTag50eff->at(i) >= 2;
                prerec_tag_map["w_prerec_50eff_D2masscut_WMatch"] = rljet_smoothWTag50eff->at(i) == 3;
            } else {
                prerec_tag_map["w_prerec_25eff_D2cut_WUnMatch"    ] = rljet_smoothWTag25eff->at(i) == 1 || rljet_smoothWTag25eff->at(i) == 3;
                prerec_tag_map["w_prerec_25eff_masscut_WUnMatch"  ] = rljet_smoothWTag25eff->at(i) >= 2;
                prerec_tag_map["w_prerec_25eff_D2masscut_WUnMatch"] = rljet_smoothWTag25eff->at(i) == 3;
                prerec_tag_map["w_prerec_50eff_D2cut_WUnMatch"    ] = rljet_smoothWTag50eff->at(i) == 1 || rljet_smoothWTag50eff->at(i) == 3;
                prerec_tag_map["w_prerec_50eff_masscut_WUnMatch"  ] = rljet_smoothWTag50eff->at(i) >= 2;
                prerec_tag_map["w_prerec_50eff_D2masscut_WUnMatch"] = rljet_smoothWTag50eff->at(i) == 3;
           }
        } // end of MC truth match/unmatch tagging for prec smoothed taggers

        for (const auto& itag : prerec_tag_map) {
            hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
            hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
        }
    }

    /*********************************************************/
    /* BELOW HERE, ONLY SAVING VARIABLES FROM NOMINAL BRANCH */
    /*********************************************************/

    if (!on_nominal_branch)
        return kTRUE;

    /***************************/
    /* GENERAL EVENT VARIABLES */
    /***************************/

    const bool ranSD = rljet_SDw_win20_btag0->size() > 0;

    /*******************/
    /* ANTI-KT 10 JETS */
    /*******************/

    for (UInt_t i = 0; i < n_rljets_recorded; i++) {

        const bool pass_ntrk_cut =
            rljet_ungroomed_ntrk500->at(i) >= 0 && rljet_ungroomed_ntrk500->at(i) < 30;

        if (pass_ntrk_cut) {
            ntrk_prerec_tag_map["w_prerec_25eff_ntrkLT30_D2cut"     ] = rljet_smoothWTag25eff->at(i) == 1 || rljet_smoothWTag25eff->at(i) == 3;
            ntrk_prerec_tag_map["w_prerec_25eff_ntrkLT30_masscut"   ] = rljet_smoothWTag25eff->at(i) >= 2;
            ntrk_prerec_tag_map["w_prerec_25eff_ntrkLT30_D2masscut" ] = rljet_smoothWTag25eff->at(i) == 3;
            ntrk_prerec_tag_map["w_prerec_50eff_ntrkLT30_D2cut"     ] = rljet_smoothWTag50eff->at(i) == 1 || rljet_smoothWTag50eff->at(i) == 3;
            ntrk_prerec_tag_map["w_prerec_50eff_ntrkLT30_masscut"   ] = rljet_smoothWTag50eff->at(i) >= 2;
            ntrk_prerec_tag_map["w_prerec_50eff_ntrkLT30_D2masscut" ] = rljet_smoothWTag50eff->at(i) == 3;

            for (const auto& itag : ntrk_prerec_tag_map) {
                hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
            }
        }

        if (this->operating_on_mc
                && tljet_dR->at(i) < 0.4
                && tljet_pt->at(i) > 1e-6
                && tljet_m->at(i) > 1e-6
                && tljet_D2->at(i) > 1e-6
                && tljet_Tau32_wta->at(i) > 1e-6
                && rljet_pt->at(i) > 0
                && rljet_m->at(i) > 0
                && rljet_D2->at(i) > 0
                && rljet_Tau32_wta->at(i) > 0
                )
        {
            hp->h_rljet_RES_m.at(i)->fill(rljet_m->at(i) / tljet_m->at(i), weight);
            hp->h_rljet_RES_pT.at(i)->fill(rljet_pt->at(i) / tljet_pt->at(i), weight);
            hp->h_rljet_RES_D2.at(i)->fill(rljet_D2->at(i) / tljet_D2->at(i), weight);
            hp->h_rljet_RES_Tau32_wta.at(i)->fill(rljet_Tau32_wta->at(i) / tljet_Tau32_wta->at(i), weight);
        }

        // other substructure variables
        const float Tau1_wta = rljet_Tau1_wta->at(i);
        const float Tau2_wta = rljet_Tau2_wta->at(i);
        const float Tau3_wta = rljet_Tau3_wta->at(i);

        const float ECF1 = rljet_ECF1->at(i);
        const float ECF2 = rljet_ECF2->at(i);
        const float ECF3 = rljet_ECF3->at(i);

        hp->h_rljet_ECF1.at(i)->fill(ECF1, weight);
        hp->h_rljet_ECF2.at(i)->fill(ECF2, weight);
        hp->h_rljet_ECF3.at(i)->fill(ECF3, weight);

        hp->h_rljet_Tau1_wta.at(i)->fill(Tau1_wta, weight);
        hp->h_rljet_Tau2_wta.at(i)->fill(Tau2_wta, weight);
        hp->h_rljet_Tau3_wta.at(i)->fill(Tau3_wta, weight);

        const float Tau21_wta = fabs(Tau1_wta) > 1.e-6 ? Tau2_wta / Tau1_wta : -1000.;
        hp->h_rljet_Tau21_wta.at(i)->fill(Tau21_wta, weight);

        const float Tau32_wta = fabs(Tau2_wta) > 1.e-6 ? Tau3_wta / Tau2_wta : -1000.;

        const float C2 = ECF3 * ECF1 / TMath::Power(ECF2, 2);
        hp->h_rljet_C2.at(i)->fill(C2, weight);

        const float fw0 = rljet_FoxWolfram0->at(i);
        const float fw2 = rljet_FoxWolfram2->at(i);
        const float fw20 = fabs(fw0) > 1.e-6 ? fw2 / fw0 : -1000.;


        hp->h_rljet_FoxWolfram0.at(i)->fill(fw0, weight);
        hp->h_rljet_FoxWolfram2.at(i)->fill(fw2, weight);
        hp->h_rljet_FoxWolfram20.at(i)->fill(fw20, weight);

        hp->h_rljet_Qw.at(i)->fill(rljet_Qw->at(i), weight);
        hp->h_rljet_Angularity.at(i)->fill(rljet_Angularity->at(i), weight);
        hp->h_rljet_Aplanarity.at(i)->fill(rljet_Aplanarity->at(i), weight);
        hp->h_rljet_Dip12.at(i)->fill(rljet_Dip12->at(i), weight);
        hp->h_rljet_KtDR.at(i)->fill(rljet_KtDR->at(i), weight);
        hp->h_rljet_Mu12.at(i)->fill(rljet_Mu12->at(i), weight);
        hp->h_rljet_PlanarFlow.at(i)->fill(rljet_PlanarFlow->at(i), weight);
        hp->h_rljet_Sphericity.at(i)->fill(rljet_Sphericity->at(i), weight);
        hp->h_rljet_Split12.at(i)->fill(rljet_Split12->at(i), weight);
        hp->h_rljet_Split23.at(i)->fill(rljet_Split23->at(i), weight);
        hp->h_rljet_Split34.at(i)->fill(rljet_Split34->at(i), weight);
        hp->h_rljet_ThrustMaj.at(i)->fill(rljet_ThrustMaj->at(i), weight);
        hp->h_rljet_ThrustMin.at(i)->fill(rljet_ThrustMin->at(i), weight);
        hp->h_rljet_ZCut12.at(i)->fill(rljet_ZCut12->at(i), weight);

        hp->h_rljet_NTrimSubjets.at(i)->fill(rljet_NTrimSubjets->at(i), weight);
        hp->h_rljet_ungroomed_ntrk500.at(i)->fill(rljet_ungroomed_ntrk500->at(i), weight);

        if ( rljet_m->at(i)/1000. > 40
                && rljet_D2->at(i) > 0
                && C2 > 0
                && rljet_Tau32_wta->at(i) > 0
                && rljet_Tau21_wta->at(i) > 0
                && rljet_Tau1_wta->at(i) > 0
                && rljet_Tau2_wta->at(i) > 0
                && rljet_Tau3_wta->at(i) > 0
                && rljet_ECF1->at(i) > 0
                && rljet_ECF2->at(i) > 0
                && rljet_ECF3->at(i) > 0)
        {
            dnn_input_map["Tau1_wta"]  = rljet_Tau1_wta->at(i);
            dnn_input_map["Tau2_wta"]  = rljet_Tau2_wta->at(i);
            dnn_input_map["Tau3_wta"]  = rljet_Tau3_wta->at(i);
            dnn_input_map["Tau21_wta"] = Tau21_wta;
            dnn_input_map["Tau32_wta"] = Tau32_wta;
            dnn_input_map["Split12"]   = rljet_Split12->at(i);
            dnn_input_map["Split23"]   = rljet_Split23->at(i);
            dnn_input_map["Qw"]        = rljet_Qw->at(i);
            dnn_input_map["C2"]        = C2;
            dnn_input_map["D2"]        = rljet_D2->at(i);
            dnn_input_map["ECF1"]      = rljet_ECF1->at(i);
            dnn_input_map["ECF2"]      = rljet_ECF2->at(i);
            dnn_input_map["ECF3"]      = rljet_ECF3->at(i);

            hp->h_rljet_DNN_score.at(i)->fill_tagged("TEST", dnn_top_tagger->GetDiscriminant(dnn_input_map), weight, true);

            bdt_input_map["fjet_D2"]           = rljet_D2->at(i);
            bdt_input_map["fjet_ECF3"]         = rljet_ECF3->at(i);
            bdt_input_map["fjet_C2"]           = C2;
            bdt_input_map["fjet_ThrustMin"]    = rljet_ThrustMin->at(i);
            bdt_input_map["fjet_Angularity"]   = rljet_Angularity->at(i);
            bdt_input_map["fjet_PlanarFlow"]   = rljet_PlanarFlow->at(i);
            bdt_input_map["fjet_Tau21_wta"]    = Tau21_wta;
            bdt_input_map["fjet_Aplanarity"]   = rljet_Aplanarity->at(i);
            bdt_input_map["fjet_FoxWolfram20"] = fw20;
            bdt_input_map["fjet_Split12"]      = rljet_Split12->at(i);
            bdt_input_map["fjet_ZCut12"]       = rljet_ZCut12->at(i);
            bdt_input_map["fjet_KtDR"]         = rljet_KtDR->at(i);
            bdt_input_map["fjet_ECF2"]         = rljet_ECF2->at(i);
            bdt_input_map["fjet_Tau1_wta"]     = rljet_Tau1_wta->at(i);
            bdt_input_map["fjet_Sphericity"]   = rljet_Sphericity->at(i);
            bdt_input_map["fjet_Dip12"]        = rljet_Dip12->at(i);
            bdt_input_map["fjet_ThrustMaj"]    = rljet_ThrustMaj->at(i);
            bdt_input_map["fjet_Tau2_wta"]     = rljet_Tau2_wta->at(i);
            bdt_input_map["fjet_ECF1"]         = rljet_ECF1->at(i);

            for (auto& r  : bdt_readers) {
                hp->h_rljet_BDT_score.at(i)->fill_tagged(r.first, r.second->EvaluateMVA(r.first), weight, true);
            }

        } else {
            num_mva_rejected_jets++;
        } // end saving MVA tagger distributions

        // SD log(chi) variables
        if (ranSD) {
            hp->h_rljet_SDw_win20_btag0_logchi.at(i)->fill(rljet_SDw_win20_btag0->at(i), weight);
            hp->h_rljet_SDz_win20_btag0_logchi.at(i)->fill(rljet_SDz_win20_btag0->at(i), weight);
            hp->h_rljet_SDt_win50_btag0_logchi.at(i)->fill(rljet_SDt_win50_btag0->at(i), weight);

            SD_nominal_tag_map["SDw_win20_btag0"] = rljet_SDw_win20_btag0->at(i) > 2.8;
            SD_nominal_tag_map["SDz_win20_btag0"] = rljet_SDz_win20_btag0->at(i) > 2.8;
            SD_nominal_tag_map["SDt_win50_btag0"] = rljet_SDt_win50_btag0->at(i) > 4.;

            for (const auto& itag : SD_nominal_tag_map) {
                hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_pt->at(i)/1000., weight, itag.second);
            }

            if (this->operating_on_mc) {
                SD_systematic_tag_map["SDw_win20_btag0_UP"] = rljet_SDw_win20_btag0_UP->at(i) > 2.8;
                SD_systematic_tag_map["SDz_win20_btag0_UP"] = rljet_SDz_win20_btag0_UP->at(i) > 2.8;
                SD_systematic_tag_map["SDt_win50_btag0_UP"] = rljet_SDt_win50_btag0_UP->at(i) > 4.;
                SD_systematic_tag_map["SDw_win20_btag0_DOWN"] = rljet_SDw_win20_btag0_DOWN->at(i) > 2.8;
                SD_systematic_tag_map["SDz_win20_btag0_DOWN"] = rljet_SDz_win20_btag0_DOWN->at(i) > 2.8;
                SD_systematic_tag_map["SDt_win50_btag0_DOWN"] = rljet_SDt_win50_btag0_DOWN->at(i) > 4.;

                for (const auto& itag : SD_systematic_tag_map) {
                    hp->h_rljet_m.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
                    hp->h_rljet_pt.at(i)->fill_tagged(itag.first, rljet_m->at(i)/1000., weight, itag.second);
                }
            } // end of saving systematic branch SD-tagged variables
        } // end of saving SD-tagged variables


    } // end of saving all anti-kt R = 1.0 distributions

    if (rljet_count >= 2) {
        hp->h_rljet_mjj->fill(rljet_mjj/1000., weight);
        hp->h_rljet_ptasym->fill(rljet_ptasym, weight);
        hp->h_rljet_dy->fill(rljet_dy, weight);
        hp->h_rljet_dR->fill(rljet_dR, weight);
    }

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
            hp->h_htt_pt.at(ijet)->fill_tagged(htt_config_strings[ihtt]  , htt_pt[ihtt]->at(ijet) / 1000. , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_eta.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_eta[ihtt]->at(ijet)        , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_phi.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_phi[ihtt]->at(ijet)        , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_m.at(ijet)->fill_tagged(htt_config_strings[ihtt]   , htt_m[ihtt]->at(ijet) / 1000.  , weight , htt_tag[ihtt]->at(ijet));

            hp->h_htt_atan1312.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_atan1312[ihtt]->at(ijet) , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_m23m123.at(ijet)->fill_tagged(htt_config_strings[ihtt]  , htt_m23m123[ihtt]->at(ijet)  , weight , htt_tag[ihtt]->at(ijet));

            hp->h_htt_pts1.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_pts1[ihtt]->at(ijet) , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_pts2.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_pts2[ihtt]->at(ijet) , weight , htt_tag[ihtt]->at(ijet));
            hp->h_htt_pts3.at(ijet)->fill_tagged(htt_config_strings[ihtt] , htt_pts3[ihtt]->at(ijet) , weight , htt_tag[ihtt]->at(ijet));

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

    std::stringstream ss;
    ss << "WRITING: " << this->output_filepath;
    this->log(ss.str());

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

    ss.str(std::string());
    ss << "FINISHED: " << root_dir_str << "/" << sub_dir_str;

    this->log(ss.str());

    delete hp;
}


