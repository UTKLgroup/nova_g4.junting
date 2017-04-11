#ifndef NovaWlsFiber_h
#define NovaWlsFiber_h

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"

class NovaWlsFiber
{
  public:
    NovaWlsFiber();
    ~NovaWlsFiber();
    G4LogicalVolume* BuildStraight(G4double radius,G4double length);
    G4LogicalVolume* BuildLoop(G4double radius,G4double length, G4double curve_radius);

  private:

    G4Element* elementH;
    G4Element* elementC;
    G4Element* elementO;

    G4Material* Polystyrene;
    G4Material* PMMA;
    G4Material* Fluorinated_Polymer;
    G4Material* CoreMat;

    G4MaterialPropertiesTable* wlsMPT;
    G4MaterialPropertiesTable* innercladMPT;
    G4MaterialPropertiesTable* outercladMPT;

    G4LogicalVolume* l_fiber;
    G4LogicalVolume* l_core;
    G4LogicalVolume* l_cladding1;
    G4LogicalVolume* l_cladding2;

    G4LogicalVolume* l_cladding;
    G4LogicalVolume* l_torus;

    G4VPhysicalVolume* p_core;
    G4VPhysicalVolume* p_cladding1;
    G4VPhysicalVolume* p_cladding2;

    G4VPhysicalVolume* p_cladding;

    G4VisAttributes* yellow;
    G4VisAttributes* red;
    G4VisAttributes* blue;

};

#endif
