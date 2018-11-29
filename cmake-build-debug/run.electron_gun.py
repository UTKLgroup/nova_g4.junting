from subprocess import call
import numpy as np

def write_mac(**kwargs):
    mac_filename = kwargs.get('mac_filename')
    event_count = kwargs.get('event_count')
    liquid_scintillator_light_yield = kwargs.get('liquid_scintillator_light_yield')
    electron_kinetic_energy = kwargs.get('electron_kinetic_energy', 1.) # MeV

    with open(mac_filename, 'w') as f_mac:
        f_mac.write('/control/verbose 2\n')

        f_mac.write('/gps/particle e-\n')
        f_mac.write('/gps/energy {} MeV\n'.format(electron_kinetic_energy))
        f_mac.write('/gps/direction 0 -1 0\n')
        f_mac.write('/gps/position 0 0.007 0 m\n')

        f_mac.write('/nova/simulationMode electron_gun\n')
        f_mac.write('/nova/detector/photodetectorType pmt\n')
        f_mac.write('/nova/detector/liquidScintillatorLightYield {}\n'.format(liquid_scintillator_light_yield))
        f_mac.write('/tracking/verbose 0\n')
        f_mac.write('/run/initialize\n')
        f_mac.write('/run/beamOn {}\n'.format(event_count))

def run():
    mac_filename = 'run.electron_gun.mac'
    event_count = 1000
    liquid_scintillator_light_yield = 3000
    random_seed = 1

    for electron_kinetic_energy in np.arange(0.1, 3, 0.1):
        write_mac(mac_filename=mac_filename,
                  event_count=event_count,
                  liquid_scintillator_light_yield=liquid_scintillator_light_yield,
                  electron_kinetic_energy=electron_kinetic_energy)
        call('./nova {} {}'.format(mac_filename, random_seed), shell=True)
        call('mv nova.root run.electron_gun.event_count_{}.electron_kinetic_energy_{}mev.root'.format(event_count, electron_kinetic_energy), shell=True)

if __name__ == '__main__':
    run()
