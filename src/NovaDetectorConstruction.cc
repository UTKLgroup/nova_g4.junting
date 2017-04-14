#include <TString.h>
#include "NovaDetectorConstruction.hh"
#include "NovaPmtSd.hh"
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

NovaDetectorConstruction::NovaDetectorConstruction()
{
  nistManager = G4NistManager::Instance();
  isUpdated = false;
  setDefaults();
}

NovaDetectorConstruction::~NovaDetectorConstruction()
{}

void NovaDetectorConstruction::defineLiquidScintillator(G4String materialName)
{
  liquidScintillator = new G4Material(materialName,  1.032*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  liquidScintillator->AddElement(H, 0.666);
  liquidScintillator->AddElement(C, 0.334);
  liquidScintillatorMpt = new G4MaterialPropertiesTable();

  std::vector<G4double> emissionEnergies;
  std::vector<G4double> fastEmissions;
  std::vector<G4double> slowEmissions;
  std::ifstream fEmission(getFilePath(LIQUID_SCINTILLATOR_EMISSION_FILENAME));
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
  std::ifstream fAbsorption(getFilePath(LIQUID_SCINTILLATOR_ABSORPTION_LENGTH_FILENAME));
  if(fAbsorption.is_open()){
    while(!fAbsorption.eof()){
      fAbsorption >> inputWavelength >> filler >> inputVariable;
      absorptionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      absorptions.push_back(1.8 * inputVariable * m);
    }
  }

  std::vector<G4double> wlsAbsorptionEnergies;
  std::vector<G4double> wlsAbsorptions;
  std::ifstream fWlsAbsorption(getFilePath(LIQUID_SCINTILLATOR_WLS_ABSORPTION_LENGTH_FILENAME));
  if (fWlsAbsorption.is_open()){
    while (!fWlsAbsorption.eof()){
      fWlsAbsorption >> inputWavelength >> filler >> inputVariable;
      wlsAbsorptionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      wlsAbsorptions.push_back(1.8 * inputVariable * m);
    }
  }

  std::vector<G4double> wlsEmissionEnergies;
  std::vector<G4double> wlsEmissions;
  std::ifstream fWlsEmission(getFilePath(LIQUID_SCINTILLATOR_WLS_EMISSION_FILENAME));
  if(fWlsEmission.is_open()){
    while (!fWlsEmission.eof()){
      fWlsEmission >> inputWavelength >> filler >> inputVariable;
      wlsEmissionEnergies.push_back(convertWavelengthToEnergy(inputWavelength));
      wlsEmissions.push_back(inputVariable);
    }
  }

  G4double refractionIndex = 1.47;
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

void NovaDetectorConstruction::defineFiberCore(G4String materialName)
{
  fiberCore = new G4Material(materialName,  1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  fiberCore->AddElement(H, 0.498);
  fiberCore->AddElement(C, 0.502);

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

  fiberCoreMpt = new G4MaterialPropertiesTable();
  fiberCoreMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  fiberCoreMpt->AddProperty("WLSABSLENGTH", &wlsAbsorptionEnergies[0],  &wlsAbsorptions[0],  (G4int) wlsAbsorptionEnergies.size());
  fiberCoreMpt->AddProperty("WLSCOMPONENT", &wlsEmissionEnergies[0], &wlsEmissions[0], (G4int) wlsEmissionEnergies.size());
  fiberCoreMpt->AddConstProperty("WLSTIMECONSTANT", 11.8 * ns);
  fiberCoreMpt->AddProperty("RINDEX", &wlsAbsorptionEnergies[0], &refractionIndices[0], (G4int) wlsAbsorptionEnergies.size());
  fiberCoreMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.7);
  fiberCore->SetMaterialPropertiesTable(fiberCoreMpt);
}

void NovaDetectorConstruction::definePmma(G4String materialName)
{
  pmma = new G4Material(materialName, 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  pmma->AddElement(H, 0.532);
  pmma->AddElement(C, 0.336);
  pmma->AddElement(O, 0.132);

  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;

  std::ifstream fPmmaAbsorption(getFilePath(FIBER_CLADDING_ABSORPTION_LENGTH_FILENAME));
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

void NovaDetectorConstruction::defineFluorinatedPolymer(G4String materialName)
{
  fluorinatedPolymer = new G4Material(materialName, 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
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

void NovaDetectorConstruction::definePvc(G4String materialName)
{
  tiO2 = new G4Material("tiO2", 4.23*g/cm3, 2, kStateSolid);
  tiO2->AddElement(O, 2);
  tiO2->AddElement(Ti, 1);

  polystyrene = new G4Material("polystyrene", 1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  polystyrene->AddElement(H, 0.498);
  polystyrene->AddElement(C, 0.502);

  pvc = new G4Material(materialName, 1.4316*g/cm3, 2, kStateSolid);
  pvc->AddMaterial(tiO2, 0.15);
  pvc->AddMaterial(polystyrene, 0.85);
}

void NovaDetectorConstruction::defineGalactic()
{
  nistManager->FindOrBuildMaterial("G4_Galactic");
  const G4int energyCount = 2;
  G4double energies[energyCount] = {convertWavelengthToEnergy(200.0), convertWavelengthToEnergy(700.0)};
  G4double refractionIndices[energyCount] = {1.0, 1.0};
  G4MaterialPropertiesTable* galacticMpt = new G4MaterialPropertiesTable();
  galacticMpt->AddProperty("RINDEX",
                           energies,
                           refractionIndices,
                           energyCount);
  G4Material::GetMaterial("G4_Galactic")->SetMaterialPropertiesTable(galacticMpt);
}

void NovaDetectorConstruction::defineMaterials()
{
  H = nistManager->FindOrBuildElement("H");
  C = nistManager->FindOrBuildElement("C");
  O = nistManager->FindOrBuildElement("O");
  Ti = nistManager->FindOrBuildElement("Ti");

  nistManager->FindOrBuildMaterial("G4_Al");

  defineLiquidScintillator("liquidScintillator");
  defineFiberCore("fiberCore");
  definePmma("pmma");
  defineFluorinatedPolymer("fluorinatedPolymer");
  definePvc("pvc");
  defineGlass();
  defineGalactic();
}

G4VPhysicalVolume* NovaDetectorConstruction::Construct()
{
  defineMaterials();
  return constructDetector();
}

G4VPhysicalVolume* NovaDetectorConstruction::constructDetector()
{
  return constructSingleWlsFiber();
}

G4VPhysicalVolume* NovaDetectorConstruction::constructSingleWlsFiber()
{
  G4double fiberLength = 0.1 * m;
  G4double experimentalHallX = fiberRadius * 2.0;
  G4double experimentalHallY = fiberRadius * 2.0;
  G4double experimentalHallZ = fiberLength * 0.6;

  experimentalHallSolid = new G4Box("experimentalHallSolid", experimentalHallX, experimentalHallY, experimentalHallZ);
  experimentalHallLogicalVolume  = new G4LogicalVolume(experimentalHallSolid,
                                                       G4Material::GetMaterial("G4_Galactic"),
                                                       "experimentalHallLogicalVolume",
                                                       0, 0, 0);
  experimentalHallPhysicalVolume = new G4PVPlacement(0,
                                                     G4ThreeVector(),
                                                     experimentalHallLogicalVolume,
                                                     "experimentalHallPhysicalVolume",
                                                     0, false, 0);

  G4double coreFraction = 1.0 - 2.0 * fiberCladdingFraction;
  G4double coreRadius  = fiberRadius * coreFraction;
  G4double innerCladdingRadius = coreRadius + fiberRadius * fiberCladdingFraction;

  G4Tubs* outerCladdingSolid = new G4Tubs("outerCladdingSolid", 0, fiberRadius, fiberLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* outerCladdingLogicalVolume = new G4LogicalVolume(outerCladdingSolid,
                                                                    G4Material::GetMaterial("fluorinatedPolymer"),
                                                                    "outerCladdingLogicalVolume");

  G4Tubs* innerCladdingSolid = new G4Tubs("innerCladdingSolid", 0, innerCladdingRadius, fiberLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* innerCladdingLogicalVolume = new G4LogicalVolume(innerCladdingSolid,
                                                                    G4Material::GetMaterial("pmma"),
                                                                    "innerCladdingLogicalVolume");

  G4Tubs* coreSolid = new G4Tubs("coreSolid", 0, coreRadius, fiberLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* coreLogicalVolume = new G4LogicalVolume(coreSolid,
                                                           G4Material::GetMaterial("fiberCore"),
                                                           "coreLogicalVolume");

  new G4PVPlacement(0, G4ThreeVector(), coreLogicalVolume, "core", innerCladdingLogicalVolume, false, 0);
  new G4PVPlacement(0, G4ThreeVector(), innerCladdingLogicalVolume, "innerCladding", outerCladdingLogicalVolume, false, 0);
  new G4PVPlacement(0, G4ThreeVector(), outerCladdingLogicalVolume, "wlsFiber", experimentalHallLogicalVolume, false, 0);

  G4LogicalVolume* pmtLogicalVolume = makePmt();
  new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, -fiberLength / 2.0 - pmtThickness / 2.0),
                    pmtLogicalVolume,
                    "pmt",
                    experimentalHallLogicalVolume,
                    false,
                    0);

  return experimentalHallPhysicalVolume;
}

G4VPhysicalVolume* NovaDetectorConstruction::constructLiquidScintillatorCell()
{
  G4double experimentalHallX = getCellWidth() / 2.0  + 20.0*cm;
  G4double experimentalHallY = getCellHeight() / 2.0  + 20.0*cm;
  G4double experimentalHallZ = getCellToPmtDistance() + 20.0*cm;

  experimentalHallSolid = new G4Box("experimentalHallSolid",experimentalHallX, experimentalHallY, experimentalHallZ);
  experimentalHallLogicalVolume  = new G4LogicalVolume(experimentalHallSolid,
                                                       G4Material::GetMaterial("G4_Galactic"),
                                                       "experimentalHallLogicalVolume",
                                                       0, 0, 0);
  experimentalHallPhysicalVolume = new G4PVPlacement(0,
                                                     G4ThreeVector(),
                                                     experimentalHallLogicalVolume,
                                                     "experimentalHallPhysicalVolume",
                                                     0, false, 0);

  G4UnionSolid* pvcSolid = makePvcCell();
  G4LogicalVolume* pvcLogicalVolume = new G4LogicalVolume(pvcSolid,
                                                          G4Material::GetMaterial("pvc"),
                                                          "pvcLogicalVolume",
                                                          0, 0, 0);
  new G4PVPlacement(0,
                    G4ThreeVector(0, -getCellHeight() / 2.0 + pvcThickness / 2.0),
                    pvcLogicalVolume,
                    "pvc",
                    experimentalHallLogicalVolume,
                    false, 0);

  G4UnionSolid* liquidScintillatorSolid = makeLiquidScintillator();
  G4LogicalVolume* liquidScintillatorLogicalVolume = new G4LogicalVolume(liquidScintillatorSolid,
                                                                         G4Material::GetMaterial("liquidScintillator"),
                                                                         "liquidScintillatorLogicalVolume",
                                                                         0, 0, 0);
  new G4PVPlacement(0,
                    G4ThreeVector(),
                    liquidScintillatorLogicalVolume,
                    "liquidScintillator",
                    experimentalHallLogicalVolume,
                    false, 0);

  G4LogicalVolume* wlsFiberLogicalVolume = makeWlsFiber();
  new G4PVPlacement(0, G4ThreeVector(), wlsFiberLogicalVolume, "wlsFiber", liquidScintillatorLogicalVolume, false, 0);

  return experimentalHallPhysicalVolume;
}

G4UnionSolid* NovaDetectorConstruction::makePvcCell()
{
  G4double outerCornerRadius = getOuterCornerRadius();

  G4Box* pvcHorizontal = new G4Box("pvcHorizontal",
                                   rectangleWidth / 2.0,
                                   pvcThickness / 2.0,
                                   cellLength / 2.0);

  G4Box* pvcVertical = new G4Box("pvcVertical",
                                 rectangleHeight / 2.0,
                                 pvcThickness / 2.0,
                                 cellLength / 2.0);

  G4Tubs* pvcCorner = new G4Tubs("pvcCorner",
                                 innerCornerRadius,
                                 outerCornerRadius,
                                 cellLength / 2.0,
                                 0.0,
                                 0.5 * CLHEP::pi * rad);

  G4RotationMatrix rotate90, rotate180, rotate270;
  rotate90.rotateZ(90 * deg);
  rotate180.rotateZ(180 * deg);
  rotate270.rotateZ(270 * deg);

  G4UnionSolid* pvcPart1 = new G4UnionSolid("pvcPart1",
                                            pvcHorizontal,
                                            pvcCorner,
                                            &rotate90,
                                            G4ThreeVector(rectangleWidth / 2.0,
                                                          innerCornerRadius + pvcThickness / 2.0));

  G4UnionSolid* pvcPart2 = new G4UnionSolid("pvcPart2",
                                            pvcPart1,
                                            pvcVertical,
                                            &rotate90,
                                            G4ThreeVector(rectangleWidth / 2.0 + innerCornerRadius + pvcThickness / 2.0,
                                                          innerCornerRadius + pvcThickness / 2.0 + rectangleHeight / 2.0));

  G4UnionSolid* pvcPart3 = new G4UnionSolid("pvcPart3",
                                            pvcPart2,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(rectangleWidth / 2.0,
                                                          innerCornerRadius + pvcThickness / 2.0 + rectangleHeight));

  G4UnionSolid* pvcPart4 = new G4UnionSolid("pvcPart4",
                                            pvcPart3,
                                            pvcHorizontal,
                                            0,
                                            G4ThreeVector(0,
                                                          2.0 * innerCornerRadius + rectangleHeight + pvcThickness));

  G4UnionSolid* pvcPart5 = new G4UnionSolid("pvcPart5",
                                            pvcPart4,
                                            pvcCorner,
                                            &rotate270,
                                            G4ThreeVector(-rectangleWidth / 2.0,
                                                          innerCornerRadius + pvcThickness / 2.0 + rectangleHeight));

  G4UnionSolid* pvcPart6 = new G4UnionSolid("pvcPart6",
                                            pvcPart5,
                                            pvcVertical,
                                            &rotate90,
                                            G4ThreeVector(-(rectangleWidth / 2.0 + innerCornerRadius + pvcThickness / 2.0),
                                                          innerCornerRadius + pvcThickness / 2.0 + rectangleHeight / 2.0));

  G4UnionSolid* pvcPart7 = new G4UnionSolid("pvcPart7",
                                            pvcPart6,
                                            pvcCorner,
                                            &rotate180,
                                            G4ThreeVector(-rectangleWidth / 2.0,
                                                          innerCornerRadius + pvcThickness / 2.0));

  return pvcPart7;
}

G4UnionSolid* NovaDetectorConstruction::makeLiquidScintillator(){
  G4double fullHeight = rectangleHeight + 2. * innerCornerRadius;

  G4Box* box1 = new G4Box("box1",
                          rectangleWidth / 2.,
                          fullHeight / 2.,
                          cellLength / 2.);

  G4Box* box2 = new G4Box("box2",
                          innerCornerRadius / 2.,
                          rectangleHeight / 2.,
                          cellLength / 2.);

  G4Tubs* innerCorner = new G4Tubs("innerCorner",
                                   0.0 * mm,
                                   innerCornerRadius,
                                   cellLength / 2.0,
                                   0.0,
                                   CLHEP::pi / 2.0 * rad);

  G4UnionSolid* unionBox1 = new G4UnionSolid("unionBox1",
                                             box1,
                                             box2,
                                             0,
                                             G4ThreeVector(rectangleWidth / 2. + innerCornerRadius / 2., 0, 0));

  G4UnionSolid* unionBox2 = new G4UnionSolid("unionBox2",
                                             unionBox1,
                                             box2,
                                             0,
                                             G4ThreeVector(-rectangleWidth / 2. - innerCornerRadius / 2., 0, 0));

  G4UnionSolid* unionCorner1 = new G4UnionSolid("unionCorner1",
                                                unionBox2,
                                                innerCorner,
                                                0,
                                                G4ThreeVector(rectangleWidth / 2.,
                                                              rectangleHeight / 2.,
                                                              0));

  G4RotationMatrix rotate270;
  rotate270.rotateZ(270 * deg);
  G4UnionSolid* unionCorner2 = new G4UnionSolid("unionCorner2",
                                                unionCorner1,
                                                innerCorner,
                                                &rotate270,
                                                G4ThreeVector(-rectangleWidth / 2.,
                                                              rectangleHeight / 2.,
                                                              0));

  G4RotationMatrix rotate90;
  rotate90.rotateZ(90 * deg);
  G4UnionSolid* unionCorner3 = new G4UnionSolid("unionCorner3",
                                                unionCorner2,
                                                innerCorner,
                                                &rotate90,
                                                G4ThreeVector(rectangleWidth / 2.,
                                                              -rectangleHeight / 2.,
                                                              0));

  G4RotationMatrix rotate180;
  rotate180.rotateZ(180 * deg);
  G4UnionSolid* unionCorner4 = new G4UnionSolid("unionCorner4",
                                                unionCorner3,
                                                innerCorner,
                                                &rotate180,
                                                G4ThreeVector(-rectangleWidth / 2.,
                                                              -rectangleHeight / 2.,
                                                              0));

  return unionCorner4;
}

G4LogicalVolume* NovaDetectorConstruction::makeWlsFiber()
{
  G4double coreFraction = 1. - 2. * fiberCladdingFraction;
  G4double coreRadius  = fiberRadius * coreFraction;
  G4double innerCladdingRadius = coreRadius + fiberRadius * fiberCladdingFraction;

  G4Tubs* outerCladdingSolid = new G4Tubs("outerCladdingSolid", 0, fiberRadius, cellLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* outerCladdingLogicalVolume = new G4LogicalVolume(outerCladdingSolid,
                                                                    G4Material::GetMaterial("fluorinatedPolymer"),
                                                                    "outerCladdingLogicalVolume");

  G4Tubs* innerCladdingSolid = new G4Tubs("innerCladdingSolid", 0, innerCladdingRadius, cellLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* innerCladdingLogicalVolume = new G4LogicalVolume(innerCladdingSolid,
                                                                    G4Material::GetMaterial("pmma"),
                                                                    "innerCladdingLogicalVolume");

  G4Tubs* coreSolid = new G4Tubs("coreSolid", 0, coreRadius, cellLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* coreLogicalVolume = new G4LogicalVolume(coreSolid,
                                                           G4Material::GetMaterial("fiberCore"),
                                                           "coreLogicalVolume");

  new G4PVPlacement(0, G4ThreeVector(), coreLogicalVolume, "core", innerCladdingLogicalVolume, false, 0);
  new G4PVPlacement(0, G4ThreeVector(), innerCladdingLogicalVolume, "innerCladding", outerCladdingLogicalVolume, false, 0);

  return outerCladdingLogicalVolume;
}

G4LogicalVolume* NovaDetectorConstruction::makePmt()
{
  G4Tubs* pmtSolid = new G4Tubs("pmtSolid",
                                0.0,
                                fiberRadius,
                                pmtThickness / 2.0,
                                0.0,
                                CLHEP::twopi);
  G4LogicalVolume* pmtLogicalVolume = new G4LogicalVolume(pmtSolid,
                                                          G4Material::GetMaterial("G4_GLASS_PLATE"),
                                                          "pmtLogicalVolume");

  G4Tubs* photocathodeSolid = new G4Tubs("photocathodeSolid",
                                         0.0,
                                         fiberRadius,
                                         pmtThickness / 4.0,
                                         0.0,
                                         CLHEP::twopi);
  G4LogicalVolume* photocathodeLogicalVolume = new G4LogicalVolume(photocathodeSolid,
                                                                   G4Material::GetMaterial("G4_Al"),
                                                                   "photocathodeLogicalVolume");
  new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, pmtThickness / 4.0),
                    photocathodeLogicalVolume,
                    "photocathode",
                    pmtLogicalVolume,
                    false,
                    0);

  return pmtLogicalVolume;
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
  rectangleHeight = 40.0 * mm;
  rectangleWidth = 17.7 * mm;
  innerCornerRadius = 9.7 * mm;
  pvcThickness = 3.3 * mm;
  fiberRadius = 0.35 * mm;
  fiberCladdingFraction = 0.03;
  cellLength = 120.0 * cm;
  fiber1X = 1.0 * cm;
  fiber1Y = 0.0;
  fiber2X = -1.0 * cm;
  fiber2Y = 0.0;
  cellToPmtDistance = 100.0 * cm;
  fiberTailLength = 10.0 * cm;
  usePmt = true;
  pmtThickness = 1.0 * mm;
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
