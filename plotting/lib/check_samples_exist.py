import pyAMI.client
import pyAMI.atlas.api as api
import sys

if (len(sys.argv)) < 2:
  print "pass txt file containing sample list as first argument to ", sys.argv[0]

allSamples = {}
with open(sys.argv[1]) as f:
  allSamples = f.readlines()

allSamples = [x.strip() for x in allSamples] 

# INIT ATLAS API
api.init()

# INSTANTIATE THE PYAMI CLIENT FOR ATLAS
client = pyAMI.client.Client('atlas')

print "The following samples cannot be found:"

for sample in allSamples:
  if (sample[0] != '#'):
    try:
      api.get_dataset_info(client,sample)
    except:
      splitSample = sample.split(':')
      if (len(splitSample) > 1):
        try:
          api.get_dataset_info(client,splitSample[1])
        except:
          print sample
      else:
          print sample

print "Finished."
