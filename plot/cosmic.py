from rootalias import *

f_data = TFile('photon_timing_mc.costheta.root')
f_mc = TFile('photon_timing_data.costheta.root')

def plot_slice_duration(width):
    h_data = f_data.Get('photontimingana/fSliceDuration{}'.format(width))
    h_mc = f_mc.Get('photontimingana/fSliceDuration{}'.format(width))
    h_data.Scale(1.0 / h_data.Integral())
    h_mc.Scale(1.0 / h_mc.Integral())

    h_data.Rebin(20)
    h_mc.Rebin(20)

    gStyle.SetOptStat('nmr')
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()

    if width == '':
        width = '100'
    print 'width = ', width

    set_h1_style(h_data)
    h_data.SetName('FD Cosmic Data')
    # h_data.SetTitle('')
    # h_data.GetXaxis().SetTitle('Track Segment Duration (ns)')
    h_data.GetXaxis().SetTitle('Track Segment {}% Timing Width (ns)'.format(width))
    h_data.GetYaxis().SetTitle('Track Count (area norm.)')
    h_data.SetLineColor(kRed)
    h_data.GetXaxis().SetRangeUser(0, 700)
    h_data.Draw()

    set_h1_style(h_mc)
    h_mc.SetName('FD Cosmic MC')
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
    c1.SaveAs('figures/cosmic/plot_slice_duration.pdf')
    c1.SaveAs('figures/cosmic/plot_slice_duration.width_{}.png'.format(width))
    raw_input('Press any key to continue.')

def plot_cell_count():
    h_data = f_data.Get('photontimingana/fCellCountPerSlice')
    h_mc = f_mc.Get('photontimingana/fCellCountPerSlice')
    h_data.Scale(1.0 / h_data.Integral())
    h_mc.Scale(1.0 / h_mc.Integral())

    c1 = TCanvas('c1', 'c1', 800, 600)
    h_data.GetXaxis().SetRangeUser(0, 50)
    h_data.Draw()

    h_mc.SetLineColor(kRed)
    h_mc.Draw('sames')
    c1.Update()
    c1.SaveAs('figures/cosmic/plot_cell_count.pdf')
    raw_input('Press any key to continue.')

# plot_slice_duration('60')
# plot_slice_duration('70')
# plot_slice_duration('80')
# plot_slice_duration('90')
# plot_slice_duration('')
# plot_cell_count()
