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


// so that we can specify the selector by command line argument string
const std::unordered_map<std::string, EventSelector>
DataMCbackgroundSelector::available_event_selectors = {
    { "NO_SELECTION"   , &DataMCbackgroundSelector::NO_SELECTION   },
    { "GAMMAJET_TIGHT" , &DataMCbackgroundSelector::GAMMAJET_TIGHT },
    { "GAMMAJET_LOOSE" , &DataMCbackgroundSelector::GAMMAJET_LOOSE }
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

    std::string BDTpathPrefix = "$ROOTCOREBIN/data/BDTWTopTagger/";
    std::string DNNpathPrefix = "$ROOTCOREBIN/data/NNTaggingTools/";

    BDT_topTag = make_unique<BDTWTopTagger>(
            (BDTpathPrefix + std::string("TMVAClassification_BDTG.TOP3q_NvarM_pT350to2000GeV.01062017.weights.xml")).c_str(),
            tag::t,
            BDTpathPrefix + std::string("FitFunctions_BDT_top80.root"),
            "BDT_TOP3qtag_smooth_cut_BDT_TOP3qtag+BDT_TOP3qtag_80wp");

    // BDT W taggers
    BDT_WTag = make_unique<BDTWTopTagger>(
            (BDTpathPrefix + std::string("TMVAClassification_BDTG.W2q_NvarM_pT200to2000GeV.01062017.weights.xml")).c_str(),
            tag::W,
            BDTpathPrefix + std::string("FitFunctions_BDT_W50.root"),
            "BDT_W2qtag_smooth_cut_BDT_W2qtag+BDT_W2qtag_50wp");

    // DNN top tagger
    DNN_topTag = make_unique<DNNWTopTagger>(
            DNNpathPrefix + std::string("6JuneTopTaggingContained.json"),
            "top_dnn_BN_6june",
            DNNpathPrefix + std::string("FitFunctions_DNN_top80.root"),
            "top_contained_DNN_june6_smooth_cut_top_contained_DNN_june6+top_contained_DNN_june6_80wp");
    // DNN W tagger
    DNN_WTag = make_unique<DNNWTopTagger>(DNNpathPrefix + std::string("6JuneWTaggingContained.json"),
            "W_dnn_BN_june6",
            DNNpathPrefix + std::string("FitFunctions_DNN_W50.root"),
            "W_contained_DNN_june6_smooth_cut_W_contained_DNN_june6+W_contained_DNN_june6_50wp");

    if (this->on_nominal_branch) {
      this->h_bdt_top_vs_mu = new TH2F("h_bdt_top_vs_mu" , "h_bdt_top_vs_mu" , 60 , 0 , 60 , 200 , -1.0 , 1.0);
      this->h_bdt_w_vs_mu   = new TH2F("h_bdt_w_vs_mu"   , "h_bdt_w_vs_mu"   , 60 , 0 , 60 , 200 , -1.0 , 1.0);
      this->h_dnn_top_vs_mu = new TH2F("h_dnn_top_vs_mu" , "h_dnn_top_vs_mu" , 60 , 0 , 60 , 200 , 0.0  , 1.0);
      this->h_dnn_w_vs_mu   = new TH2F("h_dnn_w_vs_mu"   , "h_dnn_w_vs_mu"   , 60 , 0 , 60 , 200 , 0.0  , 1.0);

      h_bdt_top_vs_mu->Sumw2();
      h_bdt_w_vs_mu->Sumw2();
      h_dnn_top_vs_mu->Sumw2();
      h_dnn_w_vs_mu->Sumw2();
    }

    const std::string rc = std::string(getenv("ROOTCOREBIN"));
    std::string f_sdw_filepath = rc + "/data/DataMCbackgroundTools/SDW.root";
    std::string f_sdtop_filepath = rc + "/data/DataMCbackgroundTools/SDTop.root";
    TFile* f_sdw_file = TFile::Open(f_sdw_filepath.c_str(), "READ");
    TFile* f_sdtop_file = TFile::Open(f_sdtop_filepath.c_str(), "READ");

    this->f_sdw = (TF1*) f_sdw_file->Get("fjet_SDw_win20_btag1_smooth_cut_fjet_SDw_win20_btag1+fjet_SDw_win20_btag1_50wp");
    this->f_sdtop =(TF1*) f_sdtop_file->Get("fjet_SDt_Dcut1_smooth_cut_fjet_SDt_Dcut1+fjet_SDt_Dcut1_80wp");
}

