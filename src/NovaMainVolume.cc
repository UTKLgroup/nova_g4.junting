#include <G4SIunits.hh>
#include <G4NistManager.hh>
#include "NovaMainVolume.hh"
#include "G4SDManager.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "LXePMTSD.hh"
#include "LXeScintSD.hh"

LXeScintSD* NovaMainVolume::scintSd=NULL;
LXePMTSD* NovaMainVolume::pmtSd=NULL;
G4LogicalVolume* NovaMainVolume::housingLog = NULL;

NovaMainVolume::NovaMainVolume(G4RotationMatrix* pRot, const G4ThreeVector &tlate, G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo, NovaDetectorConstruction* c)
    : G4PVPlacement(pRot,tlate, new G4LogicalVolume(new G4Box("temp",1,1,1), G4Material::GetMaterial("Vacuum"), "temp", 0, 0, 0), "housing",pMotherLogical,pMany,pCopyNo),fConstructor(c)
{
  CopyValues();

  if(!housingLog || isUpdated){

    housingSolid = new G4Box("housingSolid",
                           cellWidth / 2.0 + 10.0*cm,
                           cellHeight / 2.0 + 10.0*cm,
                           cellToPmtDistance + 10.0*cm);
    housingLog = new G4LogicalVolume(housingSolid, G4Material::GetMaterial("G4_Galactic"), "housingLog", 0, 0, 0);

    pvcSolid = makePvc(innerCornerRadius, outerCornerRadius);
    pvcLog = new G4LogicalVolume(pvcSolid, G4Material::GetMaterial("pvc"), "pvcLog", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(0, -cellHeight / 2.0 + pvcThickness / 2.0), pvcLog, "pvc", housingLog, false, 0);

    G4UnionSolid* scintillatorSolid = makeCell(innerCornerRadius);
    G4Tubs* fiberSolid = new G4Tubs("fiberSolid", 0, fiberRadius, cellLength/2, 0, twopi*rad);
    G4SubtractionSolid* scintillatorSubtractOneFiber = new G4SubtractionSolid("scintillatorSubtractOneFiber",
                                                                              scintillatorSolid,
                                                                              fiberSolid,
                                                                              0,
                                                                              G4ThreeVector(fiber1X, fiber1Y));

    scintillator = new G4SubtractionSolid("scintillator",
                                          scintillatorSubtractOneFiber,
                                          fiberSolid,
                                          0,
                                          G4ThreeVector(fiber2X, fiber2Y));

    scintillatorLog = new G4LogicalVolume(scintillator, G4Material::GetMaterial("liquidScintillator"), "scintillatorLog", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(), scintillatorLog, "scintillator", housingLog, false, 0);

    wlsFiber = new NovaWlsFiber();
    fiberLog = wlsFiber->BuildStraight(fiberRadius, fullFiberLength);
    new G4PVPlacement(0, G4ThreeVector(fiber1X, fiber1Y, fiberZ), fiberLog, "outerCladding", housingLog, false, 0);
    new G4PVPlacement(0, G4ThreeVector(fiber2X, fiber2Y, fiberZ), fiberLog, "outerCladding", housingLog, false, 1);

    G4double pmtInnerRadius = 0.0*cm;
    G4double pmtStartAngle = 0.0*deg;
    G4double pmtSpanningAngle = 360.0*deg;

    pmt = new G4Tubs("pmt",
                     pmtInnerRadius,
                     fiberRadius,
                     pmtHeight,
                     pmtStartAngle,
                     pmtSpanningAngle);

    photoCathode = new G4Tubs("photoCathode",
                              pmtInnerRadius,
                              fiberRadius,
                              pmtHeight / 2.0,
                              pmtStartAngle,
                              pmtSpanningAngle);

    pmtLog = new G4LogicalVolume(pmt,
                                 G4Material::GetMaterial("G4_GLASS_PLATE"),
                                 "pmt_log");

    photocathodeLog = new G4LogicalVolume(photoCathode,
                                          G4Material::GetMaterial("G4_Al"),
                                          "photoCathodeLog");

    new G4PVPlacement(0,
                      G4ThreeVector(0.0, 0.0, -pmtHeight / 2.0),
                      photocathodeLog,
                      "photoCathode",
                      pmtLog,
                      false,
                      0);

    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    if(!pmtSd){
      pmtSd = new LXePMTSD("/LXeDet/pmtSD");
      sdManager->AddNewDetector(pmtSd);
    }

    pmtSd->InitPMTs(2);
    new G4PVPlacement(0, G4ThreeVector(fiber1X, fiber1Y, pmtZ), pmtLog, "pmt", housingLog, false, 0);
    new G4PVPlacement(0, G4ThreeVector(fiber2X, fiber2Y, pmtZ), pmtLog, "pmt", housingLog, false, 1);
    pmtSd->SetPMTPos(0, fiber1X, fiber1Y, pmtZ);
    pmtSd->SetPMTPos(1, fiber2X, fiber2Y, pmtZ);

    if(!scintSd){
      scintSd = new LXeScintSD("/LXeDet/scintSD");
      sdManager->AddNewDetector(scintSd);
    }

    scintillatorLog->SetSensitiveDetector(scintSd);
    photocathodeLog->SetSensitiveDetector(pmtSd);

    VisAttributes();
    SurfaceProperties();
  }

  SetLogicalVolume(housingLog);
}


