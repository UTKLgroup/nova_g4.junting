from rootalias import *
from pprint import pprint


def get_event_tnss(filename):
    event_tnss = []
    slice_tnss = []
    cell_tnss = []
    with open(filename) as f_tns:
        for row in f_tns:
            row = row.strip()
            if row.startswith('Cell'):
                cell_tnss.append(float(row.split()[2]))
            if row.startswith('MinTNS'):
                slice_tnss.append(cell_tnss)
                cell_tnss = []
            if row.startswith('finishing'):
                event_tnss.append(slice_tnss)
                slice_tnss = []
    return event_tnss

def get_slice_tnss(filename):
    slice_tnss = []
    for event_tns in get_event_tnss(filename):
        slice_tnss += event_tns
    return slice_tnss

def get_slice_durations(filename):
    slice_durations = []
    for slice_tns in get_slice_tnss(filename):
        slice_durations.append(max(slice_tns) - min(slice_tns))
    return sorted(slice_durations)

def get_h1_slice_durations(filename):
    h1 = TH1D('h1', 'h1', 50, 0, 500)
    for slice_duration in get_slice_durations(filename):
        h1.Fill(slice_duration)
    return h1

def plot_slice_durations():
    h1 = get_h1_slice_durations('tns40.txt')
    h2 = get_h1_slice_durations('tns35.txt')
    h3 = get_h1_slice_durations('tns41.txt')

    h1.Scale(h3.Integral() / h1.Integral())
    h1.Scale(float(len(get_event_tnss('tns41.txt'))) / len(get_event_tnss('tns40.txt')))

    c1 = TCanvas('c1', 'c1', 800, 600)
    h1.SetLineColor(kBlack)
    h1.GetYaxis().SetRangeUser(0, h1.GetMaximum() * 1.2)
    h1.SetName('h1')
    h1.Draw()

    h2.SetLineColor(kBlue)
    h2.SetName('h2')
    h2.Draw('sames')

    h3.SetLineColor(kRed)
    h3.SetName('h3')
    h3.Draw('sames')

    c1.Update()
    raw_input('Press anykey to continue.\n')

def get_h1_cell_tns(filename):
    h_cell_tns = TH1D('h_cell_tns', 'h_cell_tns', 50, 0, 200)
    for cell_tnss in get_slice_tnss(filename):
        min_cell_tns = min(cell_tnss)
        cell_tnss = sorted(map(lambda x: x - min_cell_tns, cell_tnss))
        for cell_tns in cell_tnss:
            h_cell_tns.Fill(cell_tns)
            
    return h_cell_tns

def plot_cell_tns():
    h1 = get_h1_cell_tns('tns40.txt')
    h2 = get_h1_cell_tns('tns37.txt')
    h3 = get_h1_cell_tns('tns41.txt')

    c1 = TCanvas('c1', 'c1', 800, 600)
    h1.SetLineColor(kBlack)
    h1.SetName('h1')
    h1.GetYaxis().SetRangeUser(0, h1.GetMaximum() * 1.2)
    h1.Draw()

    h2.SetLineColor(kBlue)
    h2.SetName('h2')
    h2.Draw('sames')

    h3.SetLineColor(kRed)
    h3.SetName('h3')
    h3.Draw('sames')
    
    c1.Update()
    raw_input('Press anykey to continue.\n')

def get_h1_absolute_cell_tns(filename):
    h_absolute_cell_tns = TH1D('h_absolute_cell_tns', 'h_absolute_cell_tns', 100, 210000, 230000)
    for cell_tnss in get_slice_tnss(filename):
        for cell_tns in cell_tnss:
            h_absolute_cell_tns.Fill(cell_tns)
    return h_absolute_cell_tns

def plot_absolute_cell_tns():
    h1 = get_h1_absolute_cell_tns('tns36.txt')
    h2 = get_h1_absolute_cell_tns('tns35.txt')
    h3 = get_h1_absolute_cell_tns('tns37.txt')

    c1 = TCanvas('c1', 'c1', 800, 600)
    h1.SetLineColor(kBlack)
    h1.SetName('h1')
    h1.GetYaxis().SetRangeUser(0, h1.GetMaximum() * 1.2)
    h1.Draw()

    h2.SetLineColor(kBlue)
    h2.SetName('h2')
    h2.Draw('sames')

    h3.SetLineColor(kRed)
    h3.SetName('h3')
    h3.Draw('sames')

    c1.Update()
    raw_input('Press anykey to continue.\n')

