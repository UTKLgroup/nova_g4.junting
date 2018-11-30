from rootalias import *

FIGURE_DIR = '/Users/juntinghuang/beamer/20181129_electron_gun_physics/figures'

def plot_dedx_vs_kinetic_energy():
    event_count = 10000

    h1 = TH2D('h1', 'h1', 23, 0.05, 2.35, 500, 0., 20.)
    for electron_kinetic_energy in np.arange(0.1, 2.4, 0.1):
        filename = '../cmake-build-debug/run.electron_gun.event_count_{}.electron_kinetic_energy_{:.1f}mev.root'.format(event_count, electron_kinetic_energy)
        print('filename = {}'.format(filename))
        tf = TFile(filename)
        for event in tf.Get('eventTree'):
            energy_deposition = event.energyDepositionPrimary
            track_length = event.trackLengthPrimary
            dedx = energy_deposition / track_length
            h1.Fill(electron_kinetic_energy, dedx)
        tf.Close()

    tp = h1.ProfileX()
    print('tp.GetBinContent(1) = {}'.format(tp.GetBinContent(1)))
    print('tp.GetNbinsX() = {}'.format(tp.GetNbinsX()))

    c1 = TCanvas('c1', 'c1', 600, 600)
    gPad.SetLogz()
    set_margin()
    set_h2_style(h1)
    set_h2_color_style()

    h1.GetYaxis().SetTitle('dE/dx (MeV / cm)')
    h1.GetXaxis().SetTitle('Kinetic Energy (MeV)')
    h1.GetYaxis().SetRangeUser(0, 10)
    h1.Draw('colz')

    c1.Update()
    c1.SaveAs('{}/plot_dedx_vs_kinetic_energy.th2.pdf'.format(FIGURE_DIR))

    c2 = TCanvas('c2', 'c2', 600, 600)
    gPad.SetGrid()
    set_margin()
    set_h1_style(tp)

    tp.GetYaxis().SetTitle('dE/dx (MeV / cm)')
    tp.GetXaxis().SetTitle('Kinetic Energy (MeV)')
    tp.GetYaxis().SetRangeUser(0, 10)
    tp.Draw()

    c2.Update()
    c2.SaveAs('{}/plot.electron_gun.tprofile.pdf'.format(FIGURE_DIR))
    input('Press any key to continue.')


def plot_light_yield_vs_kinetic_energy():
    event_count = 10000
    h_photon_count = TH2D('h_photon_count', 'h_photon_count', 23, 0.05, 2.35, 1000, 0., 1.e5)
    h_scintillation_photon_count = TH2D('h_scintillation_photon_count', 'h_scintillation_photon_count', 23, 0.05, 2.35, 1000, 0., 1.e5)
    h_cerenkov_photon_count = TH2D('h_cerenkov_photon_count', 'h_cerenkov_photon_count', 23, 0.05, 2.35, 1000, 0., 1.e5)

    for electron_kinetic_energy in np.arange(0.1, 2.4, 0.1):
        filename = '../cmake-build-debug/run.electron_gun.event_count_{}.electron_kinetic_energy_{:.1f}mev.root'.format(event_count, electron_kinetic_energy)
        print('filename = {}'.format(filename))
        tf = TFile(filename)
        for event in tf.Get('eventTree'):
            energy_deposition = event.energyDepositionPrimary
            track_length = event.trackLengthPrimary
            dedx = energy_deposition / track_length
            scintillation_photon_count = event.scintillationPhotonCount
            cerenkov_photon_count = event.cherenkovPhotonCount
            photon_count = scintillation_photon_count + cerenkov_photon_count

            h_photon_count.Fill(electron_kinetic_energy, photon_count / track_length)
            h_scintillation_photon_count.Fill(electron_kinetic_energy, scintillation_photon_count / track_length)
            h_cerenkov_photon_count.Fill(electron_kinetic_energy, cerenkov_photon_count / track_length)

    tp_photon_count = h_photon_count.ProfileX()
    tp_scintillation_photon_count = h_scintillation_photon_count.ProfileX()
    tp_cerenkov_photon_count = h_cerenkov_photon_count.ProfileX()

    c1 = TCanvas('c1', 'c1', 600, 600)
    set_margin()
    set_h2_color_style()
    set_h2_style(h_photon_count)
    h_photon_count.GetYaxis().SetMaxDigits(2)

    h_photon_count.GetXaxis().SetTitle('Kinetic Energy (MeV)')
    h_photon_count.GetYaxis().SetTitle('dN/dx (cm^{-1})')
    h_photon_count.Draw('colz')
    h_photon_count.GetYaxis().SetRangeUser(0, 4e4)
    h_photon_count.GetYaxis().SetTitleOffset(1.6)

    c1.Update()
    c1.SaveAs('{}/plot_light_yield_vs_kinetic_energy.th2.pdf'.format(FIGURE_DIR))

    lg1 = TLegend(0.22, 0.77, 0.7, 0.88)
    set_legend_style(lg1)

    c2 = TCanvas('c2', 'c2', 600, 600)
    set_margin()
    set_h1_style(tp_photon_count)
    tp_photon_count.GetYaxis().SetMaxDigits(2)
    tp_photon_count.GetXaxis().SetTitle('Kinetic Energy (MeV)')
    tp_photon_count.GetYaxis().SetTitle('dN/dx (cm^{-1})')
    tp_photon_count.Draw()
    tp_photon_count.GetYaxis().SetRangeUser(0, 4e4)
    tp_photon_count.GetYaxis().SetTitleOffset(1.6)
    tp_photon_count.SetLineColor(kBlack)
    lg1.AddEntry(tp_photon_count, 'scintillation + Cerenkov', 'l')

    set_h1_style(tp_scintillation_photon_count)
    tp_scintillation_photon_count.Draw('sames')
    tp_scintillation_photon_count.SetLineColor(kRed)
    lg1.AddEntry(tp_scintillation_photon_count, 'scintillation', 'l')
    lg1.Draw()

    c2.Update()
    c2.SaveAs('{}/plot_light_yield_vs_kinetic_energy.tprofile.pdf'.format(FIGURE_DIR))

    input('Press any key to continue.')


