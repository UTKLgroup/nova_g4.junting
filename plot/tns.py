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
    # h2 = get_h1_slice_durations('tns35.txt')
    h3 = get_h1_slice_durations('tns41.txt')

    # h1.Scale(h3.Integral() / h1.Integral())
    h1.Scale(float(len(get_event_tnss('tns41.txt'))) / len(get_event_tnss('tns40.txt')))

    c1 = TCanvas('c1', 'c1', 800, 600)
    h1.SetLineColor(kBlack)
    h1.GetYaxis().SetRangeUser(0, h1.GetMaximum() * 1.2)
    h1.SetName('h1')
    h1.Draw()

    # h2.SetLineColor(kBlue)
    # h2.SetName('h2')
    # h2.Draw('sames')

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

def get_h1_absolute_cell_tns(filename):
    h_absolute_cell_tns = TH1D('h_absolute_cell_tns', 'h_absolute_cell_tns', 100, 210000, 230000)
    for cell_tnss in get_slice_tnss(filename):
        for cell_tns in cell_tnss:
            h_absolute_cell_tns.Fill(cell_tns)
    return h_absolute_cell_tns

def plot_cell_tns():
    h1 = get_h1_cell_tns('tns40.txt')
    # h2 = get_h1_cell_tns('tns37.txt')
    h3 = get_h1_cell_tns('tns41.txt')

    c1 = TCanvas('c1', 'c1', 800, 600)
    h1.SetLineColor(kBlack)
    h1.SetName('h1')
    h1.GetYaxis().SetRangeUser(0, h1.GetMaximum() * 1.2)
    h1.Draw()

    # h2.SetLineColor(kBlue)
    # h2.SetName('h2')
    # h2.Draw('sames')

    h3.SetLineColor(kRed)
    h3.SetName('h3')
    h3.Draw('sames')
    
    c1.Update()
    raw_input('Press anykey to continue.\n')

def plot_h1_absolute_cell_tns():
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

def plot_slice_durations_from_hist():
    f1 = TFile('photon_timing_mc.40.root')
    h_slice_duration_1 = f1.Get('photontimingana/fSliceDuration')
    h_slice_count_1 = f1.Get('photontimingana/fSliceCount')
    event_count_1 = h_slice_count_1.GetEntries()

    f2 = TFile('photon_timing_mc.41.root')
    h_slice_duration_2 = f2.Get('photontimingana/fSliceDuration')
    h_slice_count_2 = f2.Get('photontimingana/fSliceCount')
    event_count_2 = h_slice_count_2.GetEntries()

    f_data = TFile('data/photon_timing_data_1.root')
    h_slice_duration_data = f_data.Get("photontimingana/fSliceDuration")
    h_slice_count_data = f_data.Get("photontimingana/fSliceCount")
    event_count_data = h_slice_count_data.GetEntries()

    h_slice_duration_1.Scale(float(event_count_2) / event_count_1)
    h_slice_duration_data.Scale(float(event_count_2) / event_count_data)

    c1 = TCanvas('c1', 'c1', 800, 600)
    set_margin()

    set_h1_style(h_slice_duration_1)
    h_slice_duration_1.SetLineColor(kBlue + 2)
    h_slice_duration_1.SetName('h1')
    h_slice_duration_1.GetXaxis().SetTitle('Slice Duration (ns)')
    h_slice_duration_1.GetYaxis().SetTitle('Slice Count')
    h_slice_duration_1.Draw('hist')

    set_h1_style(h_slice_duration_2)
    h_slice_duration_2.SetLineColor(kGreen + 2)
    h_slice_duration_2.Draw('sames, hist')

    set_h1_style(h_slice_duration_data)
    h_slice_duration_data.SetLineColor(kRed + 2)
    h_slice_duration_data.Draw('sames')

    lg1 = TLegend(0.5, 0.6, 0.85, 0.85)
    lg1.AddEntry(h_slice_duration_data, 'Default Data', 'l')
    lg1.AddEntry(h_slice_duration_2, 'Default MC', 'l')
    lg1.AddEntry(h_slice_duration_1, 'MC with 20 WLS', 'l')
    set_legend_style(lg1)
    lg1.Draw()

    c1.Update()
    c1.SaveAs('figures/plot_slice_durations_from_hist.pdf')
    raw_input('Press anykey to continue.\n')

