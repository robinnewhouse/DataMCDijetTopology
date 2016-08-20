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
        std::cout << "USAGE: dmd-control-plots <path-to-input-file> [FLAG]..." << std::endl;
        std::cout << "see README.txt or dmd-control-plots.cxx for details" << std::endl;
        return EXIT_FAILURE;
    }

    InputManager* input_manager = new InputManager(argv[1]);

    std::string output_filepath       = "dmd_control_plots.root";
    std::string data_trigger          = "HLT_j400_a10_lcw_L1J100";
    float luminosity                  = 3.20905;
    bool compute_systematics_branches = false;

    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "So:t:L:")) != -1) {
        switch (c)
        {
            case 'o':
                output_filepath = std::string(optarg);
                break;
            case 't':
                data_trigger = std::string(optarg);
                break;
            case 'L':
                luminosity = atof(optarg);
                break;
            case 'S':
                compute_systematics_branches = true;
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

    std::ifstream outfile_test(output_filepath, std::ifstream::in);
    if (outfile_test.good()) {
        std::cout << "WARNING: output file " << output_filepath << " already exists." << std::endl;
        std::cout << "EXITING..." << std::endl;
        outfile_test.close();
        return EXIT_FAILURE;
    } else {
        outfile_test.close();
    }

    //TProof* p = TProof::Open("lite://");
    //p->SetProgressDialog(0);
    //p->SetParallel(5);
    //


    auto tchain_map = input_manager->get_tchain_map();

    TSelector* dmd_selector;
    for (auto& x : tchain_map) {
        std::string gen_name = x.first;
        std::vector<TChain*> tchains = x.second;

        for (auto& tchain : tchains) {
            std::string tchain_name = std::string(tchain->GetName());

            if (compute_systematics_branches
                    || tchain_name == "nominal"
                    || tchain_name == "Nominal") {

                dmd_selector = new DataMCbackgroundSelector(output_filepath, gen_name,
                        tchain_name, data_trigger, luminosity);

                //tchain->SetProof();
                tchain->Process(dmd_selector);

                delete dmd_selector;
            }
        }
    }

    return EXIT_SUCCESS;
}

