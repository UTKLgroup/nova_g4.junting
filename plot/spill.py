from rootalias import *

f_data = TFile('photon_timing.nd_spill_data.root')
f_mc = TFile('photon_timing.nd_spill_mc.root')
figure_dir = '/Users/juntinghuang/google_drive/slides/beamer/20170621_cosmic_timing/figures'

def plot_tns_distribution():
    h_data = f_data.Get('photontimingspillana/fSliceCellHitTns')
    h_mc = f_mc.Get('photontimingspillana/fSliceCellHitTns')
    h_data.Scale(1.0 / h_data.Integral())
    h_mc.Scale(1.0 / h_mc.Integral())

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    set_h1_style(h_data)
    h_data.SetLineColor(kRed)
    h_data.GetXaxis().SetRangeUser(1, 500)
    h_data.GetYaxis().SetRangeUser(0, max(h_data.GetMaximum(), h_mc.GetMaximum()) * 1.2)
    h_data.SetName('FD Cosmic Data')
    h_data.GetXaxis().SetTitle('Time to First Selected Cell Hit (ns)')
    h_data.GetYaxis().SetTitle('Cell Hit Count')
    h_data.GetYaxis().SetTitleOffset(1.6)
    h_data.Draw('hist')

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
    c1.SaveAs('{}/spill_plot_tns_distribution.pdf'.format(figure_dir))
    raw_input('Press any key to continue.')
    
plot_tns_distribution()
