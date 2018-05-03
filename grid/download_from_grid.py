#!/usr/bin/env python
import TopExamples.grid

# ## Dijet
suffix = 'dijet_20180405_syst_v7'

scope     = 'user.rnewhous'
dir_data  = '/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/'+suffix+'/data'
dir_mc15c = '/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/'+suffix+'/mc15c'


# TopExamples.grid.download(scope , 'user.rnewhous.*period*.data_'+suffix+'*_output.root' , dir_data)
TopExamples.grid.download(scope , 'user.rnewhous.*.mc15_'+suffix+'*_output.root' , dir_mc15c)

## Gammajet

suffix = 'gammajet_20180405_syst_v2'
# suffix = 'gammajet_20180405'

scope     = 'user.rnewhous'
dir_data  = '/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/'+suffix+'/data'
dir_mc15c = '/data/newhouse/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/'+suffix+'/mc15c'


# TopExamples.grid.download(scope , 'user.rnewhous.*period*.data_'+suffix+'*_output.root' , dir_data)
# TopExamples.grid.download(scope , 'user.rnewhous.*.mc15_'+suffix+'_output.root' , dir_mc15c)