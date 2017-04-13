#include <TString.h>
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
#include "G4SystemOfUnits.hh"


NovaDetectorConstruction::NovaDetectorConstruction()
{
  liquidScintillatorMpt = NULL;
  experimentalHallSolid = NULL;
  experimentalHallLogicalVolume = NULL;
  experimentalHallPhysicalVolume = NULL;
  tiO2 = pvc = liquidScintillator = NULL;
  isUpdated = false;

  setDefaults();
}

NovaDetectorConstruction::~NovaDetectorConstruction()
{}

void NovaDetectorConstruction::defineLiquidScintillator()
{
  liquidScintillator = new G4Material("liquidScintillator",  1.032*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  liquidScintillator->AddElement(H, 0.666);
  liquidScintillator->AddElement(C, 0.334);
  liquidScintillatorMpt = new G4MaterialPropertiesTable();
  G4double refractionIndex = 1.47;

  std::vector<G4double> emissionEnergies;
  std::vector<G4double> fastEmissions;
  std::vector<G4double> slowEmissions;
  std::ifstream fEmission(getFilePath(PPO_EMISSION_FILENAME));
  if(fEmission.is_open()){
    while(!fEmission.eof()){
      fEmission >> inputWavelength >> filler >> inputVariable;
      G4double photonEnergy =convertWavelengthToEnergy(inputWavelength);
      emissionEnergies.push_back(photonEnergy);
      fastEmissions.push_back(inputVariable);
      slowEmissions.push_back(0.0);
    }
  }

  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;
  std::ifstream fAbsorption(getFilePath(POLYSTYRENE_BULK_ABSORPTION_FILENAME));
  if(fAbsorption.is_open()){
    while(!fAbsorption.eof()){
      fAbsorption >> inputWavelength >> filler >> inputVariable;
      absorptionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      absorptions.push_back(1.8 * inputVariable * m);
    }
  }

  std::vector<G4double> wlsAbsorptionEnergies;
  std::vector<G4double> wlsAbsorptions;
  std::ifstream fWlsAbsorption(getFilePath(LIQUID_SCINTILLATOR_WLS_ABSORPTION_FILENAME));
  if (fWlsAbsorption.is_open()){
    while (!fWlsAbsorption.eof()){
      fWlsAbsorption >> inputWavelength >> filler >> inputVariable;
      wlsAbsorptionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      wlsAbsorptions.push_back(1.8 * inputVariable * m);
    }
  }

  std::vector<G4double> wlsEmissionEnergies;
  std::vector<G4double> wlsEmissions;
  std::ifstream fWlsEmission(getFilePath(BISMSB_EMISSION_FILENAME));
  if(fWlsEmission.is_open()){
    while (!fWlsEmission.eof()){
      fWlsEmission >> inputWavelength >> filler >> inputVariable;
      wlsEmissionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      wlsEmissions.push_back(inputVariable);
    }
  }

  std::vector<G4double> refractionIndices(emissionEnergies.size(), refractionIndex);

  liquidScintillatorMpt->AddProperty("RINDEX", &emissionEnergies[0], &refractionIndices[0], (G4int) emissionEnergies.size());
  liquidScintillatorMpt->AddProperty("FASTCOMPONENT", &emissionEnergies[0], &fastEmissions[0], (G4int) emissionEnergies.size());
  liquidScintillatorMpt->AddProperty("SLOWCOMPONENT", &emissionEnergies[0], &slowEmissions[0], (G4int) emissionEnergies.size());
  liquidScintillatorMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  liquidScintillatorMpt->AddProperty("WLSABSLENGTH", &wlsAbsorptionEnergies[0], &wlsAbsorptions[0], (G4int) wlsAbsorptionEnergies.size());
  liquidScintillatorMpt->AddProperty("WLSCOMPONENT", &wlsEmissionEnergies[0], &wlsEmissions[0], (G4int) wlsEmissionEnergies.size());
  liquidScintillatorMpt->AddConstProperty("SCINTILLATIONYIELD", liquidScintillatorLightYield / MeV);
  liquidScintillatorMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.9);
  liquidScintillatorMpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  liquidScintillatorMpt->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns);
  liquidScintillatorMpt->AddConstProperty("SLOWTIMECONSTANT", 10.0 * ns);
  liquidScintillatorMpt->AddConstProperty("WLSTIMECONSTANT", 1.0 * ns);
  liquidScintillatorMpt->AddConstProperty("YIELDRATIO", 1.0);
  liquidScintillator->SetMaterialPropertiesTable(liquidScintillatorMpt);
  liquidScintillator->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
}

