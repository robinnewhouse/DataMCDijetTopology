#ifndef VVJJSELECTOR_H_
#define VVJJSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

namespace top {

/**
 * @brief A tool that selects events with a certain number of good photons.
 */
class VVJJSelector : public EventSelectorBase {
public:
    explicit VVJJSelector(void);

    bool apply(const top::Event& event) const override;

    std::string name(void) const override;
};

}

#endif
