import os
import time
import random
import sys
import tempfile
import argparse

parser = argparse.ArgumentParser(description='For running a histogram-factory job' , add_help = True)
parser.add_argument('--input-file'   , type=str)
parser.add_argument('--output-file'  , type=str)
parser.add_argument('--sample-type'  , type=str)
parser.add_argument('--data-trigger' , type=str)
parser.add_argument('--selection'    , type=str)
parser.add_argument('--luminosity'   , type=str)
args = parser.parse_args()

assert(args.input_file != args.output_file)

WORKAREA = "/afs/cern.ch/work/z/zmeadows/public/TopBosonTag/DataMCDijetTopology"

os.chdir("/tmp/zmeadows")
RECO_TMP_DIR=tempfile.mkdtemp()
os.chdir(RECO_TMP_DIR)

TMP_OUTPUT_FILE = os.path.basename(args.output_file)

with open('job.sh', 'w') as fout:
      def write_cmd(cmd_str):
          fout.write(cmd_str)
          fout.write("\n")
      write_cmd("#!/bin/sh\n")
      write_cmd("export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase")
      write_cmd("source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh")
      write_cmd("cd " + WORKAREA)
      write_cmd("lsetup rcsetup")
      write_cmd("cd " + RECO_TMP_DIR)
      job_cmd = ' '.join([
        "histogram-factory",
        "-i", args.input_file,
        "-o", TMP_OUTPUT_FILE,
        "-t", args.data_trigger,
        "-L", args.luminosity,
        "-E", args.selection,
        "-p", args.sample_type,
        "-S"
        ])
      write_cmd(job_cmd)
      #write_cmd("xrdcp -r *.cp.root* root://eosatlas.cern.ch/" + os.path.dirname(args.output_file) + "/")
      write_cmd("cp *.cp.root* " + os.path.dirname(args.output_file) + "/")
      write_cmd("rm -rf *.root")
      fout.close()

os.system("chmod 755 job.sh")
os.system("./job.sh")
