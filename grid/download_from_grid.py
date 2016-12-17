#!/usr/bin/env python
import TopExamples.grid

scope     = 'user.zmeadows'
dir_data  = '/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20161216/data'
dir_mc15c = '/eos/atlas/atlascerngroupdisk/perf-jets/JSS/TopBosonTagAnalysis2016/NTuples_DataMC_dijets/20161216/mc15c'

TopExamples.grid.download(scope , 'user.zmeadows.*.TopWZTag_mc15c_16122016_v1_output.root' , dir_mc15c)
TopExamples.grid.download(scope , 'user.zmeadows.*.TopWZTag_data_16122016_v1_output.root' , dir_data)
