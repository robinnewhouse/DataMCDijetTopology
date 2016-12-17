#ifndef LOCALTOOLS_H_
#define LOCALTOOLS_H_

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCaloEvent/CaloCluster.h>
#include "xAODCore/ShallowCopy.h"
#include "xAODCore/ShallowAuxContainer.h"

#include <TFile.h>
#include "TChain.h"
#include <memory>

#include <vector>

class BinManager {
private:
    const std::vector<float> bin_boundaries;

public:
    BinManager(const std::vector<float>& bin_boundaries_);

    unsigned get_num_bins(void) const { return bin_boundaries.size() + 1; }
    int get_bin_index(const float& var_value) const;
};

const xAOD::Jet* get_nearest_jet_in_collection(const xAOD::Jet* ref_jet,
        const xAOD::JetContainer* collection);

/** Get sum of weight from sumWeights tree
  * in file produced by AnalysisTop.
    * This gives info on the sum of weights of the original AOD,
    * from which a DAOD was produced (i.e. without skimming) */
float get_sum_weights_sample(TFile* inputFile);

/** Get total number of events from sumWeights tree
  * in file produced by AnalysisTop.
    * This gives info on the number of events in the original AOD,
    * from which a DAOD was produced. */
ULong64_t getTotalEventsSample(TFile* inputFile);

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
        return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

bool has_suffix(const std::string& str, const std::string& suffix);

std::string getHTTBranchName(const std::string &var, const std::string &config);

struct pt_sort
{
    inline bool operator() (const TLorentzVector& lhs, const TLorentzVector& rhs)
    {
        return (lhs.Pt() > rhs.Pt());
    }

    inline bool operator() (const TLorentzVector* lhs, const TLorentzVector* rhs)
    {
        return (lhs->Pt() > rhs->Pt());
    }

    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs)
    {
        return (lhs.pt() > rhs.pt());
    }

    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
    {
        return (lhs->pt() > rhs->pt());
    }
};

template<typename T>
T sort_container_pt(T* inCont) {
    T sortedCont(SG::VIEW_ELEMENTS);
    for(auto x : *inCont) sortedCont.push_back( x );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return sortedCont;
}

template<typename T>
const T sort_container_pt(const T* inCont){
    ConstDataVector<T> sortedCont(SG::VIEW_ELEMENTS);

    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return *sortedCont.asDataVector();
}

// fills rawM variable with nonsense, used to avoid segfaults
// when algorithms check for it (but don't use it)
void fakeClusterEMScale(const xAOD::CaloClusterContainer* cont);

std::vector<TChain*> get_branch_tchains(const std::string input_filepath);

int combine_bits(const bool bit_one, const bool bit_two);

#endif
