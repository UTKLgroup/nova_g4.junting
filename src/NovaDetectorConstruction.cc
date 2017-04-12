#include "NovaDetectorConstruction.hh"
#include "LXePMTSD.hh"
#include "NovaMainVolume.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4UImanager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4NistManager.hh"


NovaDetectorConstruction::NovaDetectorConstruction()
{
  liquidScintillatorMpt = NULL;
  experimentalHallBox = NULL;
  experimentalHallLog = NULL;
  experimentalHallPhy = NULL;
  tiO2 = pvc = liquidScintillator = NULL;
  oxygen = carbon = hydrogen = titanium = NULL;
  isUpdated = false;

  setDefaults();
}

NovaDetectorConstruction::~NovaDetectorConstruction()
{}

void NovaDetectorConstruction::DefineMaterials()
{
  hydrogen = new G4Element("H", "H", 1., 1.01*g/mole);
  carbon = new G4Element("C", "C", 6., 12.01*g/mole);
  oxygen = new G4Element("O", "O", 8., 16.00*g/mole);
  titanium = new G4Element( "Ti", "Ti", 22., 47.87*g/mole);

  vacuum = new G4Material("vacuum",
                           1.,
                           1.01*g/mole,
                           universe_mean_density,
                           kStateGas,
                           0.1*kelvin,
                           1.e-19*pascal);

  tiO2 = new G4Material("tiO2", 4.23*g/cm3, 2, kStateSolid);
  tiO2->AddElement(oxygen, 2);
  tiO2->AddElement(titanium, 1);


  liquidScintillator = new G4Material("liquidScintillator",  1.032*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  liquidScintillator->AddElement(hydrogen, 0.666);
  liquidScintillator->AddElement(carbon, 0.334);

  polystyrene = new G4Material("polystyrene",  1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  polystyrene->AddElement(hydrogen, 0.498);
  polystyrene->AddElement(carbon, 0.502);

  pmma = new G4Material("pmma", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  pmma->AddElement(hydrogen, 0.532);
  pmma->AddElement(carbon, 0.336);
  pmma->AddElement(oxygen, 0.132);


  fluorinatedPolymer = new G4Material("fluorinatedPolymer", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  fluorinatedPolymer->AddElement(hydrogen, 0.532);
  fluorinatedPolymer->AddElement(carbon, 0.336);
  fluorinatedPolymer->AddElement(oxygen, 0.132);

  // PVC
  pvc = new G4Material("pvc", 1.4316*g/cm3, 2, kStateSolid);
  pvc->AddMaterial(tiO2, 0.15);
  pvc->AddMaterial(polystyrene, 0.85);

  //Glass
  glass = new G4Material("glass", 1.032*g/cm3, 2);
  glass->AddElement(carbon, 91.533*perCent);
  glass->AddElement(hydrogen, 8.467*perCent);

  //***Material properties tables

  // glass
  const G4int lxenum = 2;
  G4double lxe_Energy[lxenum] = {1240. / 200. * eV, 1240. / 700. * eV};
  G4double glass_RIND[lxenum] = {1.49, 1.49};
  G4double glass_AbsLength[lxenum] = {420.*cm, 420.*cm};
  G4MaterialPropertiesTable *glass_mt = new G4MaterialPropertiesTable();
  glass_mt->AddProperty("ABSLENGTH", lxe_Energy, glass_AbsLength, lxenum);
  glass_mt->AddProperty("RINDEX", lxe_Energy, glass_RIND, lxenum);
  glass->SetMaterialPropertiesTable(glass_mt);

  // liquid scintillator
  liquidScintillatorMpt = new G4MaterialPropertiesTable();
  G4double ScintRIndexConst = 1.47;
  G4double VacAbsConst = 100.*m;
  G4double wavelength;
  G4double variable;
  G4String filler;

  std::vector<G4double> VacIndexE;
  std::vector<G4double> VacIndex;

  std::vector<G4double> VacAbsE;
  std::vector<G4double> VacAbs;

  std::vector<G4double> ScintRIndexE;
  std::vector<G4double> ScintRIndex;

  std::vector<G4double> ScintAbsE;
  std::vector<G4double> ScintAbs;

  std::vector<G4double> ScintEmitE;
  std::vector<G4double> ScintEmitFast;
  std::vector<G4double> ScintEmitSlow;

  std::vector<G4double> ScintWLSabsE;
  std::vector<G4double> ScintWLSabs;

  std::vector<G4double> ScintWLSemitE;
  std::vector<G4double> ScintWLSemit;

  // scintillator emission (fast and slow)
  std::ifstream ReadScint;
  ReadScint.open(inputDir + "ppo_emission.txt");
  if(ReadScint.is_open()){
    while(!ReadScint.eof()){
      ReadScint >> wavelength >> filler >> variable;
      ScintEmitE.push_back(1240.0 / wavelength * eV);
      ScintEmitFast.push_back(variable);
      ScintEmitSlow.push_back(0.0);

      ScintRIndexE.push_back(1240.0 / wavelength * eV);
      ScintRIndex.push_back(ScintRIndexConst);

      VacIndexE.push_back(1240.0 / wavelength * eV);
      VacIndex.push_back(1.0);

      VacAbsE.push_back(1240.0 / wavelength * eV);
      VacAbs.push_back(VacAbsConst);
    }
  }
  else
    G4cout << "Error opening file: "  << G4endl;
  ReadScint.close();

  // vacuum
  vacuumMpt = new G4MaterialPropertiesTable();
  vacuumMpt->AddProperty("RINDEX", &VacIndexE[0], &VacIndex[0], (G4int) VacIndexE.size());
  vacuumMpt->AddProperty("ABSLENGTH", &VacAbsE[0], &VacAbs[0], (G4int) VacAbsE.size());
  vacuum->SetMaterialPropertiesTable(vacuumMpt);

  // bulk absorption
  std::ifstream Readabsorb;
  Readabsorb.open(inputDir + "PSTBulkAbsorb.cfg");
  if(Readabsorb.is_open()){
    while(!Readabsorb.eof()){
      Readabsorb >> wavelength >> filler >> variable;
      ScintAbsE.push_back(1240.0 / wavelength * eV);
      ScintAbs.push_back(1.8 * variable * m);
    }
  }
  else
    G4cout << "Error opening file: " << G4endl;
  Readabsorb.close();

  // WLS absorption
  std::ifstream ReadWLSa;
  ReadWLSa.open(inputDir + "UPS923.cfg");
  if (ReadWLSa.is_open()){
    while (!ReadWLSa.eof()){
      ReadWLSa >> wavelength >> filler >> variable;
      ScintWLSabsE.push_back(1240 / wavelength * eV);
      ScintWLSabs.push_back(1.8 * variable * m);
    }
  }
  else
    G4cout << "Error opening file: " << G4endl;
  ReadWLSa.close();

  // WLS emission
  std::ifstream ReadWLSe;
  ReadWLSe.open(inputDir + "bisMSB_emission.txt");
  if(ReadWLSe.is_open()){
    while (!ReadWLSe.eof()){
      ReadWLSe >> wavelength >> filler >> variable;
      ScintWLSemitE.push_back(1240.0 / wavelength * eV);
      ScintWLSemit.push_back(variable);
    }
  }
  else
    G4cout << "Error opening file: " << G4endl;
  ReadWLSe.close();

  liquidScintillatorMpt->AddProperty("RINDEX", &ScintRIndexE[0], &ScintRIndex[0], ScintRIndexE.size());
  liquidScintillatorMpt->AddProperty("ABSLENGTH", &ScintAbsE[0], &ScintAbs[0], ScintAbsE.size());
  liquidScintillatorMpt->AddProperty("FASTCOMPONENT", &ScintEmitE[0], &ScintEmitFast[0], ScintEmitE.size());
  liquidScintillatorMpt->AddProperty("SLOWCOMPONENT", &ScintEmitE[0], &ScintEmitSlow[0], ScintEmitE.size());
  liquidScintillatorMpt->AddProperty("WLSABSLENGTH", &ScintWLSabsE[0], &ScintWLSabs[0], ScintWLSabsE.size());
  liquidScintillatorMpt->AddProperty("WLSCOMPONENT", &ScintWLSemitE[0], &ScintWLSemit[0], ScintWLSemitE.size());
  liquidScintillatorMpt->AddConstProperty("SCINTILLATIONYIELD", scintillatorLightYield / MeV);
  liquidScintillatorMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.9);
  liquidScintillatorMpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  liquidScintillatorMpt->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);
  liquidScintillatorMpt->AddConstProperty("SLOWTIMECONSTANT", 10.0*ns);
  liquidScintillatorMpt->AddConstProperty("WLSTIMECONSTANT", 1.0*ns);
  liquidScintillatorMpt->AddConstProperty("YIELDRATIO", 1.0);
  liquidScintillator->SetMaterialPropertiesTable(liquidScintillatorMpt);
  liquidScintillator->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

  G4double coreIndexconst = 1.59;
  G4double InCladRIndexConst = 1.49;
  G4double OutCladRIndexConst = 1.42;

  std::vector < G4double > CoreWLSabsE;
  std::vector < G4double > CoreWLSabs;

  std::vector < G4double > CoreWLSemitE;
  std::vector < G4double > CoreWLSemit;

  std::vector < G4double > RIndexE;
  std::vector < G4double > CoreRIndex;
  std::vector < G4double > InCladRIndex;
  std::vector < G4double > OutCladRIndex;

  std::vector < G4double > CoreAbsE;
  std::vector < G4double > CoreAbs;

  std::vector < G4double > CladAbsE;
  std::vector < G4double > CladAbs;

  // WLS absorption
  ReadWLSa.open(inputDir + "y11_abs_length.dat");
  if (ReadWLSa.is_open()){
    while(!ReadWLSa.eof()){
      ReadWLSa >> wavelength >> filler >> variable;

      CoreWLSabsE.push_back(1240 / wavelength * eV);
      CoreWLSabs.push_back(variable * m);

      RIndexE.push_back(1240 / wavelength * eV);
      CoreRIndex.push_back(coreIndexconst);
      InCladRIndex.push_back(InCladRIndexConst);
      OutCladRIndex.push_back(OutCladRIndexConst);
    }
  }
  else
    G4cout<<"Error opening file: "<< "y11_abs_length.dat" <<G4endl;
  ReadWLSa.close();

  // WLS emission
  ReadWLSe.open(inputDir + "WLSemit.cfg");
  if(ReadWLSe.is_open()){
    while(!ReadWLSe.eof()){
      ReadWLSe >> wavelength >> filler >> variable;
      CoreWLSemitE.push_back(1240 / wavelength * eV);
      CoreWLSemit.push_back(variable);
    }
  }
  else
    G4cout<<"Error opening file: "<< "WLSemit.cfg" <<G4endl;
  ReadWLSe.close();

  // core bulk absorption
  std::ifstream ReadBulk;
  ReadBulk.open(inputDir + "fiberPSTabsorb.dat");
  if(ReadBulk.is_open()){
    while(!ReadBulk.eof()){
      ReadBulk >> wavelength >> filler >> variable;
      CoreAbsE.push_back(1240 / wavelength * eV);
      CoreAbs.push_back(variable * m);
    }
  }
  else
    G4cout<<"Error opening file: "<< "fiberPSTabsorb.dat" <<G4endl;
  ReadBulk.close();

  // first cladding bulk absorption
  std::ifstream Read_pmma_Bulk;
  Read_pmma_Bulk.open(inputDir + "PMMABulkAbsorb.dat");
  if(Read_pmma_Bulk.is_open()){
    while(!Read_pmma_Bulk.eof()){
      Read_pmma_Bulk >> wavelength >> filler >> variable;
      CladAbsE.push_back(1240 / wavelength * eV);
      CladAbs.push_back(variable * m);
    }
  }
  else
    G4cout << "Error opening file: " << "PMMABulkAbsorb.dat" << G4endl;
  Read_pmma_Bulk.close();

  polystyreneMpt = new G4MaterialPropertiesTable();
  polystyreneMpt->AddProperty("RINDEX", &RIndexE[0], &CoreRIndex[0], RIndexE.size());
  polystyreneMpt->AddProperty("ABSLENGTH", &CoreAbsE[0], &CoreAbs[0], CoreAbsE.size());
  polystyreneMpt->AddProperty("WLSABSLENGTH", &CoreWLSabsE[0],  &CoreWLSabs[0],  CoreWLSabsE.size());
  polystyreneMpt->AddProperty("WLSCOMPONENT", &CoreWLSemitE[0], &CoreWLSemit[0], CoreWLSemitE.size());
  polystyreneMpt->AddConstProperty("WLSTIMECONSTANT", 11.8*ns);
  polystyreneMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.7);
  polystyrene->SetMaterialPropertiesTable(polystyreneMpt);

  pmmaMpt = new G4MaterialPropertiesTable();
  pmmaMpt->AddProperty("RINDEX", &RIndexE[0], &InCladRIndex[0], RIndexE.size());
  pmmaMpt->AddProperty("ABSLENGTH", &CladAbsE[0], &CladAbs[0], CladAbsE.size());
  pmma->SetMaterialPropertiesTable(pmmaMpt);

  fluorinatedPolymerMpt = new G4MaterialPropertiesTable();
  fluorinatedPolymerMpt->AddProperty("RINDEX", &RIndexE[0], &OutCladRIndex[0], RIndexE.size());
  fluorinatedPolymerMpt->AddProperty("ABSLENGTH", &CladAbsE[0], &CladAbs[0], CladAbsE.size());
  fluorinatedPolymer->SetMaterialPropertiesTable(fluorinatedPolymerMpt);
}

G4VPhysicalVolume* NovaDetectorConstruction::Construct()
{
  DefineMaterials();
  return ConstructDetector();
}

G4VPhysicalVolume* NovaDetectorConstruction::ConstructDetector()
{
  G4double expHall_x = getCellWidth() / 2.0  + 20.0*cm;
  G4double expHall_y = getCellHeight() / 2.0  + 20.0*cm;
  G4double expHall_z = getCellToPmtDistance() + 20.0*cm;

  experimentalHallBox  = new G4Box("expHall_box", expHall_x, expHall_y, expHall_z);
  experimentalHallLog  = new G4LogicalVolume(experimentalHallBox, vacuum, "expHall_log", 0, 0, 0);
  experimentalHallPhy = new G4PVPlacement(0, G4ThreeVector(), experimentalHallLog, "expHall", 0, false, 0);

  if(mainVolume){
    new NovaMainVolume(0, G4ThreeVector(), experimentalHallLog, false, 0, this);
  }

  return experimentalHallPhy;
}

void NovaDetectorConstruction::printSettings()
{
  G4cout << std::setw(25) << "scintillatorLightYield = " << std::setw(10) << liquidScintillatorMpt->GetConstProperty("SCINTILLATIONYIELD") << std::setw(10) << " MeV-1" << G4endl;
  G4cout << std::setw(25) << "cellLength = " << std::setw(10) << cellLength / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "rectangleWidth = " << std::setw(10) << rectangleWidth / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(25) << "rectangleHeight = " << std::setw(10) << rectangleHeight / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(25) << "innerCornerRadius = " << std::setw(10) << innerCornerRadius / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(25) << "pvcThickness = " << std::setw(10) << pvcThickness / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(25) << "fiberRadius = " << std::setw(10) << fiberRadius / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(25) << "fiber1X = " << std::setw(10) << fiber1X / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "fiber1Y = " << std::setw(10) << fiber1Y / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "fiber2X = " << std::setw(10) << fiber2X / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "fiber2Y = " << std::setw(10) << fiber2Y / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "cellToPmtDistance = " << std::setw(10) << cellToPmtDistance / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "fiberTailLength = " << std::setw(10) << fiberTailLength / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(25) << "usePmt = " << std::setw(10) << usePmt << G4endl;
  G4cout << G4endl;
}

void NovaDetectorConstruction::setDefaults()
{
  scintillatorLightYield = 100.0;
  rectangleHeight = 40.0*mm;
  rectangleWidth = 17.7*mm;
  innerCornerRadius = 9.7*mm;
  pvcThickness = 3.3*mm;
  fiberRadius = 0.35*mm;
  cellLength = 120.0*cm;
  fiber1X = 1.0*cm;
  fiber1Y = 0.0;
  fiber2X = -1.0*cm;
  fiber2Y = 0.0;
  cellToPmtDistance = 100.0*cm;
  fiberTailLength = 10.0*cm;
  usePmt = true;
  mainVolume = false;
  inputDir = "/Users/juntinghuang/Desktop/nova/input/";
  G4UImanager::GetUIpointer()->ApplyCommand("/LXe/detector/scintYieldFactor 1.");
  isUpdated = true;
}

void NovaDetectorConstruction::updateGeometry()
{
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalSkinSurface::CleanSurfaceTable();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4SurfaceProperty::CleanSurfacePropertyTable();

  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  isUpdated = false;
}

void NovaDetectorConstruction::setMainScintYield(G4double y)
{
  liquidScintillatorMpt->AddConstProperty("SCINTILLATIONYIELD", y/MeV);
}