def plot_effect_of_removing_cell_hit():
    h_mc_slice_duration = TH1D('h_mc_slice_duration', 'h_mc_slice_duration', 100, 0, 500)
    h_mc_slice_duration_minus_1 = TH1D('h_mc_slice_duration_minus_1', 'h_mc_slice_duration_minus_1', 100, 0, 500)
    h_mc_slice_duration_minus_2 = TH1D('h_mc_slice_duration_minus_2', 'h_mc_slice_duration_minus_2', 100, 0, 500)
    h_mc_slice_duration_minus_3 = TH1D('h_mc_slice_duration_minus_3', 'h_mc_slice_duration_minus_3', 100, 0, 500)
    h_mc_slice_duration_minus_4 = TH1D('h_mc_slice_duration_minus_4', 'h_mc_slice_duration_minus_4', 100, 0, 500)
    for slice_tns in get_slice_tnss('tns41.txt'):
        slice_tns = sorted(slice_tns)
        h_mc_slice_duration.Fill(max(slice_tns) - min(slice_tns))
        h_mc_slice_duration_minus_1.Fill(max(slice_tns[1:-1]) - min(slice_tns[1:-1]))
        h_mc_slice_duration_minus_2.Fill(max(slice_tns[2:-2]) - min(slice_tns[2:-2]))
        # h_mc_slice_duration_minus_3.Fill(max(slice_tns[3:-3]) - min(slice_tns[3:-3]))
        # h_mc_slice_duration_minus_4.Fill(max(slice_tns[4:-4]) - min(slice_tns[4:-4]))
    event_count_mc = float(len(get_event_tnss('tns41.txt')))

    f1 = TFile('photon_timing.root')
    h_data_slice_count = f1.Get('photontimingana/fSliceCount')
    event_count_data = h_data_slice_count.GetEntries()
    h_data_slice_duration = f1.Get('photontimingana/fSliceDuration')
    h_data_slice_duration_minus_1 = f1.Get('photontimingana/fSliceDurationMinus1')
    h_data_slice_duration_minus_2 = f1.Get('photontimingana/fSliceDurationMinus2')
    h_data_slice_duration_minus_3 = f1.Get('photontimingana/fSliceDurationMinus3')
    h_data_slice_duration_minus_4 = f1.Get('photontimingana/fSliceDurationMinus4')

    h_data_slice_duration.Scale(event_count_mc / event_count_data)
    h_data_slice_duration_minus_1.Scale(event_count_mc / event_count_data)
    h_data_slice_duration_minus_2.Scale(event_count_mc / event_count_data)
    h_data_slice_duration_minus_3.Scale(event_count_mc / event_count_data)
    h_data_slice_duration_minus_4.Scale(event_count_mc / event_count_data)


    gStyle.SetOptStat(0)
    # c0 = TCanvas('c0', 'c0', 800, 600)
    # set_margin()
    # h_data_slice_duration.SetLineColor(kRed + 1)
    # h_data_slice_duration.GetXaxis().SetRangeUser(0, 300)
    # h_data_slice_duration.GetYaxis().SetRangeUser(0, max(h_data_slice_duration.GetMaximum(), h_mc_slice_duration.GetMaximum()) * 1.2)
    # h_data_slice_duration.GetXaxis().SetTitle('Slice Duration (ns)')
    # h_data_slice_duration.GetYaxis().SetTitle('Slice Count')
    # set_h1_style(h_data_slice_duration)
    # h_data_slice_duration.Draw()

    # set_h1_style(h_mc_slice_duration)
    # h_mc_slice_duration.Draw('sames')

    # c0.Update()
    # c0.SaveAs('figures/plot_effect_of_removing_cell_hit.pdf')

    # c1 = TCanvas('c1', 'c1', 800, 600)
    # set_margin()
    # h_data_slice_duration_minus_1.SetLineColor(kRed + 1)
    # h_data_slice_duration_minus_1.GetXaxis().SetRangeUser(0, 300)
    # h_data_slice_duration_minus_1.GetYaxis().SetRangeUser(0, max(h_data_slice_duration_minus_1.GetMaximum(), h_mc_slice_duration_minus_1.GetMaximum()) * 1.2)
    # h_data_slice_duration_minus_1.GetXaxis().SetTitle('Slice Duration (ns)')
    # h_data_slice_duration_minus_1.GetYaxis().SetTitle('Slice Count')
    # set_h1_style(h_data_slice_duration_minus_1)
    # h_data_slice_duration_minus_1.Draw()

    # set_h1_style(h_mc_slice_duration_minus_1)
    # h_mc_slice_duration_minus_1.Draw('sames')

    # c1.Update()
    # c1.SaveAs('figures/plot_effect_of_removing_cell_hit_minus_1.pdf')

    c2 = TCanvas('c2', 'c2', 800, 600)
    set_margin()
    h_data_slice_duration_minus_2.SetLineColor(kRed + 1)
    h_data_slice_duration_minus_2.GetXaxis().SetRangeUser(0, 300)
    h_data_slice_duration_minus_2.GetYaxis().SetRangeUser(0, max(h_data_slice_duration_minus_2.GetMaximum(), h_mc_slice_duration_minus_2.GetMaximum()) * 1.2)
    h_data_slice_duration_minus_2.GetXaxis().SetTitle('Slice Duration (ns)')
    h_data_slice_duration_minus_2.GetYaxis().SetTitle('Slice Count')
    set_h1_style(h_data_slice_duration_minus_2)
    h_data_slice_duration_minus_2.Draw()

    set_h1_style(h_mc_slice_duration_minus_2)
    h_mc_slice_duration_minus_2.Draw('sames')

    c2.Update()
    c2.SaveAs('figures/plot_effect_of_removing_cell_hit_minus_2.pdf')

    raw_input('Press anykey to continue.\n')

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
# print get_slice_tnss('tns30.txt')
# print_run_info('tns32.txt')

# plot_slice_durations()
# plot_cell_tns()
# plot_h1_absolute_cell_tns()
plot_slice_durations_from_hist()
# plot_effect_of_removing_cell_hit()
