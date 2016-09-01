#!/usr/bin/env python
import TopExamples.grid

scope      = 'user.zmeadows'
dir_data16 = '/titan_rw/atlas/common/zmeadows/TopBosonTagging/gamma_jet/08222016_data16_pythia_mc15c_first_attempt/data16'
dir_mc15c  = '/titan_rw/atlas/common/zmeadows/TopBosonTagging/gamma_jet/08222016_data16_pythia_mc15c_first_attempt/mc15c'

TopExamples.grid.download(scope , '*data16_gammajet_08222016_v0_output.root' , dir_data16)
TopExamples.grid.download(scope , '*pythia_gammajet_08222016_v0_output.root' , dir_mc15c)
