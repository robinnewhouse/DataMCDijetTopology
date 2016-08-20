#include "DataMCbackgroundTools/GammaJetSelector.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/PhotonContainer.h"

#include <algorithm>

namespace top {

GammaJetSelector::GammaJetSelector(void) : EventSelectorBase()
{ }


std::string GammaJetSelector::name(void) const {
    return "GAMMAJET";
}

bool GammaJetSelector::apply(const top::Event& event) const {

    if (event.m_photons.size() == 0) {
        return false;
    }

    auto pt_sort_func = [&](const xAOD::IParticle* p1, const xAOD::IParticle* p2){
        return p1->pt() > p2->pt();
    };

    const xAOD::Photon* lead_photon = *std::max_element(
            event.m_photons.begin(), event.m_photons.end(), pt_sort_func);

    xAOD::JetContainer rljets = event.m_largeJets;

    rljets.sort(
            [](const xAOD::Jet* j1, const xAOD::Jet* j2) -> bool {
                return j1->pt() > j2->pt();
            });

    if (deltaPhi(*lead_photon, *rljets[0]) < 2.9) {
        return false;
    }

    if (rljets.size() > 1 && rljets[1]->pt() > 0.5 * lead_photon->pt()) {
        return false;
    }

    return true;
}


}
