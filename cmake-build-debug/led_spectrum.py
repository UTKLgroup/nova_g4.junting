import csv

PLANK_CONSTANT = 1239.84193

def convert_wavelength_to_energy(wavelength):
    return PLANK_CONSTANT / wavelength;

# led_wavelength = 360
# led_wavelength = 395
# led_wavelength = 430
led_wavelength = 470

mac_filename = 'led_{}nm_spectrum.mac'.format(led_wavelength)
csv_filename = 'led_{}nm_spectrum.csv'.format(led_wavelength)

with open(mac_filename, 'w') as f_mac:
    f_mac.write('/gps/ene/type Arb\n')
    f_mac.write('/gps/hist/type arb\n')

    energy_intensities = []
    with open(csv_filename) as f_csv:
        rows = csv.reader(f_csv, delimiter=',')
        for row in rows:
            energy = convert_wavelength_to_energy(float(row[0])) / 1000.0
            intensity = float(row[1])
            energy_intensities.append((energy, intensity))
    energy_intensities = sorted(energy_intensities, key=lambda x: x[0])

    for energy_intensity in energy_intensities:
        f_mac.write('/gps/hist/point {} {}\n'.format(energy_intensity[0], energy_intensity[1]))
    f_mac.write('/gps/hist/inter Lin')
