from rootalias import *
from pprint import pprint

# mc_7 = TFile('photon_timing_mc.7.root')
# mc_8 = TFile('photon_timing_mc.8.root')


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
    h1 = TH1D('h1', 'h1', 50, 0, 200)
    for slice_duration in get_slice_durations(filename):
        h1.Fill(slice_duration)
    return h1

def plot_slice_durations():
    h1 = get_h1_slice_durations('tns30.txt')
    h2 = get_h1_slice_durations('tns31.txt')
    h3 = get_h1_slice_durations('tns29.txt')

    c1 = TCanvas('c1', 'c1', 800, 600)
    h1.SetLineColor(kBlack)
    h1.GetYaxis().SetRangeUser(0, h1.GetMaximum() * 1.2)
    h1.Draw()

    h2.SetLineColor(kBlue)
    h2.Draw('sames')

    h3.SetLineColor(kRed)
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
    h_default = get_h1_cell_tns('tns30.txt')
    h_tau = get_h1_cell_tns('tns31.txt')
    h_tau_2wls = get_h1_cell_tns('tns29.txt')

    c1 = TCanvas('c1', 'c1', 800, 600)
    h_default.SetLineColor(kBlack)
    h_default.SetName('h_default')
    h_default.GetYaxis().SetRangeUser(0, h_default.GetMaximum() * 1.2)
    h_default.Draw()

    h_tau.SetLineColor(kBlue)
    h_tau.SetName('h_tau')
    h_tau.Draw('sames')

    h_tau_2wls.SetLineColor(kRed)
    h_tau_2wls.SetName('h_tau_2wls')
    h_tau_2wls.Draw('sames')
    
    c1.Update()
    raw_input('Press anykey to continue.\n')

def plot_h1_absolute_cell_tns():
    h1 = get_h1_absolute_cell_tns('tns32.txt')
    h2 = get_h1_absolute_cell_tns('tns33.txt')
    h3 = get_h1_absolute_cell_tns('tns34.txt')

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

# pprint(sorted(map(lambda x: len(x), get_slice_tnss('tns21.txt'))))
# pprint(sorted(map(lambda x: len(x), get_slice_tnss('tns23.txt'))))
# pprint(sorted(map(lambda x: len(x), get_slice_tnss('tns20.txt'))))

# pprint(len(sorted(map(lambda x: len(x), get_slice_tnss('tns30.txt')))))
# pprint(len(sorted(map(lambda x: len(x), get_slice_tnss('tns31.txt')))))
# pprint(len(sorted(map(lambda x: len(x), get_slice_tnss('tns29.txt')))))

# pprint(sum(map(lambda x: len(x), get_slice_tnss('tns30.txt'))))
# pprint(sum(map(lambda x: len(x), get_slice_tnss('tns29.txt'))))

# pprint(get_slice_tnss('tns20.txt'))

# pprint(get_slice_durations('tns22.txt'))
# pprint(get_slice_durations('tns21.txt'))
# plot_slice_durations()
# plot_cell_tns()
# plot_cell_absolute_tns()

# print get_slice_tnss('tns30.txt')
# print_run_info('tns32.txt')

plot_h1_absolute_cell_tns()