void NovaMainVolume::CopyValues(){
  isUpdated= fConstructor->getIsUpdated();

  rectangleWidth = fConstructor->getRectangleWidth();
  rectangleHeight = fConstructor->getRectangleHeight();
  cellLength = fConstructor->getCellLength();
  pvcThickness = fConstructor->getPvcThickness();
  innerCornerRadius = fConstructor->getInnerCornerRadius();
  fiberRadius = fConstructor->getFiberRadius();
  fiber1X = fConstructor->getFiber1X();
  fiber1Y = fConstructor->getFiber1Y();
  fiber2X = fConstructor->getFiber2X();
  fiber2Y = fConstructor->getFiber2Y();
  cellToPmtDistance = fConstructor->getCellToPmtDistance();
  fiberTailLength = fConstructor->getFiberTailLength();

  pmtHeight = fConstructor->getPmtHeight();
  pmtZ = fConstructor->getPmtZ();
  fullFiberLength = fConstructor->getFullFiberLength();
  fiberZ = fConstructor->getFiberZ();
  cellWidth = fConstructor->getCellWidth();
  cellHeight = fConstructor->getCellHeight();
  outerCornerRadius = fConstructor->getOuterCornerRadius();

  usePMT = fConstructor->getUsePMT();
}

void NovaMainVolume::VisAttributes(){

  G4VisAttributes* clear = new G4VisAttributes( G4Colour(255/255. ,255/255. ,255/255. ));
  clear->SetForceWireframe(true);
  G4VisAttributes* white = new G4VisAttributes(G4Color(1,1,1));
  G4VisAttributes* green = new G4VisAttributes(G4Colour(0,1,0));
  green->SetForceWireframe(true);
  G4VisAttributes* blue = new G4VisAttributes( G4Colour(0/255. ,0/255. ,255/255. ));

  housingLog->SetVisAttributes(G4VisAttributes::Invisible);
  pvcLog->SetVisAttributes(white);
  scintillatorLog->SetVisAttributes(blue);
  fiberLog->SetVisAttributes(green);
  photocathodeLog->SetVisAttributes(blue);
}

