from rootalias import *

def convert_th1i_to_th1d(th1i):
    th1d = TH1D(th1i.GetName(),
                th1i.GetTitle(),
                th1i.GetNbinsX(),
                th1i.GetBinLowEdge(1),
                th1i.GetBinLowEdge(th1i.GetNbinsX()) + th1i.GetBinWidth(th1i.GetNbinsX()))
    for i in range(1, th1i.GetNbinsX() + 1):
        th1d.SetBinContent(i, th1i.GetBinContent(i))
    return th1d

f_mc = TFile('photon_timing_mc.root')
f_data = TFile('photon_timing.root')

h_mc_slice_duration = f_mc.Get('photontimingana/fSliceDuration')
h_mc_slice_duration_quadrant_1 = f_mc.Get('photontimingana/fSliceDurationQuadrant1')
h_mc_slice_duration_quadrant_2 = f_mc.Get('photontimingana/fSliceDurationQuadrant2')
h_mc_slice_duration_quadrant_3 = f_mc.Get('photontimingana/fSliceDurationQuadrant3')
h_mc_slice_duration_quadrant_4 = f_mc.Get('photontimingana/fSliceDurationQuadrant4')
h_mc_slice_duration_ncell_25 = f_mc.Get('photontimingana/fSliceDurationNCell25')
h_mc_slice_duration_ncell_50 = f_mc.Get('photontimingana/fSliceDurationNCell50')
h_mc_slice_duration_ncell_75 = f_mc.Get('photontimingana/fSliceDurationNCell75')
h_mc_slice_duration_ncell_100 = f_mc.Get('photontimingana/fSliceDurationNCell100')
h_mc_slice_duration_ncell_125 = f_mc.Get('photontimingana/fSliceDurationNCell125')
h_mc_slice_duration_ncell_150 = f_mc.Get('photontimingana/fSliceDurationNCell150')
h_mc_slice_count = f_mc.Get('photontimingana/fSliceCount')
h_mc_ncell = convert_th1i_to_th1d(f_mc.Get('photontimingana/fNCell'))
mc_event_count = h_mc_slice_count.Integral()

h_data_slice_duration = f_data.Get('photontimingana/fSliceDuration')
h_data_slice_duration_quadrant_1 = f_data.Get('photontimingana/fSliceDurationQuadrant1')
h_data_slice_duration_quadrant_2 = f_data.Get('photontimingana/fSliceDurationQuadrant2')
h_data_slice_duration_quadrant_3 = f_data.Get('photontimingana/fSliceDurationQuadrant3')
h_data_slice_duration_quadrant_4 = f_data.Get('photontimingana/fSliceDurationQuadrant4')
h_data_slice_duration_ncell_25 = f_data.Get('photontimingana/fSliceDurationNCell25')
h_data_slice_duration_ncell_50 = f_data.Get('photontimingana/fSliceDurationNCell50')
h_data_slice_duration_ncell_75 = f_data.Get('photontimingana/fSliceDurationNCell75')
h_data_slice_duration_ncell_100 = f_data.Get('photontimingana/fSliceDurationNCell100')
h_data_slice_duration_ncell_125 = f_data.Get('photontimingana/fSliceDurationNCell125')
h_data_slice_duration_ncell_150 = f_data.Get('photontimingana/fSliceDurationNCell150')
h_data_slice_count = f_data.Get('photontimingana/fSliceCount')
h_data_ncell = convert_th1i_to_th1d(f_data.Get('photontimingana/fNCell'))
data_event_count = h_data_slice_count.Integral()

h_mcs = [h_mc_slice_duration,
         h_mc_slice_duration_quadrant_1,
         h_mc_slice_duration_quadrant_2,
         h_mc_slice_duration_quadrant_3,
         h_mc_slice_duration_quadrant_4,
         h_mc_slice_duration_ncell_25,
         h_mc_slice_duration_ncell_50,
         h_mc_slice_duration_ncell_75,
         h_mc_slice_duration_ncell_100,
         h_mc_slice_duration_ncell_125,
         h_mc_slice_duration_ncell_150,
         h_mc_ncell]

