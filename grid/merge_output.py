import os
import sys
import glob
import argparse

parser = argparse.ArgumentParser(description = "merge output files from ATLAS grid jobs")

parser.add_argument("-i", "--input", type=str)
parser.add_argument("-o", "--output", type=str)
parser.add_argument("-mc", "--ismc", action="store_true")

args = parser.parse_args()

if args.output[-1] != '/':
    args.output += '/'

if args.input[-1] != '/':
    args.input += '/'

folders = glob.glob(args.input + '*user.*')
folders.sort()

for i in range(len(folders)):
    if folders[i][-1] != '/':
        folders[i] += '/'

def merge_mc(mc_dir):
    # split up the string on periods
    mc_name = os.path.basename(mc_dir[:-1])
    mc_split = mc_name.split('.')

    # find the DSID from the folder name
    dsid = 0
    for substr in mc_split:
        try:
            if int(substr) > 99999 and int(substr) < 1e7:
                dsid = int(substr)
                break
        except:
            pass

    if dsid == 0:
        print "Couldn't identify DSID for MC directory: " + mc_dir
        return None

    output_name = str(dsid) + '.merged.root'
    input_files = glob.glob(mc_dir + '*.root*')

    hadd_cmd = 'hadd -f ' + args.output + output_name + ' ' + ' '.join(input_files)
    os.system(hadd_cmd)

def merge_data(data_dirs):
    input_files = []
    for dd in data_dirs:
        input_files += glob.glob(dd + '*.root*')
    hadd_cmd = 'hadd -f ' + args.output + 'data.merged.root' + ' ' + ' '.join(input_files)
    os.system(hadd_cmd)

if args.ismc:
    for mc_sample_dir in folders:
        merge_mc(mc_sample_dir)
else:
    merge_data(folders)