def print_run_info(filename):
    print len(get_event_tnss(filename))
    for i, event_tns in enumerate(get_event_tnss('tns32.txt')):
        print '\nevent {} has {} slices'.format(i, len(event_tns))
        for j, slice_tns in enumerate(event_tns):
            print 'slice {} has {} cell hits, with mean {}'.format(j, len(slice_tns), sum(slice_tns) / len(slice_tns))

def plot_effect_of_20_wls():
    gStyle.SetOptStat('nmri')

    f_mc_20wls = TFile('data/photon_timing_mc.40.root')
    h_mc_20wls = f_mc_20wls.Get('photontimingana/fSliceDuration')
    h_mc_20wls_slice_count = f_mc_20wls.Get('photontimingana/fSliceCount')
    mc_20wls_event_count = h_mc_20wls_slice_count.GetEntries()

    f_mc = TFile('data/photon_timing_mc.45.root')
    h_mc = f_mc.Get('photontimingana/fSliceDuration')
    h_mc_slice_count = f_mc.Get('photontimingana/fSliceCount')
    mc_event_count = h_mc_slice_count.GetEntries()

    f_data = TFile('data/photon_timing_data_1.root')
    h_data = f_data.Get("photontimingana/fSliceDuration")
    h_data_slice_count = f_data.Get("photontimingana/fSliceCount")
    data_event_count = h_data_slice_count.GetEntries()

    h_mc_20wls.Rebin(2)
    h_mc.Rebin(2)
    h_data.Rebin(2)

    print 'mc_20wls_event_count = ', mc_20wls_event_count
    print 'mc_event_count = ', mc_event_count
    print 'data_event_count = ', data_event_count

    h_mc_20wls.Scale(float(mc_event_count) / mc_20wls_event_count)
    h_data.Scale(float(mc_event_count) / data_event_count)

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()

    set_h1_style(h_mc_20wls)
    h_mc_20wls.GetYaxis().SetRangeUser(0, 1200)
    h_mc_20wls.SetLineColor(kRed + 2)
    h_mc_20wls.SetLineWidth(3)
    h_mc_20wls.SetName('h1')
    h_mc_20wls.GetXaxis().SetTitle('Slice Duration (ns)')
    h_mc_20wls.GetYaxis().SetTitle('Slice Count')
    h_mc_20wls.SetName('MC with 20 WLS')
    h_mc_20wls.Draw('hist')

    set_h1_style(h_mc)
    h_mc.SetLineColor(kBlue + 2)
    h_mc_20wls.SetLineWidth(3)
    h_mc.SetName('Default MC')
    h_mc.Draw('sames, hist')

    set_h1_style(h_data)
    h_data.SetLineColor(kGreen + 2)
    h_data.SetName('Default Data')
    h_data.Draw('sames')

    c1.Update()
    p_data = h_data.GetListOfFunctions().FindObject("stats")
    p_data.SetTextColor(kGreen + 2)
    p_data.SetLineColor(kGreen + 2)
    p_data.SetX1NDC(0.72)
    p_data.SetY1NDC(0.75)
    p_data.SetX2NDC(0.95)
    p_data.SetY2NDC(0.95)
    p_data.Draw()

    p_mc = h_mc.GetListOfFunctions().FindObject("stats")
    p_mc.SetTextColor(kBlue + 2)
    p_mc.SetLineColor(kBlue + 2)
    p_mc.SetX1NDC(0.72)
    p_mc.SetY1NDC(0.51)
    p_mc.SetX2NDC(0.95)
    p_mc.SetY2NDC(0.71)
    p_mc.Draw()

    p_mc_20wls = h_mc_20wls.GetListOfFunctions().FindObject("stats")
    p_mc_20wls.SetTextColor(kRed + 2)
    p_mc_20wls.SetLineColor(kRed + 2)
    p_mc_20wls.SetX1NDC(0.72)
    p_mc_20wls.SetY1NDC(0.27)
    p_mc_20wls.SetX2NDC(0.95)
    p_mc_20wls.SetY2NDC(0.47)
    p_mc_20wls.Draw()

    c1.Update()
    c1.SaveAs('figures/plot_effect_of_20_wls.pdf')
    raw_input('Press anykey to continue.\n')