void NovaDetectorConstruction::defineGlass()
{
  G4Material* glass = nistManager->FindOrBuildMaterial("G4_GLASS_PLATE");

  const G4int energyCount = 2;
  G4double energies[energyCount] = {convertWavelengthToEnergy(200.0), convertWavelengthToEnergy(700.0)};
  G4double refractionIndices[energyCount] = {1.49, 1.49};
  G4double absorptionLengths[energyCount] = {420.0 * cm, 420.0 * cm};
  G4MaterialPropertiesTable* glassMpt = new G4MaterialPropertiesTable();
  glassMpt->AddProperty("ABSLENGTH", energies, absorptionLengths, energyCount);
  glassMpt->AddProperty("RINDEX", energies, refractionIndices, energyCount);
  glass->SetMaterialPropertiesTable(glassMpt);
}

void NovaDetectorConstruction::definePolystyrene()
{
  polystyrene = new G4Material("polystyrene",  1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  polystyrene->AddElement(H, 0.498);
  polystyrene->AddElement(C, 0.502);

  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;
  std::ifstream fAbsorption(getFilePath(FIBER_CORE_ABSORPTION_LENGTH_FILENAME));
  if(fAbsorption.is_open()) {
    while(!fAbsorption.eof()) {
      fAbsorption >> inputWavelength >> filler >> inputVariable;
      absorptionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      absorptions.push_back(inputVariable * m);
    }
  }

  std::vector<G4double> wlsAbsorptionEnergies;
  std::vector<G4double> wlsAbsorptions;
  std::ifstream fWlsAbsorption(getFilePath(FIBER_CORE_WLS_ABSORPTION_LENGTH_FILENAME));
  if (fWlsAbsorption.is_open()) {
    while(!fWlsAbsorption.eof()) {
      fWlsAbsorption >> inputWavelength >> filler >> inputVariable;
      G4double energy = convertWavelengthToEnergy(inputWavelength);
      wlsAbsorptionEnergies.push_back(energy);
      wlsAbsorptions.push_back(inputVariable * m);
    }
  }

  std::vector<G4double> wlsEmissionEnergies;
  std::vector<G4double> wlsEmissions;
  std::ifstream fWlsEmission(getFilePath(FIBER_CORE_WLS_EMISSION_FILENAME));
  if(fWlsEmission.is_open()){
    while(!fWlsEmission.eof()){
      fWlsEmission >> inputWavelength >> filler >> inputVariable;
      wlsEmissionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      wlsEmissions.push_back(inputVariable);
    }
  }

  G4double refractionIndex = 1.59;
  std::vector<G4double> refractionIndices(wlsAbsorptionEnergies.size(), refractionIndex);

  polystyreneMpt = new G4MaterialPropertiesTable();
  polystyreneMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  polystyreneMpt->AddProperty("WLSABSLENGTH", &wlsAbsorptionEnergies[0],  &wlsAbsorptions[0],  (G4int) wlsAbsorptionEnergies.size());
  polystyreneMpt->AddProperty("WLSCOMPONENT", &wlsEmissionEnergies[0], &wlsEmissions[0], (G4int) wlsEmissionEnergies.size());
  polystyreneMpt->AddConstProperty("WLSTIMECONSTANT", 11.8 * ns);
  polystyreneMpt->AddProperty("RINDEX", &wlsAbsorptionEnergies[0], &refractionIndices[0], (G4int) wlsAbsorptionEnergies.size());
  polystyreneMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.7);
  polystyrene->SetMaterialPropertiesTable(polystyreneMpt);
}

