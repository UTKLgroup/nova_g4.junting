#include "WLSfiber.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Transform3D.hh"
#include <fstream>
#include <iostream>
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4Torus.hh"
#include "G4UnionSolid.hh"

using namespace std;

WLSfiber::WLSfiber()
{
  /*
  //---------- materials

  elementH = new G4Element( "Hydrogen", "H", 1. , 1.00794*g/mole );
  elementC = new G4Element( "Carbon", "C", 6. , 12.011*g/mole );
  elementO = new G4Element( "Oxygen", "O", 8. , 16.011*g/mole );

  Polystyrene = new G4Material("polystyrene",  1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere );
  Polystyrene->AddElement( elementH, 0.498 );
  Polystyrene->AddElement( elementC, 0.502 );

  PMMA = new G4Material("PMMA",  1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere );
  PMMA->AddElement( elementH, 0.532 );
  PMMA->AddElement( elementC, 0.336 );
  PMMA->AddElement( elementO, 0.132 );

  Fluorinated_Polymer = new G4Material("Fluorinated_Polymer",  1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere );
  Fluorinated_Polymer->AddElement( elementH, 0.532 );
  Fluorinated_Polymer->AddElement( elementC, 0.336 );
  Fluorinated_Polymer->AddElement( elementO, 0.132 );

  //---------- read files

  G4int wlsEntries = 0;
  G4int bulkEntries = 0;
  G4int pmmaEntries = 0;
  G4double wlsEnergy[500];
  G4double wlsEmit[500];
  G4double wlsAbsorb[500];
  G4double coreIndex[500];
  G4double pWavelength;
  G4double absorblength;
  G4double wls_absorblength;

  G4double bulkAbsorb[500];
  G4double pmmabulkAbsorb[500];
  G4double bulkEnergy[500];
  G4double pmmaEnergy[500];
  G4double cladIndex1[500];
  G4double cladIndex2[500];

  G4double coreIndexconst = 1.59;
  G4double cladIndexconst1 = 1.49;
  G4double cladIndexconst2 = 1.42;

  // WLS absorption
  ifstream ReadWLSa;
  ReadWLSa.open("input/y11_abs_length.dat");
  if (ReadWLSa.is_open()){
    while(!ReadWLSa.eof()){
      G4String filler;
      ReadWLSa >> pWavelength >> filler >> wls_absorblength;
      wlsEnergy[wlsEntries] = (1240/pWavelength)*eV;
      wlsAbsorb[wlsEntries] = 1.0*wls_absorblength*m;
      coreIndex[wlsEntries] = coreIndexconst;
      cladIndex1[wlsEntries] = cladIndexconst1;
      cladIndex2[wlsEntries] = cladIndexconst2;
      wlsEntries++;
    }
  }
  else
    G4cout<<"Error opening file: "<< "y11_abs_length.dat" <<G4endl;  
  ReadWLSa.close();

  // WLS emission
  wlsEntries = 0;
  ifstream ReadWLSe;
  ReadWLSe.open("input/WLSemit.cfg");
  if(ReadWLSe.is_open()){
    while(!ReadWLSe.eof()){
      G4String filler;
      ReadWLSe >> pWavelength >> filler >> wlsEmit[wlsEntries];
      wlsEnergy[wlsEntries] = (1240/pWavelength)*eV;
      wlsEntries++;
    }
  }
  else
    G4cout<<"Error opening file: "<< "WLSemit.cfg" <<G4endl; 
  ReadWLSe.close();

  // core bulk absorption 
  bulkEntries = 0;
  ifstream ReadBulk;
  ReadBulk.open("input/fiberPSTabsorb.dat");
  if(ReadBulk.is_open()){
    while(!ReadBulk.eof()){
      G4String filler;
      ReadBulk >> pWavelength >> filler >> absorblength;
      bulkEnergy[bulkEntries] = (1240/pWavelength)*eV;
      bulkAbsorb[bulkEntries] = absorblength * m;
      bulkEntries++;
    }
  }
  else
    G4cout<<"Error opening file: "<< "fiberPSTabsorb.dat" <<G4endl; 
  ReadBulk.close();

  // first cladding bulk absorption
  pmmaEntries = 0;
  ifstream Read_pmma_Bulk;
  Read_pmma_Bulk.open("input/PMMABulkAbsorb.dat");
  if(Read_pmma_Bulk.is_open()){
    while(!Read_pmma_Bulk.eof()){
      G4String filler;
      Read_pmma_Bulk >> pWavelength >> filler >> absorblength;
      pmmaEnergy[pmmaEntries] = (1240/pWavelength)*eV;
      pmmabulkAbsorb[pmmaEntries] = 1.0*absorblength*m;
      pmmaEntries++;
    }
  }
  else
    G4cout << "Error opening file: " << "PMMABulkAbsorb.dat" << G4endl; 
  Read_pmma_Bulk.close();

  //---------- set MPT

  wlsMPT = new G4MaterialPropertiesTable();
  wlsMPT->AddProperty("RINDEX",wlsEnergy,coreIndex,wlsEntries);
  wlsMPT->AddProperty("ABSLENGTH",bulkEnergy,bulkAbsorb,bulkEntries);
  wlsMPT->AddProperty("WLSABSLENGTH",wlsEnergy,wlsAbsorb,wlsEntries);
  wlsMPT->AddProperty("WLSCOMPONENT",wlsEnergy,wlsEmit,wlsEntries);
  wlsMPT->AddConstProperty("WLSTIMECONSTANT",12*ns);
  wlsMPT->AddConstProperty("CONSTANTQUANTUMYIELD",0.7);
  Polystyrene->SetMaterialPropertiesTable(wlsMPT);

  innercladMPT = new G4MaterialPropertiesTable();
  innercladMPT->AddProperty("RINDEX", wlsEnergy, cladIndex1, wlsEntries);
  innercladMPT->AddProperty("ABSLENGTH",wlsEnergy,pmmabulkAbsorb,wlsEntries);
  PMMA->SetMaterialPropertiesTable(innercladMPT);

  outercladMPT = new G4MaterialPropertiesTable();
  outercladMPT->AddProperty("RINDEX", wlsEnergy, cladIndex2, wlsEntries);
  outercladMPT->AddProperty("ABSLENGTH",wlsEnergy,pmmabulkAbsorb,wlsEntries);
  Fluorinated_Polymer->SetMaterialPropertiesTable(outercladMPT);
  */
}