def plot_data_vs_mc(h_data, h_mc, x_title, y_title, name_data, name_mc, figure_filename):
    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()
    h_data.SetLineColor(kRed + 2)
    h_data.GetXaxis().SetRangeUser(0, 300)
    h_data.GetYaxis().SetRangeUser(0, max(h_data.GetMaximum(), h_mc.GetMaximum()) * 1.2)
    h_data.GetXaxis().SetTitle(x_title)
    h_data.GetYaxis().SetTitle(y_title)
    set_h1_style(h_data)
    h_data.SetName(name_data)
    h_data.Draw()

    set_h1_style(h_mc)
    h_mc.SetName(name_mc)
    h_mc.Draw('sames')

    c1.Update()
    p_data = h_data.GetListOfFunctions().FindObject("stats")
    p_data.SetTextColor(kRed + 2)
    p_data.SetLineColor(kRed + 2)
    p_data.SetX1NDC(0.72)
    p_data.SetY1NDC(0.75)
    p_data.SetX2NDC(0.95)
    p_data.SetY2NDC(0.95)
    p_data.Draw()

    p_mc = h_mc.GetListOfFunctions().FindObject("stats")
    p_mc.SetTextColor(kBlue + 2)
    p_mc.SetLineColor(kBlue + 2)
    p_mc.SetX1NDC(0.72)
    p_mc.SetY1NDC(0.51)
    p_mc.SetX2NDC(0.95)
    p_mc.SetY2NDC(0.71)
    p_mc.Draw()

    c1.Update()
    c1.SaveAs(figure_filename)
    raw_input('Press anykey to continue.\n')


def plot_effect_of_removing_cell_hit():
    gStyle.SetOptStat('nmri')

    f_mc = TFile('data/photon_timing_mc.45.root')
    h_mc_slice_duration = f_mc.Get('photontimingana/fSliceDuration')
    h_mc_slice_duration_minus_1 = f_mc.Get('photontimingana/fSliceDurationMinus1')
    h_mc_slice_duration_minus_2 = f_mc.Get('photontimingana/fSliceDurationMinus2')
    h_slice_count = f_mc.Get('photontimingana/fSliceCount')
    mc_event_count = h_slice_count.GetEntries()

    f_data = TFile('data/photon_timing.root')
    h_data_slice_duration = f_data.Get('photontimingana/fSliceDuration')
    h_data_slice_duration_minus_1 = f_data.Get('photontimingana/fSliceDurationMinus1')
    h_data_slice_duration_minus_2 = f_data.Get('photontimingana/fSliceDurationMinus2')
    h_slice_count = f_data.Get('photontimingana/fSliceCount')
    data_event_count = h_slice_count.GetEntries()

    print 'mc_event_count = ', mc_event_count
    print 'data_event_count = ', data_event_count

    h_data_slice_duration.Scale(mc_event_count / data_event_count)
    h_data_slice_duration_minus_1.Scale(mc_event_count / data_event_count)
    h_data_slice_duration_minus_2.Scale(mc_event_count / data_event_count)

    plot_data_vs_mc(h_data_slice_duration, h_mc_slice_duration,
                    'Slice Duration (ns)', 'Slice Count',
                    'Data', 'MC', 'figures/plot_effect_of_removing_cell_hit.pdf')

    plot_data_vs_mc(h_data_slice_duration_minus_1, h_mc_slice_duration_minus_1,
                    'Slice Duration (ns)', 'Slice Count',
                    'Data', 'MC', 'figures/plot_effect_of_removing_cell_hit_minus_1.pdf')

    plot_data_vs_mc(h_data_slice_duration_minus_2, h_mc_slice_duration_minus_2,
                    'Slice Duration (ns)', 'Slice Count',
                    'Data', 'MC', 'figures/plot_effect_of_removing_cell_hit_minus_2.pdf')


# pprint(sorted(map(lambda x: len(x), get_slice_tnss('tns21.txt'))))
# pprint(sorted(map(lambda x: len(x), get_slice_tnss('tns23.txt'))))
# pprint(sorted(map(lambda x: len(x), get_slice_tnss('tns20.txt'))))
# pprint(len(sorted(map(lambda x: len(x), get_slice_tnss('tns36.txt')))))
# pprint(len(sorted(map(lambda x: len(x), get_slice_tnss('tns35.txt')))))
# pprint(len(sorted(map(lambda x: len(x), get_slice_tnss('tns29.txt')))))
# pprint(sum(map(lambda x: len(x), get_slice_tnss('tns30.txt'))))
# pprint(sum(map(lambda x: len(x), get_slice_tnss('tns29.txt'))))
# pprint(get_slice_tnss('tns20.txt'))
# pprint(get_slice_durations('tns22.txt'))
# pprint(get_slice_durations('tns21.txt'))

# plot_slice_durations()
# plot_cell_tns()
# plot_absolute_cell_tns()
# plot_effect_of_20_wls()
plot_effect_of_removing_cell_hit()
# print_run_info('tns32.txt')
