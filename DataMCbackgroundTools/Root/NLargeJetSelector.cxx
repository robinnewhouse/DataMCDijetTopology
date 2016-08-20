#include "DataMCbackgroundTools/NLargeJetSelector.h"

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/Event.h"


#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetContainer.h"

#include <vector>
#include <cmath>

namespace top {

NLargeJetSelector::NLargeJetSelector(const std::string& params) :
	SignValueSelector("LARGEJET_N", params, true)
{
	checkMultiplicityIsInteger();
}
	
bool NLargeJetSelector::apply(const top::Event& event) const
{
	auto func = [&](const xAOD::Jet* jetPtr){ return jetPtr->pt() > value(); };
	auto count = std::count_if(event.m_largeJets.begin(), event.m_largeJets.end(), func);
	return checkInt(count, multiplicity());
}
	
}
