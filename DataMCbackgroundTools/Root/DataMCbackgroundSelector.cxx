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

#include "TopExamples/AnalysisTools.h"

EventSelector NO_SELECTION = [](const DataMCbackgroundSelector* /* sel */) {
    return true;
};

EventSelector DIJET_LOOSE = [](const DataMCbackgroundSelector* sel) {
    if (sel->rljet_pt_comb->at(0)/1000. < 450.) return false;

    return true;
};

EventSelector DIJET_TIGHT = [](const DataMCbackgroundSelector* sel) {
    if (sel->rljet_pt_comb->at(0)/1000. < 500.)
        return false;

    if (sel->rljet_D2->at(0) <= 0.)
        return false;

    return true;
};

EventSelector VVJJ_SELECTION = [](const DataMCbackgroundSelector* sel ) {
    if ( sel->rljet_pt_comb->at(0) / 1000. < 450
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

// so that we can specify the selector by command line argument string
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
    operating_on_mc(root_dir_str_.find("data") == std::string::npos),
    on_nominal_branch(sub_dir_str_ == "nominal"),
    luminosity(luminosity_)
{
    const std::string rc = std::string(getenv("ROOTCOREBIN"));

    weight_tool = make_unique<WeightTool>();

    std::string xsection_file = rc + "/data/DataMCbackgroundTools/sample_weights_JETM6.txt";
    if (!weight_tool->read_weights_file(xsection_file))
        exit(1);

    std::string bugged_events_file = rc + "/data/DataMCbackgroundTools/bugged_events.txt";
    if (!weight_tool->read_bugged_events_file(bugged_events_file))
        exit(1);

    try {
        std::cout << "OFFLINE EVENT SELECTION SPECIFIED: " << event_selector_str_ << std::endl;
        chosen_event_selector = available_event_selectors.at(event_selector_str_);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
        std::cout << "EventSelector: " << event_selector_str_ << " not available." << std::endl;
        std::cout << "see DataMCbackgroundSelector.cxx for more details." << std::endl;
    }

    // BDT
    // top taggers (80% eff. WP, medium variable set)
    // m_topTagger_BDT_qqb = make_unique<JSSWTopTaggerBDT>("topTagger_BDT_qqb");
    // m_topTagger_BDT_qqb->setProperty("TopTagger_fullyContained", true);
    // m_topTagger_BDT_qqb->initialize();

    // m_topTagger_BDT_inclusive = make_unique<JSSWTopTaggerBDT>("topTagger_BDT_inclusive");
    // m_topTagger_BDT_inclusive->setProperty("TopTagger", true);
    // m_topTagger_BDT_inclusive->initialize();

    // W tagger (80% eff. WP, medium variable set)
    // m_wTagger_BDT = make_unique<JSSWTopTaggerBDT>("m_wTagger_BDT");
    // m_wTagger_BDT->setProperty("WTagger", true);
    // m_wTagger_BDT->initialize();

    // m_topTagger_DNN_qqb = make_unique<JSSWTopTaggerDNN>("m_topTagger_DNN_qqb");
    // m_topTagger_DNN_qqb->setProperty("TopTagger", true);
    // m_topTagger_DNN_qqb->setProperty("KerasConfigFile", "keras_top_dnn_BN_december11_contained.json");
    // m_topTagger_DNN_qqb->setProperty("KerasOutput", "top_dnn_BN_dec11_contained");
    // m_topTagger_DNN_qqb->initialize();

    // m_topTagger_DNN_inclusive = make_unique<JSSWTopTaggerDNN>("m_topTagger_DNN_inclusive");
    // m_topTagger_DNN_inclusive->setProperty("TopTagger", true);
    // m_topTagger_DNN_inclusive->setProperty("KerasConfigFile", "keras_top_dnn_BN_december11_all.json");
    // m_topTagger_DNN_inclusive->setProperty("KerasOutput", "top_dnn_BN_dec11_all");
    // m_topTagger_DNN_inclusive->initialize();
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

    this->hp = new HistoPack(1);
    // save 'anti-tags' for the tagged mass spectra
    // this->hp->h_rljet_m_comb.at(0)->set_fill_veto(true);
    // this->hp->h_rljet_m_calo.at(0)->set_fill_veto(true);
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

    if (this->operating_on_mc) {
        b_weight_mc->GetEntry(entry);
        b_weight_pileup->GetEntry(entry);
        b_weight_leptonSF->GetEntry(entry);
        b_weight_jvt->GetEntry(entry);
        b_randomRunNumber->GetEntry(entry);
    } else {
        b_HLT_jet_trigger->GetEntry(entry);
    }

    b_eventNumber->GetEntry(entry);
    b_runNumber->GetEntry(entry);
    b_mcChannelNumber->GetEntry(entry);
    b_mu->GetEntry(entry);
    b_backgroundFlags->GetEntry(entry);
    //b_met_met->GetEntry(entry);
    //b_met_phi->GetEntry(entry);
    b_dijets->GetEntry(entry);
    b_rljet_eta->GetEntry(entry);
    b_rljet_phi->GetEntry(entry);
    b_rljet_m_comb->GetEntry(entry);
    b_rljet_pt_comb->GetEntry(entry);
    b_rljet_D2->GetEntry(entry);
    b_rljet_Tau32_wta->GetEntry(entry);
    b_rljet_Qw->GetEntry(entry);
    b_rljet_Split23->GetEntry(entry);
    b_m_rljet_smooth16Top_Tau32Split23Tag50eff->GetEntry(entry);
    b_m_rljet_smooth16Top_Tau32Split23Tag80eff->GetEntry(entry);
    b_m_rljet_smooth16Top_MassTau32Tag50eff->GetEntry(entry);
    b_m_rljet_smooth16Top_MassTau32Tag80eff->GetEntry(entry);
    b_m_rljet_smooth16Top_QwTau32Tag50eff->GetEntry(entry);
    b_m_rljet_smooth16Top_QwTau32Tag80eff->GetEntry(entry);
    b_rljet_smooth16WTag_50eff->GetEntry(entry);
    b_rljet_smooth16WTag_80eff->GetEntry(entry);
    b_rljet_smooth16ZTag_50eff->GetEntry(entry);
    b_rljet_smooth16ZTag_80eff->GetEntry(entry);

    if (this->on_nominal_branch) {
        b_NPV->GetEntry(entry);
        b_rljet_count->GetEntry(entry);
        b_rljet_mjj->GetEntry(entry);
        b_rljet_ptasym->GetEntry(entry);
        b_rljet_dy->GetEntry(entry);
        b_rljet_dR->GetEntry(entry);
        b_rljet_dphi->GetEntry(entry);
        b_rljet_deta->GetEntry(entry);
        b_rljet_m_calo->GetEntry(entry);
        b_rljet_pt_calo->GetEntry(entry);
        b_rljet_m_ta->GetEntry(entry);
        b_rljet_pt_ta->GetEntry(entry);
        b_m_rljet_smooth15Top_MassTau32Tag50eff->GetEntry(entry);
        b_m_rljet_smooth15Top_MassTau32Tag80eff->GetEntry(entry);
        b_rljet_smooth15WTag_50eff->GetEntry(entry);
        b_rljet_smooth15WTag_25eff->GetEntry(entry);
        b_rljet_smooth15ZTag_50eff->GetEntry(entry);
        b_rljet_smooth15ZTag_25eff->GetEntry(entry);
        b_rljet_Tau1_wta->GetEntry(entry);
        b_rljet_Tau2_wta->GetEntry(entry);
        b_rljet_Tau3_wta->GetEntry(entry);
        b_rljet_ECF1->GetEntry(entry);
        b_rljet_ECF2->GetEntry(entry);
        b_rljet_ECF3->GetEntry(entry);
        b_rljet_FoxWolfram0->GetEntry(entry);
        b_rljet_FoxWolfram2->GetEntry(entry);
        b_rljet_Angularity->GetEntry(entry);
        b_rljet_Aplanarity->GetEntry(entry);
        b_rljet_Dip12->GetEntry(entry);
        b_rljet_KtDR->GetEntry(entry);
        b_rljet_Mu12->GetEntry(entry);
        b_rljet_PlanarFlow->GetEntry(entry);
        b_rljet_Sphericity->GetEntry(entry);
        b_rljet_Split12->GetEntry(entry);
        b_rljet_Split34->GetEntry(entry);
        b_rljet_ThrustMaj->GetEntry(entry);
        b_rljet_ThrustMin->GetEntry(entry);
        b_rljet_ZCut12->GetEntry(entry);
        b_rljet_NTrimSubjets->GetEntry(entry);
        b_rljet_ungroomed_ntrk500->GetEntry(entry);
        b_htt_pt_def->GetEntry(entry);
        b_htt_eta_def->GetEntry(entry);
        b_htt_phi_def->GetEntry(entry);
        b_htt_m_def->GetEntry(entry);
        b_htt_m123_def->GetEntry(entry);
        b_htt_m23m123_def->GetEntry(entry);
        b_htt_atan1312_def->GetEntry(entry);
        b_htt_nTagCands_def->GetEntry(entry);
        b_htt_tag_def->GetEntry(entry);
        b_htt_pts1_def->GetEntry(entry);
        b_htt_pts2_def->GetEntry(entry);
        b_htt_pts3_def->GetEntry(entry);
        b_htt_caJet_pt->GetEntry(entry);
        b_htt_caJet_eta->GetEntry(entry);
        b_htt_caJet_phi->GetEntry(entry);
        b_htt_caJet_m->GetEntry(entry);
        b_caJet_count->GetEntry(entry);
        b_htt_caGroomJet_pt_def->GetEntry(entry);
        b_htt_caGroomJet_eta_def->GetEntry(entry);
        b_htt_caGroomJet_phi_def->GetEntry(entry);
        b_htt_caGroomJet_m_def->GetEntry(entry);
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
            b_htt_caGroomJet_pt_sjcalib1030->GetEntry(entry);
            b_htt_caGroomJet_eta_sjcalib1030->GetEntry(entry);
            b_htt_caGroomJet_phi_sjcalib1030->GetEntry(entry);
            b_htt_caGroomJet_m_sjcalib1030->GetEntry(entry);
            b_htt_caGroomJet_pt_sjcalib0970->GetEntry(entry);
            b_htt_caGroomJet_eta_sjcalib0970->GetEntry(entry);
            b_htt_caGroomJet_phi_sjcalib0970->GetEntry(entry);
            b_htt_caGroomJet_m_sjcalib0970->GetEntry(entry);
            b_htt_pt_sjcalib1030->GetEntry(entry);
            b_htt_eta_sjcalib1030->GetEntry(entry);
            b_htt_phi_sjcalib1030->GetEntry(entry);
            b_htt_m_sjcalib1030->GetEntry(entry);
            b_htt_m123_sjcalib1030->GetEntry(entry);
            b_htt_m23m123_sjcalib1030->GetEntry(entry);
            b_htt_atan1312_sjcalib1030->GetEntry(entry);
            b_htt_nTagCands_sjcalib1030->GetEntry(entry);
            b_htt_tag_sjcalib1030->GetEntry(entry);
            b_htt_pts1_sjcalib1030->GetEntry(entry);
            b_htt_pts2_sjcalib1030->GetEntry(entry);
            b_htt_pts3_sjcalib1030->GetEntry(entry);
            b_htt_pt_sjcalib0970->GetEntry(entry);
            b_htt_eta_sjcalib0970->GetEntry(entry);
            b_htt_phi_sjcalib0970->GetEntry(entry);
            b_htt_m_sjcalib0970->GetEntry(entry);
            b_htt_m123_sjcalib0970->GetEntry(entry);
            b_htt_m23m123_sjcalib0970->GetEntry(entry);
            b_htt_atan1312_sjcalib0970->GetEntry(entry);
            b_htt_nTagCands_sjcalib0970->GetEntry(entry);
            b_htt_tag_sjcalib0970->GetEntry(entry);
            b_htt_pts1_sjcalib0970->GetEntry(entry);
            b_htt_pts2_sjcalib0970->GetEntry(entry);
            b_htt_pts3_sjcalib0970->GetEntry(entry);
        }
    }

    if (entry == 0) { // calculate luminosity SF only once per file
        TFile* current_file = this->fChain->GetCurrentFile();

        this->log("PROCESSING FILE: " + std::string(current_file->GetName()));
        this->log("BRANCH: " + std::string(fChain->GetName()));

        this->SF_lumi_Fb = 1.0;
        this->max_weight = 0.;

        if (this->operating_on_mc) {
            const ULong64_t nevents_skimmed = getTotalEventsSample(current_file);
            const float sum_weights               = get_sum_weights_sample(current_file);
            const float xsection                  = weight_tool->get_xsection(mcChannelNumber);
            const float nevents                   = weight_tool->get_nevents(mcChannelNumber);
            const float filtereff                 = weight_tool->get_filtereff(mcChannelNumber);

            this->log("TOTAL EVENTS (SKIMMED): "   + std::to_string(nevents_skimmed));
            this->log("TOTAL EVENTS (UNSKIMMED): " + std::to_string(nevents));
            this->log("SUM WEIGHTS (UNSKIMMED): "  + std::to_string(sum_weights));

            const bool is_pythia_JZXW_slice = mcChannelNumber >= 361020 && mcChannelNumber <= 361032;
            const bool is_herwig_JZXW_slice = mcChannelNumber >= 426040 && mcChannelNumber <= 426052;
            const bool is_sherpa_JZX_slice  = mcChannelNumber >= 426131 && mcChannelNumber <= 426142;

            this->processing_dijet_slice = is_pythia_JZXW_slice || is_herwig_JZXW_slice || is_sherpa_JZX_slice;

            if (is_pythia_JZXW_slice) {
                this->dijet_slice_number = mcChannelNumber - 361020;
            } else if (is_herwig_JZXW_slice) {
                this->dijet_slice_number = mcChannelNumber - 426040;
            } else if (is_sherpa_JZX_slice) {
                this->dijet_slice_number = mcChannelNumber - 426130;
            }

            if (processing_dijet_slice) {
                this->SF_lumi_Fb = xsection * filtereff / nevents;
            } else {
                this->SF_lumi_Fb = xsection * filtereff / sum_weights;
            }
        }

        current_file = nullptr; // because ROOT ownership semantics are weird and I trust nothing
    }

    // events in data must pass the specified trigger
    if (!this->operating_on_mc && HLT_jet_trigger == 0)
        return kFALSE;

    // and all events must pass the specified offline cuts
    if (!chosen_event_selector(this))
        return kFALSE;

    float weight;
    if (this->operating_on_mc) {
        weight = weight_mc * weight_pileup * weight_jvt * this->SF_lumi_Fb * this->luminosity;

        if (weight > max_weight) {
            max_weight = weight;

            std::stringstream ss;
            ss << std::fixed;
            ss << "FOUND HIGH WEIGHT -- ENTRY NUMBER: " << entry << " ";
            ss << "EVENT NUMBER: " << eventNumber << " ";
            ss << std::scientific;
            ss << "WEIGHT: " << weight << " ";
            ss << "LEAD JET pT: " << rljet_pt_comb->at(0) / 1000. << " GeV";
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
    } else {
        // if event is data
        weight = 1.;
    }

    /***************************************************************/
    /* VARIABLES TO RECORD FOR ALL BRANCHES (NOMINAL + SYSTEMATIC) */
    /***************************************************************/

    const UInt_t n_rljets_recorded = rljet_pt_comb->size();
    for (UInt_t i = 0; i < 3 && i < n_rljets_recorded; i++) {
        // kinematics
        hp->h_rljet_pt_comb.at(i)->fill(rljet_pt_comb->at(i)/1000., weight);
        hp->h_rljet_eta.at(i)->fill(rljet_eta->at(i), weight);
        hp->h_rljet_phi.at(i)->fill(rljet_phi->at(i), weight);
        hp->h_rljet_m_comb.at(i)->fill(rljet_m_comb->at(i)/1000., weight);

        // mass-cut-tagged versions kept for more 'signal-like' jet distributions
        // since low mass jets won't be kept by any of the simple taggers anyway
        hp->h_rljet_D2.at(i)->fill(rljet_D2->at(i), weight);
        hp->h_rljet_D2.at(i)->fill_tagged("_combMgt50GeV", rljet_D2->at(i), weight, rljet_m_comb->at(i) / 1000. > 50.);

        hp->h_rljet_Tau32_wta.at(i)->fill(rljet_Tau32_wta->at(i), weight);
        hp->h_rljet_Tau32_wta.at(i)->fill_tagged("_combMgt100GeV", rljet_Tau32_wta->at(i), weight, rljet_m_comb->at(i) / 1000. > 100.);

        hp->h_rljet_Qw.at(i)->fill(rljet_Qw->at(i), weight);
        hp->h_rljet_Qw.at(i)->fill_tagged("_combMgt100GeV", rljet_Qw->at(i), weight, rljet_m_comb->at(i) / 1000. > 100.);

        hp->h_rljet_Split23.at(i)->fill(rljet_Split23->at(i), weight);
        hp->h_rljet_Split23.at(i)->fill_tagged("_combMgt100GeV", rljet_Split23->at(i), weight, rljet_m_comb->at(i) / 1000. > 100.);

        // SMOOTHED SUBSTRUCTURE TAGGERS
        smooth_tag_map["smooth16Top_Tau32Split23Tag50eff"] = rljet_smooth16Top_Tau32Split23Tag50eff->at(i) == 3;
        smooth_tag_map["smooth16Top_Tau32Split23Tag80eff"] = rljet_smooth16Top_Tau32Split23Tag80eff->at(i) == 3;
        smooth_tag_map["smooth16Top_QwTau32Tag50eff"] = rljet_smooth16Top_QwTau32Tag50eff->at(i) == 3;
        smooth_tag_map["smooth16Top_QwTau32Tag80eff"] = rljet_smooth16Top_QwTau32Tag80eff->at(i) == 3;
        smooth_tag_map["smooth16Top_MassTau32Tag50eff_JSSCut"] = rljet_smooth16Top_MassTau32Tag50eff->at(i) == 3 || rljet_smooth16Top_MassTau32Tag50eff->at(i) == 2;
        smooth_tag_map["smooth16Top_MassTau32Tag50eff_MassJSSCut"] = rljet_smooth16Top_MassTau32Tag50eff->at(i) == 3;
        smooth_tag_map["smooth16Top_MassTau32Tag80eff_JSSCut"] = rljet_smooth16Top_MassTau32Tag80eff->at(i) == 3 || rljet_smooth16Top_MassTau32Tag80eff->at(i) == 2;
        smooth_tag_map["smooth16Top_MassTau32Tag80eff_MassJSSCut"] = rljet_smooth16Top_MassTau32Tag80eff->at(i) == 3;

        smooth_tag_map["smooth16WTag_50eff_JSSCut"] = rljet_smooth16WTag_50eff->at(i) == 1 || rljet_smooth16WTag_50eff->at(i) == 16 || rljet_smooth16WTag_50eff->at(i) == 4;
        smooth_tag_map["smooth16WTag_50eff_MassJSSCut"] = rljet_smooth16WTag_50eff->at(i) == 1;
        smooth_tag_map["smooth16WTag_80eff_JSSCut"] = rljet_smooth16WTag_80eff->at(i) == 1 || rljet_smooth16WTag_80eff->at(i) == 16 || rljet_smooth16WTag_80eff->at(i) == 4;
        smooth_tag_map["smooth16WTag_80eff_MassJSSCut"] = rljet_smooth16WTag_80eff->at(i) == 1;
        smooth_tag_map["smooth16ZTag_50eff_JSSCut"] = rljet_smooth16ZTag_50eff->at(i) == 1 || rljet_smooth16ZTag_50eff->at(i) == 16 || rljet_smooth16ZTag_50eff->at(i) == 4;
        smooth_tag_map["smooth16ZTag_50eff_MassJSSCut"] = rljet_smooth16ZTag_50eff->at(i) == 1;
        smooth_tag_map["smooth16ZTag_80eff_JSSCut"] = rljet_smooth16ZTag_80eff->at(i) == 1 || rljet_smooth16ZTag_80eff->at(i) == 16 || rljet_smooth16ZTag_80eff->at(i) == 4;
        smooth_tag_map["smooth16ZTag_80eff_MassJSSCut"] = rljet_smooth16ZTag_80eff->at(i) == 1;


        for (const auto& itag : smooth_tag_map) {
            hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
            hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
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

    hp->h_NPV->fill( (Float_t) NPV, weight);

    if (!this->operating_on_mc) {
        hp->h_mu->fill(mu, weight);
        hp->h_mu->fill_tagged("NPV_sf"      , mu * 1./1.16              , weight , true);
        hp->h_mu->fill_tagged("lumi_NPV_sf" , mu * (1./1.055) * 1./1.16 , weight , true);
    } else {
        // note: we don't actually correct the mc mu distribution, making the below tagged
        // histograms just makes it easier to do data/mc comparison plots later
        hp->h_mu->fill(mu, weight);
        hp->h_mu->fill_tagged("NPV_sf"      , mu, weight, true);
        hp->h_mu->fill_tagged("lumi_NPV_sf" , mu, weight, true);
    }

    /*******************/
    /* ANTI-KT 10 JETS */
    /*******************/

    for (UInt_t i = 0; i < n_rljets_recorded; i++) {
        hp->h_rljet_pt_calo.at(i)->fill(rljet_pt_calo->at(i)/1000., weight);
        hp->h_rljet_m_calo.at(i)->fill(rljet_m_calo->at(i)/1000., weight);
        hp->h_rljet_pt_ta.at(i)->fill(rljet_pt_ta->at(i)/1000., weight);
        hp->h_rljet_m_ta.at(i)->fill(rljet_m_ta->at(i)/1000., weight);

        // JZX(W) SLICE TAGS
        if (this->operating_on_mc && processing_dijet_slice) {
            for (const auto& itag : smooth_tag_map) {
                std::string tmp_tag_str = itag.first + "_JZ" + std::to_string(dijet_slice_number);
                hp->h_rljet_m_comb.at(i)->fill_tagged(tmp_tag_str, rljet_m_comb->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt_comb.at(i)->fill_tagged(tmp_tag_str, rljet_pt_comb->at(i)/1000., weight, itag.second);
            }
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

        const float C2 = fabs(ECF2) > 1.e-6 ? ECF3 * ECF1 / TMath::Power(ECF2, 2) : -1000.;
        hp->h_rljet_C2.at(i)->fill(C2, weight);

        const float fw0 = rljet_FoxWolfram0->at(i);
        const float fw2 = rljet_FoxWolfram2->at(i);
        const float fw20 = fabs(fw0) > 1.e-6 ? fw2 / fw0 : -1000.;

        hp->h_rljet_FoxWolfram0.at(i)->fill(fw0, weight);
        hp->h_rljet_FoxWolfram2.at(i)->fill(fw2, weight);
        hp->h_rljet_FoxWolfram20.at(i)->fill(fw20, weight);

        hp->h_rljet_Angularity.at(i)->fill(rljet_Angularity->at(i), weight);
        hp->h_rljet_Aplanarity.at(i)->fill(rljet_Aplanarity->at(i), weight);
        hp->h_rljet_Dip12.at(i)->fill(rljet_Dip12->at(i), weight);
        hp->h_rljet_KtDR.at(i)->fill(rljet_KtDR->at(i), weight);
        hp->h_rljet_Mu12.at(i)->fill(rljet_Mu12->at(i), weight);
        hp->h_rljet_PlanarFlow.at(i)->fill(rljet_PlanarFlow->at(i), weight);
        hp->h_rljet_Sphericity.at(i)->fill(rljet_Sphericity->at(i), weight);
        hp->h_rljet_Split12.at(i)->fill(rljet_Split12->at(i), weight);
        hp->h_rljet_Split34.at(i)->fill(rljet_Split34->at(i), weight);
        hp->h_rljet_ThrustMaj.at(i)->fill(rljet_ThrustMaj->at(i), weight);
        hp->h_rljet_ThrustMin.at(i)->fill(rljet_ThrustMin->at(i), weight);
        hp->h_rljet_ZCut12.at(i)->fill(rljet_ZCut12->at(i), weight);

        hp->h_rljet_NTrimSubjets.at(i)->fill(rljet_NTrimSubjets->at(i), weight);
        hp->h_rljet_ungroomed_ntrk500.at(i)->fill(rljet_ungroomed_ntrk500->at(i), weight);

        smooth_tag_map_nominal["smooth15WTag_50eff_JSSCut"    ] = rljet_smooth15WTag_50eff->at(i) == 1 || rljet_smooth15WTag_50eff->at(i) == 3;
        smooth_tag_map_nominal["smooth15WTag_50eff_MassJSSCut"] = rljet_smooth15WTag_50eff->at(i) == 3;
        smooth_tag_map_nominal["smooth15ZTag_50eff_JSSCut"    ] = rljet_smooth15ZTag_50eff->at(i) == 1 || rljet_smooth15ZTag_50eff->at(i) == 3;
        smooth_tag_map_nominal["smooth15ZTag_50eff_MassJSSCut"] = rljet_smooth15ZTag_50eff->at(i) == 3;
        smooth_tag_map_nominal["smooth15Top_MassTau32Tag50eff_MassJSSCut" ] = rljet_smooth15Top_MassTau32Tag50eff->at(i) == 3;
        smooth_tag_map_nominal["smooth15Top_MassTau32Tag80eff_MassJSSCut" ] = rljet_smooth15Top_MassTau32Tag80eff->at(i) == 3;

        for (const auto& itag : smooth_tag_map_nominal) {
            hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
            hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
            hp->h_rljet_m_calo.at(i)->fill_tagged(itag.first, rljet_m_calo->at(i)/1000., weight, itag.second);
            hp->h_rljet_pt_calo.at(i)->fill_tagged(itag.first, rljet_pt_calo->at(i)/1000., weight, itag.second);
        }

        std::string npv_bin_str;
        if (NPV < 10) {
            npv_bin_str = "NPVLT10";
        } else if (NPV >= 10 && NPV < 15) {
            npv_bin_str = "NPVGT10LT15";
        } else if (NPV >= 15 && NPV < 20) {
            npv_bin_str = "NPVGT15LT20";
        } else if (NPV >= 20) {
            npv_bin_str = "NPVGT20";
        }

        hp->h_rljet_m_comb.at(i)->fill_tagged(npv_bin_str, rljet_m_comb->at(i)/1000., weight, true);
        hp->h_rljet_pt_comb.at(i)->fill_tagged(npv_bin_str, rljet_pt_comb->at(i)/1000., weight, true);

        for (const auto& itag : smooth_tag_map) {
            std::string tmp_tag_str = itag.first + "_" + npv_bin_str;
            hp->h_rljet_m_comb.at(i)->fill_tagged(tmp_tag_str, rljet_m_comb->at(i)/1000., weight, itag.second);
            hp->h_rljet_pt_comb.at(i)->fill_tagged(tmp_tag_str, rljet_pt_comb->at(i)/1000., weight, itag.second);
        }

        /***************/
        /* MVA TAGGERS */
        /***************/

        /*
        const bool IS_GOOD_MVA_JET =
            rljet_m_calo->at(i) / 1000. > 40.
            && rljet_D2->at(i) > 0
            && C2 > 0
            && Tau32_wta > 0
            && Tau21_wta > 0
            && rljet_Tau1_wta->at(i) > 0
            && rljet_Tau2_wta->at(i) > 0
            && rljet_Tau3_wta->at(i) > 0
            && rljet_ECF1->at(i) > 0
            && rljet_ECF2->at(i) > 0
            && rljet_ECF3->at(i) > 0;

        if (IS_GOOD_MVA_JET) {
            // make an xAOD::Jet to pass to the BDT/DNN tools
            xAOD::Jet* mva_jet = new xAOD::Jet();
            mva_jet->makePrivateStore();

            mva_jet->setAttribute<float>("pt"  , rljet_pt_calo->at(i));
            mva_jet->setAttribute<float>("eta" , rljet_eta->at(i));
            mva_jet->setAttribute<float>("phi" , rljet_phi->at(i));
            mva_jet->setAttribute<float>("m"   , rljet_m_calo->at(i));

            mva_jet->setAttribute<float>("C2"           , C2);
            mva_jet->setAttribute<float>("Angularity"   , rljet_Angularity->at(i));
            mva_jet->setAttribute<float>("Aplanarity"   , rljet_Aplanarity->at(i));
            mva_jet->setAttribute<float>("D2"           , rljet_D2->at(i));
            mva_jet->setAttribute<float>("Dip12"        , rljet_Dip12->at(i));
            mva_jet->setAttribute<float>("ECF1"         , rljet_ECF1->at(i));
            mva_jet->setAttribute<float>("ECF2"         , rljet_ECF2->at(i));
            mva_jet->setAttribute<float>("ECF3"         , rljet_ECF3->at(i));
            mva_jet->setAttribute<float>("FoxWolfram0"  , rljet_FoxWolfram0->at(i));
            mva_jet->setAttribute<float>("FoxWolfram2"  , rljet_FoxWolfram2->at(i));
            mva_jet->setAttribute<float>("FoxWolfram20" , fw20);
            mva_jet->setAttribute<float>("KtDR"         , rljet_KtDR->at(i));
            mva_jet->setAttribute<float>("Mu12"         , rljet_Mu12->at(i));
            mva_jet->setAttribute<float>("PlanarFlow"   , rljet_PlanarFlow->at(i));
            mva_jet->setAttribute<float>("Qw"           , rljet_Qw->at(i));
            mva_jet->setAttribute<float>("Sphericity"   , rljet_Sphericity->at(i));
            mva_jet->setAttribute<float>("Split12"      , rljet_Split12->at(i));
            mva_jet->setAttribute<float>("Split23"      , rljet_Split23->at(i));
            mva_jet->setAttribute<float>("Split34"      , rljet_Split34->at(i));
            mva_jet->setAttribute<float>("Tau1_wta"     , rljet_Tau1_wta->at(i));
            mva_jet->setAttribute<float>("Tau21_wta"    , Tau21_wta);
            mva_jet->setAttribute<float>("Tau2_wta"     , rljet_Tau2_wta->at(i));
            mva_jet->setAttribute<float>("Tau32_wta"    , Tau32_wta);
            mva_jet->setAttribute<float>("Tau3_wta"     , rljet_Tau3_wta->at(i));
            mva_jet->setAttribute<float>("ThrustMaj"    , rljet_ThrustMaj->at(i));
            mva_jet->setAttribute<float>("ThrustMin"    , rljet_ThrustMin->at(i));
            mva_jet->setAttribute<float>("ZCut12"       , rljet_ZCut12->at(i));

            // std::cout << "C2:" << mva_jet->getAttribute<float>("C2") << std::endl;
            // std::cout << "Angularity:" << mva_jet->getAttribute<float>("Angularity") << std::endl;
            // std::cout << "Aplanarity:" << mva_jet->getAttribute<float>("Aplanarity") << std::endl;
            // std::cout << "D2:" << mva_jet->getAttribute<float>("D2") << std::endl;
            // std::cout << "Dip12:" << mva_jet->getAttribute<float>("Dip12") << std::endl;
            // std::cout << "ECF1:" << mva_jet->getAttribute<float>("ECF1") << std::endl;
            // std::cout << "ECF2:" << mva_jet->getAttribute<float>("ECF2") << std::endl;
            // std::cout << "ECF3:" << mva_jet->getAttribute<float>("ECF3") << std::endl;
            // std::cout << "FoxWolfram0:" << mva_jet->getAttribute<float>("FoxWolfram0") << std::endl;
            // std::cout << "FoxWolfram2:" << mva_jet->getAttribute<float>("FoxWolfram2") << std::endl;
            // std::cout << "FoxWolfram20:" << mva_jet->getAttribute<float>("FoxWolfram20") << std::endl;
            // std::cout << "KtDR:" << mva_jet->getAttribute<float>("KtDR") << std::endl;
            // std::cout << "Mu12:" << mva_jet->getAttribute<float>("Mu12") << std::endl;
            // std::cout << "PlanarFlow:" << mva_jet->getAttribute<float>("PlanarFlow") << std::endl;
            // std::cout << "Qw:" << mva_jet->getAttribute<float>("Qw") << std::endl;
            // std::cout << "Sphericity:" << mva_jet->getAttribute<float>("Sphericity") << std::endl;
            // std::cout << "Split12:" << mva_jet->getAttribute<float>("Split12") << std::endl;
            // std::cout << "Split23:" << mva_jet->getAttribute<float>("Split23") << std::endl;
            // std::cout << "Split34:" << mva_jet->getAttribute<float>("Split34") << std::endl;
            // std::cout << "Tau1_wta:" << mva_jet->getAttribute<float>("Tau1_wta") << std::endl;
            // std::cout << "Tau21_wta:" << mva_jet->getAttribute<float>("Tau21_wta") << std::endl;
            // std::cout << "Tau2_wta:" << mva_jet->getAttribute<float>("Tau2_wta") << std::endl;
            // std::cout << "Tau32_wta:" << mva_jet->getAttribute<float>("Tau32_wta") << std::endl;
            // std::cout << "Tau3_wta:" << mva_jet->getAttribute<float>("Tau3_wta") << std::endl;
            // std::cout << "ThrustMaj:" << mva_jet->getAttribute<float>("ThrustMaj") << std::endl;
            // std::cout << "ThrustMin:" << mva_jet->getAttribute<float>("ThrustMin") << std::endl;
            // std::cout << "ZCut12:" << mva_jet->getAttribute<float>("ZCut12") << std::endl;
            // std::cout << std::endl

			const bool CLEAN = rljet_Dip12->at(i) > 0;

            const int BDT_top_qqb_result = static_cast<int>(m_topTagger_BDT_qqb->result(*mva_jet));
            hp->h_rljet_BDT_score.at(i)->fill_tagged("top_qqb", m_topTagger_BDT_qqb->getBDTScore(), weight, true);
            hp->h_rljet_BDT_score.at(i)->fill_tagged("top_qqb_CLEAN", m_topTagger_BDT_qqb->getBDTScore(), weight, CLEAN);

            const int BDT_top_inclusive_result = static_cast<int>(m_topTagger_BDT_inclusive->result(*mva_jet));
            hp->h_rljet_BDT_score.at(i)->fill_tagged("top_inclusive", m_topTagger_BDT_inclusive->getBDTScore(), weight, true);
            hp->h_rljet_BDT_score.at(i)->fill_tagged("top_inclusive_CLEAN", m_topTagger_BDT_inclusive->getBDTScore(), weight, CLEAN);

            const int BDT_w_result = static_cast<int>(m_wTagger_BDT->result(*mva_jet));
            hp->h_rljet_BDT_score.at(i)->fill_tagged("w", m_wTagger_BDT->getBDTScore(), weight, true);
            hp->h_rljet_BDT_score.at(i)->fill_tagged("w_CLEAN", m_wTagger_BDT->getBDTScore(), weight, CLEAN);

            mva_tag_map["BDT_top_qqb_JSSCut"]           =  (BDT_top_qqb_result == 0 || BDT_top_qqb_result == 1);
            mva_tag_map["BDT_top_qqb_MassJSSCut"]       =  BDT_top_qqb_result == 0;
            mva_tag_map["BDT_top_inclusive_JSSCut"]     =  (BDT_top_inclusive_result == 0 || BDT_top_inclusive_result == 1);
            mva_tag_map["BDT_top_inclusive_MassJSSCut"] =  BDT_top_inclusive_result == 0;
            mva_tag_map["BDT_w_JSSCut"]                 =  (BDT_w_result == 0 || BDT_w_result == 1);
            mva_tag_map["BDT_w_MassJSSCut"]             =  BDT_w_result == 0;

            const float DNN_top_qqb_score = m_topTagger_DNN_qqb->getScore(*mva_jet);
            const float DNN_top_inclusive_score = m_topTagger_DNN_inclusive->getScore(*mva_jet);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("top_qqb"       , DNN_top_qqb_score       , weight , true);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("top_qqb_CLEAN"       , DNN_top_qqb_score       , weight , CLEAN);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("top_inclusive" , DNN_top_inclusive_score , weight , true);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("top_inclusive_CLEAN" , DNN_top_inclusive_score , weight , CLEAN);

            // const int DNN_top_qqb_result = static_cast<int>(m_topTagger_DNN_qqb->result(*mva_jet));
            // const int DNN_top_inclusive_result = static_cast<int>(m_topTagger_DNN_inclusive->result(*mva_jet));

            // mva_tag_map["DNN_top_qqb_MassJSSCut"]       = DNN_top_qqb_result == 1;
            // mva_tag_map["DNN_top_inclusive_MassJSSCut"] = DNN_top_inclusive_result == 1;

            for (const auto& itag : mva_tag_map) {
                hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
                hp->h_rljet_m_calo.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt_calo.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
            }

            delete mva_jet;
        }
        */

        // SD log(chi) variables
        if (ranSD) {
            hp->h_rljet_SDw_win20_btag0_logchi.at(i)->fill(rljet_SDw_win20_btag0->at(i), weight);
            hp->h_rljet_SDz_win20_btag0_logchi.at(i)->fill(rljet_SDz_win20_btag0->at(i), weight);
            hp->h_rljet_SDt_win50_btag0_logchi.at(i)->fill(rljet_SDt_win50_btag0->at(i), weight);

            const bool in_SD_W_mass_window   = abs( (rljet_m_comb->at(i) / 1000.) - 80.  ) < 20;
            const bool in_SD_Z_mass_window   = abs( (rljet_m_comb->at(i) / 1000.) - 91.  ) < 20;
            const bool in_SD_Top_mass_window = abs( (rljet_m_comb->at(i) / 1000.) - 172. ) < 50;

            SD_nominal_tag_map["SDw_win20_btag0"]    = rljet_SDw_win20_btag0->at(i) > 3.25;
            SD_nominal_tag_map["SDz_win20_btag0"]    = rljet_SDz_win20_btag0->at(i) > 3.25;
            SD_nominal_tag_map["SDt_win50_btag0"]    = rljet_SDt_win50_btag0->at(i) > 4.5;
            SD_nominal_tag_map["SDw_win20CUT_btag0"] = rljet_SDw_win20_btag0->at(i) > 3.25 && in_SD_W_mass_window;
            SD_nominal_tag_map["SDz_win20CUT_btag0"] = rljet_SDz_win20_btag0->at(i) > 3.25 && in_SD_Z_mass_window;
            SD_nominal_tag_map["SDt_win50CUT_btag0"] = rljet_SDt_win50_btag0->at(i) > 4.5 && in_SD_Top_mass_window;

            for (const auto& itag : SD_nominal_tag_map) {
                hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
            }

            if (this->operating_on_mc) {
                SD_systematic_tag_map["SDw_win20_btag0_UP"] = rljet_SDw_win20_btag0_UP->at(i) > 3;
                SD_systematic_tag_map["SDz_win20_btag0_UP"] = rljet_SDz_win20_btag0_UP->at(i) > 3;
                SD_systematic_tag_map["SDt_win50_btag0_UP"] = rljet_SDt_win50_btag0_UP->at(i) > 4.25;

                SD_systematic_tag_map["SDw_win20_btag0_DOWN"] = rljet_SDw_win20_btag0_DOWN->at(i) > 3;
                SD_systematic_tag_map["SDz_win20_btag0_DOWN"] = rljet_SDz_win20_btag0_DOWN->at(i) > 3;
                SD_systematic_tag_map["SDt_win50_btag0_DOWN"] = rljet_SDt_win50_btag0_DOWN->at(i) > 4.25;

                SD_systematic_tag_map["SDw_win20CUT_btag0_UP"] = rljet_SDw_win20_btag0_UP->at(i) > 3 && in_SD_W_mass_window;
                SD_systematic_tag_map["SDz_win20CUT_btag0_UP"] = rljet_SDz_win20_btag0_UP->at(i) > 3 && in_SD_Z_mass_window;
                SD_systematic_tag_map["SDt_win50CUT_btag0_UP"] = rljet_SDt_win50_btag0_UP->at(i) > 4.25 && in_SD_Top_mass_window;

                SD_systematic_tag_map["SDw_win20CUT_btag0_DOWN"] = rljet_SDw_win20_btag0_DOWN->at(i) > 3 && in_SD_W_mass_window;
                SD_systematic_tag_map["SDz_win20CUT_btag0_DOWN"] = rljet_SDz_win20_btag0_DOWN->at(i) > 3 && in_SD_Z_mass_window;
                SD_systematic_tag_map["SDt_win50CUT_btag0_DOWN"] = rljet_SDt_win50_btag0_DOWN->at(i) > 4.25 && in_SD_Top_mass_window;

                for (const auto& itag : SD_systematic_tag_map) {
                    hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
                    hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
                }
            } // end of saving systematic branch SD-tagged variables
        } // end of saving SD-tagged variables
    } // end of saving all anti-kt R = 1.0 distributions

    if (rljet_count >= 2) {
        hp->h_rljet_mjj->fill(rljet_mjj/1000., weight);
        hp->h_rljet_ptasym->fill(rljet_ptasym, weight);
        hp->h_rljet_dy->fill(rljet_dy, weight);
        hp->h_rljet_dR->fill(rljet_dR, weight);

        hp->h_rljet_deta->fill(rljet_deta, weight);
        hp->h_rljet_dphi->fill(rljet_dphi, weight);
    }

    /**************************/
    /* C/A 15 JETS (FROM HTT) */
    /**************************/

    const UInt_t n_cajets_recorded = htt_caJet_pt->size();
    if (n_cajets_recorded > 0 && htt_caJet_pt->at(0) / 1000. > 550.) {
        for (UInt_t ijet = 0; ijet < 3 && ijet < n_cajets_recorded; ijet++)
        {
            hp->h_htt_caJet_pt.at(ijet)->fill(htt_caJet_pt->at(ijet) / 1000., weight);
            hp->h_htt_caJet_eta.at(ijet)->fill(htt_caJet_eta->at(ijet), weight);
            hp->h_htt_caJet_phi.at(ijet)->fill(htt_caJet_phi->at(ijet), weight);
            hp->h_htt_caJet_m.at(ijet)->fill(htt_caJet_m->at(ijet) / 1000., weight);

            hp->h_htt_pt.at(ijet)->fill(htt_pt_def->at(ijet) / 1000. , weight);
            hp->h_htt_eta.at(ijet)->fill(htt_eta_def->at(ijet)        , weight);
            hp->h_htt_phi.at(ijet)->fill(htt_phi_def->at(ijet)        , weight);
            hp->h_htt_m.at(ijet)->fill(htt_m_def->at(ijet) / 1000.  , weight);
            hp->h_htt_atan1312.at(ijet)->fill(htt_atan1312_def->at(ijet) , weight);
            hp->h_htt_m23m123.at(ijet)->fill(htt_m23m123_def->at(ijet)  , weight);
            hp->h_htt_pts1.at(ijet)->fill(htt_pts1_def->at(ijet) / 1000., weight);
            hp->h_htt_pts2.at(ijet)->fill(htt_pts2_def->at(ijet) / 1000., weight);
            hp->h_htt_pts3.at(ijet)->fill(htt_pts3_def->at(ijet) / 1000., weight);
            hp->h_htt_caGroomJet_pt.at(ijet)->fill(htt_caGroomJet_pt_def->at(ijet) / 1000. , weight);
            hp->h_htt_caGroomJet_eta.at(ijet)->fill(htt_caGroomJet_eta_def->at(ijet)        , weight);
            hp->h_htt_caGroomJet_phi.at(ijet)->fill(htt_caGroomJet_phi_def->at(ijet)        , weight);
            hp->h_htt_caGroomJet_m.at(ijet)->fill(htt_caGroomJet_m_def->at(ijet) / 1000.  , weight);

            if (this->operating_on_mc) {
                hp->h_htt_pt.at(ijet)->fill_tagged("sjcalib0970", htt_pt_sjcalib0970->at(ijet) / 1000. , weight, true);
                hp->h_htt_eta.at(ijet)->fill_tagged("sjcalib0970", htt_eta_sjcalib0970->at(ijet)        , weight, true);
                hp->h_htt_phi.at(ijet)->fill_tagged("sjcalib0970", htt_phi_sjcalib0970->at(ijet)        , weight, true);
                hp->h_htt_m.at(ijet)->fill_tagged("sjcalib0970", htt_m_sjcalib0970->at(ijet) / 1000.  , weight, true);
                hp->h_htt_atan1312.at(ijet)->fill_tagged("sjcalib0970", htt_atan1312_sjcalib0970->at(ijet) , weight, true);
                hp->h_htt_m23m123.at(ijet)->fill_tagged("sjcalib0970", htt_m23m123_sjcalib0970->at(ijet)  , weight, true);
                hp->h_htt_pts1.at(ijet)->fill_tagged("sjcalib0970", htt_pts1_sjcalib0970->at(ijet) / 1000., weight, true);
                hp->h_htt_pts2.at(ijet)->fill_tagged("sjcalib0970", htt_pts2_sjcalib0970->at(ijet) / 1000., weight, true);
                hp->h_htt_pts3.at(ijet)->fill_tagged("sjcalib0970", htt_pts3_sjcalib0970->at(ijet) / 1000., weight, true);
                hp->h_htt_caGroomJet_pt.at(ijet)->fill_tagged("sjcalib0970", htt_caGroomJet_pt_sjcalib0970->at(ijet) / 1000. , weight, true);
                hp->h_htt_caGroomJet_eta.at(ijet)->fill_tagged("sjcalib0970", htt_caGroomJet_eta_sjcalib0970->at(ijet)        , weight, true);
                hp->h_htt_caGroomJet_phi.at(ijet)->fill_tagged("sjcalib0970", htt_caGroomJet_phi_sjcalib0970->at(ijet)        , weight, true);
                hp->h_htt_caGroomJet_m.at(ijet)->fill_tagged("sjcalib0970", htt_caGroomJet_m_sjcalib0970->at(ijet) / 1000.  , weight, true);

                hp->h_htt_pt.at(ijet)->fill_tagged("sjcalib1030", htt_pt_sjcalib1030->at(ijet) / 1000. , weight, true);
                hp->h_htt_eta.at(ijet)->fill_tagged("sjcalib1030", htt_eta_sjcalib1030->at(ijet)        , weight, true);
                hp->h_htt_phi.at(ijet)->fill_tagged("sjcalib1030", htt_phi_sjcalib1030->at(ijet)        , weight, true);
                hp->h_htt_m.at(ijet)->fill_tagged("sjcalib1030", htt_m_sjcalib1030->at(ijet) / 1000.  , weight, true);
                hp->h_htt_atan1312.at(ijet)->fill_tagged("sjcalib1030", htt_atan1312_sjcalib1030->at(ijet) , weight, true);
                hp->h_htt_m23m123.at(ijet)->fill_tagged("sjcalib1030", htt_m23m123_sjcalib1030->at(ijet)  , weight, true);
                hp->h_htt_pts1.at(ijet)->fill_tagged("sjcalib1030", htt_pts1_sjcalib1030->at(ijet) / 1000., weight, true);
                hp->h_htt_pts2.at(ijet)->fill_tagged("sjcalib1030", htt_pts2_sjcalib1030->at(ijet) / 1000., weight, true);
                hp->h_htt_pts3.at(ijet)->fill_tagged("sjcalib1030", htt_pts3_sjcalib1030->at(ijet) / 1000., weight, true);
                hp->h_htt_caGroomJet_pt.at(ijet)->fill_tagged("sjcalib1030", htt_caGroomJet_pt_sjcalib1030->at(ijet) / 1000. , weight, true);
                hp->h_htt_caGroomJet_eta.at(ijet)->fill_tagged("sjcalib1030", htt_caGroomJet_eta_sjcalib1030->at(ijet)        , weight, true);
                hp->h_htt_caGroomJet_phi.at(ijet)->fill_tagged("sjcalib1030", htt_caGroomJet_phi_sjcalib1030->at(ijet)        , weight, true);
                hp->h_htt_caGroomJet_m.at(ijet)->fill_tagged("sjcalib1030", htt_caGroomJet_m_sjcalib1030->at(ijet) / 1000.  , weight, true);
            }
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

    // we run over multiple branches for the same file when dealing
    // with systematics, so don't RECREATE the file, but UPDATE
    TFile* output_file = new TFile(this->output_filepath.c_str(), "UPDATE");

    // make root/sub directory if it doesn't exist, otherwise just cd into them
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

    // some histograms are only saved for the nominal branch
    hp->WriteCommonHistograms();
    if (sub_dir_str == "nominal")
        hp->WriteNominalOnlyHistograms();

    output_file->Close();
    delete output_file;

    ss.str(std::string());
    ss << "FINISHED: " << root_dir_str << "/" << sub_dir_str;

    this->log(ss.str());

    delete hp;
}


