from rootalias import *

f_data = TFile('photon_timing.nd_spill_data.root')
f_mc = TFile('photon_timing.nd_spill_mc.root')
figure_dir = '/Users/juntinghuang/google_drive/slides/beamer/20170621_cosmic_timing/figures'

def plot_tns_distribution():
    hist_names = ['fSliceCellHitTns1st', 'fSliceCellHitTns2nd', 'fSliceCellHitTns3rd', 'fSliceCellHitTns4th']
    x_titles = ['Time to 1st Hit (ns)',
                'Time to 2nd Hit (ns)',
                'Time to 3rd Hit (ns)',
                'Time to 4th Hit (ns)']
    h_datas = []
    h_mcs = []
    for hist_name in hist_names:
        h_data = f_data.Get('photontimingspillana/{}'.format(hist_name))
        h_data.Scale(1.0 / h_data.Integral())
        set_h1_style(h_data)
        h_data.SetLineColor(kRed)
        h_datas.append(h_data)

        h_mc = f_mc.Get('photontimingspillana/{}'.format(hist_name))
        h_mc.Scale(1.0 / h_mc.Integral())
        set_h1_style(h_mc)
        h_mcs.append(h_mc)

    gStyle.SetOptStat('nmr')
    c1 = TCanvas('c1', 'c1', 1200, 800)
    c1.Divide(2, 2)
    for i in range(4):
        c1.cd(i + 1)
        h_data = h_datas[i]
        h_mc = h_mcs[i]
        h_data.SetName('data')
        h_mc.SetName('MC')

        gPad.SetTopMargin(0.05)
        gPad.SetBottomMargin(0.2)
        gPad.SetLeftMargin(0.15)
        h_mc.Rebin(2)
        h_data.Rebin(2)

        h_mc.Draw('hist')
        h_mc.GetYaxis().SetTitle('Cell Hit Count')
        h_mc.GetXaxis().SetTitle(x_titles[i])
        h_mc.GetXaxis().SetTitleOffset(2.2)
        h_mc.GetYaxis().SetTitleOffset(2.2)
        h_mc.GetXaxis().SetRangeUser(0, 200)
        h_data.Draw('hist,sames')

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

    c1.SaveAs('{}/spill_plot_tns_distribution.pdf'.format(figure_dir))
    raw_input('Press anykey to continue.\n')
    

def plot_gev_distribution(hist_name):
    h_data = f_data.Get('photontimingspillana/{}'.format(hist_name))
    h_mc = f_mc.Get('photontimingspillana/{}'.format(hist_name))
    h_data.Scale(1.0 / h_data.Integral())
    h_mc.Scale(1.0 / h_mc.Integral())

    c1 = TCanvas('c1', 'c1', 800, 800)
    set_margin()
    set_h1_style(h_data)
    h_data.SetLineColor(kRed)
    h_data.GetXaxis().SetRangeUser(0, 0.03)
    h_data.GetYaxis().SetRangeUser(0, max(h_data.GetMaximum(), h_mc.GetMaximum()) * 1.2)
    h_data.SetName('data')
    h_data.GetXaxis().SetTitle('RecoHit.GeV() (GeV)')
    h_data.GetYaxis().SetTitle('Reco Hit Count')
    h_data.GetYaxis().SetTitleOffset(1.6)
    h_data.Draw('hist')

    set_h1_style(h_mc)
    h_mc.SetName('MC')
    h_mc.Draw('sames, hist')

    c1.Update()
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

    c1.Update()
    c1.SaveAs('{}/spill_plot_gev_distribution.{}.pdf'.format(figure_dir, hist_name))
    raw_input('Press any key to continue.')