h_datas = [h_data_slice_duration,
           h_data_slice_duration_quadrant_1,
           h_data_slice_duration_quadrant_2,
           h_data_slice_duration_quadrant_3,
           h_data_slice_duration_quadrant_4,
           h_data_slice_duration_ncell_25,
           h_data_slice_duration_ncell_50,
           h_data_slice_duration_ncell_75,
           h_data_slice_duration_ncell_100,
           h_data_slice_duration_ncell_125,
           h_data_slice_duration_ncell_150,
           h_data_ncell]

h_alls = h_mcs + h_datas

for h_all in h_alls:
    set_h1_style(h_all)
    h_all.GetXaxis().SetTitle('Slice Duration (ns)')
    h_all.GetYaxis().SetTitle('Slice Count')
    h_all.Rebin(5)

for h_data in h_datas:
    h_data.SetLineColor(kRed)
    h_data.Scale(float(mc_event_count) / data_event_count)


def plot_quadrant(h_mcs, h_datas):
    gStyle.SetOptStat('nmr')
    c1 = TCanvas('c1', 'c1', 1200, 800)
    c1.Divide(2, 2)
    hist_indices = [1, 4, 2, 3]

    for i in [1, 2, 3, 4]:
        c1.cd(i)
        hist_index = hist_indices[i - 1]
        h_data = h_datas[hist_index]
        h_mc = h_mcs[hist_index]
        h_data.SetName('Q{} Data'.format(hist_index))
        h_mc.SetName('Q{} MC'.format(hist_index))

        gPad.SetTopMargin(0.05)
        gPad.SetBottomMargin(0.2)
        gPad.SetLeftMargin(0.15)
        h_mc.GetXaxis().SetTitleOffset(2.2)
        h_mc.GetYaxis().SetTitleOffset(2.2)
        h_mc.Draw()
        h_data.Draw('sames')
        h_mc.Rebin(2)
        h_data.Rebin(2)

        c1.Update()
        p_data = h_data.GetListOfFunctions().FindObject("stats")
        p_data.SetTextColor(h_data.GetLineColor())
        p_data.SetLineColor(h_data.GetLineColor())
        p_data.SetX1NDC(0.62)
        p_data.SetY1NDC(0.68)
        p_data.SetX2NDC(0.85)
        p_data.SetY2NDC(0.88)
        p_data.Draw()

        p_mc = h_mc.GetListOfFunctions().FindObject("stats")
        p_mc.SetTextColor(h_mc.GetLineColor())
        p_mc.SetLineColor(h_mc.GetLineColor())
        p_mc.SetX1NDC(0.62)
        p_mc.SetY1NDC(0.44)
        p_mc.SetX2NDC(0.85)
        p_mc.SetY2NDC(0.64)
        p_mc.Draw()

        c1.Update()

    c1.SaveAs('figures/plot_quadrant.pdf')
    raw_input('Press anykey to continue.\n')


