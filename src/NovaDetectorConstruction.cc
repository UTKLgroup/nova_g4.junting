#include <TString.h>
#include <G4PVPlacement.hh>
#include <G4SubtractionSolid.hh>
#include "NovaDetectorConstruction.hh"
#include "NovaPmtSd.hh"
#include "NovaLiquidScintillatorSd.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4OpticalSurface.hh"


NovaDetectorConstruction::NovaDetectorConstruction()
{
  nistManager = G4NistManager::Instance();
  defineMaterials();

  isUpdated = false;
  setDefaults();
  new NovaDetectorMessenger(this);
}

NovaDetectorConstruction::~NovaDetectorConstruction()
{}

void NovaDetectorConstruction::setLiquidScintillatorProperty()
{
  liquidScintillatorMpt = new G4MaterialPropertiesTable();

  std::vector<G4double> emissionEnergies;
  std::vector<G4double> fastEmissions;
  readCsvFile(LIQUID_SCINTILLATOR_EMISSION_FILENAME, emissionEnergies, fastEmissions, 1.0);
  std::vector<G4double> slowEmissions(emissionEnergies.size(), 0.0);

  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;
  readCsvFile(LIQUID_SCINTILLATOR_ABSORPTION_LENGTH_FILENAME, absorptionEnergies, absorptions, 1.8 * m);

  std::vector<G4double> wlsAbsorptionEnergies;
  std::vector<G4double> wlsAbsorptions;
  readCsvFile(LIQUID_SCINTILLATOR_WLS_ABSORPTION_LENGTH_FILENAME, wlsAbsorptionEnergies, wlsAbsorptions, 1.8 * m);

  std::vector<G4double> wlsEmissionEnergies;
  std::vector<G4double> wlsEmissions;
  readCsvFile(LIQUID_SCINTILLATOR_WLS_EMISSION_FILENAME, wlsEmissionEnergies, wlsEmissions, 1.0);

  G4double refractionIndex = 1.47;
  std::vector<G4double> refractionIndices(emissionEnergies.size(), refractionIndex);

  liquidScintillatorMpt->AddProperty("RINDEX", &emissionEnergies[0], &refractionIndices[0], (G4int) emissionEnergies.size());
  liquidScintillatorMpt->AddProperty("FASTCOMPONENT", &emissionEnergies[0], &fastEmissions[0], (G4int) emissionEnergies.size());
  liquidScintillatorMpt->AddProperty("SLOWCOMPONENT", &emissionEnergies[0], &slowEmissions[0], (G4int) emissionEnergies.size());
  liquidScintillatorMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  liquidScintillatorMpt->AddProperty("WLSABSLENGTH", &wlsAbsorptionEnergies[0], &wlsAbsorptions[0], (G4int) wlsAbsorptionEnergies.size());
  liquidScintillatorMpt->AddProperty("WLSCOMPONENT", &wlsEmissionEnergies[0], &wlsEmissions[0], (G4int) wlsEmissionEnergies.size());
  liquidScintillatorMpt->AddConstProperty("SCINTILLATIONYIELD", liquidScintillatorLightYield);
  liquidScintillatorMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.9);
  liquidScintillatorMpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  liquidScintillatorMpt->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns);
  liquidScintillatorMpt->AddConstProperty("SLOWTIMECONSTANT", 10.0 * ns);
  liquidScintillatorMpt->AddConstProperty("WLSTIMECONSTANT", 1.0 * ns);
  liquidScintillatorMpt->AddConstProperty("YIELDRATIO", 1.0);
  liquidScintillator->SetMaterialPropertiesTable(liquidScintillatorMpt);
  liquidScintillator->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
}

void NovaDetectorConstruction::setGlassProperty()
{
  const G4int energyCount = 2;
  G4double energies[energyCount] = {convertWavelengthToEnergy(200.0), convertWavelengthToEnergy(700.0)};
  G4double refractionIndices[energyCount] = {1.49, 1.49};
  G4double absorptionLengths[energyCount] = {420.0 * cm, 420.0 * cm};
  G4MaterialPropertiesTable* glassMpt = new G4MaterialPropertiesTable();
  glassMpt->AddProperty("ABSLENGTH", energies, absorptionLengths, energyCount);
  glassMpt->AddProperty("RINDEX", energies, refractionIndices, energyCount);
  glass->SetMaterialPropertiesTable(glassMpt);
}

