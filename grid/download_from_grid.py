#!/usr/bin/env python
import TopExamples.grid

scope      = 'user.zmeadows'
dir_data16 = '/titan_rw/atlas/common/zmeadows/TopBosonTagging/dijet/09072016/data16'
dir_mc15c  = '/titan_rw/atlas/common/zmeadows/TopBosonTagging/dijet/09072016/mc15c'

TopExamples.grid.download(scope , '*data16_dijet_0907_v0_output.root' , dir_data16)
TopExamples.grid.download(scope , '*wz_plus_jets_0907_v0_output.root' , dir_mc15c)
TopExamples.grid.download(scope , '*ttbar_0907_v0_output.root'        , dir_mc15c)
TopExamples.grid.download(scope , '*herwig_dijet_0907_v0_output.root' , dir_mc15c)
TopExamples.grid.download(scope , '*pythia_dijet_0907_v0_output.root' , dir_mc15c)
