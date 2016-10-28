#!/usr/bin/env python
import TopExamples.grid

scope      = 'user.zmeadows'
dir_data16 = '/titan_rw/atlas/common/zmeadows/TopBosonTagging/dijet/10242016/data16'
dir_mc15c  = '/titan_rw/atlas/common/zmeadows/TopBosonTagging/dijet/10242016/mc15c'

TopExamples.grid.download(scope , 'user.zmeadows.*.data16_10242016_v1_output.root' , dir_data16)
TopExamples.grid.download(scope , 'user.zmeadows.*.mc15c_10242016_v1_output.root' , dir_mc15c)