void NovaDetectorConstruction::definePmma()
{
  pmma = new G4Material("pmma", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  pmma->AddElement(H, 0.532);
  pmma->AddElement(C, 0.336);
  pmma->AddElement(O, 0.132);

  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;

  std::ifstream fPmmaAbsorption(getFilePath(PMMA_ABSORPTION_LENGTH_FILENAME));
  if(fPmmaAbsorption.is_open()){
    while(!fPmmaAbsorption.eof()){
      fPmmaAbsorption >> inputWavelength >> filler >> inputVariable;
      absorptionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      absorptions.push_back(inputVariable * m);
    }
  }

  G4double refractionIndex = 1.49;
  std::vector<G4double> refractionIndices(absorptionEnergies.size(), refractionIndex);

  pmmaMpt = new G4MaterialPropertiesTable();
  pmmaMpt->AddProperty("RINDEX", &absorptionEnergies[0], &refractionIndices[0], (G4int) absorptionEnergies.size());
  pmmaMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  pmma->SetMaterialPropertiesTable(pmmaMpt);
}

void NovaDetectorConstruction::defineFluorinatedPolymer()
{
  fluorinatedPolymer = new G4Material("fluorinatedPolymer", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  fluorinatedPolymer->AddElement(H, 0.532);
  fluorinatedPolymer->AddElement(C, 0.336);
  fluorinatedPolymer->AddElement(O, 0.132);

  G4MaterialPropertyVector* pmmaAbslengthMaterialPropertyVector = pmma->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH");

  std::vector<G4double> absorptionEnergies;
  for (size_t i = 0; i < pmmaAbslengthMaterialPropertyVector->GetVectorLength(); i++) {
    G4double energy = pmmaAbslengthMaterialPropertyVector->Energy(i);
    absorptionEnergies.push_back(energy);
  }

  G4double refractionIndex = 1.42;
  std::vector<G4double> refractionIndices(absorptionEnergies.size(), refractionIndex);

  fluorinatedPolymerMpt = new G4MaterialPropertiesTable();
  fluorinatedPolymerMpt->AddProperty("RINDEX", &absorptionEnergies[0], &refractionIndices[0], (G4int) absorptionEnergies.size());
  fluorinatedPolymerMpt->AddProperty("ABSLENGTH", pmmaAbslengthMaterialPropertyVector);
  fluorinatedPolymer->SetMaterialPropertiesTable(fluorinatedPolymerMpt);
}

void NovaDetectorConstruction::definePvc()
{
  tiO2 = new G4Material("tiO2", 4.23*g/cm3, 2, kStateSolid);
  tiO2->AddElement(O, 2);
  tiO2->AddElement(Ti, 1);

  pvc = new G4Material("pvc", 1.4316*g/cm3, 2, kStateSolid);
  pvc->AddMaterial(tiO2, 0.15);
  pvc->AddMaterial(polystyrene, 0.85);
}

void NovaDetectorConstruction::defineMaterials()
{
  nistManager = G4NistManager::Instance();
  H = nistManager->FindOrBuildElement("H");
  C = nistManager->FindOrBuildElement("C");
  O = nistManager->FindOrBuildElement("O");
  Ti = nistManager->FindOrBuildElement("Ti");

  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_Al");

  defineLiquidScintillator();
  defineGlass();
  definePolystyrene();
  definePmma();
  defineFluorinatedPolymer();
  definePvc();
}

G4VPhysicalVolume* NovaDetectorConstruction::Construct()
{
  defineMaterials();
  return constructDetector();
}

G4VPhysicalVolume* NovaDetectorConstruction::constructDetector()
{
  G4double expHall_x = getCellWidth() / 2.0  + 20.0*cm;
  G4double expHall_y = getCellHeight() / 2.0  + 20.0*cm;
  G4double expHall_z = getCellToPmtDistance() + 20.0*cm;

  experimentalHallSolid = new G4Box("experimentalHallSolid", expHall_x, expHall_y, expHall_z);
  experimentalHallLogicalVolume  = new G4LogicalVolume(experimentalHallSolid, G4Material::GetMaterial("G4_Galactic"), "experimentalHallLogicalVolume", 0, 0, 0);
  experimentalHallPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(), experimentalHallLogicalVolume, "experimentalHallPhysicalVolume", 0, false, 0);

  if(mainVolume){
    new NovaMainVolume(0, G4ThreeVector(), experimentalHallLogicalVolume, false, 0, this);
  }

  return experimentalHallPhysicalVolume;
}

void NovaDetectorConstruction::printSettings()
{
  G4cout << std::setw(25) << "liquidScintillatorLightYield = " << std::setw(10) << liquidScintillatorMpt->GetConstProperty("SCINTILLATIONYIELD") << std::setw(10) << " MeV-1" << G4endl;
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
  liquidScintillatorLightYield = 100.0;
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

  G4RunManager::GetRunManager()->DefineWorldVolume(constructDetector());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  isUpdated = false;
}

void NovaDetectorConstruction::setMainScintYield(G4double y)
{
  liquidScintillatorMpt->AddConstProperty("SCINTILLATIONYIELD", y / MeV);
}