void NovaDetectorConstruction::setFiberCoreProperty()
{
  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;
  readCsvFile(FIBER_CORE_ABSORPTION_LENGTH_FILENAME, absorptionEnergies, absorptions, m);

  std::vector<G4double> wlsAbsorptionEnergies;
  std::vector<G4double> wlsAbsorptions;
  readCsvFile(FIBER_CORE_WLS_ABSORPTION_LENGTH_FILENAME, wlsAbsorptionEnergies, wlsAbsorptions, m);

  std::vector<G4double> wlsEmissionEnergies;
  std::vector<G4double> wlsEmissions;
  readCsvFile(FIBER_CORE_WLS_EMISSION_FILENAME, wlsEmissionEnergies, wlsEmissions, 1.0);

  std::vector<G4double> refractionIndexEnergies;
  std::vector<G4double> refractionIndices;
  readCsvFile(FIBER_CORE_REFRACTION_INDEX_FILENAME, refractionIndexEnergies, refractionIndices, 1.0);

  fiberCoreMpt = new G4MaterialPropertiesTable();
  fiberCoreMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  fiberCoreMpt->AddProperty("WLSABSLENGTH", &wlsAbsorptionEnergies[0],  &wlsAbsorptions[0],  (G4int) wlsAbsorptionEnergies.size());
  fiberCoreMpt->AddProperty("WLSCOMPONENT", &wlsEmissionEnergies[0], &wlsEmissions[0], (G4int) wlsEmissionEnergies.size());
  fiberCoreMpt->AddConstProperty("WLSTIMECONSTANT", 11.8 * ns);
  fiberCoreMpt->AddProperty("RINDEX", &refractionIndexEnergies[0], &refractionIndices[0], (G4int) refractionIndexEnergies.size());
  fiberCoreMpt->AddConstProperty("CONSTANTQUANTUMYIELD", 0.7);
  fiberCore->SetMaterialPropertiesTable(fiberCoreMpt);
}

void NovaDetectorConstruction::setPmmaProperty()
{
  std::vector<G4double> absorptionEnergies;
  std::vector<G4double> absorptions;
  readCsvFile(FIBER_CLADDING_ABSORPTION_LENGTH_FILENAME, absorptionEnergies, absorptions, m);

  std::vector<G4double> refractionIndexEnergies;
  std::vector<G4double> refractionIndices;
  readCsvFile(FIBER_FIRST_CLADDING_REFRACTION_INDEX_FILENAME, refractionIndexEnergies, refractionIndices, 1.0);

  pmmaMpt = new G4MaterialPropertiesTable();
  pmmaMpt->AddProperty("RINDEX", &refractionIndexEnergies[0], &refractionIndices[0], (G4int) refractionIndexEnergies.size());
  pmmaMpt->AddProperty("ABSLENGTH", &absorptionEnergies[0], &absorptions[0], (G4int) absorptionEnergies.size());
  pmma->SetMaterialPropertiesTable(pmmaMpt);
}

void NovaDetectorConstruction::setFluorinatedPolymerProperty()
{
  G4MaterialPropertyVector* pmmaAbslengthMaterialPropertyVector = pmma->GetMaterialPropertiesTable()->GetProperty("ABSLENGTH");

  std::vector<G4double> absorptionEnergies;
  for (size_t i = 0; i < pmmaAbslengthMaterialPropertyVector->GetVectorLength(); i++) {
    G4double energy = pmmaAbslengthMaterialPropertyVector->Energy(i);
    absorptionEnergies.push_back(energy);
  }

  std::vector<G4double> refractionIndexEnergies;
  std::vector<G4double> refractionIndices;
  readCsvFile(FIBER_SECOND_CLADDING_REFRACTION_INDEX_FILENAME, refractionIndexEnergies, refractionIndices, 1.0);

  fluorinatedPolymerMpt = new G4MaterialPropertiesTable();
  fluorinatedPolymerMpt->AddProperty("RINDEX", &refractionIndexEnergies[0], &refractionIndices[0], (G4int) refractionIndexEnergies.size());
  fluorinatedPolymerMpt->AddProperty("ABSLENGTH", pmmaAbslengthMaterialPropertyVector);
  fluorinatedPolymer->SetMaterialPropertiesTable(fluorinatedPolymerMpt);
}

void NovaDetectorConstruction::setGalacticProperty()
{
  const G4int energyCount = 2;
  G4double energies[energyCount] = {convertWavelengthToEnergy(200.0), convertWavelengthToEnergy(700.0)};
  G4double refractionIndices[energyCount] = {1.0, 1.0};
  G4MaterialPropertiesTable* galacticMpt = new G4MaterialPropertiesTable();
  galacticMpt->AddProperty("RINDEX",
                           energies,
                           refractionIndices,
                           energyCount);
  galactic->SetMaterialPropertiesTable(galacticMpt);
}

