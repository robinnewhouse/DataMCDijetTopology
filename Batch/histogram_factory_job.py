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

WORKAREA = "/home/newhouse/public/Analysis/TopBosonTagging/DataMCDijetTopology/"

os.chdir("/home/newhouse/tmp/")
RECO_TMP_DIR=tempfile.mkdtemp()
os.chdir(RECO_TMP_DIR)

TMP_OUTPUT_FILE = os.path.basename(args.output_file)

with open('job.sh', 'w') as fout:
      def write_cmd(cmd_str):
          fout.write(cmd_str)
          fout.write("\n")
      write_cmd("#!/bin/sh\n")
      write_cmd("#PBS -l  walltime=24:00:00\n\n")
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
        "-S" # This toggles systematics 
        ])
      write_cmd(job_cmd)
      OUTPUT_BASE = ".".join(TMP_OUTPUT_FILE.split(".")[0:3])
      #write_cmd("xrdcp -r *.cp.root* root://eosatlas.cern.ch/" + os.path.dirname(args.output_file) + "/")
      write_cmd("cp "+ OUTPUT_BASE +".*.root" + " " + os.path.dirname(args.output_file) + "/")
      write_cmd("cp "+ OUTPUT_BASE+".root.log"+ " " + os.path.dirname(args.output_file) + "/")
      write_cmd("rm -rf "+ OUTPUT_BASE +".*.root" + " " + OUTPUT_BASE+".root.log")
      # write_cmd("cd " + WORKAREA)
      # write_cmd("rm *.root.log")
      fout.close()

os.system("chmod 755 job.sh")
os.system("qsub -V job.sh")
print