def plot_light_yield_vs_dedx():
    event_count = 10000

    h_dedx = TH2D('h_dedx', 'h_dedx', 23, 0.05, 2.35, 500, 0., 20.)
    h_photon_count = TH2D('h_photon_count', 'h_photon_count', 23, 0.05, 2.35, 1000, 1e3, 1.e5)
    h_scintillation_photon_count = TH2D('h_scintillation_photon_count', 'h_scintillation_photon_count', 23, 0.05, 2.35, 1000, 0., 1.e5)
    h_cerenkov_photon_count = TH2D('h_cerenkov_photon_count', 'h_cerenkov_photon_count', 23, 0.05, 2.35, 1000, 0., 1.e5)
    for electron_kinetic_energy in np.arange(0.1, 2.4, 0.1):
        filename = '../cmake-build-debug/run.electron_gun.event_count_{}.electron_kinetic_energy_{:.1f}mev.root'.format(event_count, electron_kinetic_energy)
        print('filename = {}'.format(filename))
        tf = TFile(filename)
        for event in tf.Get('eventTree'):
            energy_deposition = event.energyDepositionPrimary
            track_length = event.trackLengthPrimary
            dedx = energy_deposition / track_length
            scintillation_photon_count = event.scintillationPhotonCount
            cerenkov_photon_count = event.cherenkovPhotonCount
            photon_count = scintillation_photon_count + cerenkov_photon_count

            h_dedx.Fill(electron_kinetic_energy, dedx)
            h_photon_count.Fill(electron_kinetic_energy, photon_count / track_length)
            h_cerenkov_photon_count.Fill(electron_kinetic_energy, cerenkov_photon_count / track_length)
            h_scintillation_photon_count.Fill(electron_kinetic_energy, scintillation_photon_count / track_length)
        # break

    tp_dedx = h_dedx.ProfileX()
    tp_photon_count = h_photon_count.ProfileX()
    tp_scintillation_photon_count = h_scintillation_photon_count.ProfileX()
    tp_cerenkov_photon_count = h_cerenkov_photon_count.ProfileX()

    dedxs = []
    photon_counts = []
    scintillation_photon_counts = []
    cerenkov_photon_counts = []
    for i in range(1, tp_dedx.GetNbinsX() + 1):
        dedxs.append(tp_dedx.GetBinContent(i))
        photon_counts.append(tp_photon_count.GetBinContent(i))
        scintillation_photon_counts.append(tp_scintillation_photon_count.GetBinContent(i))
        cerenkov_photon_counts.append(tp_cerenkov_photon_count.GetBinContent(i))

    gr_photon_count = TGraph(len(dedxs), np.array(dedxs), np.array(photon_counts))
    gr_scintillation_photon_count = TGraph(len(dedxs), np.array(dedxs), np.array(scintillation_photon_counts))

    # f1 = TF1('f1', 'pol1', 0., 2.)
    # f1.SetLineColor(kBlue)

    lg1 = TLegend(0.17, 0.72, 0.65, 0.86)
    set_legend_style(lg1)

    lg1.Draw()


    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    set_graph_style(gr_photon_count)
    set_graph_style(gr_scintillation_photon_count)

    gr_photon_count.GetYaxis().SetTitleOffset(1.7)
    gr_photon_count.GetXaxis().SetTitle('dE/dx (MeV/cm)')
    gr_photon_count.GetYaxis().SetTitle('dN/dx (cm^{-1})')
    gr_photon_count.Draw('ALP')
    lg1.AddEntry(gr_photon_count, 'scintillation + Cerenkov', 'lp')

    gr_scintillation_photon_count.SetLineColor(kRed)
    gr_scintillation_photon_count.SetMarkerColor(kRed)
    gr_scintillation_photon_count.Draw('LP,sames')
    lg1.AddEntry(gr_scintillation_photon_count, 'scintillation', 'lp')

    lg1.Draw()

    c1.Update()
    c1.SaveAs('{}/plot_light_yield_vs_dedx.pdf'.format(FIGURE_DIR))
    input('Press any key to continue.')


gStyle.SetOptStat(0)
# plot_dedx_vs_kinetic_energy()
plot_light_yield_vs_kinetic_energy()
# plot_light_yield_vs_dedx()