void NovaDetectorConstruction::defineMaterials()
{
  H = nistManager->FindOrBuildElement("H");
  C = nistManager->FindOrBuildElement("C");
  O = nistManager->FindOrBuildElement("O");
  Ti = nistManager->FindOrBuildElement("Ti");

  aluminum = nistManager->FindOrBuildMaterial("G4_Al");
  galactic = nistManager->FindOrBuildMaterial("G4_Galactic");
  glass = nistManager->FindOrBuildMaterial("G4_GLASS_PLATE");

  liquidScintillator = new G4Material("liquidScintillator", 1.032*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  liquidScintillator->AddElement(H, 0.666);
  liquidScintillator->AddElement(C, 0.334);

  fiberCore = new G4Material("fiberCore", 1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  fiberCore->AddElement(H, 0.498);
  fiberCore->AddElement(C, 0.502);

  pmma = new G4Material("pmma", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  pmma->AddElement(H, 0.532);
  pmma->AddElement(C, 0.336);
  pmma->AddElement(O, 0.132);

  fluorinatedPolymer = new G4Material("fluorinatedPolymer", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  fluorinatedPolymer->AddElement(H, 0.532);
  fluorinatedPolymer->AddElement(C, 0.336);
  fluorinatedPolymer->AddElement(O, 0.132);

  tiO2 = new G4Material("tiO2", 4.23*g/cm3, 2, kStateSolid);
  tiO2->AddElement(O, 2);
  tiO2->AddElement(Ti, 1);

  polystyrene = new G4Material("polystyrene", 1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
  polystyrene->AddElement(H, 0.498);
  polystyrene->AddElement(C, 0.502);

  pvc = new G4Material("pvc", 1.4316*g/cm3, 2, kStateSolid);
  pvc->AddMaterial(tiO2, 0.15);
  pvc->AddMaterial(polystyrene, 0.85);
}

void NovaDetectorConstruction::setMaterialProperties()
{
  setLiquidScintillatorProperty();
  setFiberCoreProperty();
  setPmmaProperty();
  setFluorinatedPolymerProperty();
  setGlassProperty();
  setGalacticProperty();
}

G4VPhysicalVolume* NovaDetectorConstruction::Construct()
{
  setMaterialProperties();
  return makeDetectorPhysicalVolume();
}

G4VPhysicalVolume* NovaDetectorConstruction::makeDetectorPhysicalVolume()
{
  if (simulationMode == "fiber")
    return makeSingleWlsFiberPhysicalVolume();
  else if (simulationMode == "cell")
    return makeNovaCellPhysicalVolume();
  else
    throw "The selected simulation mode does not exist.";
}

G4VPhysicalVolume* NovaDetectorConstruction::makeSingleWlsFiberPhysicalVolume()
{
  G4double fiberLength = detectorLength;
  G4double experimentalHallX = fiberRadius * 2.0;
  G4double experimentalHallY = fiberRadius * 2.0;
  G4double experimentalHallZ = fiberLength * 0.6;

  experimentalHallSolid = new G4Box("experimentalHallSolid", experimentalHallX, experimentalHallY, experimentalHallZ);
  experimentalHallLogicalVolume  = new G4LogicalVolume(experimentalHallSolid,
                                                       galactic,
                                                       "experimentalHallLogicalVolume",
                                                       0, 0, 0);
  experimentalHallPhysicalVolume = new G4PVPlacement(0,
                                                     G4ThreeVector(),
                                                     experimentalHallLogicalVolume,
                                                     "experimentalHall",
                                                     0, false, 0);

  G4double coreFraction = 1.0 - 2.0 * fiberCladdingFraction;
  G4double coreRadius = fiberRadius * coreFraction;
  G4double innerCladdingRadius = coreRadius + fiberRadius * fiberCladdingFraction;

  G4Tubs* outerCladdingSolid = new G4Tubs("outerCladdingSolid", 0, fiberRadius, fiberLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* outerCladdingLogicalVolume = new G4LogicalVolume(outerCladdingSolid,
                                                                    fluorinatedPolymer,
                                                                    "outerCladdingLogicalVolume");

  G4Tubs* innerCladdingSolid = new G4Tubs("innerCladdingSolid", 0, innerCladdingRadius, fiberLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* innerCladdingLogicalVolume = new G4LogicalVolume(innerCladdingSolid,
                                                                    pmma,
                                                                    "innerCladdingLogicalVolume");

  G4Tubs* coreSolid = new G4Tubs("coreSolid", 0, coreRadius, fiberLength / 2.0, 0.0, 360.0 * deg);
  G4LogicalVolume* coreLogicalVolume = new G4LogicalVolume(coreSolid,
                                                           fiberCore,
                                                           "coreLogicalVolume");

  new G4PVPlacement(0, G4ThreeVector(), coreLogicalVolume, "core", innerCladdingLogicalVolume, false, 0);
  new G4PVPlacement(0, G4ThreeVector(), innerCladdingLogicalVolume, "innerCladding", outerCladdingLogicalVolume, false, 0);
  new G4PVPlacement(0, G4ThreeVector(), outerCladdingLogicalVolume, "outerCladding", experimentalHallLogicalVolume, false, 0);

  G4LogicalVolume* pmtLogicalVolume = makePmtLogicalVolume();
  G4double pmtZ = -fiberLength / 2.0 - pmtThickness / 2.0;
  new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, pmtZ),
                    pmtLogicalVolume,
                    "pmt",
                    experimentalHallLogicalVolume,
                    false,
                    0);

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  NovaPmtSd* pmtSd = (NovaPmtSd*) sdManager->FindSensitiveDetector(PMT_SENSITIVE_DETECTOR_NAME, false);
  if (!pmtSd) {
    pmtSd = new NovaPmtSd(PMT_SENSITIVE_DETECTOR_NAME);
    pmtSd->initPmts(1);
    pmtSd->setPmtPosition(0, 0.0, 0.0, pmtZ);
    sdManager->AddNewDetector(pmtSd);
  }
  pmtLogicalVolume->SetSensitiveDetector(pmtSd);

  return experimentalHallPhysicalVolume;
}

G4VPhysicalVolume* NovaDetectorConstruction::makeNovaCellPhysicalVolume()
{
  G4double experimentalHallX = getCellHeight() / 2.0 + 20.0 * cm;
  G4double experimentalHallY = getCellHeight() / 2.0 + 20.0 * cm;
  G4double experimentalHallZ = detectorLength / 2.0 + snoutLength + 20.0 * cm;

  experimentalHallSolid = new G4Box("experimentalHallSolid", experimentalHallX, experimentalHallY, experimentalHallZ);
  experimentalHallLogicalVolume = new G4LogicalVolume(experimentalHallSolid,
                                                       galactic,
                                                       "experimentalHallLogicalVolume",
                                                       0, 0, 0);
  experimentalHallPhysicalVolume = new G4PVPlacement(0,
                                                     G4ThreeVector(),
                                                     experimentalHallLogicalVolume,
                                                     "experimentalHallPhysicalVolume",
                                                     0, false, 0);

  // fiber
  G4double fiberStraightLength = detectorLength - fiberCurveToEndPlateDistance + snoutLength;
  G4LogicalVolume* fiberLogicalVolume = makeWlsFiberLogicalVolume(fiberStraightLength);
  G4RotationMatrix* fiberRotationMatrix = new G4RotationMatrix();
  fiberRotationMatrix->rotateX(CLHEP::pi / 2.0 * rad);
  G4ThreeVector fiberThreeVector = G4ThreeVector(0, 0, -detectorLength / 2.0 + fiberCurveRadius + fiberCurveToEndPlateDistance);
  new G4PVPlacement(fiberRotationMatrix,
                    fiberThreeVector,
                    fiberLogicalVolume,
                    "fiber",
                    experimentalHallLogicalVolume,
                    false, 0);
  G4VisAttributes* fiberVisAttributes = new G4VisAttributes();
  fiberVisAttributes->SetVisibility(true);
  fiberVisAttributes->SetForceSolid(true);
  fiberLogicalVolume->SetVisAttributes(fiberVisAttributes);

  // liquid scintillator
  G4Box* liquidScintillatorSolid = new G4Box("liquidScintillatorSolid",
                                             getCellWidth() / 2.0,
                                             getCellHeight() / 2.0,
                                             detectorLength / 2.0);
  G4UnionSolid* fiberLoopSolid = makeFiberLoopSolid(fiberRadius, fiberCurveRadius, fiberStraightLength);
  G4SubtractionSolid* liquidScintillatorSubtractionSolid =
      new G4SubtractionSolid("liquidScintillatorSubtractionSolid",
                             liquidScintillatorSolid,
                             fiberLoopSolid,
                             fiberRotationMatrix,
                             fiberThreeVector);
  G4LogicalVolume* liquidScintillatorLogicalVolume =
      new G4LogicalVolume(liquidScintillatorSubtractionSolid,
                          liquidScintillator,
                          "liquidScintillatorLogicalVolume",
                          0, 0, 0);
  new G4PVPlacement(0,
                    G4ThreeVector(),
                    liquidScintillatorLogicalVolume,
                    "liquidScintillator",
                    experimentalHallLogicalVolume,
                    false, 0);
  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  G4VSensitiveDetector* liquidScintillatorSd = (NovaLiquidScintillatorSd*) sdManager->FindSensitiveDetector(LIQUID_SCINTILLATOR_SENSITIVE_DETECTOR_NAME, false);
  if (!liquidScintillatorSd) {
    liquidScintillatorSd = new NovaLiquidScintillatorSd(LIQUID_SCINTILLATOR_SENSITIVE_DETECTOR_NAME);
    sdManager->AddNewDetector(liquidScintillatorSd);
  }
  liquidScintillatorLogicalVolume->SetSensitiveDetector(liquidScintillatorSd);

  // pvc
  G4Box* pvcSolid = new G4Box("pvcSolid", getCellWidth() / 2.0, getCellHeight() / 2.0, detectorLength / 2.0);
  G4SubtractionSolid* pvcSubtractionSolid = new G4SubtractionSolid("endPlatePart1",
                                                                   pvcSolid,
                                                                   makeCellSolid(pvcThickness, detectorLength * 1.2));
  G4LogicalVolume* pvcLogicalVolume = new G4LogicalVolume(pvcSubtractionSolid,
                                                          pvc,
                                                          "pvcLogicalVolume",
                                                          0, 0, 0);
  setPvcSurfaceProperty(pvcLogicalVolume);
  new G4PVPlacement(0,
                    G4ThreeVector(),
                    pvcLogicalVolume,
                    "pvc",
                    liquidScintillatorLogicalVolume,
                    false, 0);

  // pmt
  G4LogicalVolume* pmtLogicalVolume = makePmtLogicalVolume();
  G4double pmtZ = fiberThreeVector.getZ() + fiberStraightLength + pmtThickness / 2.0;
  new G4PVPlacement(0,
                    G4ThreeVector(fiberCurveRadius, 0.0, pmtZ),
                    pmtLogicalVolume,
                    "pmt1",
                    experimentalHallLogicalVolume,
                    false,
                    0);
  new G4PVPlacement(0,
                    G4ThreeVector(-fiberCurveRadius, 0.0, pmtZ),
                    pmtLogicalVolume,
                    "pmt2",
                    experimentalHallLogicalVolume,
                    false,
                    0);

  NovaPmtSd* pmtSd = (NovaPmtSd*) sdManager->FindSensitiveDetector(PMT_SENSITIVE_DETECTOR_NAME, false);
  if (!pmtSd) {
    pmtSd = new NovaPmtSd(PMT_SENSITIVE_DETECTOR_NAME);
    pmtSd->initPmts(2);
    pmtSd->setPmtPosition(0, fiberCurveRadius, 0.0, pmtZ);
    pmtSd->setPmtPosition(1, -fiberCurveRadius, 0.0, pmtZ);
    sdManager->AddNewDetector(pmtSd);
  }
  pmtLogicalVolume->SetSensitiveDetector(pmtSd);

  // front end-plate
  G4Box* endPlateSolid = new G4Box("endPlateSolid", getCellWidth() / 2.0, getCellHeight() / 2.0, pvcThickness / 2.0);
  G4SubtractionSolid* endPlateSubtractionSolid =
      new G4SubtractionSolid("endPlateSubtractionSolid",
                             endPlateSolid,
                             fiberLoopSolid,
                             fiberRotationMatrix,
                             G4ThreeVector(0, 0, -fiberCurveRadius));
  G4LogicalVolume* frontEndPlateLogicalVolume =
      new G4LogicalVolume(endPlateSubtractionSolid, pvc, "frontEndPlateLogicalVolume", 0, 0, 0);
  setPvcSurfaceProperty(frontEndPlateLogicalVolume, true);
  new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, detectorLength / 2.0 + pvcThickness / 2.0),
                    frontEndPlateLogicalVolume,
                    "frontEndPlate",
                    experimentalHallLogicalVolume,
                    false, 0);

  // back end-plate
  G4LogicalVolume* backEndPlateLogicalVolume =
      new G4LogicalVolume(endPlateSolid, pvc, "backEndPlateLogicalVolume", 0, 0, 0);
  setPvcSurfaceProperty(backEndPlateLogicalVolume);
  new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, -detectorLength / 2.0 - pvcThickness / 2.0),
                    backEndPlateLogicalVolume,
                    "backEndPlate",
                    experimentalHallLogicalVolume,
                    false, 0);

  return experimentalHallPhysicalVolume;
}

void NovaDetectorConstruction::setPvcSurfaceProperty(G4LogicalVolume* pvcLogicalVolume, G4bool turnOffReflectivity)
{
  std::vector<G4double> reflectivityEnergies;
  std::vector<G4double> reflectivities;
  readCsvFile(PVC_REFLECTIVITY_FILENAME, reflectivityEnergies, reflectivities, 0.01);
  std::vector<G4double> zeroConstants(reflectivityEnergies.size(), 0.0);
  if (turnOffReflectivity) {
    reflectivities = zeroConstants;
  }

  G4MaterialPropertiesTable* pvcOpticalSurfaceMpt = new G4MaterialPropertiesTable();
  pvcOpticalSurfaceMpt->AddProperty("REFLECTIVITY", &reflectivityEnergies[0], &reflectivityEnergies[0], (G4int) reflectivityEnergies.size());
  pvcOpticalSurfaceMpt->AddProperty("SPECULARLOBECONSTANT", &reflectivityEnergies[0], &zeroConstants[0], (G4int) reflectivityEnergies.size());
  pvcOpticalSurfaceMpt->AddProperty("SPECULARSPIKECONSTANT", &reflectivityEnergies[0], &zeroConstants[0], (G4int) reflectivityEnergies.size());
  pvcOpticalSurfaceMpt->AddProperty("BACKSCATTERCONSTANT", &reflectivityEnergies[0], &zeroConstants[0], (G4int) reflectivityEnergies.size());

  G4OpticalSurface* pvcOpticalSurface = new G4OpticalSurface("pvcSurface");
  pvcOpticalSurface->SetModel(unified);
  pvcOpticalSurface->SetPolish(0);
  pvcOpticalSurface->SetFinish(groundfrontpainted);
  pvcOpticalSurface->SetType(dielectric_dielectric);
  pvcOpticalSurface->SetMaterialPropertiesTable(pvcOpticalSurfaceMpt);

  new G4LogicalSkinSurface("pvcLogicalSkinSurface", pvcLogicalVolume, pvcOpticalSurface);
}

G4UnionSolid* NovaDetectorConstruction::makeCellSolid(G4double deltaSize, G4double length)
{
  G4Box* pvcHorizontal = new G4Box("pvcHorizontal",
                                   getCellWidth() / 2.0 - deltaSize,
                                   straightHeight / 2.0,
                                   length / 2.0);

  G4Box* pvcVertical = new G4Box("pvcVertical",
                                 straightWidth / 2.0,
                                 getCellHeight() / 2.0 - deltaSize,
                                 length / 2.0);

  G4Tubs* pvcCorner = new G4Tubs("pvcCorner",
                                 0.0,
                                 getOuterCellCornerRadius() - deltaSize,
                                 length / 2.0,
                                 0.0,
                                 CLHEP::twopi * rad);

  G4UnionSolid* pvcPart1 = new G4UnionSolid("pvcPart1",
                                            pvcHorizontal,
                                            pvcVertical,
                                            0,
                                            G4ThreeVector());

  G4UnionSolid* pvcPart2 = new G4UnionSolid("pvcPart2",
                                            pvcPart1,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(straightWidth / 2.0, straightHeight / 2.0, 0.0));

  G4UnionSolid* pvcPart3 = new G4UnionSolid("pvcPart3",
                                            pvcPart2,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(-straightWidth / 2.0, straightHeight / 2.0, 0.0));

  G4UnionSolid* pvcPart4 = new G4UnionSolid("pvcPart4",
                                            pvcPart3,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(-straightWidth / 2.0, -straightHeight / 2.0, 0.0));

  G4UnionSolid* pvcPart5 = new G4UnionSolid("pvcPart5",
                                            pvcPart4,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(straightWidth / 2.0, -straightHeight / 2.0, 0.0));

  return pvcPart5;
}

G4UnionSolid* NovaDetectorConstruction::makePvcCellSolid()
{
  G4double outerCornerRadius = getOuterCellCornerRadius();

  G4Box* pvcHorizontal = new G4Box("pvcHorizontal",
                                   straightWidth / 2.0,
                                   pvcThickness / 2.0,
                                   detectorLength / 2.0);

  G4Box* pvcVertical = new G4Box("pvcVertical",
                                 straightHeight / 2.0,
                                 pvcThickness / 2.0,
                                 detectorLength / 2.0);

  G4Tubs* pvcCorner = new G4Tubs("pvcCorner",
                                 innerCellCornerRadius,
                                 outerCornerRadius,
                                 detectorLength / 2.0,
                                 0.0,
                                 0.5 * CLHEP::pi * rad);

  G4RotationMatrix* rotate90 = new G4RotationMatrix();
  rotate90->rotateZ(90 * deg);
  G4RotationMatrix* rotate180 = new G4RotationMatrix();
  rotate180->rotateZ(180 * deg);
  G4RotationMatrix* rotate270 = new G4RotationMatrix();
  rotate270->rotateZ(270 * deg);

  G4UnionSolid* pvcPart1 = new G4UnionSolid("pvcPart1",
                                            pvcHorizontal,
                                            pvcCorner,
                                            rotate90,
                                            G4ThreeVector(straightWidth / 2.0,
                                                          innerCellCornerRadius + pvcThickness / 2.0));

  G4UnionSolid* pvcPart2 = new G4UnionSolid("pvcPart2",
                                            pvcPart1,
                                            pvcVertical,
                                            rotate90,
                                            G4ThreeVector(straightWidth / 2.0 + innerCellCornerRadius + pvcThickness / 2.0,
                                                          innerCellCornerRadius + pvcThickness / 2.0 + straightHeight / 2.0));

  G4UnionSolid* pvcPart3 = new G4UnionSolid("pvcPart3",
                                            pvcPart2,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(straightWidth / 2.0,
                                                          innerCellCornerRadius + pvcThickness / 2.0 + straightHeight));

  G4UnionSolid* pvcPart4 = new G4UnionSolid("pvcPart4",
                                            pvcPart3,
                                            pvcHorizontal,
                                            0,
                                            G4ThreeVector(0,
                                                          2.0 * innerCellCornerRadius + straightHeight + pvcThickness));

  G4UnionSolid* pvcPart5 = new G4UnionSolid("pvcPart5",
                                            pvcPart4,
                                            pvcCorner,
                                            rotate270,
                                            G4ThreeVector(-straightWidth / 2.0,
                                                          innerCellCornerRadius + pvcThickness / 2.0 + straightHeight));

  G4UnionSolid* pvcPart6 = new G4UnionSolid("pvcPart6",
                                            pvcPart5,
                                            pvcVertical,
                                            rotate90,
                                            G4ThreeVector(-(straightWidth / 2.0 + innerCellCornerRadius + pvcThickness / 2.0),
                                                          innerCellCornerRadius + pvcThickness / 2.0 + straightHeight / 2.0));

  G4UnionSolid* pvcPart7 = new G4UnionSolid("pvcPart7",
                                            pvcPart6,
                                            pvcCorner,
                                            rotate180,
                                            G4ThreeVector(-straightWidth / 2.0,
                                                          innerCellCornerRadius + pvcThickness / 2.0));

  return pvcPart7;
}

G4UnionSolid* NovaDetectorConstruction::makeLiquidScintillatorSolid()
{
  G4double fullHeight = straightHeight + 2. * innerCellCornerRadius;

  G4Box* box1 = new G4Box("box1",
                          straightWidth / 2.,
                          fullHeight / 2.,
                          detectorLength / 2.);

  G4Box* box2 = new G4Box("box2",
                          innerCellCornerRadius / 2.,
                          straightHeight / 2.,
                          detectorLength / 2.);

  G4Tubs* innerCorner = new G4Tubs("innerCorner",
                                   0.0 * mm,
                                   innerCellCornerRadius,
                                   detectorLength / 2.0,
                                   0.0,
                                   CLHEP::pi / 2.0 * rad);

  G4UnionSolid* unionBox1 = new G4UnionSolid("unionBox1",
                                             box1,
                                             box2,
                                             0,
                                             G4ThreeVector(straightWidth / 2. + innerCellCornerRadius / 2., 0, 0));

  G4UnionSolid* unionBox2 = new G4UnionSolid("unionBox2",
                                             unionBox1,
                                             box2,
                                             0,
                                             G4ThreeVector(-straightWidth / 2. - innerCellCornerRadius / 2., 0, 0));

  G4UnionSolid* unionCorner1 = new G4UnionSolid("unionCorner1",
                                                unionBox2,
                                                innerCorner,
                                                0,
                                                G4ThreeVector(straightWidth / 2.,
                                                              straightHeight / 2.,
                                                              0));

  G4RotationMatrix* rotate270 = new G4RotationMatrix();
  rotate270->rotateZ(270 * deg);
  G4UnionSolid* unionCorner2 = new G4UnionSolid("unionCorner2",
                                                unionCorner1,
                                                innerCorner,
                                                rotate270,
                                                G4ThreeVector(-straightWidth / 2.,
                                                              straightHeight / 2.,
                                                              0));

  G4RotationMatrix* rotate90 = new G4RotationMatrix();
  rotate90->rotateZ(90 * deg);
  G4UnionSolid* unionCorner3 = new G4UnionSolid("unionCorner3",
                                                unionCorner2,
                                                innerCorner,
                                                rotate90,
                                                G4ThreeVector(straightWidth / 2.,
                                                              -straightHeight / 2.,
                                                              0));

  G4RotationMatrix* rotate180 = new G4RotationMatrix();
  rotate180->rotateZ(180 * deg);
  G4UnionSolid* unionCorner4 = new G4UnionSolid("unionCorner4",
                                                unionCorner3,
                                                innerCorner,
                                                rotate180,
                                                G4ThreeVector(-straightWidth / 2.,
                                                              -straightHeight / 2.,
                                                              0));

  return unionCorner4;
}

G4UnionSolid* NovaDetectorConstruction::makeFiberLoopSolid(G4double radius, G4double curveRadius, G4double straightLength)
{
  G4Tubs* straightSolid = new G4Tubs("straightSolid", 0, radius, straightLength / 2.0, 0.0, CLHEP::twopi * rad);
  G4Torus* curveSolid = new G4Torus("curveSolid", 0.0, radius, curveRadius, 0.0, CLHEP::pi * rad);

  G4RotationMatrix* rotateX = new G4RotationMatrix();
  rotateX->rotateX(CLHEP::pi / 2.0 * rad);
  G4UnionSolid* fiberPart1 = new G4UnionSolid("fiberPart1",
                                              curveSolid,
                                              straightSolid,
                                              rotateX,
                                              G4ThreeVector(-curveRadius, -straightLength / 2.0, 0.0));

  G4UnionSolid* fiberPart2 = new G4UnionSolid("fiberPart2",
                                              fiberPart1,
                                              straightSolid,
                                              rotateX,
                                              G4ThreeVector(curveRadius, -straightLength / 2.0, 0.0));
  return fiberPart2;
}

G4LogicalVolume* NovaDetectorConstruction::makeWlsFiberLogicalVolume(G4double straightLength)
{
  G4double coreRadius = fiberRadius * (1.0 - 2.0 * fiberCladdingFraction);
  G4double innerCladdingRadius = fiberRadius * (1.0 - fiberCladdingFraction);

  G4UnionSolid* outerCladdingSolid = makeFiberLoopSolid(fiberRadius, fiberCurveRadius, straightLength);
  G4LogicalVolume* outerCladdingLogicalVolume = new G4LogicalVolume(outerCladdingSolid,
                                                                    fluorinatedPolymer,
                                                                    "outerCladdingLogicalVolume");

  G4UnionSolid* innerCladdingSolid = makeFiberLoopSolid(innerCladdingRadius, fiberCurveRadius, straightLength);
  G4LogicalVolume* innerCladdingLogicalVolume = new G4LogicalVolume(innerCladdingSolid,
                                                                    pmma,
                                                                    "innerCladdingLogicalVolume");

  G4UnionSolid* coreSolid = makeFiberLoopSolid(coreRadius, fiberCurveRadius, straightLength);
  G4LogicalVolume* coreLogicalVolume = new G4LogicalVolume(coreSolid,
                                                           fiberCore,
                                                           "coreLogicalVolume");

  new G4PVPlacement(0, G4ThreeVector(), coreLogicalVolume, "core", innerCladdingLogicalVolume, false, 0);
  new G4PVPlacement(0, G4ThreeVector(), innerCladdingLogicalVolume, "innerCladding", outerCladdingLogicalVolume, false, 0);

  return outerCladdingLogicalVolume;
}

G4LogicalVolume* NovaDetectorConstruction::makePmtLogicalVolume()
{
  G4Tubs* pmtSolid = new G4Tubs("pmtSolid",
                                0.0,
                                fiberRadius,
                                pmtThickness / 2.0,
                                0.0,
                                CLHEP::twopi);
  G4LogicalVolume* pmtLogicalVolume = new G4LogicalVolume(pmtSolid,
                                                          glass,
                                                          "pmtLogicalVolume");

  G4Tubs* photocathodeSolid = new G4Tubs("photocathodeSolid",
                                         0.0,
                                         fiberRadius,
                                         pmtThickness / 4.0,
                                         0.0,
                                         CLHEP::twopi);
  G4LogicalVolume* photocathodeLogicalVolume = new G4LogicalVolume(photocathodeSolid,
                                                                   aluminum,
                                                                   "photocathodeLogicalVolume");

  G4String quantumEfficiencyFilename;
  if (photodetectorType == "apd") {
    quantumEfficiencyFilename = APD_QUANTUM_EFFICIENCY_FILENAME;
  }
  else if (photodetectorType == "pmt") {
    quantumEfficiencyFilename = PMT_QUANTUM_EFFICIENCY_FILENAME;
  }
  else
    throw "The selected photodetector type does not exist.";

  std::vector<G4double> energies;
  std::vector<G4double> quantumEfficiencies;
  readCsvFile(quantumEfficiencyFilename, energies, quantumEfficiencies, 1.0);

  std::vector<G4double> refractionIndices(energies.size(), 1.49);
  std::vector<G4double> reflectivities(energies.size(), 0.0);

  G4OpticalSurface* opticalSurface = new G4OpticalSurface("opticalSurface");
  G4MaterialPropertiesTable* opticalSurfaceMpt = new G4MaterialPropertiesTable();
  opticalSurfaceMpt->AddProperty("EFFICIENCY", &energies[0], &quantumEfficiencies[0], (G4int) energies.size());
  opticalSurfaceMpt->AddProperty("REFLECTIVITY", &energies[0], &reflectivities[0], (G4int) energies.size());
  opticalSurfaceMpt->AddProperty("RINDEX", &energies[0], &refractionIndices[0], (G4int) energies.size());
  opticalSurface->SetModel(glisur);
  opticalSurface->SetFinish(polished);
  opticalSurface->SetType(dielectric_metal);
  opticalSurface->SetMaterialPropertiesTable(opticalSurfaceMpt);
  new G4LogicalSkinSurface("pmtLogicalSkinSurface", photocathodeLogicalVolume, opticalSurface);

  new G4PVPlacement(0,
                    G4ThreeVector(0.0, 0.0, pmtThickness / 4.0),
                    photocathodeLogicalVolume,
                    "photocathode",
                    pmtLogicalVolume,
                    false,
                    0);

  return pmtLogicalVolume;
}

void NovaDetectorConstruction::printSetting()
{
  G4cout << std::setw(50) << "liquidScintillatorLightYield = " << std::setw(10) << liquidScintillatorMpt->GetConstProperty("SCINTILLATIONYIELD") << std::setw(10) << " MeV-1" << G4endl;
  G4cout << std::setw(50) << "detectorLength = " << std::setw(10) << detectorLength / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(50) << "snoutLength = " << std::setw(10) << snoutLength / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(50) << "straightWidth = " << std::setw(10) << straightWidth / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(50) << "straightHeight = " << std::setw(10) << straightHeight / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(50) << "innerCellCornerRadius = " << std::setw(10) << innerCellCornerRadius / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(50) << "pvcThickness = " << std::setw(10) << pvcThickness / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(50) << "fiberRadius = " << std::setw(10) << fiberRadius / mm << std::setw(10) << " mm" << G4endl;
  G4cout << std::setw(50) << "fiberCurveRadius = " << std::setw(10) << fiberCurveRadius / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(50) << "fiberCurveToEndPlateDistance = " << std::setw(10) << fiberCurveToEndPlateDistance / cm << std::setw(10) << " cm" << G4endl;
  G4cout << std::setw(50) << "photodetectorType = " << std::setw(10) << photodetectorType << G4endl;
  G4cout << std::setw(50) << "simulationMode = " << std::setw(10) << simulationMode << G4endl;
  G4cout << G4endl;
}

void NovaDetectorConstruction::setDefaults()
{
  liquidScintillatorLightYield = 1.0;
  straightHeight = 40.0 * mm;
  straightWidth = 17.7 * mm;
  innerCellCornerRadius = 9.7 * mm;
  pvcThickness = 3.3 * mm;
  fiberRadius = 0.35 * mm;
  fiberCladdingFraction = 0.03;
  fiberCurveRadius = 10.0 * mm;
  detectorLength = 120.0 * cm;
  snoutLength = 635.0 * mm;
  fiberCurveToEndPlateDistance = 2.0 * cm;
  pmtThickness = 1.0 * mm;
  photodetectorType = "apd";
  simulationMode = "cell";
  isUpdated = true;
}

void NovaDetectorConstruction::updateDetector()
{
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalSkinSurface::CleanSurfaceTable();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4SurfaceProperty::CleanSurfacePropertyTable();

  setMaterialProperties();
  G4RunManager::GetRunManager()->DefineWorldVolume(makeDetectorPhysicalVolume());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  isUpdated = false;
}

void NovaDetectorConstruction::readCsvFile(G4String filename,
                                           std::vector<G4double>& energies,
                                           std::vector<G4double>& values,
                                           double valueUnit)
{
  G4String wavelengthString;
  G4String valueString;
  std::ifstream fCsv(getFilePath(filename));
  if(fCsv.is_open()){
    while (std::getline(fCsv, wavelengthString, ',')) {
      std::getline(fCsv, valueString);
      energies.push_back(convertWavelengthToEnergy(std::stof(wavelengthString)));
      values.push_back(std::stof(valueString) * valueUnit);
    }
  }
}
