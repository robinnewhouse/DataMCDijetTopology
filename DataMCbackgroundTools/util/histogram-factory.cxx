#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>

#include <TChain.h>
#include <TSelector.h>
//#include <TProof.h>

#include "DataMCbackgroundTools/DataMCbackgroundSelector.h"
#include "DataMCbackgroundTools/InputManager.h"

int
main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "USAGE: histogram-factory [FLAG]..." << std::endl;
        std::cout << "where FLAG is one of:" << std::endl;
        std::cout << "-i <input_file>: path to ntuple produced by AnalysisTop." << std::endl;
        std::cout << "-o <output_file>: root file where the output will be stored. default: raw_histograms.root" << std::endl;
        std::cout << "-t <trigger>: trigger to use for selecting events in data. ex: HLT_j420_a10r_L1J100" << std::endl;
        std::cout << "-E <event_selector>: Offline event selection criteria to use, ex: NO_SELECTION" << std::endl;
        std::cout << "-L <luminosity>: luminosity of dataset to scale to (in inverse femtobarns): ex: 3.20905" << std::endl;
        std::cout << "-S: process all systematics trees (all trees ending with 1up/1down)" << std::endl;
        std::cout << "see README.md or source code for more details" << std::endl;
        return EXIT_FAILURE;
    }

    // ZM: OCT 18 2016: No longer using InputManager in favour of batch system
    // if invalid file, InputManager will throw exception
    // InputManager* input_manager = new InputManager(argv[1]);

    std::string input_filepath, output_filepath,
        data_trigger, event_selector, sample_type;
    float luminosity            = -1.;
    bool process_systematics    = false;

    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "Si:o:E:t:L:p:")) != -1) {
        switch (c)
        {
            case 'i':
                input_filepath = std::string(optarg);
                break;
            case 'o':
                output_filepath = std::string(optarg);
                break;
            case 't':
                data_trigger = std::string(optarg);
                break;
            case 'E':
                event_selector = std::string(optarg);
                break;
            case 'p':
                sample_type = std::string(optarg);
                break;
            case 'L':
                luminosity = atof(optarg);
                break;
            case 'S':
                process_systematics = true;
                break;
            case '?':
                if (optopt == 'o' || optopt == 't')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return EXIT_FAILURE;
            default:
                abort ();
        }
    }

    if (input_filepath.empty()) {
        std::cout << "ERROR: no input file specified" << std::endl;
        std::cout << "EXITING..." << std::endl;
        return EXIT_FAILURE;
    }

    if (output_filepath.empty()) {
        std::cout << "ERROR: no output file specified" << std::endl;
        std::cout << "EXITING..." << std::endl;
        return EXIT_FAILURE;
    }

    if (data_trigger.empty()) {
        std::cout << "ERROR: no output file specified" << std::endl;
        std::cout << "EXITING..." << std::endl;
        return EXIT_FAILURE;
    }

    if (event_selector.empty()) {
        std::cout << "ERROR: no output file specified" << std::endl;
        std::cout << "EXITING..." << std::endl;
        return EXIT_FAILURE;
    }

    if (luminosity <= 0) {
        std::cout << "ERROR: no luminosity specified" << std::endl;
        std::cout << "EXITING..." << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream outfile_test(output_filepath, std::ifstream::in);
    if (outfile_test.good()) {
        std::cout << "ERROR: output file " << output_filepath << " already exists." << std::endl;
        std::cout << "EXITING..." << std::endl;
        outfile_test.close();
        return EXIT_FAILURE;
    } else {
        outfile_test.close();
    }

    std::vector<TChain*> tchains = get_branch_tchains(input_filepath);

    TSelector* dmd_selector;
    for (auto& t : tchains) {
        const std::string tchain_name = std::string(t->GetName());
        const bool is_a_desired_tchain = process_systematics || tchain_name == "nominal" || tchain_name == "Nominal";

        if (t->GetEntries() == 0) {
          std::cout << "SKIPPING EMPTY FILE: " << input_filepath << std::endl;
          continue;
        }

        if (is_a_desired_tchain) {
            std::cout << tchain_name << std::endl;
            dmd_selector = new DataMCbackgroundSelector(output_filepath, sample_type,
                            tchain_name, data_trigger, event_selector, luminosity);

            t->Process(dmd_selector);
            delete dmd_selector;

            if (process_systematics && tchain_name == "nominal" && event_selector.find("GAMMA") != std::string::npos && sample_type.find("data") == std::string::npos) {
              dmd_selector = new DataMCbackgroundSelector(output_filepath, sample_type,
                              "photonSF_ID_UP", data_trigger, event_selector, luminosity);
              t->Process(dmd_selector);
              delete dmd_selector;
              dmd_selector = new DataMCbackgroundSelector(output_filepath, sample_type,
                              "photonSF_ID_DOWN", data_trigger, event_selector, luminosity);
              t->Process(dmd_selector);
              delete dmd_selector;
              dmd_selector = new DataMCbackgroundSelector(output_filepath, sample_type,
                              "photonSF_effTrkIso_UP", data_trigger, event_selector, luminosity);
              t->Process(dmd_selector);
              delete dmd_selector;
              dmd_selector = new DataMCbackgroundSelector(output_filepath, sample_type,
                              "photonSF_effTrkIso_DOWN", data_trigger, event_selector, luminosity);
              t->Process(dmd_selector);
              delete dmd_selector;
            }

        }

    }

    return EXIT_SUCCESS;
}

