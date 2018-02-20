#!/usr/bin/env python
import TopExamples.grid

suffix = 'gammajet_20180219'

scope     = 'user.rnewhous'
dir_data  = '/data/newhouse/output/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/'+suffix+'/data'
dir_mc15c = '/data/newhouse/output/TopBosonTagAnalysis2018/NTuples_DataMC_dijets/'+suffix+'/mc15c'


TopExamples.grid.download(scope , 'user.rnewhous.*period*.data_'+suffix+'*_output.root' , dir_data)
TopExamples.grid.download(scope , 'user.rnewhous.*.mc15_'+suffix+'*_output.root' , dir_mc15c)

