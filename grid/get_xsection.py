import pyAMI.client
import pyAMI.atlas.api as api
import json
import sys
from samples import *

my_weights_file = open("sample_weights.txt", 'w')
analysis_top_weights_file = open("sample_weights_AnalysisTopFormat.txt", 'w')

# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

my_weights_file.write("# DSID NUM_EVENTS X_SECTION(fb) FILTER_EFFICIENCY GENERATOR\n")

for sample_name in samples_AOD.keys():
    if ("sherpa_dijet" in sample_name): continue

    my_weights_file.write("# " + sample_name + "\n")
    analysis_top_weights_file.write("# " + sample_name + "\n")

    print("# " + sample_name)

    evnt_files = samples_EVNT[sample_name]
    aod_files = samples_AOD[sample_name]

    # each EVNT file must correspond to exactly one AOD file
    assert(len(evnt_files) == len(aod_files))

    for i_file in range(len(evnt_files)):
        f_aod  = aod_files[i_file]
        f_evnt = evnt_files[i_file]

        # we have to look at the AOD file just for NEvents
        # since not all generated events necessarily make it into the AOD
        aod_res_dict = api.get_dataset_info(client, f_aod)
        evnt_res_dict = api.get_dataset_info(client, f_evnt)

        if len(aod_res_dict) == 0:
            print "WARNING: could not find file: ", f_aod
            sys.exit(1)
        if len(evnt_res_dict) == 0:
            print "WARNING: could not find file: ", f_evnt
            sys.exit(1)

        total_events   = aod_res_dict[0]["totalEvents"]
        filt_eff_mean  = evnt_res_dict[0]["GenFiltEff_mean"]
        xsection_mean  = evnt_res_dict[0]["crossSection_mean"]
        dsid           = evnt_res_dict[0]["datasetNumber"]
        generator_name = evnt_res_dict[0]["generatorName"]

        if (evnt_res_dict[0]["crossSection_unit"] == "nano barn"):
            xsection_mean = str(float(xsection_mean) * 1e6)
        elif (evnt_res_dict[0]["crossSection_unit"] == "pico barn"):
            xsection_mean = str(float(xsection_mean) * 1e3)
        elif (evnt_res_dict[0]["crossSection_unit"] == "femto barn"):
            pass
        else:
            print "unrecognized cross section unit: ", evnt_res_dict[0]["crossSection_unit"]
            sys.exit(1)

        my_weights_str = ' '.join([dsid, total_events, xsection_mean, filt_eff_mean, generator_name])
        my_weights_file.write(my_weights_str + "\n")
        print my_weights_str

        analysis_top_xsection_str = ' '.join([
            dsid, str(1e3 * float(xsection_mean) * float(filt_eff_mean)), "1.0", generator_name])
        analysis_top_weights_file.write(analysis_top_xsection_str + "\n")

my_weights_file.close()
analysis_top_weights_file.close()