void DataMCbackgroundSelector::SlaveBegin(TTree * /*tree*/)
{
    // The SlaveBegin() function is called after the Begin() function.
    // When running with PROOF SlaveBegin() is called on each slave server.
    // The tree argument is deprecated (on PROOF 0 is passed).
    TString option = GetOption();

    this->hp = new HistoPack(2);
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

    if (keptPhotons) {
      b_weight_photonSF->GetEntry(entry);
      if (sub_dir_str.find("photon") != std::string::npos) {
        b_weight_photonSF_ID_UP->GetEntry(entry);
        b_weight_photonSF_ID_DOWN->GetEntry(entry);
        b_weight_photonSF_effTrkIso_UP->GetEntry(entry);
        b_weight_photonSF_effTrkIso_DOWN->GetEntry(entry);
      }
    }
  } else {
    b_HLT_trigger->GetEntry(entry);
  }

    b_eventNumber->GetEntry(entry);
    b_runNumber->GetEntry(entry);
    b_mcChannelNumber->GetEntry(entry);
    b_mu->GetEntry(entry);
    b_backgroundFlags->GetEntry(entry);
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

    b_rljet_topTag_BDT_qqb->GetEntry(entry);
    b_rljet_topTag_BDT_qqb_score->GetEntry(entry);
    b_rljet_wTag_BDT_qq->GetEntry(entry);
    b_rljet_wTag_BDT_qq_score->GetEntry(entry);
    b_rljet_topTag_DNN_qqb_score->GetEntry(entry);
    b_rljet_topTag_DNN_qqb->GetEntry(entry);
    b_rljet_wTag_DNN_qq_score->GetEntry(entry);
    b_rljet_wTag_DNN_qq->GetEntry(entry);
    b_rljet_topTag_TopoTagger_20wp->GetEntry(entry);
    b_rljet_topTag_TopoTagger_50wp->GetEntry(entry);
    b_rljet_topTag_TopoTagger_80wp->GetEntry(entry);
    b_rljet_topTag_TopoTagger_score->GetEntry(entry);

    // b_rljet_smooth16Top_MassTau32Tag50eff_nocontain->GetEntry(entry);
    // b_rljet_smooth16Top_MassTau32Tag80eff_nocontain->GetEntry(entry);
    // b_rljet_smooth16WTag_50eff_nocontain->GetEntry(entry);
    // b_rljet_smooth16WTag_80eff_nocontain->GetEntry(entry);
    // b_rljet_smooth16ZTag_50eff_nocontain->GetEntry(entry);
    // b_rljet_smooth16ZTag_80eff_nocontain->GetEntry(entry);

    if (this->on_nominal_branch) {
        b_NPV->GetEntry(entry);

        if (this->operating_on_mc) {
          b_rljet_pdgid->GetEntry(entry);
        }

        // 2017 MVA
        // b_rljet_BDT_score_top_qqb->GetEntry(entry);
        // b_rljet_BDT_score_top_inclusive->GetEntry(entry);
        // b_rljet_BDT_score_w->GetEntry(entry);
        // b_rljet_DNN_score_top->GetEntry(entry);
        // b_rljet_DNN_score_w->GetEntry(entry);

        if (keptPhotons) {
          //   b_photon_ptcone20->GetEntry(entry);
          //   b_photon_ptcone40->GetEntry(entry);
          //   b_photon_topoetcone20->GetEntry(entry);
          //   b_photon_topoetcone40->GetEntry(entry);
          if (default_photon_vars) {
            b_ph_pt->GetEntry(entry);
            b_ph_phi->GetEntry(entry);
            b_ph_eta->GetEntry(entry);
            b_ph_e->GetEntry(entry);
          } else {
            b_photon0_phi->GetEntry(entry);
          }
        }
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
        // b_m_rljet_smooth15Top_MassTau32Tag50eff->GetEntry(entry);
        // b_m_rljet_smooth15Top_MassTau32Tag80eff->GetEntry(entry);
        // b_rljet_smooth15WTag_50eff->GetEntry(entry);
        // b_rljet_smooth15WTag_25eff->GetEntry(entry);
        // b_rljet_smooth15ZTag_50eff->GetEntry(entry);
        // b_rljet_smooth15ZTag_25eff->GetEntry(entry);
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
        b_rljet_n_constituents->GetEntry(entry);
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

        if (ranSD) {
          b_rljet_SDw_calib->GetEntry(entry);
          b_rljet_SDw_uncalib->GetEntry(entry);
          b_rljet_SDw_combined->GetEntry(entry);
          b_rljet_SDw_dcut->GetEntry(entry);
          b_rljet_SDt_calib->GetEntry(entry);
          b_rljet_SDt_uncalib->GetEntry(entry);
          b_rljet_SDt_combined->GetEntry(entry);
          b_rljet_SDt_dcut->GetEntry(entry);
        }

        if (this->operating_on_mc) {

          if (ranSD) {
            b_rljet_SDw_calib_DOWN->GetEntry(entry);
            b_rljet_SDw_uncalib_DOWN->GetEntry(entry);
            b_rljet_SDw_combined_DOWN->GetEntry(entry);
            b_rljet_SDw_dcut_DOWN->GetEntry(entry);
            b_rljet_SDt_calib_DOWN->GetEntry(entry);
            b_rljet_SDt_uncalib_DOWN->GetEntry(entry);
            b_rljet_SDt_combined_DOWN->GetEntry(entry);
            b_rljet_SDt_dcut_DOWN->GetEntry(entry);
            b_rljet_SDw_calib_UP->GetEntry(entry);
            b_rljet_SDw_uncalib_UP->GetEntry(entry);
            b_rljet_SDw_combined_UP->GetEntry(entry);
            b_rljet_SDw_dcut_UP->GetEntry(entry);
            b_rljet_SDt_calib_UP->GetEntry(entry);
            b_rljet_SDt_uncalib_UP->GetEntry(entry);
            b_rljet_SDt_combined_UP->GetEntry(entry);
            b_rljet_SDt_dcut_UP->GetEntry(entry);
          }

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
    if (!this->operating_on_mc && HLT_trigger == 0)
        return kFALSE;

    // and all events must pass the specified offline cuts
    if (!(this->*chosen_event_selector)())
        return kFALSE;

    float weight;
    if (this->operating_on_mc) {
        weight = weight_mc
          * weight_pileup
          * this->SF_lumi_Fb
          * this->luminosity;

        if (keptPhotons) {
          if (this->on_nominal_branch) {
            weight = weight * weight_photonSF;
          } else {
            if (sub_dir_str.find("photonSF_ID_UP") != std::string::npos)
                weight = weight * weight_photonSF_ID_UP;
            if (sub_dir_str.find("photonSF_ID_DOWN") != std::string::npos)
                weight = weight * weight_photonSF_ID_DOWN;
            if (sub_dir_str.find("photonSF_effTrkIso_UP") != std::string::npos)
                weight = weight * weight_photonSF * weight_photonSF_effTrkIso_UP;
            if (sub_dir_str.find("photonSF_effTrkIso_DOWN") != std::string::npos)
                weight = weight * weight_photonSF * weight_photonSF_effTrkIso_DOWN;
          }
        }

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

        hp->h_rljet_Tau32_wta.at(i)->fill_tagged("_combMlt100GeV"      , rljet_Tau32_wta->at(i) , weight , rljet_m_comb->at(i) / 1000. < 100.);
        hp->h_rljet_Tau32_wta.at(i)->fill_tagged("_combMgt100lt150GeV" , rljet_Tau32_wta->at(i) , weight , rljet_m_comb->at(i) / 1000. >= 100. && rljet_m_comb->at(i) / 1000. < 150.);
        hp->h_rljet_Tau32_wta.at(i)->fill_tagged("_combMgt150lt180GeV" , rljet_Tau32_wta->at(i) , weight , rljet_m_comb->at(i) / 1000. >= 150. && rljet_m_comb->at(i) / 1000. < 180.);
        hp->h_rljet_Tau32_wta.at(i)->fill_tagged("_combMgt180GeV"      , rljet_Tau32_wta->at(i) , weight , rljet_m_comb->at(i) / 1000. >= 180.);

        hp->h_rljet_Qw.at(i)->fill(rljet_Qw->at(i)/1000., weight);
        hp->h_rljet_Qw.at(i)->fill_tagged("_combMgt100GeV", rljet_Qw->at(i)/1000., weight, rljet_m_comb->at(i) / 1000. > 100.);

        hp->h_rljet_Split23.at(i)->fill(rljet_Split23->at(i)/1000., weight);
        hp->h_rljet_Split23.at(i)->fill_tagged("_combMgt100GeV", rljet_Split23->at(i)/1000., weight, rljet_m_comb->at(i) / 1000. > 100.);

        // MVA Taggers
        hp->h_rljet_topTag_BDT_qqb.at(i)->fill(rljet_topTag_BDT_qqb->at(i), weight);
        hp->h_rljet_topTag_BDT_qqb_score.at(i)->fill(rljet_topTag_BDT_qqb_score->at(i), weight);
        hp->h_rljet_wTag_BDT_qq.at(i)->fill(rljet_wTag_BDT_qq->at(i), weight);
        hp->h_rljet_wTag_BDT_qq_score.at(i)->fill(rljet_wTag_BDT_qq_score->at(i), weight);
        hp->h_rljet_topTag_DNN_qqb_score.at(i)->fill(rljet_topTag_DNN_qqb_score->at(i), weight);
        hp->h_rljet_topTag_DNN_qqb.at(i)->fill(rljet_topTag_DNN_qqb->at(i), weight);
        hp->h_rljet_wTag_DNN_qq_score.at(i)->fill(rljet_wTag_DNN_qq_score->at(i), weight);
        hp->h_rljet_wTag_DNN_qq.at(i)->fill(rljet_wTag_DNN_qq->at(i), weight);
        hp->h_rljet_topTag_TopoTagger_20wp.at(i)->fill(rljet_topTag_TopoTagger_20wp->at(i), weight);
        hp->h_rljet_topTag_TopoTagger_50wp.at(i)->fill(rljet_topTag_TopoTagger_50wp->at(i), weight);
        hp->h_rljet_topTag_TopoTagger_80wp.at(i)->fill(rljet_topTag_TopoTagger_80wp->at(i), weight);
        hp->h_rljet_topTag_TopoTagger_score.at(i)->fill(rljet_topTag_TopoTagger_score->at(i), weight);

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

        // smooth_tag_map["smooth16WTag_50eff_nocontain_JSSCut"] = rljet_smooth16WTag_50eff_nocontain->at(i) == 1 || rljet_smooth16WTag_50eff_nocontain->at(i) == 16 || rljet_smooth16WTag_50eff_nocontain->at(i) == 4;
        // smooth_tag_map["smooth16WTag_50eff_nocontain_MassJSSCut"] = rljet_smooth16WTag_50eff_nocontain->at(i) == 1;
        // smooth_tag_map["smooth16WTag_80eff_nocontain_JSSCut"] = rljet_smooth16WTag_80eff_nocontain->at(i) == 1 || rljet_smooth16WTag_80eff_nocontain->at(i) == 16 || rljet_smooth16WTag_80eff_nocontain->at(i) == 4;
        // smooth_tag_map["smooth16WTag_80eff_nocontain_MassJSSCut"] = rljet_smooth16WTag_80eff_nocontain->at(i) == 1;
        // smooth_tag_map["smooth16ZTag_50eff_nocontain_JSSCut"] = rljet_smooth16ZTag_50eff_nocontain->at(i) == 1 || rljet_smooth16ZTag_50eff_nocontain->at(i) == 16 || rljet_smooth16ZTag_50eff_nocontain->at(i) == 4;
        // smooth_tag_map["smooth16ZTag_50eff_nocontain_MassJSSCut"] = rljet_smooth16ZTag_50eff_nocontain->at(i) == 1;
        // smooth_tag_map["smooth16ZTag_80eff_nocontain_JSSCut"] = rljet_smooth16ZTag_80eff_nocontain->at(i) == 1 || rljet_smooth16ZTag_80eff_nocontain->at(i) == 16 || rljet_smooth16ZTag_80eff_nocontain->at(i) == 4;
        // smooth_tag_map["smooth16ZTag_80eff_nocontain_MassJSSCut"] = rljet_smooth16ZTag_80eff_nocontain->at(i) == 1;

        // smooth_tag_map["smooth16Top_MassTau32Tag50eff_nocontain_JSSCut"] = rljet_smooth16Top_MassTau32Tag50eff_nocontain->at(i) == 3 || rljet_smooth16Top_MassTau32Tag50eff_nocontain->at(i) == 2;
        // smooth_tag_map["smooth16Top_MassTau32Tag50eff_nocontain_MassJSSCut"] = rljet_smooth16Top_MassTau32Tag50eff_nocontain->at(i) == 3;
        // smooth_tag_map["smooth16Top_MassTau32Tag80eff_nocontain_JSSCut"] = rljet_smooth16Top_MassTau32Tag80eff_nocontain->at(i) == 3 || rljet_smooth16Top_MassTau32Tag80eff_nocontain->at(i) == 2;
        // smooth_tag_map["smooth16Top_MassTau32Tag80eff_nocontain_MassJSSCut"] = rljet_smooth16Top_MassTau32Tag80eff_nocontain->at(i) == 3;



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

    hp->h_NPV->fill( (Float_t) NPV, weight);

    if (!this->operating_on_mc) {
        hp->h_mu->fill(mu              , weight);
        hp->h_mu->fill_tagged("corrSF" , mu * 1./1.09 , weight , true);
    } else {
        // note: we don't actually correct the mc mu distribution, making the below tagged
        // histograms just makes it easier to do data/mc comparison plots later
        hp->h_mu->fill(mu              , weight);
        hp->h_mu->fill_tagged("corrSF" , mu       , weight , true);
    }

    /*******************/
    /* ANTI-KT 10 JETS */
    /*******************/

    for (UInt_t i = 0; i < n_rljets_recorded; i++) {
        hp->h_rljet_pt_calo.at(i)->fill(rljet_pt_calo->at(i)/1000., weight);
        hp->h_rljet_m_calo.at(i)->fill(rljet_m_calo->at(i)/1000., weight);
        hp->h_rljet_pt_ta.at(i)->fill(rljet_pt_ta->at(i)/1000., weight);
        hp->h_rljet_m_ta.at(i)->fill(rljet_m_ta->at(i)/1000., weight);

        hp->h_rljet_D2.at(i)->fill_tagged("_NTrimSubjetsEQ1", rljet_D2->at(i), weight, rljet_NTrimSubjets->at(i) > 4);
        hp->h_rljet_D2.at(i)->fill_tagged("_NTrimSubjetsGT1", rljet_D2->at(i), weight, rljet_NTrimSubjets->at(i) > 4);
        hp->h_rljet_D2.at(i)->fill_tagged("_NTrimSubjetsGT2", rljet_D2->at(i), weight, rljet_NTrimSubjets->at(i) > 4);
        hp->h_rljet_D2.at(i)->fill_tagged("_NTrimSubjetsGT3", rljet_D2->at(i), weight, rljet_NTrimSubjets->at(i) > 4);
        hp->h_rljet_D2.at(i)->fill_tagged("_NTrimSubjetsGT4", rljet_D2->at(i), weight, rljet_NTrimSubjets->at(i) > 4);

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

        if (this->operating_on_mc) {
          hp->h_rljet_m_comb.at(i)->fill_tagged("lightquark_b", rljet_m_comb->at(i)/1000., weight, TMath::Abs(rljet_pdgid->at(i)) < 6);
          hp->h_rljet_pt_comb.at(i)->fill_tagged("lightquark_b", rljet_pt_comb->at(i)/1000., weight, TMath::Abs(rljet_pdgid->at(i)) < 6);
          hp->h_rljet_m_comb.at(i)->fill_tagged("lightquark", rljet_m_comb->at(i)/1000., weight, TMath::Abs(rljet_pdgid->at(i)) < 3);
          hp->h_rljet_pt_comb.at(i)->fill_tagged("lightquark", rljet_pt_comb->at(i)/1000., weight, TMath::Abs(rljet_pdgid->at(i)) < 3);
          hp->h_rljet_m_comb.at(i)->fill_tagged("gluon", rljet_m_comb->at(i)/1000., weight, rljet_pdgid->at(i) == 21);
          hp->h_rljet_pt_comb.at(i)->fill_tagged("gluon", rljet_pt_comb->at(i)/1000., weight, rljet_pdgid->at(i) == 21);
        }

        /***************/
        /* MVA TAGGERS */
        /***************/

        if (rljet_n_constituents->at(i) > 2 && rljet_m_comb->at(i)/1000. > 40) {
            jetMoments["Tau1_wta"]                       = rljet_Tau1_wta->at(i);
            jetMoments["Tau2_wta"]                       = rljet_Tau2_wta->at(i);
            jetMoments["Tau3_wta"]                       = rljet_Tau3_wta->at(i);
            jetMoments["Tau21_wta"]                      = Tau21_wta;
            jetMoments["Tau32_wta"]                      = rljet_Tau32_wta->at(i);
            jetMoments["Split12"]                        = rljet_Split12->at(i);
            jetMoments["Split23"]                        = rljet_Split23->at(i);
            jetMoments["C2"]                             = C2;
            jetMoments["D2"]                             = rljet_D2->at(i);
            jetMoments["ECF1"]                           = rljet_ECF1->at(i);
            jetMoments["ECF2"]                           = rljet_ECF2->at(i);
            jetMoments["ECF3"]                           = rljet_ECF3->at(i);
            jetMoments["e3"]                             = ECF3 / TMath::Power(ECF1, 3);
            jetMoments["Qw"]                             = rljet_Qw->at(i);
            jetMoments["KtDR"]                           = rljet_KtDR->at(i);
            jetMoments["Angularity"]                     = rljet_Angularity->at(i);
            jetMoments["Aplanarity"]                     = rljet_Aplanarity->at(i);
            jetMoments["PlanarFlow"]                     = rljet_PlanarFlow->at(i);
            jetMoments["Dip12"]                          = rljet_Dip12->at(i);
            jetMoments["ZCut12"]                         = rljet_ZCut12->at(i);
            jetMoments["ThrustMaj"]                      = rljet_ThrustMaj->at(i);
            jetMoments["ThrustMin"]                      = rljet_ThrustMin->at(i);
            jetMoments["Sphericity"]                     = rljet_Sphericity->at(i);
            jetMoments["FoxWolfram20"]                   = fw20;
            jetMoments["CaloTACombinedMassUncorrelated"] = rljet_m_comb->at(i);
            jetMoments["JetpTCorrByCombinedMass"]        = rljet_pt_comb->at(i);

            const float bdt_top = BDT_topTag->getMVAdiscriminant(jetMoments);
            const float bdt_w   = BDT_WTag->getMVAdiscriminant(jetMoments);
            const float dnn_top = DNN_topTag->GetDiscriminant(jetMoments);
            const float dnn_w   = DNN_WTag->GetDiscriminant(jetMoments);

            hp->h_rljet_BDT_score.at(i)->fill_tagged("top" , bdt_top , weight , true);
            hp->h_rljet_BDT_score.at(i)->fill_tagged("w"   , bdt_w   , weight , true);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("top" , dnn_top , weight , true);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("w"   , dnn_w   , weight , true);

            hp->h_rljet_BDT_score.at(i)->fill_tagged("top_combMgt100GeV" , bdt_top , weight , rljet_m_comb->at(i)/1000. > 100);
            hp->h_rljet_BDT_score.at(i)->fill_tagged("w_combMgt50GeV"    , bdt_w   , weight , rljet_m_comb->at(i)/1000. > 50);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("top_combMgt100GeV" , dnn_top , weight , rljet_m_comb->at(i)/1000. > 100);
            hp->h_rljet_DNN_score.at(i)->fill_tagged("w_combMgt50GeV"    , dnn_w   , weight , rljet_m_comb->at(i)/1000. > 50);

            if (!this->operating_on_mc) {
              h_bdt_top_vs_mu->Fill(mu/1.09 , bdt_top);
              h_bdt_w_vs_mu->Fill(mu/1.09   , bdt_w);
              h_dnn_top_vs_mu->Fill(mu/1.09 , dnn_top);
              h_dnn_w_vs_mu->Fill(mu/1.09   , dnn_w);
            } else {
              h_bdt_top_vs_mu->Fill(mu , bdt_top);
              h_bdt_w_vs_mu->Fill(mu   , bdt_w);
              h_dnn_top_vs_mu->Fill(mu , dnn_top);
              h_dnn_w_vs_mu->Fill(mu   , dnn_w);
            }

            mva_tag_map["BDT_Top"] = BDT_topTag->isTagged(jetMoments);
            mva_tag_map["BDT_W"]   = BDT_WTag->isTagged(jetMoments);
            mva_tag_map["DNN_Top"] = DNN_topTag->isTagged(jetMoments);
            mva_tag_map["DNN_W"]   = DNN_WTag->isTagged(jetMoments);

            for (const auto& itag : mva_tag_map) {
              hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
              hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
            }
        }

        // SD log(chi) variables
        if (ranSD) {
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_calib", rljet_SDw_calib->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_uncalib", rljet_SDw_uncalib->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_combined", rljet_SDw_combined->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_dcut", rljet_SDw_dcut->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_calib", rljet_SDt_calib->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_uncalib", rljet_SDt_uncalib->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_combined", rljet_SDt_combined->at(i), weight, true);
            hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_dcut", rljet_SDt_dcut->at(i), weight, true);

            SD_nominal_tag_map["SDw_dcut"]     = rljet_SDw_dcut->at(i) > f_sdw->Eval(rljet_pt_comb->at(i)/1000.);
            SD_nominal_tag_map["SDt_dcut"]     = rljet_SDt_dcut->at(i) > f_sdtop->Eval(rljet_pt_comb->at(i)/1000.);

            for (const auto& itag : SD_nominal_tag_map) {
                hp->h_rljet_m_comb.at(i)->fill_tagged(itag.first, rljet_m_comb->at(i)/1000., weight, itag.second);
                hp->h_rljet_pt_comb.at(i)->fill_tagged(itag.first, rljet_pt_comb->at(i)/1000., weight, itag.second);
            }

            if (this->operating_on_mc) {
                SD_systematic_tag_map["SDw_dcut_UP"]     = rljet_SDw_dcut_UP->at(i) > f_sdw->Eval(rljet_pt_comb->at(i)/1000.);
                SD_systematic_tag_map["SDt_dcut_UP"]     = rljet_SDt_dcut_UP->at(i) > f_sdtop->Eval(rljet_pt_comb->at(i)/1000.);
                SD_systematic_tag_map["SDw_dcut_DOWN"]     = rljet_SDw_dcut_DOWN->at(i) > f_sdw->Eval(rljet_pt_comb->at(i)/1000.);
                SD_systematic_tag_map["SDt_dcut_DOWN"]     = rljet_SDt_dcut_DOWN->at(i) > f_sdtop->Eval(rljet_pt_comb->at(i)/1000.);

                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_calib_DOWN", rljet_SDw_calib_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_uncalib_DOWN", rljet_SDw_uncalib_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_combined_DOWN", rljet_SDw_combined_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_dcut_DOWN", rljet_SDw_dcut_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_calib_DOWN", rljet_SDt_calib_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_uncalib_DOWN", rljet_SDt_uncalib_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_combined_DOWN", rljet_SDt_combined_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_dcut_DOWN", rljet_SDt_dcut_DOWN->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_calib_UP", rljet_SDw_calib_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_uncalib_UP", rljet_SDw_uncalib_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_combined_UP", rljet_SDw_combined_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("w_dcut_UP", rljet_SDw_dcut_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_calib_UP", rljet_SDt_calib_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_uncalib_UP", rljet_SDt_uncalib_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_combined_UP", rljet_SDt_combined_UP->at(i), weight, true);
                hp->h_rljet_SD_logchi.at(i)->fill_tagged("t_dcut_UP", rljet_SDt_dcut_UP->at(i), weight, true);

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

    // if (keptPhotons && ph_pt->size() >= 1 && ph_pt->at(0) > 1e-6) {
    //   hp->h_topoetcone40_over_pt->fill((photon_topoetcone40 - 2.45e6)/ph_pt->at(0), weight);
    //   hp->h_topoetcone20_over_pt->fill(photon_topoetcone20/ph_pt->at(0), weight);
    //   hp->h_ptcone40_over_pt->fill(photon_ptcone40/ph_pt->at(0), weight);
    //   hp->h_ptcone20_over_pt->fill(photon_ptcone20/ph_pt->at(0), weight);
    // }

    /**************************/
    /* C/A 15 JETS (FROM HTT) */
    /**************************/

    UInt_t n_cajets_recorded = htt_caJet_pt->size();
    std::vector<UInt_t> ca_jet_indicies;

    if (keptPhotons) {
      Float_t tmp_phi = default_photon_vars ? ph_phi->at(0) : photon0_phi;
      for (UInt_t ijet = 0; ijet < n_cajets_recorded; ijet++) {
        if ( fabs(htt_caJet_phi->at(ijet) - tmp_phi) > M_PI / 2 )
          ca_jet_indicies.push_back(ijet);
      }
    } else {
      for (UInt_t i = 0; i < n_cajets_recorded; i++)
        ca_jet_indicies.push_back(i);
    }

    if (n_cajets_recorded > 0) {
        for (const UInt_t ijet : ca_jet_indicies)
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

            const bool is_htt_tagged = htt_tag_def->at(ijet) > 0
              && htt_m_def->at(ijet)/1000. >= 140
              && htt_m_def->at(ijet)/1000. <= 210;

            hp->h_htt_caGroomJet_pt.at(ijet)->fill_tagged ("HTT_CAND", htt_caGroomJet_pt_def->at(ijet) / 1000. , weight, is_htt_tagged);
            hp->h_htt_caGroomJet_eta.at(ijet)->fill_tagged("HTT_CAND", htt_caGroomJet_eta_def->at(ijet)        , weight, is_htt_tagged);
            hp->h_htt_caGroomJet_phi.at(ijet)->fill_tagged("HTT_CAND", htt_caGroomJet_phi_def->at(ijet)        , weight, is_htt_tagged);
            hp->h_htt_caGroomJet_m.at(ijet)->fill_tagged  ("HTT_CAND", htt_caGroomJet_m_def->at(ijet) / 1000.  , weight, is_htt_tagged);

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

                const bool is_htt_tagged_sjcalib0970 =
                  htt_tag_sjcalib0970->at(ijet) > 0
                  && htt_m_sjcalib0970->at(ijet)/1000. >= 140
                  && htt_m_sjcalib0970->at(ijet)/1000. <= 210;

                const bool is_htt_tagged_sjcalib1030 =
                  htt_tag_sjcalib1030->at(ijet) > 0
                  && htt_m_sjcalib1030->at(ijet)/1000. >= 140
                  && htt_m_sjcalib1030->at(ijet)/1000. <= 210;

                hp->h_htt_caGroomJet_pt.at(ijet)->fill_tagged ("HTT_CAND_sjcalib0970", htt_caGroomJet_pt_sjcalib0970->at(ijet) / 1000. , weight, is_htt_tagged_sjcalib0970);
                hp->h_htt_caGroomJet_m.at(ijet)->fill_tagged  ("HTT_CAND_sjcalib0970", htt_caGroomJet_m_sjcalib0970->at(ijet) / 1000.  , weight, is_htt_tagged_sjcalib0970);

                hp->h_htt_caGroomJet_pt.at(ijet)->fill_tagged ("HTT_CAND_sjcalib1030", htt_caGroomJet_pt_sjcalib1030->at(ijet) / 1000. , weight, is_htt_tagged_sjcalib1030);
                hp->h_htt_caGroomJet_m.at(ijet)->fill_tagged  ("HTT_CAND_sjcalib1030", htt_caGroomJet_m_sjcalib1030->at(ijet) / 1000.  , weight, is_htt_tagged_sjcalib1030);
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
    if (sub_dir_str == "nominal") {
        hp->WriteNominalOnlyHistograms();
        h_bdt_top_vs_mu->Write();
        h_bdt_w_vs_mu->Write();
        h_dnn_top_vs_mu->Write();
        h_dnn_w_vs_mu->Write();
    }

    output_file->Close();
    delete output_file;

    ss.str(std::string());
    ss << "FINISHED: " << root_dir_str << "/" << sub_dir_str;

    this->log(ss.str());

    delete hp;
}


