from ROOT import *
import uuid

class PlotBase(object):
    ''' A base class that contains only the objects/information
    that every ROOT plot would contain. Only classes derived
    from this should ever be initialized.
    '''
    def __init__(self,
            x_title         = "",
            y_title         = "",
            x_units         = "GeV",
            y_units         = "",
            x_min           = None,
            x_max           = None,
            y_min           = None,
            y_max           = None,
            show_overflow   = False,
            show_underflow  = False,
            height          = 600,
            width           = 800,
            name            = None,
            log_scale       = False,
            empty_scale     = 1.0,
            legend_loc      = None,
            atlas_loc       = None,
            extra_lines_loc = None,
            debug           = False,
            atlas_mod       = "Internal", # 'Internal', 'Preliminary', etc
            lumi_val        = "X.X",      # ex: '3.2'
            lumi_units      = "fb",       # ex: 'fb'
            com_energy      = "13",       # in TeV
            extra_legend_lines = [],
            rebin = None,
            **kwargs):

        super(PlotBase, self).__init__(**kwargs)

        self.log = []

        self.x_title = x_title
        self.y_title = y_title
        self.x_units = x_units
        self.y_units = y_units

        self.x_min = x_min
        self.x_max = x_max

        self.y_min = y_min
        self.y_max = y_max

        if (name == None):
            # random unique default name to avoid name overlap
            self.name = uuid.uuid4().hex
        else:
            self.name = name

        self.width  = width
        self.height = height

        if log_scale:
            self.name += "_log"
        self.log_scale = log_scale

        self.empty_scale = empty_scale

        self.x_units_str = ""
        if self.x_units != "":
            self.x_units_str = "[" + self.x_units + "]"

        self.legend_loc = [0.69,0.92,0.92,0.82] if width == 800 else [0.69,0.92,0.92,0.82]
        if (legend_loc):
            assert(len(legend_loc) == 4)
            self.legend_loc = legend_loc

        self.atlas_loc = [0.2,0.90] if (width == 800) else [0.2,0.90]
        if (atlas_loc):
            assert(len(atlas_loc) == 2)
            self.atlas_loc = atlas_loc

        self.extra_lines_loc = [0.2,0.80] if width == 800 else [0.2,0.80]
        if (extra_lines_loc):
            assert(len(extra_lines_loc) == 2)
            self.extra_lines_loc = extra_lines_loc

        self.extra_legend_lines = extra_legend_lines

        self.atlas_mod  = atlas_mod
        self.lumi_val   = lumi_val
        self.lumi_units = lumi_units
        self.com_energy = com_energy

        self.rebin = rebin

        self._make_canvas()
        self._make_decorations()

    def determine_y_axis_title(self, histo, label = "Events"):
        bin_width = histo.GetXaxis().GetBinWidth(1)

        self.y_title = label + " / "
        self.y_title += format_bin_width(bin_width)
        self.y_title += " " + self.x_units

    def set_x_axis_bounds(self, histo):
        if (self.x_max or self.x_min):
            tmp_x_min = histo.GetXaxis().GetXmin()
            tmp_x_max = histo.GetXaxis().GetXmax()

            if (self.x_max): tmp_x_max = self.x_max
            if (self.x_min): tmp_x_min = self.x_min

            histo.GetXaxis().SetRangeUser(tmp_x_min , tmp_x_max)

    def pad_empty_space(self, histos):
        if (self.y_max != None):
            print("WARNING: attempted to pad empty space and set y-maximum at the same time.")
            return
        ''' rescale y-axis to add/subtract empty space '''
        if self.y_max != None:
            print "warning: attmempted to set y_max and pad empty space at the same time"
            return

        y_max = max(map(lambda h: h.GetMaximum(), histos))

        if self.log_scale:
            y_max *= 10**self.empty_scale
        else:
            y_max *= self.empty_scale

        for h in histos:
            h.SetMaximum(y_max)

    def set_y_min(self, histo):
        if (self.y_min != None):
            histo.SetMinimum(self.y_min)

    def set_y_max(self, histo):
        if (self.y_max != None):
            histo.SetMaximum(self.y_max)

    def _make_canvas(self):
        canvas_name = "c_" + self.name
        self.canvas = TCanvas(canvas_name, canvas_name, self.width, self.height)
        self.canvas.Draw()
        self.canvas.Modified()

    def _make_decorations(self):
        ''' Make the ATLAS label, luminosity, extra lines, legend, etc '''
        self.canvas.cd()

        def set_default_tex_props(tex):
            tex.SetNDC()
            tex.SetLineWidth(2)
            if (self.width == 800):
                tex.SetTextSize(0.04)
            else:
                tex.SetTextSize(0.033)

        self.ATLAS_tex = TLatex(self.atlas_loc[0], self.atlas_loc[1], "ATLAS")
        set_default_tex_props(self.ATLAS_tex)
        self.ATLAS_tex.SetTextFont(72)

        self.ATLAS_MOD_tex = TLatex(self.atlas_loc[0], self.atlas_loc[1], (14 * ' ') + self.atlas_mod)
        set_default_tex_props(self.ATLAS_MOD_tex)
        self.ATLAS_MOD_tex.SetTextFont(42)

        lum_str = self.lumi_val + " " + self.lumi_units + "^{-1}"

        self.ILUM_tex = TLatex(self.atlas_loc[0], self.atlas_loc[1] - 0.05,
                "#sqrt{s} = " + self.com_energy + " TeV, " + lum_str)
        set_default_tex_props(self.ILUM_tex)
        self.ILUM_tex.SetTextFont(42)

        self.extra_latex = []
        y_tmp = self.extra_lines_loc[1]
        for txt_line in self.extra_legend_lines:
            tex = TLatex(self.extra_lines_loc[0], y_tmp, txt_line);
            set_default_tex_props(tex)
            tex.SetTextFont(42)
            self.extra_latex.append(tex)
            y_tmp -= 0.04

        self.leg = TLegend(self.legend_loc[0], self.legend_loc[1],
                self.legend_loc[2], self.legend_loc[3])

        self.leg.SetBorderSize(0)

    def log_line(self, text_line):
        self.log.append(text_line)

    def _draw_decorations(self):
        ''' Draw the ATLAS label, luminosity, extra lines, legend, etc '''
        self.canvas.cd()
        self.leg.Draw()
        self.ATLAS_tex.Draw()
        self.ATLAS_MOD_tex.Draw()
        self.ILUM_tex.Draw()
        for tex in self.extra_latex:
            tex.Draw()
        self.canvas.Update()
        self.canvas.Modified()

    def print_to_file(self, filename, index = None, num_plots = None):
        ''' Assumes the derived class has already called Draw on all of its content,
        populated the legend, etc
        '''
        self._draw_decorations()

        if (index is None):
            self.canvas.Print(filename)
            return

        if (index is 0):
            self.canvas.Print(filename + ".pdf(", "pdf")
        elif index == num_plots - 1:
            self.canvas.Print(filename + ".pdf)", "pdf")
        else:
            self.canvas.Print(filename + ".pdf", "pdf")

def format_bin_width(bin_spacing):
    """ format the bin width decimal string for the Y-axis, in a smart way. """
    if bin_spacing < 0.1:
        return str(round(bin_spacing * 50) / 50)
    if bin_spacing < 0.5:
        return str(round(bin_spacing * 20) / 20)
    if bin_spacing < 1.0:
        return str(round(bin_spacing * 10) / 10)
    elif bin_spacing < 10:
        if (int(bin_spacing) == bin_spacing):
            return str(int(round(bin_spacing * 4) / 4))
        else:
            return str(round(bin_spacing * 4) / 4)
    elif bin_spacing < 100:
        return str(round(bin_spacing * 2) / 2)
    else:
        return str(int(round(bin_spacing)))
