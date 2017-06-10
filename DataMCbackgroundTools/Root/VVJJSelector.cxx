#include "DataMCbackgroundTools/VVJJSelector.h"
#include "DataMCbackgroundTools/LocalTools.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/PhotonContainer.h"

#include <algorithm>

namespace top {

VVJJSelector::VVJJSelector(void) : EventSelectorBase() {}

std::string VVJJSelector::name(void) const {
    return "VVJJ";
}

bool VVJJSelector::apply(const top::Event& event) const {

    if (event.m_largeJets.size() < 2)
        return false;

    auto pt_sort_func = [&](const xAOD::IParticle* p1, const xAOD::IParticle* p2){
        return p1->pt() > p2->pt();
    };

    std::vector<const xAOD::Jet*> sorted_rljets;
    for (auto jet : event.m_largeJets) sorted_rljets.push_back(jet);
    std::sort(sorted_rljets.begin(), sorted_rljets.end(), pt_sort_func);

    if (sorted_rljets.at(0)->pt()/1000. < 450)
      return false;
    if (sorted_rljets.at(1)->pt()/1000. < 200)
      return false;
    if (sorted_rljets.at(0)->m()/1000. < 50)
      return false;
    if (sorted_rljets.at(1)->m()/1000. < 50)
      return false;
    if (sorted_rljets.at(0)->numConstituents() <= 2)
      return false;
    if (sorted_rljets.at(1)->numConstituents() <= 2)
      return false;

    TLorentzVector v_jet0, v_jet1;
    v_jet0.SetPtEtaPhiM(sorted_rljets[0]->pt(), sorted_rljets[0]->eta(), sorted_rljets[0]->phi(), sorted_rljets[0]->m());
    v_jet1.SetPtEtaPhiM(sorted_rljets[1]->pt(), sorted_rljets[1]->eta(), sorted_rljets[1]->phi(), sorted_rljets[1]->m());
    if ((v_jet0 + v_jet1).M() / 1000. < 1100.)
      return false;

    return true;
}

}
