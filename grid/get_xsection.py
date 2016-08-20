import pyAMI.client
import pyAMI.atlas.api as api
import json
import sys
from samples import samples_EVNT

dmd_weights_file = open("sample_weights.txt", 'w')
analysis_top_weights_file = open("sample_weights_ANALYSISTOP.txt", 'w')

# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

fields = 'files.cross_section,files.gen_filt_eff,nfiles,files.events,dataset_number,generator_name'

dmd_weights_file.write("# DSID NUM_EVENTS X_SECTION(fb) FILTER_EFFICIENCY GENERATOR\n")

for sample_name, sample_files in samples_EVNT.iteritems():

    dmd_weights_file.write("# " + sample_name + "\n")
    analysis_top_weights_file.write("# " + sample_name + "\n")

    print("# " + sample_name)

    for pattern in sample_files:
        resDict = api.list_datasets(client, patterns = pattern, fields = fields)

        numFiles = 0.0
        numEvents = 0.0
        avgFiltEff = 0.0
        avgXSec = 0.0
        dsid = 0
        genName = ""

        # loop over files in dataset, calculate avg filter efficiency
        for results in resDict:
            numFiles = (float)(results['nfiles'])
            if (results['generator_name'] != 'Null'): genName = results['generator_name']
            if (results['dataset_number'] != 'Null'): dsid = (int) (results['dataset_number'])
            if (results['files_gen_filt_eff'] != 'NULL'): avgFiltEff += (float) (results['files_gen_filt_eff'])
            if (results['files_cross_section'] != 'NULL'): avgXSec += (float) (results['files_cross_section'])
            if (results['files_events'] != 'NULL'): numEvents += (float) (results['files_events'])
            pass # end loop over files

        if(numFiles != 0):
            avgFiltEff = avgFiltEff/numFiles
            avgXSec = avgXSec/numFiles

        dmd_weights_str = ' '.join(map(str, [dsid, numEvents, 1e6 * avgXSec, avgFiltEff, genName]))
        dmd_weights_file.write(dmd_weights_str + "\n")

        print dmd_weights_str

        analysis_top_xsection_str = ' '.join(map(str, [dsid, 1e3 * avgXSec * avgFiltEff, 1.0, genName]))
        analysis_top_weights_file.write(analysis_top_xsection_str + "\n")

dmd_weights_file.close()
analysis_top_weights_file.close()
