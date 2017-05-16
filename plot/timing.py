from rootalias import *
import numpy as np
import math
from pprint import pprint
from scipy.stats import rv_discrete
from random import uniform


def plot_smear():
    f_smear = TFile('Dt_per_z_distribution.root')
    h_smear = f_smear.Get('Dt_per_z')

    c1 = TCanvas('c1', 'c1', 800, 600)
    h_smear.Draw()

    c1.SaveAs('figures/plot_smear.pdf')
    raw_input('Press any key to continue')

def plot_collect():
    gStyle.SetOptStat(0)
    f_collect = TFile('dT_dZ_CollectionRate.root')
    h_collect = f_collect.Get('dT_dZ_CollectionRate')

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    gPad.SetRightMargin(0.2)
    set_h2_color_style()
    set_h2_style(h_collect)
    h_collect.Draw('colz')
    c1.SaveAs('figures/plot_collect.pdf')
    raw_input('Press any key to continue')

def plot_emission_tau_effect():
    f_9 = TFile('data/photon_timing_mc_1.root')
    h_9 = f_9.Get("photontimingana/fSliceDuration")

    f_12 = TFile('data/photon_timing_mc.emission_tau_11_point_8.2.root')
    h_12 = f_12.Get("photontimingana/fSliceDuration")
    
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()

    set_h1_style(h_9)
    h_9.SetTitle('Effect of EmissionTau in MC')
    h_9.GetXaxis().SetTitle('Slice Duration (ns)')
    h_9.GetYaxis().SetTitle('Slice Count')
    h_9.SetName('9 ns')
    h_9.GetXaxis().SetRangeUser(0, 300)
    h_9.GetYaxis().SetRangeUser(0, 650)
    h_9.Draw()

    set_h1_style(h_12)
    h_12.SetName('11.8 ns')
    h_12.SetLineColor(kRed + 2)
    h_12.Draw('sames')
    
    c1.Update()
    p_9 = h_9.GetListOfFunctions().FindObject("stats")
    p_9.SetTextColor(kBlue + 2)
    p_9.SetLineColor(kBlue + 2)
    p_9.SetX1NDC(0.72)
    p_9.SetY1NDC(0.75)
    p_9.SetX2NDC(0.95)
    p_9.SetY2NDC(0.95)
    p_9.Draw()

    p_12 = h_12.GetListOfFunctions().FindObject("stats")
    p_12.SetTextColor(kRed + 2)
    p_12.SetLineColor(kRed + 2)
    p_12.SetX1NDC(0.72)
    p_12.SetY1NDC(0.51)
    p_12.SetX2NDC(0.95)
    p_12.SetY2NDC(0.71)
    p_12.Draw()

    c1.SaveAs('figures/plot_emission_tau_effect.pdf')
    c1.SaveAs('figures/plot_emission_tau_effect.png')
    raw_input('Press any key to continue')

def plot_noise_effect():
    f_standard = TFile('photon_timing_mc_1.root')
    h_standard = f_standard.Get("photontimingana/fSliceDuration")

    f_noiseless = TFile('photon_timing_mc.noiseless_rsim.root')
    h_noiseless = f_noiseless.Get("photontimingana/fSliceDuration")
    
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()

    set_h1_style(h_standard)
    h_standard.SetTitle('Effect of Noise')
    h_standard.GetXaxis().SetTitle('Slice Duration (ns)')
    h_standard.GetYaxis().SetTitle('Slice Count')
    h_standard.SetName('standard')
    h_standard.GetXaxis().SetRangeUser(0, 300)
    h_standard.GetYaxis().SetRangeUser(0, 1500)
    h_standard.Draw()

    set_h1_style(h_noiseless)
    h_noiseless.SetName('noiseless')
    h_noiseless.SetLineColor(kRed + 2)
    h_noiseless.Draw('sames')
    
    c1.Update()
    p_standard = h_standard.GetListOfFunctions().FindObject("stats")
    p_standard.SetTextColor(kBlue + 2)
    p_standard.SetLineColor(kBlue + 2)
    p_standard.SetX1NDC(0.72)
    p_standard.SetY1NDC(0.75)
    p_standard.SetX2NDC(0.95)
    p_standard.SetY2NDC(0.95)
    p_standard.Draw()

    p_noiseless = h_noiseless.GetListOfFunctions().FindObject("stats")
    p_noiseless.SetTextColor(kRed + 2)
    p_noiseless.SetLineColor(kRed + 2)
    p_noiseless.SetX1NDC(0.72)
    p_noiseless.SetY1NDC(0.51)
    p_noiseless.SetX2NDC(0.95)
    p_noiseless.SetY2NDC(0.71)
    p_noiseless.Draw()

    c1.SaveAs('figures/plot_noise_effect.pdf')
    raw_input('Press any key to continue')

def get_random_samples(root_filename, h1_name, count):
    f_pdf = TFile(root_filename)
    h_pdf = f_pdf.Get(h1_name)
    random_numbers = []
    bin_centers = []
    bin_contents = []
    half_bin_width = None
    for i in range(1, h_pdf.GetNbinsX() + 1):
        bin_centers.append(h_pdf.GetBinCenter(i))
        bin_contents.append(h_pdf.GetBinContent(i))
        if i == 1:
            half_bin_width = h_pdf.GetBinWidth(i) / 2.
    
    for random_number in np.random.choice(bin_centers, size=count, p=bin_contents):
        random_numbers.append(uniform(random_number - half_bin_width, random_number + half_bin_width))

    return random_numbers

