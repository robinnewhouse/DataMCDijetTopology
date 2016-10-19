#ifndef InputManager_h
#define InputManager_h

#include <unordered_map>
#include <vector>

#include <TChain.h>

struct InputPack {
    std::vector<std::string> filepaths;
    std::vector<std::string> tree_names;
};

const struct InputPack EMPTY_INPUT_PACK = { {} , {} };

class InputManager {
public:
    InputManager(const std::string input_filepath);

    virtual ~InputManager(void) { };

    // A map from generator names to InputPacks
    // ex: "generator name" = 'data', 'pythia_dijet', 'herwig_dijet', etc
    std::unordered_map<std::string, InputPack> m_input_packs;

    std::unordered_map<std::string, std::vector<TChain*>> get_tchain_map(void) const;

    ClassDef(InputManager, 0);
};

#endif // #ifdef InputManager_h
