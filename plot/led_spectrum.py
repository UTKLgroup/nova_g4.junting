import csv

PLANK_CONSTANT = 1239.84193

def convert_wavelength_to_energy(wavelength):
    return PLANK_CONSTANT / wavelength;

energy_intensities = []
with open('led_430nm_spectrum.mac', 'w') as f_mac:
    f_mac.write('/gps/ene/type Arb')
    f_mac.write('/gps/hist/type arb')
    with open('led_430nm_spectrum.csv') as f_csv:
        rows = csv.reader(f_csv, delimiter=',')
        for row in rows:
            energy = convert_wavelength_to_energy(float(row[0]))
            intensity = float(row[1])
            energy_intensities.append((energy, intensity))

    energy_intensities = sorted(energy_intensities, key=lambda x: x[0])
    print energy_intensities
    f_mac.write('/gps/hist/inter Lin')
