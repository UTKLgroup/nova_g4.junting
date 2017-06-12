from subprocess import call
import numpy as np

def write_mac(**kwargs):
    liquid_scintillator_light_yield = kwargs.get('liquid_scintillator_light_yield')
    detector_length = kwargs.get('detector_length')
    distance_to_front = kwargs.get('distance_to_front')
    event_count = kwargs.get('event_count')
    mac_filename = kwargs.get('mac_filename')


    gps_z_position = detector_length / 2.0 - distance_to_front
    with open(mac_filename, 'w') as f_mac:
        f_mac.write('/control/verbose 2\n')

        f_mac.write('/gps/particle mu-\n')
        f_mac.write('/gps/energy 1 GeV\n')
        f_mac.write('/gps/direction 0 -1 0\n')
        f_mac.write('/gps/position 0 0.033 {} m\n'.format(gps_z_position))

        f_mac.write('/nova/simulationMode cell\n')
        f_mac.write('/nova/detector/detectorLength {} m\n'.format(detector_length))
        f_mac.write('/nova/detector/snoutLength 0.635 m\n')
        f_mac.write('/nova/detector/photodetectorType apd\n')
        f_mac.write('/nova/detector/liquidScintillatorLightYield {}\n'.format(liquid_scintillator_light_yield))
        f_mac.write('/nova/detector/update\n')
        f_mac.write('/nova/printSetting\n')

        f_mac.write('/tracking/verbose 0\n')
        f_mac.write('/run/initialize\n')
        f_mac.write('/run/beamOn {}\n'.format(event_count))
        
def run():
    liquid_scintillator_light_yield = 3000
    detector_length = 4.0
    event_count = 40
    mac_filename = 'run.cell.mac'
    random_seed = 1

    for distance_to_front in [2.0]:
        print distance_to_front

        write_mac(detector_length=detector_length,
                  distance_to_front=distance_to_front,
                  event_count=event_count,
                  mac_filename=mac_filename,
                  liquid_scintillator_light_yield=liquid_scintillator_light_yield)
        call('./nova {} {}'.format(mac_filename, random_seed), shell=True)
        call('mv nova.root run_cell.{}m.{}m.root'.format(detector_length, distance_to_front), shell=True)

if __name__ == '__main__':
    run()
