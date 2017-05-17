#include "DataMCbackgroundTools/GammaJetSelector.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/PhotonContainer.h"

#include <algorithm>

namespace top {

GammaJetSelector::GammaJetSelector(void) : EventSelectorBase() {}

std::string GammaJetSelector::name(void) const {
    return "GAMMAJET";
}

bool GammaJetSelector::apply(const top::Event& event) const {

    if (event.m_photons.empty() || event.m_largeJets.empty())
        return false;

    auto pt_sort_func = [&](const xAOD::IParticle* p1, const xAOD::IParticle* p2){
        return p1->pt() > p2->pt();
    };

    const xAOD::Photon* lead_photon = *std::max_element(event.m_photons.begin(), event.m_photons.end(), pt_sort_func);

    if (!lead_photon) return false;

    std::vector<const xAOD::Jet*> filtered_largeR_jets;

    std::copy_if(event.m_largeJets.begin(), event.m_largeJets.end(),
            std::back_inserter(filtered_largeR_jets),
            [&](const xAOD::Jet* j) {
                return fabs(j->phi() - lead_photon->phi()) > 0.5 * M_PI;
            });

    if (filtered_largeR_jets.empty())
        return false;

    return true;
}

}
