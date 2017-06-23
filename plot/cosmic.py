from rootalias import *
import numpy as np

f_data = TFile('photon_timing_data.costheta.root')
f_mc = TFile('photon_timing_mc.costheta.root')

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
    c1.SaveAs('figures/plot_slice_duration.width_{}.pdf'.format(width))
    raw_input('Press any key to continue.')

def plot_cell_count():
    h_data_ncell = f_data.Get('photontimingana/fCellCountPerSlice')
    h_mc_ncell = f_mc.Get('photontimingana/fCellCountPerSlice')
    h_data_ncell.Scale(1.0 / h_data_ncell.Integral())
    h_mc_ncell.Scale(1.0 / h_mc_ncell.Integral())

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
    set_h1_style(h_data_ncell)
    h_data_ncell.SetLineColor(kRed)
    h_data_ncell.SetMarkerColor(kRed)
    h_data_ncell.GetYaxis().SetTitle('Track Count')
    h_data_ncell.GetYaxis().SetRangeUser(-0.02, max(h_data_ncell.GetMaximum(), h_mc_ncell.GetMaximum()) * 1.2)
    h_data_ncell.GetXaxis().SetRangeUser(0.0, 30)
    h_data_ncell.GetYaxis().SetTitleOffset(1.5)
    h_data_ncell.Draw("E")

    set_h1_style(h_mc_ncell)
    h_mc_ncell.Draw("sames, E")

    lg1 = TLegend(0.7, 0.5, 0.9, 0.8)
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
    h_ratio.GetYaxis().SetRangeUser(0., 2.2)
    h_ratio.SetLineColor(kBlack)
    h_ratio.SetTitle('')
    h_ratio.GetYaxis().SetNdivisions(205, 1)
    h_ratio.GetXaxis().SetTitle('Selected Cell Hit Count per Track')
    h_ratio.GetYaxis().SetTitle('data / MC')
    h_ratio.GetXaxis().SetTitleOffset(3)
    h_ratio.Draw('ep')

    c3.Update()
    c3.SaveAs('figures/plot_cell_count.cosmic.pdf')
    raw_input('Press any key to continue.')

def plot_cell_hits(view):
    cells = []
    planes = []
    selected_cells = []
    selected_planes = []

    for cell_hit in f_data.Get('photontimingcosmicana/fCellHitTree'):
        if (cell_hit.cellHitView == view):
            cells.append(float(cell_hit.cellHitCell))
            planes.append(float(cell_hit.cellHitPlane))
            if cell_hit.cellHitSelected == 1:
                selected_cells.append(float(cell_hit.cellHitCell))
                selected_planes.append(float(cell_hit.cellHitPlane))

    gr_cell_plane = TGraph(len(cells), np.asarray(planes), np.asarray(cells))
    gr_selected_cell_plane = TGraph(len(selected_cells), np.array(selected_planes), np.array(selected_cells))

    gStyle.SetOptStat(0)
    c1 = TCanvas('c1', 'c1', 800, 800)
    set_margin()

    set_graph_style(gr_cell_plane)
    gr_cell_plane.SetMarkerStyle(20)
    gr_cell_plane.SetMarkerSize(0.25)
    gr_cell_plane.Draw('AP')
    gr_cell_plane.GetXaxis().SetTitle('Plane Number')
    gr_cell_plane.GetYaxis().SetTitle('Cell Number')
    gr_cell_plane.GetYaxis().SetRangeUser(0, 480)
    gr_cell_plane.GetXaxis().SetTitleOffset(1.4)
    gr_cell_plane.GetYaxis().SetTitleOffset(2)

    set_graph_style(gr_selected_cell_plane)
    gr_selected_cell_plane.SetMarkerStyle(20)
    gr_selected_cell_plane.SetMarkerSize(0.3)
    gr_selected_cell_plane.SetMarkerColor(kRed)
    gr_selected_cell_plane.Draw('P')

    lg1 = TLegend(0.4, 0.8, 0.88, 0.88)
    set_legend_style(lg1)
    lg1.SetFillStyle(1001)
    lg1.SetBorderSize(1)
    lg1.SetMargin(0.15)
    lg1.AddEntry(gr_cell_plane, 'cell hits', 'p')
    lg1.AddEntry(gr_selected_cell_plane, 'cell hits in range', 'p')
    lg1.SetNColumns(2)
    lg1.Draw()

    c1.Update()
    c1.SaveAs('figures/plot_cell_hits_{}.pdf'.format('x_view' if view == 0 else 'y_view'))
    raw_input('Press any key to continue.')

# plot_slice_duration('60')
# plot_slice_duration('70')
# plot_slice_duration('80')
# plot_slice_duration('90')
# plot_slice_duration('')
plot_cell_count()
# plot_cell_hits(0)