WLSfiber::~WLSfiber()
{}

G4LogicalVolume *WLSfiber::BuildStraight(G4double radius, G4double length){
  G4double clad_percent = 0.03;
  G4double core_percent = 1. - 2. * clad_percent;
  G4double core_radius  = radius * core_percent;
  G4double inner_radius = core_radius + radius * clad_percent;

  G4Tubs* outer_clading_s = new G4Tubs("outer_clading_s", 0, radius, length / 2., 0., 360.*deg);
  G4LogicalVolume* outer_clading_l = new G4LogicalVolume(outer_clading_s, G4Material::GetMaterial("Fluorinated_Polymer"), "outer_clading_l");

  G4Tubs* inner_clading_s = new G4Tubs("inner_clading_s", 0, inner_radius, length / 2., 0., 360.*deg);
  G4LogicalVolume* inner_clading_l = new G4LogicalVolume(inner_clading_s, G4Material::GetMaterial("PMMA"), "inner_clading_l");

  G4Tubs* core_s = new G4Tubs("straight_core", 0, core_radius, length / 2. , 0., 360.*deg);      
  G4LogicalVolume* core_l = new G4LogicalVolume(core_s, G4Material::GetMaterial("Polystyrene"), "core_l");

  //---------- assemble a fiber

  new G4PVPlacement(0, G4ThreeVector(), core_l, "fibcore", inner_clading_l, false, 0);  
  new G4PVPlacement(0, G4ThreeVector(), inner_clading_l, "inner_clading", outer_clading_l, false, 0);  
  return outer_clading_l;
}