def plot_vertex_time(tfile, x1, x2, suffix):
    hist_name = 'fSliceVertexTime'
    h1 = tfile.Get('photontimingspillana/{}'.format(hist_name))
    if suffix == 'data':
        h1.Rebin(10)

    if suffix == 'mc':
        h1.Rebin(20)

    gStyle.SetOptStat(0)
    c1 = TCanvas('c1', 'c1', 800, 800)
    set_margin()

    set_h1_style(h1)
    h1.Draw()
    max_y = h1.GetMaximum() * 1.2
    h1.GetYaxis().SetRangeUser(0, max_y)
    h1.GetXaxis().SetRangeUser(215e3, 230e3)
    h1.GetXaxis().SetTitle('Vertex Time (ns)')
    h1.GetYaxis().SetTitle('Slice Count')
    h1.GetYaxis().SetTitleOffset(2)

    l1 = TLine(x1, 0, x1, max_y)
    l2 = TLine(x2, 0, x2, max_y)
    l1.SetLineColor(kRed)
    l1.SetLineStyle(2)
    l1.SetLineWidth(3)
    l2.SetLineColor(kRed)
    l2.SetLineStyle(2)
    l2.SetLineWidth(3)
    l1.Draw()
    l2.Draw()

    c1.Update()
    c1.SaveAs('{}/spill_plot_vertex_time.{}.pdf'.format(figure_dir, suffix))
    raw_input('Press any key to continue.')

def plot_slice_duration(percent, max_x):
    h_data = f_data.Get('photontimingspillana/fSliceDuration{}'.format(percent))
    h_mc = f_mc.Get('photontimingspillana/fSliceDuration{}'.format(percent))
    h_data.Scale(1.0 / h_data.Integral())
    h_mc.Scale(1.0 / h_mc.Integral())

    gStyle.SetOptStat('nmr')
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    set_h1_style(h_data)
    h_data.SetLineColor(kRed)
    x_title = 'Timing Width of Center {}% Hits (ns)'.format(percent)
    if percent == '':
        x_title = 'Timing Width of All Hits (ns)'

    h_data.Draw('hist')
    h_data.SetName('ND Spill Data')
    h_data.GetXaxis().SetTitle(x_title)
    h_data.GetYaxis().SetTitle('Slice Count')
    h_data.GetYaxis().SetTitleOffset(1.6)
    h_data.GetXaxis().SetRangeUser(0, max_x)
    h_data.GetYaxis().SetRangeUser(0, max(h_data.GetMaximum(), h_mc.GetMaximum()) * 1.2)

    set_h1_style(h_mc)
    h_mc.SetName('ND Spill MC')
    h_mc.GetXaxis().SetRangeUser(0, max_x)
    h_mc.Draw('sames, hist')

    c1.Update()
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

    c1.Update()
    c1.SaveAs('{}/spill_plot_slice_duration.fSliceDuration{}.pdf'.format(figure_dir, percent))
    raw_input('Press any key to continue.')


def plot_slice_duration_together():
    hist_names = ['fSliceDuration',
                  'fSliceDuration80',
                  'fSliceDuration70',
                  'fSliceDuration60']
    x_titles = [
        'Timing Width of All Hits (ns)',
        'Timing Width of Center 80% Hits (ns)',
        'Timing Width of Center 70% Hits (ns)',
        'Timing Width of Center 60% Hits (ns)'
    ]
    max_xs = [200, 80, 60, 50]
    h_datas = []
    h_mcs = []
    for hist_name in hist_names:
        h_data = f_data.Get('photontimingspillana/{}'.format(hist_name))
        h_data.Scale(1.0 / h_data.Integral())
        set_h1_style(h_data)
        h_data.SetLineColor(kRed)
        h_datas.append(h_data)

        h_mc = f_mc.Get('photontimingspillana/{}'.format(hist_name))
        h_mc.Scale(1.0 / h_mc.Integral())
        set_h1_style(h_mc)
        h_mcs.append(h_mc)

    gStyle.SetOptStat('nmr')
    c1 = TCanvas('c1', 'c1', 1200, 800)
    c1.Divide(2, 2)
    for i in range(4):
        c1.cd(i + 1)
        h_data = h_datas[i]
        h_mc = h_mcs[i]
        h_data.SetName('data')
        h_mc.SetName('MC')

        gPad.SetTopMargin(0.05)
        gPad.SetBottomMargin(0.2)
        gPad.SetLeftMargin(0.15)

        h_mc.Draw('hist')
        h_mc.GetYaxis().SetTitle('Slice Count')
        h_mc.GetXaxis().SetTitle(x_titles[i])
        h_mc.GetXaxis().SetTitleOffset(2.2)
        h_mc.GetYaxis().SetTitleOffset(2.3)
        h_mc.GetXaxis().SetRangeUser(0, max_xs[i])
        h_data.Draw('hist,sames')

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

    c1.SaveAs('{}/spill_plot_tns_distribution_together.pdf'.format(figure_dir))
    raw_input('Press anykey to continue.\n')


