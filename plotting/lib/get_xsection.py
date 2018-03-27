import pyAMI.client
import pyAMI.atlas.api as api
import json
import sys


if (len(sys.argv)) < 2:
  print "pass txt file containing sample list as first argument to ", sys.argv[0]

my_weights_file = open("sample_weights.txt", 'w')


# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

my_weights_file.write("# DSID NUM_EVENTS X_SECTION(fb) FILTER_EFFICIENCY GENERATOR\n")


allSamples = with open(sys.argv

for sample in allSamples:
    my_weights_file.write("# " + sample + "\n")
    print("# " + sample)

    dsId = sample.split('.')[1]
    evnt_ds = None
    aod_ds = None

    provD=api.get_dataset_prov(client,sample)
    for dsDic in provD['node']:
        #print dsDic['dataType'], dsDic['logicalDatasetName']
        if 'EVNT' == dsDic['dataType'] and dsId in dsDic['logicalDatasetName']:
            evnt_ds = dsDic['logicalDatasetName']
        elif aod_ds == None and 'AOD' == dsDic['dataType'] and dsId in dsDic['logicalDatasetName']:
            aod_ds = dsDic['logicalDatasetName']

    print aod_ds
    print evnt_ds

    # we have to look at the AOD file just for NEvents
    # since not all generated events necessarily make it into the AOD
    evnt_res_dict = api.get_dataset_info(client, evnt_ds)
    aod_res_dict = api.get_dataset_info(client, aod_ds)

    if len(aod_res_dict) == 0:
        print "WARNING: could not find file: ", f_aod
        sys.exit(1)
    if len(evnt_res_dict) == 0:
        print "WARNING: could not find file: ", f_evnt
        sys.exit(1)

    print aod_res_dict

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

my_weights_file.close()
