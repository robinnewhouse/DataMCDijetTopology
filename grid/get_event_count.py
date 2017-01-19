import pyAMI.client
import pyAMI.atlas.api as api
import json
import sys
from samples import *

# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

for sample_name in samples_JETM6.keys():
    if ("data" in sample_name): 
        continue

    print("# " + sample_name)

    #evnt_files = samples_EVNT[sample_name]
    aod_files = samples_JETM6[sample_name]

    # each EVNT file must correspond to exactly one AOD file
    #assert(len(evnt_files) == len(aod_files))

    for i_file in range(len(aod_files)):
        f_aod  = aod_files[i_file]
        #f_evnt = evnt_files[i_file]

        # we have to look at the AOD file just for NEvents
        # since not all generated events necessarily make it into the AOD
        aod_res_dict = api.get_dataset_info(client, f_aod)
        #evnt_res_dict = api.get_dataset_info(client, f_evnt)

        if len(aod_res_dict) == 0:
            print "WARNING: could not find file: ", f_aod
            sys.exit(1)
        #if len(evnt_res_dict) == 0:
        #    print "WARNING: could not find file: ", f_evnt
        #    sys.exit(1)

        total_events   = aod_res_dict[0]["totalEvents"]
        dsid           = aod_res_dict[0]["datasetNumber"]

        my_weights_str = ' '.join([dsid, total_events,])
        print my_weights_str