void NovaMainVolume::SurfaceProperties(){
  G4int pmtEntries = 0;
  G4double pmtEnergies[500];
  G4double pmtRefractionIndices[500];
  G4double pmtReflectivities[500];
  G4double pmtEfficiencies[500];
  G4double refractionIndex = 1.49;
  G4double reflectivity = 0.0;

  std::ifstream fQuantumEfficiency;
  if(usePMT)
    fQuantumEfficiency.open("input/qe_file_2.dat");
  else
    fQuantumEfficiency.open("input/apd_qe_1.dat");

  while (!fQuantumEfficiency.eof()) {
    G4String filler;
    G4double wavelength;
    fQuantumEfficiency >> wavelength >> filler >> pmtEfficiencies[pmtEntries];
    pmtEfficiencies[pmtEntries]  = pmtEfficiencies[pmtEntries];
    pmtEnergies[pmtEntries]  = (1240. / wavelength) * eV;
    pmtRefractionIndices[pmtEntries]   = refractionIndex;
    pmtReflectivities[pmtEntries] = reflectivity;
    pmtEntries++;
  }
  fQuantumEfficiency.close();

  opt_phodet = new G4OpticalSurface("opt_phodet");
  phodetMPT = new G4MaterialPropertiesTable();
  phodetMPT->AddProperty("EFFICIENCY",   pmtEnergies, pmtEfficiencies,  pmtEntries);
  phodetMPT->AddProperty("REFLECTIVITY", pmtEnergies, pmtReflectivities, pmtEntries);
  phodetMPT->AddProperty("RINDEX",       pmtEnergies, pmtRefractionIndices,   pmtEntries);
  opt_phodet->SetModel(glisur);
  opt_phodet->SetFinish(polished);
  opt_phodet->SetType(dielectric_metal);
  opt_phodet->SetMaterialPropertiesTable(phodetMPT);
  new G4LogicalSkinSurface("skin_pmt", photocathodeLog, opt_phodet);
  phodetMPT->DumpTable();

  G4String filler;
  G4int Ti02_entries = 0;
  G4double Ti02_energy[500];
  G4double Ti02_reflect[500];
  G4double zero[500];
  std::ifstream Read_Ti02;
  G4String Ti02_file = "input/avg_ext.txt";
  Read_Ti02.open(Ti02_file);
  if (Read_Ti02.is_open()){
    while(!Read_Ti02.eof()){
      G4double wavelength;
      G4double Ti02_ref_coeff;
      Read_Ti02 >> wavelength >> filler >> Ti02_ref_coeff;
      Ti02_energy[Ti02_entries] = (1240/wavelength)*eV;
      Ti02_reflect[Ti02_entries] = Ti02_ref_coeff/100.;
      zero[Ti02_entries] = 1e-6;
      Ti02_entries++;
    }
  }
  else
    G4cout << "Error opening file: " << Ti02_file << G4endl;
  Read_Ti02.close();

  op_paint = new G4OpticalSurface("op_paint");
  op_paint->SetModel(unified);
  op_paint->SetPolish(0);
  op_paint->SetFinish(groundfrontpainted);
  op_paint->SetType(dielectric_metal);
  paintMPT = new G4MaterialPropertiesTable();
  paintMPT->AddProperty("REFLECTIVITY",          Ti02_energy, Ti02_reflect, Ti02_entries);
  paintMPT->AddProperty("SPECULARLOBECONSTANT",  Ti02_energy, zero,         Ti02_entries);
  paintMPT->AddProperty("SPECULARSPIKECONSTANT", Ti02_energy, zero,         Ti02_entries);
  paintMPT->AddProperty("BACKSCATTERCONSTANT",   Ti02_energy, zero,         Ti02_entries);
  op_paint->SetMaterialPropertiesTable(paintMPT);
  new G4LogicalSkinSurface("paint_b", pvcLog, op_paint);
}

