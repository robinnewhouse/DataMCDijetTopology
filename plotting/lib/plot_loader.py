from ROOT import TFile

class PlotLoader(object):
    ''' A class that holds a TFile, and has convenient methods for
    accessing the histograms it contains.
    '''

    def __init__(self, filepath):
        self.filepath = filepath
        self.tfile = TFile.Open(self.filepath, "READ")

        # check that the file exists and opened successfully
        if not self.tfile:
            print("Failed to open raw control plot file: " + self.filepath)
            print "exiting..."
            exit(1)

    def get_hist(self, dir_path, hist_name):
        ''' Grab a histogram from the file.  '''
        try:
            current_dir = self.tfile
            for d in dir_path:
                current_dir = current_dir.Get(d)

            hist = current_dir.Get(hist_name)
            if (not hist):
                print "In path: " + str(dir_path)
                print "hist not found: " + str(hist_name)
                print "Returning None" + str(hist_name)
                return None
                print
            return hist.Clone()
        except Exception as e:
            print "In path: " + str(dir_path)
            print "hist not found: " + str(hist_name)
            print "exiting"
            print
            quit() # remove this for more information to be raised in the exception
            raise e

    def get_systematics_dict(self,
            hist_name,
            sys_branches,
            dir_path_to_branches = [],
            dir_path_to_histo = []
            ):

        systematics = {}

        current_directory = self.tfile
        for d in dir_path_to_branches:
            current_directory = current_directory.Get(d)

        for sys in sys_branches:
            h_sys_up = current_directory.Get(sys + "__1up").Get(hist_name)
            h_sys_down = current_directory.Get(sys + "__1down").Get(hist_name)
            systematics[sys] = { "up" : h_sys_up.Clone(), "down" : h_sys_down.Clone() }

        return systematics
