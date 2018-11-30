from rootalias import *

FIGURE_DIR = '/Users/juntinghuang/beamer/20181129_electron_gun_physics/figures'

def plot_dedx_vs_kinetic_energy():
    event_count = 1000
    
    h1 = TH2D('h1', 'h1', 23, 0.05, 2.35, 100, 0., 10.)
    for electron_kinetic_energy in np.arange(0.1, 2.4, 0.1):
        filename = '../cmake-build-debug/run.electron_gun.event_count_{}.electron_kinetic_energy_{:.1f}mev.root'.format(event_count, electron_kinetic_energy)
        print('filename = {}'.format(filename))
        tf = TFile(filename)
        for event in tf.Get('eventTree'):
            energy_deposition = event.energyDepositionPrimary
            track_length = event.trackLengthPrimary
            dedx = energy_deposition / track_length
            h1.Fill(electron_kinetic_energy, dedx)
        break
    
    tp = h1.ProfileX()
    print('tp.GetBinContent(1) = {}'.format(tp.GetBinContent(1)))
    print('tp.GetNbinsX() = {}'.format(tp.GetNbinsX()))

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    set_h2_style(h1)
    set_h2_color_style()
    
    h1.Draw('colz')
    
    c1.Update()
    c1.SaveAs('{}/plot_dedx_vs_kinetic_energy.th2.pdf'.format(FIGURE_DIR))

    c2 = TCanvas('c2', 'c2', 800, 600)
    set_margin()
    set_h1_style(tp)

    tp.Draw()

    c2.Update()
    c2.SaveAs('{}/plot.electron_gun.tprofile.pdf'.format(FIGURE_DIR))
    input('Press any key to continue.')


def plot_light_yield_vs_dedx():
    event_count = 1000
    
    h_dedx = TH2D('h_dedx', 'h_dedx', 23, 0.05, 2.35, 100, 0., 10.)
    h_scintillation_cerenkov = TH2D('h_scintillation_cerenkov', 'h_scintillation_cerenkov', 23, 0.05, 2.35, 100, 0., 1.e4)
    for electron_kinetic_energy in np.arange(0.1, 2.4, 0.1):
        filename = '../cmake-build-debug/run.electron_gun.event_count_{}.electron_kinetic_energy_{:.1f}mev.root'.format(event_count, electron_kinetic_energy)
        print('filename = {}'.format(filename))
        tf = TFile(filename)
        for event in tf.Get('eventTree'):
            energy_deposition = event.energyDepositionPrimary
            track_length = event.trackLengthPrimary
            dedx = energy_deposition / track_length
            h_dedx.Fill(electron_kinetic_energy, dedx)
            h_scintillation_cerenkov.Fill(electron_kinetic_energy, (event.scintillationPhotonCount + event.cherenkovPhotonCount) / track_length)
        # break
    
    tp_dedx = h_dedx.ProfileX()
    tp_scintillation_cerenkov = h_scintillation_cerenkov.ProfileX()
    
    dedxs = []
    scintillation_cerenkovs = []
    for i in range(1, tp_dedx.GetNbinsX() + 1):
        dedxs.append(tp_dedx.GetBinContent(i))
        scintillation_cerenkovs.append(tp_scintillation_cerenkov.GetBinContent(i))

    print('scintillation_cerenkovs = {}'.format(scintillation_cerenkovs))
    print('dedxs = {}'.format(dedxs))
    gr = TGraph(len(dedxs), np.array(dedxs), np.array(scintillation_cerenkovs))

    # c1 = TCanvas('c1', 'c1', 800, 600)
    # set_margin()
    # # h_scintillation_cerenkov.Draw()
    # # tp_scintillation_cerenkov.Draw()
    # tp_dedx.Draw()
    # c1.Update()
    # c1.SaveAs('{}/plot_light_yield_vs_dedx.pdf'.format(FIGURE_DIR))

    c2 = TCanvas('c2', 'c2', 800, 600)
    set_margin()

    gr.Draw('AL*')

    c2.Update()
    c2.SaveAs('{}/plot_light_yield_vs_dedx.pdf'.format(FIGURE_DIR))
    input('Press any key to continue.')

gStyle.SetOptStat(0)
# plot_dedx_vs_kinetic_energy()
plot_light_yield_vs_dedx()
