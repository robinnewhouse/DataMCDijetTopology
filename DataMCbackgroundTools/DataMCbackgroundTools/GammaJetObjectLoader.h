#ifndef DataMCbackgroundGammaJetObjectLoader_h
#define DataMCbackgroundGammaJetObjectLoader_h

#include "TopAnalysis/ObjectLoaderBase.h"

namespace top {

/**
 * @brief This loads our "standard" object selection used in the top group.
 *
 * You can modify this directly, inherit from it and make changes or write your
 * own.  It's really your decision and I can't tell you what to do (you should
 * write your own in a separate library then when you change release it wont
 * be a big deal).
 */
class GammaJetObjectLoader : public ObjectLoaderBase {
public:

    /**
     * @brief Setup the cuts.  Probably easiest if you look at the code, no?
     *
     * @param topConfig The config that has been loaded.
     * @return A fully configured object that can be used to control which
     * objects (physics ones, not computer ones) are selected by the program.
     */
    top::TopObjectSelection* init(std::shared_ptr<top::TopConfig> topConfig);

    ClassDef(top::GammaJetObjectLoader, 0)
};

}

#endif
