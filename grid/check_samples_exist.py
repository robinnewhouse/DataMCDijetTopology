import subprocess
from samples import *

num_samples_checked = 0
num_samples_failed = 0

def check_sample_dict(sample_dict):
    global num_samples_checked
    global num_samples_failed
    for sample_set, sample_files in sample_dict.iteritems():
        for f in sample_files:
            ret_code = subprocess.call(["ami","show","dataset","info",f],
                    stdout = subprocess.PIPE)
            if (ret_code != 0):
                num_samples_failed += 1
                print "NOT FOUND: ", f
            num_samples_checked += 1

check_sample_dict(samples_AOD)
check_sample_dict(samples_EVNT)
check_sample_dict(samples_JETM8)

print "NUMBER SAMPLES CHECKED: ", num_samples_checked
print "NUMBER SAMPLES FAILED: ", num_samples_failed

if (num_samples_failed == 0):
    print "All samples exist."