G4UnionSolid* NovaMainVolume::makePvc(G4double innerRadius, G4double outerRadius){
  G4Box* pvcHorizontal = new G4Box("pvcHorizontal",
                                   rectangleWidth / 2.0,
                                   pvcThickness / 2.0,
                                   cellLength / 2.0);

  G4Box* pvcVertical = new G4Box("pvcVertical",
                                 rectangleHeight / 2.0,
                                 pvcThickness / 2.0,
                                 cellLength / 2.0);

  G4Tubs* pvcCorner = new G4Tubs("pvcCorner",
                                 innerRadius,
                                 outerRadius,
                                 cellLength / 2.0,
                                 0.0,
                                 0.5 * pi * rad);

  G4RotationMatrix Rotate90, Rotate180, Rotate270;
  Rotate90.rotateZ(90 * deg);
  Rotate180.rotateZ(180 * deg);
  Rotate270.rotateZ(270 * deg);

  G4UnionSolid* pvcPart1 = new G4UnionSolid("pvcPart1",
                                            pvcHorizontal,
                                            pvcCorner,
                                            &Rotate90,
                                            G4ThreeVector(rectangleWidth / 2., innerRadius + pvcThickness / 2.));

  G4UnionSolid* pvcPart2 = new G4UnionSolid("pvcPart2",
                                            pvcPart1,
                                            pvcVertical,
                                            &Rotate90,
                                            G4ThreeVector(rectangleWidth / 2. + innerRadius + pvcThickness / 2., innerRadius + pvcThickness / 2. + rectangleHeight / 2.));

  G4UnionSolid* pvcPart3 = new G4UnionSolid("pvcPart3",
                                            pvcPart2,
                                            pvcCorner,
                                            0,
                                            G4ThreeVector(rectangleWidth / 2., innerRadius + pvcThickness / 2. + rectangleHeight));

  G4UnionSolid* pvcPart4 = new G4UnionSolid("pvcPart4",
                                            pvcPart3,
                                            pvcHorizontal,
                                            0,
                                            G4ThreeVector(0, 2. * innerRadius + rectangleHeight + pvcThickness));

  G4UnionSolid* pvcPart5 = new G4UnionSolid("pvcPart5",
                                            pvcPart4,
                                            pvcCorner,
                                            &Rotate270,
                                            G4ThreeVector(-rectangleWidth / 2., innerRadius + pvcThickness / 2. + rectangleHeight));

  G4UnionSolid* pvcPart6 = new G4UnionSolid("pvcPart6",
                                            pvcPart5,
                                            pvcVertical,
                                            &Rotate90,
                                            G4ThreeVector(-(rectangleWidth / 2. + innerRadius + pvcThickness / 2.), innerRadius + pvcThickness / 2. + rectangleHeight / 2.));

  G4UnionSolid* pvcPart7 = new G4UnionSolid("pvcPart7",
                                            pvcPart6,
                                            pvcCorner,
                                            &Rotate180,
                                            G4ThreeVector(-rectangleWidth / 2., innerRadius + pvcThickness / 2.));

  return pvcPart7;
}

G4UnionSolid* NovaMainVolume::makeCell(G4double radius){
  G4double fullHeight = rectangleHeight + 2. * radius;
  G4double fullWidth = rectangleWidth + 2. * radius;

  G4Box* box1 = new G4Box("box1",
                          rectangleWidth / 2.,
                          fullHeight / 2.,
                          cellLength / 2.);

  G4Box* box2 = new G4Box("box2",
                          fullWidth / 2.,
                          rectangleHeight / 2.,
                          cellLength / 2.);

  G4UnionSolid* unionBoxes = new G4UnionSolid("unionBoxes",
                                              box1,
                                              box2,
                                              0,
                                              G4ThreeVector());

  G4Tubs* innerCorner = new G4Tubs("innerCorner", 0.0*mm, radius, cellLength / 2.0, 0.0, twopi*rad);

  G4UnionSolid* unionCorner1 = new G4UnionSolid("unionCorner1",
                                                unionBoxes,
                                                innerCorner,
                                                0,
                                                G4ThreeVector(rectangleWidth / 2.,
                                                              rectangleHeight / 2.,
                                                              0));

  G4UnionSolid* unionCorner2 = new G4UnionSolid("unionCorner2",
                                                unionCorner1,
                                                innerCorner,
                                                0,
                                                G4ThreeVector(-rectangleWidth / 2.,
                                                              rectangleHeight / 2.,
                                                              0));

  G4UnionSolid* unionCorner3 = new G4UnionSolid("unionCorner3",
                                                unionCorner2,
                                                innerCorner,
                                                0,
                                                G4ThreeVector(rectangleWidth / 2.,
                                                              -rectangleHeight / 2.,
                                                              0));

  G4UnionSolid* unionCorner4 = new G4UnionSolid("unionCorner4",
                                                unionCorner3,
                                                innerCorner,
                                                0,
                                                G4ThreeVector(-rectangleWidth / 2.,
                                                              -rectangleHeight / 2.,
                                                              0));

  return unionCorner4;
}
