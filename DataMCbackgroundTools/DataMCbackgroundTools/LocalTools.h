#ifndef LOCALTOOLS_H_
#define LOCALTOOLS_H_

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include <TFile.h>
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

#endif