def plot_hit_cut(h_mcs, h_datas):
    for i in range(5, 11):
        h_mc = h_mcs[i]
        h_data = h_datas[i]
        hit_cut = (i - 4) * 25

        c2 = TCanvas('c2', 'c2', 600, 600)
        gPad.SetBottomMargin(0.15)
        gPad.SetLeftMargin(0.15)
        h_mc.GetYaxis().SetTitleOffset(1.5)
        h_mc.SetName('>= {}, MC'.format(hit_cut))
        h_mc.Draw()
        h_data.SetName('>= {}, Data'.format(hit_cut))
        h_data.Draw('sames')

        c2.Update()
        p_data = h_data.GetListOfFunctions().FindObject("stats")
        p_data.SetTextColor(h_data.GetLineColor())
        p_data.SetLineColor(h_data.GetLineColor())
        p_data.SetX1NDC(0.7)
        p_data.SetY1NDC(0.75)
        p_data.SetX2NDC(0.95)
        p_data.SetY2NDC(0.95)
        p_data.Draw()

        p_mc = h_mc.GetListOfFunctions().FindObject("stats")
        p_mc.SetTextColor(h_mc.GetLineColor())
        p_mc.SetLineColor(h_mc.GetLineColor())
        p_mc.SetX1NDC(0.7)
        p_mc.SetY1NDC(0.51)
        p_mc.SetX2NDC(0.95)
        p_mc.SetY2NDC(0.71)
        p_mc.Draw()

        c2.SaveAs('figures/plot_hit_cut_{}.pdf'.format(hit_cut))
        c2.Update()
    raw_input('Press any key to continue.\n')

def plot_cell_count(h_data_ncell, h_mc_ncell):
    gStyle.SetOptStat(0)
    c3 = TCanvas('c3', 'c3', 800, 600)
    gPad.SetBottomMargin(0.15)
    gPad.SetLeftMargin(0.15)
    
    pad1 = TPad("pad1", "pad1", 0, 0.4, 1, 1)
    pad1.SetTopMargin(0.15)
    pad1.SetBottomMargin(0)
    pad1.SetLeftMargin(0.15)
    pad1.Draw()
    pad1.cd()
    h_data_ncell.GetXaxis().SetTitle('Slice Count')
    h_data_ncell.GetXaxis().SetTitle('Hit Count per Slice')
    h_data_ncell.GetYaxis().SetRangeUser(-100.0, max(h_data_ncell.GetMaximum(), h_mc_ncell.GetMaximum()) * 1.2)
    h_data_ncell.GetXaxis().SetRangeUser(0.0, 300)
    h_data_ncell.GetYaxis().SetTitleOffset(1.5)
    h_data_ncell.Draw("E")
    h_mc_ncell.Draw("sames, E")
    
    lg1 = TLegend(0.6, 0.5, 0.9, 0.8)
    set_legend_style(lg1)
    lg1.AddEntry(h_data_ncell, 'data', 'le')
    lg1.AddEntry(h_mc_ncell, 'mc', 'le')
    lg1.Draw()

    c3.cd()
    pad2 = TPad('pad2', 'pad2', 0, 0, 1, 0.4)
    pad2.SetTopMargin(0)
    pad2.SetLeftMargin(0.15)
    pad2.SetBottomMargin(0.4)
    pad2.Draw()
    pad2.cd()
    gPad.SetGrid()

    h_ratio = h_data_ncell.Clone()
    h_ratio.Sumw2()
    h_ratio.Divide(h_mc_ncell)
    h_ratio.GetYaxis().SetRangeUser(0.5, 1.5)
    h_ratio.SetLineColor(kBlack)
    h_ratio.SetTitle('')
    h_ratio.GetYaxis().SetNdivisions(205, 1)
    h_ratio.GetXaxis().SetTitle('Hit Count per Slice')
    h_ratio.GetYaxis().SetTitle('data / MC')
    h_ratio.GetXaxis().SetTitleOffset(3)
    h_ratio.Draw('ep')
    c3.Update()
    c3.SaveAs('figures/plot_cell_count.pdf')


def plot_slice_count():
    c4 = TCanvas('c4', 'c4', 800, 600)
    gPad.SetBottomMargin(0.15)
    gPad.SetLeftMargin(0.15)
    h_mc_slice_count.Draw()
    h_mc_slice_count.GetXaxis().SetRangeUser(0, 20)
    h_data_slice_count.Draw('sames')
    c4.Update()

def draw_nd():
    pass

# plot_cell_count(h_data_ncell, h_mc_ncell)
# plot_quadrant(h_mcs, h_datas)
# plot_hit_cut(h_mcs, h_datas)
draw_nd()
raw_input('Press anykey to continue.\n')
