#include "DataMCbackgroundTools/LocalTools.h"

#include "TLorentzVector.h"
#include "TopEvent/EventTools.h"
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "TKey.h"

BinManager::BinManager(const std::vector<float>& bin_boundaries_)
    : bin_boundaries(bin_boundaries_) { }

int BinManager::get_bin_index(const float& var_value) const
{
    int bin_index = -1;

    // below min bin boundary
    if (var_value < this->bin_boundaries[0])
        return 0;

    // above maximum bin boundary
    if (var_value >= this->bin_boundaries.back())
        return this->bin_boundaries.size();

    // in one of the middle bins
    bool over_bin_min;
    bool under_bin_max;
    for (unsigned i = 1; i < this->bin_boundaries.size(); i++) {
        over_bin_min = var_value >= this->bin_boundaries[i-1];
        under_bin_max = var_value < this->bin_boundaries[i];
        if (over_bin_min && under_bin_max)
            return (int)i;
    }

    return bin_index;
}

const xAOD::Jet*
get_nearest_jet_in_collection(const xAOD::Jet* ref_jet, const xAOD::JetContainer* collection)
{

    if (collection == nullptr) return nullptr;
    if (collection->size() == 0) return nullptr;

    auto closest_jet_iter = std::min_element(collection->begin(), collection->end(),
            [&ref_jet](const xAOD::Jet* j1, const xAOD::Jet* j2) {
                return top::deltaR(*ref_jet, *j1) < top::deltaR(*ref_jet, *j2);
            });

    return *closest_jet_iter;
}

float get_sum_weights_sample(TFile* input_file)
{
    float sum_weights_total = 0.;
    float tmp = 0.;

    TTree *sum_weights_tree = (TTree*) input_file->Get("sumWeights");
    sum_weights_tree->SetBranchAddress("totalEventsWeighted", &tmp);

    for(int ientry = 0; ientry < sum_weights_tree->GetEntries(); ++ientry) {
        sum_weights_tree->GetEntry(ientry);
        sum_weights_total += tmp;
    }

    delete sum_weights_tree;
    return sum_weights_total;
}

ULong64_t getTotalEventsSample(TFile* inputFile)
{
    ULong64_t sumWTotal=0.;
    ULong64_t sumW=0.;
    TTree *sumWeights = (TTree*) inputFile->Get("sumWeights");
    sumWeights->SetBranchAddress("totalEvents", &sumW);
    for(int ientry = 0; ientry < sumWeights->GetEntries(); ++ientry) {
      sumWeights->GetEntry(ientry);
        sumWTotal += sumW;
    }

    delete sumWeights;

    return sumWTotal;
}

bool has_suffix(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string getHTTBranchName(const std::string &var, const std::string &config){
  std::string brName;
  brName = "htt_" + var;
  if(! config.empty())
    brName += "_" + config;
  return brName;
}

void fakeClusterEMScale(const xAOD::CaloClusterContainer* cont)
{
    if(cont->isAvailable<float>("rawM")) return;
    static SG::AuxElement::Decorator<float> rawMDec("rawM");
    static SG::AuxElement::Decorator<float> rawEDec("rawE");
    static SG::AuxElement::Decorator<float> rawEtaDec("rawEta");
    static SG::AuxElement::Decorator<float> rawPhiDec("rawPhi");
    for(const xAOD::CaloCluster* clust : *cont) {
        rawMDec( *clust ) = 0;
        rawEDec( *clust ) = 0;
        rawEtaDec( *clust ) = 0;
        rawPhiDec( *clust ) = 0;
    }
}

std::vector<TChain*> get_branch_tchains(const std::string input_filepath)
{
        TFile* tmp_tfile = TFile::Open(input_filepath.c_str(), "READ");
        TIter next_key(tmp_tfile->GetListOfKeys());

        std::vector<std::string> tree_names;

        while (auto tmp_key = (TKey*) next_key()) {
                std::string tmp_key_title(tmp_key->GetTitle());
                std::string tmp_key_name(tmp_key->GetName());

                bool is_tree = tmp_key_title == "tree";

                bool is_relevant_tree = tmp_key_name == "nominal" ||
                        has_suffix(tmp_key_name, "1up") ||
                        has_suffix(tmp_key_name, "1down");

                if (is_tree && is_relevant_tree)
                        tree_names.push_back(tmp_key_name);
        }

        std::vector<TChain*> tchains;
        for (auto const& n : tree_names)
                tchains.push_back(new TChain(n.c_str()));
        for (auto& t : tchains)
                t->Add(input_filepath.c_str(), 0);

        return tchains;
}
