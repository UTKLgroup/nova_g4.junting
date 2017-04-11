#include "NovaWlsFiber.hh"
#include <G4SIunits.hh>
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"

using namespace std;

NovaWlsFiber::NovaWlsFiber() {
}

NovaWlsFiber::~NovaWlsFiber() {
}

G4LogicalVolume* NovaWlsFiber::BuildStraight(G4double radius, G4double length){
    G4double clad_percent = 0.03;
    G4double core_percent = 1. - 2. * clad_percent;
    G4double core_radius  = radius * core_percent;
    G4double inner_radius = core_radius + radius * clad_percent;

    G4Tubs* outer_cladding_s = new G4Tubs("outer_cladding_s", 0, radius, length / 2., 0., 360.*deg);
    G4LogicalVolume* outer_cladding_l = new G4LogicalVolume(outer_cladding_s, G4Material::GetMaterial("fluorinatedPolymer"), "outer_cladding_l");

    G4Tubs* inner_cladding_s = new G4Tubs("inner_cladding_s", 0, inner_radius, length / 2., 0., 360.*deg);
    G4LogicalVolume* inner_cladding_l = new G4LogicalVolume(inner_cladding_s, G4Material::GetMaterial("pmma"), "inner_cladding_l");

    G4Tubs* core_s = new G4Tubs("straight_core", 0, core_radius, length / 2. , 0., 360.*deg);
    G4LogicalVolume* core_l = new G4LogicalVolume(core_s, G4Material::GetMaterial("polystyrene"), "core_l");

    new G4PVPlacement(0, G4ThreeVector(), core_l, "fibcore", inner_cladding_l, false, 0);
    new G4PVPlacement(0, G4ThreeVector(), inner_cladding_l, "inner_cladding", outer_cladding_l, false, 0);

    return outer_cladding_l;
}
