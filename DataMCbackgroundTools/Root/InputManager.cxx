#include "DataMCbackgroundTools/InputManager.h"
#include "DataMCbackgroundTools/LocalTools.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <TFile.h>
#include <TKey.h>

InputManager::InputManager(const std::string input_list_filepath)
{
    std::ifstream input_file(input_list_filepath, std::ifstream::in);

    if (!input_file.is_open()) {
        throw std::invalid_argument(
            "InputManager failed to open input file list: " + input_list_filepath);
    }

    std::string ntuple_path, ntuple_gen;
    while (input_file >> ntuple_path >> ntuple_gen) {
        auto tag_exists = m_input_packs.find(ntuple_gen) != m_input_packs.end();

        if (!tag_exists) m_input_packs[ntuple_gen] = EMPTY_INPUT_PACK;

        m_input_packs[ntuple_gen].filepaths.push_back( ntuple_path );
    }

    // populate the list of tree_names in each InputPack
    for (auto& pack : m_input_packs) {

        const std::string tmp_gen_name = pack.first;
        const std::string tmp_filepath = pack.second.filepaths[0];

        TFile* tmp_tfile = new TFile(tmp_filepath.c_str());
        TIter next_key(tmp_tfile->GetListOfKeys());

        while (auto tmp_key = (TKey*) next_key()) {

            std::string tmp_key_title(tmp_key->GetTitle());
            std::string tmp_key_name(tmp_key->GetName());

            bool is_tree = tmp_key_title == "tree";

            bool is_relevant_tree =
                  tmp_key_name == "nominal" ||
                  has_suffix(tmp_key_name, "1up") ||
                  has_suffix(tmp_key_name, "1down");

            if (is_tree && is_relevant_tree) {
                //std::cout << tmp_gen_name << ": " << tmp_key_name << std::endl;
                pack.second.tree_names.push_back(tmp_key_name);
            }
        }
    }

    // check that each file, in each input pack, has all the expected trees
    for (const auto& pack : m_input_packs) {
        for (const auto& path : pack.second.filepaths) {
            TFile* tmp_tfile = new TFile(path.c_str());
            for (const auto& tree_name : pack.second.tree_names) {
                if(!tmp_tfile->GetListOfKeys()->Contains(tree_name.c_str())) {
                    throw std::runtime_error(
                        "InputManager: failed to find expected branch titled " +
                        tree_name + " in file " + path);
                }
            }
        }
    }
}

std::unordered_map<std::string, std::vector<TChain*>>
InputManager::get_tchain_map(void) const
{
    std::unordered_map<std::string, std::vector<TChain*>> tchain_map;

    for (auto const& pack : m_input_packs)
    {
        for (auto const& tree_name : pack.second.tree_names)
            tchain_map[pack.first].push_back(new TChain(tree_name.c_str()));

        for (auto& tchain : tchain_map[pack.first]) {
            for (auto const& filepath : pack.second.filepaths) {
                tchain->Add(filepath.c_str(), 0);
            }
        }
    }

    return tchain_map;
}
