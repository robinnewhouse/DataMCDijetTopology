#ifndef GAMMAJETSELECTOR_H_
#define GAMMAJETSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good photons.
 */
class GammaJetSelector : public EventSelectorBase {
public:
    explicit GammaJetSelector(void);

    bool apply(const top::Event& event) const override;

    std::string name(void) const override;
};

}

#endif
