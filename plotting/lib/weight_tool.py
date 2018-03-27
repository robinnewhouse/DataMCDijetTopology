import sys

class WeightTool(object):
    def __init__(self, weight_file):
        self.weight_file = weight_file

        self.num_events = {}
        self.xsection   = {}
        self.filter_eff = {}
        self.generator  = {}

        with open(weight_file) as f:
            if (not f):
                print "WeightTool: Could not open weights file: ", weight_file
                sys.exit(1)
            for line in f:
                line = line.split()
                if (line[0].startswith("#")): continue
                sample = line[0]
                self.num_events[sample] = float(line[1])
                self.xsection[sample]   = float(line[2])
                self.filter_eff[sample] = float(line[3])
                self.generator[sample]  = line[4]

    def get_n_events(self, sample_substring):
        for name, n_events in self.num_events.iteritems():
            if sample_substring in name:
                return n_events
        print "WeightTool: Could not find sample: ", sample_substring
        return -1;

    def get_xsection(self, sample_substring):
        for name, xsec in self.xsection.iteritems():
            if sample_substring in name:
                return xsec
        print "WeightTool: Could not find sample: ", sample_substring
        return -1;
