import pyAMI.client
import pyAMI.atlas.api as api
import json
import sys
from samples import samples_EVNT

my_weights_file = open("sample_weights.txt", 'w')
analysis_top_weights_file = open("sample_weights_AnalysisTopFormat.txt", 'w')

# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

fields = 'files.cross_section,files.gen_filt_eff,nfiles,files.events,dataset_number,generator_name'

my_weights_file.write("# DSID NUM_EVENTS X_SECTION(fb) FILTER_EFFICIENCY GENERATOR\n")

for sample_name, sample_files in samples_EVNT.iteritems():

    my_weights_file.write("# " + sample_name + "\n")
    analysis_top_weights_file.write("# " + sample_name + "\n")

    print("# " + sample_name)

    for f in sample_files:
        res_dict = api.get_dataset_info(client, f)

        if len(res_dict) == 0:
            print "WARNING: could not find file: ", f

        if len(res_dict) > 1:
            print "WARNING: result dictionary length greater than one. shouldn't happen."
            sys.exit(1)

        total_events = res_dict[0]["totalEvents"]
        filt_eff_mean = res_dict[0]["GenFiltEff_mean"]
        xsection_mean = res_dict[0]["crossSection_mean"]
        dsid = res_dict[0]["datasetNumber"]
        generator_name = res_dict[0]["generatorName"]

        if (res_dict[0]["crossSection_unit"] == "nano barn"):
            xsection_mean = str(float(xsection_mean) * 1e6)
        elif (res_dict[0]["crossSection_unit"] == "pico barn"):
            xsection_mean = str(float(xsection_mean) * 1e3)
        elif (res_dict[0]["crossSection_unit"] == "femto barn"):
            pass
        else:
            print "unrecognized cross section unit: ", res_dict[0]["crossSection_unit"]
            sys.exit(1)

        my_weights_str = ' '.join([dsid, total_events, xsection_mean, filt_eff_mean, generator_name])
        my_weights_file.write(my_weights_str + "\n")
        print my_weights_str

        analysis_top_xsection_str = ' '.join([
            dsid, str(1e3 * float(xsection_mean) * float(filt_eff_mean)), "1.0", generator_name])
        analysis_top_weights_file.write(analysis_top_xsection_str + "\n")

my_weights_file.close()
analysis_top_weights_file.close()