def plot_gev_slice_vs_track(tfile, suffix):
    h_slice = tfile.Get('photontimingspillana/fSliceRecoHitGeV')
    h_track = tfile.Get('photontimingspillana/fSliceTrackRecoHitGeV')

    c1 = TCanvas('c1', 'c1', 800, 800)
    set_margin()
    set_h1_style(h_slice)
    h_slice.SetLineColor(kRed)
    h_slice.GetXaxis().SetRangeUser(0, 0.03)
    h_slice.GetYaxis().SetRangeUser(0, max(h_slice.GetMaximum(), h_track.GetMaximum()) * 1.2)
    h_slice.SetName('full slice')
    h_slice.GetXaxis().SetTitle('Reco Hit GeV (GeV)')
    h_slice.GetYaxis().SetTitle('Reco Hit Count')
    h_slice.GetYaxis().SetTitleOffset(1.6)
    h_slice.Draw('hist')

    set_h1_style(h_track)
    h_track.SetName('tracks in slice')
    h_track.Draw('sames, hist')

    c1.Update()
    p_slice = h_slice.GetListOfFunctions().FindObject("stats")
    p_slice.SetTextColor(h_slice.GetLineColor())
    p_slice.SetLineColor(h_slice.GetLineColor())
    p_slice.SetX1NDC(0.7)
    p_slice.SetY1NDC(0.75)
    p_slice.SetX2NDC(0.95)
    p_slice.SetY2NDC(0.95)
    p_slice.Draw()

    p_track = h_track.GetListOfFunctions().FindObject("stats")
    p_track.SetTextColor(h_track.GetLineColor())
    p_track.SetLineColor(h_track.GetLineColor())
    p_track.SetX1NDC(0.7)
    p_track.SetY1NDC(0.51)
    p_track.SetX2NDC(0.95)
    p_track.SetY2NDC(0.71)
    p_track.Draw()

    c1.Update()
    c1.SaveAs('{}/spill_plot_gev_slice_vs_track.{}.pdf'.format(figure_dir, suffix))
    raw_input('Press any key to continue.')

# plot_tns_distribution()
# plot_tns_distribution('fSliceCellHitTns1st')
# plot_tns_distribution('fSliceCellHitTns2nd')
# plot_tns_distribution('fSliceCellHitTns3rd')
# plot_tns_distribution('fSliceCellHitTns4th')
# plot_gev_distribution('fSliceRecoHitGeV')
# plot_gev_distribution('fSliceTrackRecoHitGeV')
# plot_vertex_time(f_data, 218.7e3, 226.5e3, 'data')
# plot_vertex_time(f_mc, 219.5e3, 227.3e3, 'mc')
# plot_gev_slice_vs_track(f_data, 'data')
# plot_gev_slice_vs_track(f_mc, 'mc')

# plot_slice_duration('fSliceDuration')
plot_slice_duration('80', 80)
# plot_slice_duration('fSliceDuration60')
# plot_slice_duration('fSliceDurationSmallTimeWindow', 200)
# plot_slice_duration_together()
