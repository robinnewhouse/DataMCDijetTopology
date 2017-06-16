from MyGenUtil import *

import argparse
import os
import sys
import time
import re

# CONSTANTS

MY_DATE             = time.strftime("%d%m%Y_%H:%M:%S")
JOB_SCRIPT          = '/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology/Batch/histogram_factory_job.py'

# ARGUMENTS

parser = argparse.ArgumentParser(description='For generating commands to run athena jobs locally or on batch system' , add_help = True)
dry_run_parser = parser.add_mutually_exclusive_group(required=False)
dry_run_parser.add_argument('--dry-run', dest='dry_run', action='store_true')
dry_run_parser.add_argument('--submit', dest='dry_run', action='store_false')
parser.set_defaults(dry_run=True)
parser.add_argument('--input-dir'   , type=str)
parser.add_argument('--selection'   , type=str, default="NO_SELECTION")
parser.add_argument('--queue'       , type=str  , default="8nh")
parser.add_argument('--luminosity'  , type=float)
args = parser.parse_args()

print ""
print "== ARGS =="
print(args)
print "=========="
print ""

assert(args.queue in [ "8nm", "1nh", "8nh", "1nd", "2nd", "1nw", "2nw" ] )

def get_data_trigger(filepath):
  if "mc15" in filepath:
    return "none"
  if "gamma" in filepath:
    return "HLT_g140_loose"
  elif "dijet" in filepath:
    if "grp16" in filepath:
      return "HLT_j420_a10_lcw_L1J100"
    else:
      return "HLT_j360_a10_lcw_sub_L1J100"

  print "ERROR: couldn't determine data trigger from filepath: ", filepath
  sys.exit(1)

def get_sample_type(filepath):
  if "physics_Main" in filepath:
    return "data"

  possible_dsids = filepath.replace("/",".").split('.')
  possible_dsids = filter(lambda x: x.isdigit() and len(x) == 6, possible_dsids)

  for x in possible_dsids:
    x_int = int(x)
    if   x_int >= 303722 and x_int <= 303726:
      return "ttbar_allhad"
    elif x_int >= 304307 and x_int <= 304309:
      return "sherpa_wjets"
    elif x_int >= 304623 and x_int <= 304627:
      return "pythia_wjets"
    elif x_int >= 304628 and x_int <= 304632:
      return "pythia_zjets"
    elif x_int >= 304673 and x_int <= 304677:
      return "herwig_wjets"
    elif x_int >= 304678 and x_int <= 304682:
      return "herwig_zjets"
    elif x_int >= 304707 and x_int <= 304709:
      return "sherpa_zjets"
    elif x_int >= 361020 and x_int <= 361032:
      return "pythia_dijet"
    elif x_int >= 426040 and x_int <= 426052:
      return "herwig_dijet"
    elif x_int >= 426131 and x_int <= 426142:
      return "sherpa_dijet"
    elif x_int >= 423103 and x_int <= 423112:
      return "pythia_gammajet"
    elif x_int >= 361039 and x_int <= 361062:
      return "sherpa_gammajet"
    elif x_int >= 305435 and x_int <= 305444:
      return "sherpa_wz_gamma"
    elif x_int == 410087:
      return "ttbar_gamma"

  print "ERROR: couldn't find DSID from list of possibilities: ", possible_dsids
  sys.exit(1)

INPUT_FILES = list(find_files(args.input_dir, "*.root*"))
INPUT_FILES = filter(lambda x: "cp." not in x, INPUT_FILES)

if (len(INPUT_FILES) == 0):
    print "ERROR: Could not locate any root files in directory: ", args.input_dir
    sys.exit(1)

for input_file in INPUT_FILES:
  output_file = os.path.dirname(input_file) + "/" + os.path.basename(input_file).replace(".root",".cp.root")

  if os.path.isfile(output_file):
    print "skipping file: ", output_file, "because it already exists."
    continue

  job_cmd = ' '.join([
    "python", JOB_SCRIPT,
    "--input-file", input_file,
    "--output-file", output_file,
    "--sample-type", get_sample_type(input_file),
    "--luminosity", str(args.luminosity),
    "--data-trigger", get_data_trigger(input_file),
    "--selection", args.selection
    ])

  bsub_cmd = """bsub -R \"pool>3000\" -q """ + args.queue + " \"" + job_cmd + "\""

  if (args.dry_run):
    print job_cmd
  else:
    os.system(bsub_cmd)
