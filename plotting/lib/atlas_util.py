from ROOT import *
from math import *
import array as arr
import os

DERIVATIONS = map(lambda x: "JETM" + str(x), range(0,10)) + map(lambda x: "EXOT" + str(x), range(0,10))

def getDxAODacro(sample_name):
  daod_acro = None
  for d in DERIVATIONS:
    if d in sample_name:
      daod_acro = d
  return daod_acro

def DAODtoEVNT(sample_name):
  daod_acro = getDxAODacro(sample_name)
  if (daod_acro):
      evnt_str = sample_name.replace("merge.DAOD_" + daod_acro, "evgen.EVNT")
      substrs = evnt_str.split(".")
      substrs[-1] = substrs[-1][0:5]
      return '.'.join(substrs)
  else:
      print "WARNING: DAODtoEVNT: Couldn't determine DxAOD acronym for sample: ", sample_name

def DAODtoAOD(sample_name):
  daod_acro = getDxAODacro(sample_name)
  if (daod_acro):
      daod_acro = getDxAODacro(sample_name)
      aod_str = sample_name.replace("merge.DAOD_" + daod_acro, "merge.AOD")
      substrs = aod_str.split(".")
      substrs[-1] = substrs[-1][:-6]
      return '.'.join(substrs)
  else:
      print "WARNING: DAODtoAD: Couldn't determine DxAOD acronym for sample: ", sample_name
