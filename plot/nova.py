from rootalias import *

def get_h1_hit_time(root_filename):
    f_event = TFile(root_filename)
    h_hit_time = TH1D('h_hit_time', 'h_hit_time', 100, 0, 100)
    for event in f_event.eventTree:
        h_hit_time.Fill(event.hitTime)

h_1 = get_h1_hit_time('../cmake-build-debug/run_spectrum.470nm.100cm.root')
c1 = TCanvas('c1', 'c1', 800, 600)
h_1.Draw()

c1.Update()
raw_input()
