from subprocess import call

fiber_radius = 0.035

def write_side_illumination_mac(**kwargs):
    led_wavelength = kwargs['led_wavelength']
    fiber_length = kwargs['fiber_length']
    event_count = kwargs['event_count']
    mac_filename = kwargs['mac_filename']

    with open(mac_filename, 'w') as f_mac:
        f_mac.write('/control/verbose 2\n')
        f_mac.write('/control/saveHistory\n')

        f_mac.write('/gps/particle opticalphoton\n')
        f_mac.write('/gps/number 1\n')
        f_mac.write('/gps/pos/centre -{} 0 {} cm\n'.format(fiber_radius * 2.0, fiber_length / 2.0 - fiber_radius))
        f_mac.write('/gps/pos/type Plane\n')
        f_mac.write('/gps/pos/shape Circle\n')
        f_mac.write('/gps/pos/radius {} cm\n'.format(fiber_radius))
        f_mac.write('/gps/direction 1 0 0\n')
        f_mac.write('/gps/pos/rot1 0 0 1\n')
        f_mac.write('/gps/pos/rot2 0 1 0\n')
        f_mac.write('/control/execute led_{}nm_spectrum.mac\n'.format(led_wavelength))

        f_mac.write('/nova/simulationMode fiber\n')
        f_mac.write( '/nova/detector/cellLength {} cm\n'.format(fiber_length))
        f_mac.write('/nova/detector/update\n')

        f_mac.write( '/tracking/verbose 0\n')
        f_mac.write('/run/initialize\n')
        f_mac.write('/run/beamOn {}\n'.format(event_count))


def write_face_illumination_mac(**kwargs):
    led_wavelength = kwargs['led_wavelength']
    fiber_length = kwargs['fiber_length']
    event_count = kwargs['event_count']
    mac_filename = kwargs['mac_filename']

    with open(mac_filename, 'w') as f_mac:
        f_mac.write('/control/verbose 2\n')
        f_mac.write('/control/saveHistory\n')

        f_mac.write('/gps/particle opticalphoton\n')
        f_mac.write('/gps/number 1\n')
        f_mac.write('/gps/pos/centre 0 0 {} cm\n'.format(fiber_length / 2.0))
        f_mac.write('/gps/pos/type Plane\n')
        f_mac.write('/gps/pos/shape Circle\n')
        f_mac.write('/gps/pos/radius {} cm\n'.format(fiber_radius))
        f_mac.write('/gps/direction 0 0 -1\n')
        f_mac.write('/control/execute led_{}nm_spectrum.mac\n'.format(led_wavelength))

        f_mac.write( '/Nova/detector/cellLength {} cm\n'.format(fiber_length))
        f_mac.write('/Nova/detector/update\n')

        f_mac.write( '/tracking/verbose 0\n')
        f_mac.write('/run/initialize\n')
        f_mac.write('/run/beamOn {}\n'.format(event_count))

def run_spectrum():
    for random_seed in range(1, 3):
        print random_seed
        for fiber_length in [100, 200, 400]:
        # for fiber_length in [800, 1600, 3200]:
            # led_wavelength = 470
            led_wavelength = 395
            mac_filename='run.side_illumination.mac'
            event_count = 50000

            write_side_illumination_mac(led_wavelength=led_wavelength,
                                        fiber_length=fiber_length,
                                        event_count=event_count,
                                        mac_filename=mac_filename)
            call('./nova {} {}'.format(mac_filename, random_seed), shell=True)
            call('mv nova.root run_spectrum.wls_exponential.{}nm.{}cm.random_seed_{}.root'.format(led_wavelength, fiber_length, random_seed), shell=True)

def run_speed():
    led_wavelength = 395
    event_count = 20000
    random_seed = 1
    mac_filename = 'run.face_illumination.mac'

    for fiber_length in [100, 200, 300, 400, 500]:
        write_face_illumination_mac(led_wavelength=led_wavelength,
                                    fiber_length=fiber_length,
                                    event_count=event_count,
                                    mac_filename=mac_filename)
        call('./nova {} {}'.format(mac_filename, random_seed), shell=True)
        call('mv -f nova.root run_speed.{}nm.{}cm.random_seed_{}.root'.format(led_wavelength, fiber_length, random_seed), shell=True)

if __name__ == '__main__':
    # run_speed()

    run_spectrum()

    # write_side_illumination_mac(led_wavelength=395,
    #                             fiber_length=10,
    #                             event_count=20,
    #                             mac_filename='run.side_illumination.mac')

    # write_face_illumination_mac(led_wavelength=395,
    #                             fiber_length=100,
    #                             event_count=10,
    #                             mac_filename='run.face_illumination.mac')
