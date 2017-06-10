import pyAMI.client
import pyAMI.atlas.api as api
import json
import sys
from samples_vvjj import *

these_samples = samples_EXOT3

my_weights_file = open("sample_weights.txt", 'w')
analysis_top_weights_file = open("sample_weights_AnalysisTopFormat.txt", 'w')

# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

my_weights_file.write("# DSID NUM_EVENTS X_SECTION(fb) FILTER_EFFICIENCY GENERATOR\n")

for sample_name, sample_files in these_samples.iteritems():
    if ("data" in sample_name):
      continue

    print("# " + sample_name)
    my_weights_file.write("# " + sample_name + "\n")
    analysis_top_weights_file.write("# " + sample_name + "\n")

    for sample_name in sample_files:
        dsId = sample_name.split('.')[1]
        evnt_ds = None
        aod_ds = None
        provD=api.get_dataset_prov(client, sample_name)

        for dsDic in provD['node']:
            #print dsDic['dataType'], dsDic['logicalDatasetName']
            if 'EVNT' == dsDic['dataType'] and dsId in dsDic['logicalDatasetName']:
                evnt_ds = dsDic['logicalDatasetName']
            elif aod_ds == None and 'AOD' == dsDic['dataType'] and dsId in dsDic['logicalDatasetName']:
                aod_ds = dsDic['logicalDatasetName']

        # we have to look at the AOD file just for NEvents
        # since not all generated events necessarily make it into the AOD
        aod_res_dict = api.get_dataset_info(client, aod_ds)
        evnt_res_dict = api.get_dataset_info(client, evnt_ds)

        if len(aod_res_dict) == 0:
            print "WARNING: could not find file: ", f_aod
            sys.exit(1)
        if len(evnt_res_dict) == 0:
            print "WARNING: could not find file: ", f_evnt
            sys.exit(1)

        print aod_res_dict
        dsid           = evnt_res_dict[0]["datasetNumber"]
        total_events   = aod_res_dict[0]["totalEvents"]
        filt_eff_mean  = evnt_res_dict[0]["GenFiltEff_mean"]
        xsection_mean  = evnt_res_dict[0]["crossSection_mean"]
        generator_name = evnt_res_dict[0]["generatorName"]

        try:
          map(float, [dsid, total_events, filt_eff_mean, xsection_mean])
        except ValueError:
          tmp_fields = 'files.cross_section,files.gen_filt_eff,nfiles,files.events,dataset_number,generator_name'
          evnt_files = api.list_datasets(client, patterns = evnt_ds, fields = tmp_fields)

          dsid           = None
          total_events   = 0.0
          filt_eff_mean  = 0.0
          xsection_mean  = 0.0
          generator_name = None

          numFiles = 0.0

          for results in evnt_files:
            numFiles = (float)(results['nfiles'])
            if (results['generator_name'] != 'Null'): generator_name = results['generator_name']
            if (results['dataset_number'] != 'Null'): dsid = (int) (results['dataset_number'])
            if (results['files_gen_filt_eff'] != 'NULL'): filt_eff_mean += (float) (results['files_gen_filt_eff'])
            if (results['files_cross_section'] != 'NULL'): xsection_mean += (float) (results['files_cross_section'])
            if (results['files_events'] != 'NULL'): total_events += (float) (results['files_events'])

          dsid           = str(dsid)
          total_events   = str(int(total_events))
          filt_eff_mean  = str(filt_eff_mean/numFiles)
          xsection_mean  = str(xsection_mean/numFiles)

        if ("nano" in evnt_res_dict[0]["crossSection_unit"]):
            xsection_mean = str(float(xsection_mean) * 1e6)
        elif ("pico" in evnt_res_dict[0]["crossSection_unit"]):
            xsection_mean = str(float(xsection_mean) * 1e3)
        elif ("femto" in evnt_res_dict[0]["crossSection_unit"]):
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