def test_random_draw():
    f_pdf = TFile('Dt_per_z_distribution.root')
    h_pdf = f_pdf.Get('Dt_per_z')
    
    h1 = TH1D('h1', 'h1', h_pdf.GetNbinsX() * 2, 0, 0.01)
    for random_number in get_random_samples('Dt_per_z_distribution.root', 'Dt_per_z', 100000):
        h1.Fill(random_number)
    h1.Scale(2. / h1.Integral())

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    gPad.SetLogy()
    set_h1_style(h_pdf)
    h_pdf.Draw()
    h1.Draw('sames')
    c1.Update()
    c1.SaveAs('figures/test_random_draw.pdf')
    raw_input()

def get_fiber_transmission(x):
    return 0.2667 * math.exp(-x / 254.) + 0.2139 * math.exp(-x / 860.)

def test_fiber_transmission():
    lengths = []
    transmissions = []
    for i in np.arange(0, 17, 0.5):
        i = i * 100.
        lengths.append(i)
        transmissions.append(get_fiber_transmission(i))
        print i, get_fiber_transmission(i)

    gr1 = TGraph(len(lengths), np.asarray(lengths), np.asarray(transmissions))
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    set_graph_style(gr1)
    gr1.Draw('AL')
    gr1.GetXaxis().SetTitle('Fiber Length (cm)')
    gr1.GetYaxis().SetTitle('Transmission')
    gr1.GetXaxis().SetRangeUser(0, 1600)
    c1.Update()
    c1.SaveAs('figures/test_fiber_transmission.pdf')
    raw_input()

def test_random_exponential():
    h1 = TH1D('h1', 'h1', 200, 0, 100)
    emission_tau = 11.8
    for random_number in  np.random.exponential(scale = emission_tau, size = 100000):
        h1.Fill(random_number)

    h1.Scale(1. / h1.Integral() / h1.GetBinWidth(1))
    tf1_exp = TF1('tf1_exp', '1. / 11.8 * exp(-x / 11.8)', 0, 100)

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    h1.Draw()
    tf1_exp.Draw('sames')

    c1.Update()
    c1.SaveAs('figures/test_random_exponential.pdf')
    raw_input()

def get_h1_model_timing(photon_count, distance):
    emission_tau = 11.8
    fiber_index_of_refraction = 1.59
    speed_of_light = 29.9792458
    speed_of_light_fiber = speed_of_light / fiber_index_of_refraction
    base_time = distance / speed_of_light_fiber
    dt_per_zs = get_random_samples('data/Dt_per_z_distribution.root', 'Dt_per_z', photon_count)

    h_time = TH1D('h_time', 'h_time', 100, 0, 100)
    for dt_per_z in dt_per_zs:
        time = base_time
        time += np.random.exponential(emission_tau)

        fiber_spread = 0.
        weight = -1.
        uweight = 0.
        while weight < uweight:
    	    fiber_spread = distance * dt_per_z
    	    delta_distance = fiber_spread * speed_of_light_fiber
    	    weight = get_fiber_transmission(distance + delta_distance) / get_fiber_transmission(distance)
    	    uweight = uniform(0, 1)
        time += fiber_spread
        h_time.Fill(time)

    return h_time

def get_h1_multiple_wls_model_timing(photon_count, distance):
    emission_tau = 11.8
    fiber_index_of_refraction = 1.59
    speed_of_light = 29.9792458
    speed_of_light_fiber = speed_of_light / fiber_index_of_refraction
    base_time = distance / speed_of_light_fiber
    dt_per_zs = get_random_samples('data/Dt_per_z_distribution.root', 'Dt_per_z', photon_count)

    h_time = TH1D('h_time', 'h_time', 100, 0, 100)
    for dt_per_z in dt_per_zs:
        time = base_time
        time += np.random.exponential(emission_tau)
        time += np.random.exponential(emission_tau)
        time += np.random.exponential(emission_tau)

        fiber_spread = 0.
        weight = -1.
        uweight = 0.
        while weight < uweight:
    	    fiber_spread = distance * dt_per_z
    	    delta_distance = fiber_spread * speed_of_light_fiber
    	    weight = get_fiber_transmission(distance + delta_distance) / get_fiber_transmission(distance)
    	    uweight = uniform(0, 1)
        time += fiber_spread
        h_time.Fill(time)

    return h_time

def get_h1_hit_time(root_filename, photon_count):
    h_hit_time = TH1D('h_hit_time', 'h_hit_time', 100, 0, 100)
    f_event = TFile(root_filename)
    counter = 0
    for event in f_event.eventTree:
        h_hit_time.Fill(event.hitTime)
        counter += 1
        if counter >= photon_count:
            break
    return h_hit_time

def plot_timing():
    photon_count = 10000
    # fiber_length = 400
    fiber_length = 100
    h_simulation = get_h1_hit_time('../cmake-build-debug/run_spectrum.470nm.{}cm.root'.format(fiber_length), photon_count)
    # h_model = get_h1_model_timing(int(h_simulation.Integral()), fiber_length)
    h_model = get_h1_multiple_wls_model_timing(int(h_simulation.Integral()), fiber_length)

    print h_simulation
    print type(h_simulation)
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()

    set_h1_style(h_model)
    h_model.Draw()
    h_model.GetYaxis().SetRangeUser(0, 1500)
    h_model.GetXaxis().SetTitle('')
    h_simulation.Draw('sames')

    c1.Update()
    c1.SaveAs('figures/plot_timing.pdf')
    raw_input()

# plot_smear()
# plot_collect()
# plot_emission_tau_effect()
# plot_noise_effect()
# get_random_samples('Dt_per_z_distribution.root', 'Dt_per_z', 10)
# test_random_draw()
# test_fiber_transmission()
# test_random_exponential()
plot_timing()
