#include <G4SIunits.hh>
#include "NovaMainVolume.hh"
#include "G4SDManager.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "LXePMTSD.hh"
#include "LXeScintSD.hh"

LXeScintSD* NovaMainVolume::fScint_SD=NULL;
LXePMTSD* NovaMainVolume::fPmt_SD=NULL;
G4LogicalVolume* NovaMainVolume::fHousing_log = NULL;

NovaMainVolume::NovaMainVolume(G4RotationMatrix* pRot, const G4ThreeVector &tlate, G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo, NovaDetectorConstruction* c)
    : G4PVPlacement(pRot,tlate, new G4LogicalVolume(new G4Box("temp",1,1,1), G4Material::GetMaterial("Vacuum"), "temp", 0, 0, 0), "housing",pMotherLogical,pMany,pCopyNo),fConstructor(c)
{
  CopyValues();

  if(!fHousing_log || fUpdated){

    fHousing_box = new G4Box("holder_s",
                             fCellWidth / 2.  + 10.*cm,
                             fCellHeight / 2. + 10.*cm,
                             fCelltoPMTL      + 10.*cm);
    fHousing_log = new G4LogicalVolume(fHousing_box, G4Material::GetMaterial("Vacuum"), "fHousing_log", 0, 0, 0);

    PVC_s = NovaMainVolume::makePvc(fInner_Corner_Radius, fOuter_Corner_Radius);
    PVC_l = new G4LogicalVolume(PVC_s, G4Material::GetMaterial("PVC"), "PVC_l", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(0, -fCellHeight / 2. + pvcThickness / 2.), PVC_l, "PVC", fHousing_log, false, 0);

    //--------- scintillator

    G4UnionSolid* scnt_full_s = NovaMainVolume::MakeCell(fInner_Corner_Radius);
    G4Tubs* fiber_s = new G4Tubs("fiber_s", 0, fFibRadius, cellLength/2, 0, twopi*rad);
    G4SubtractionSolid* scnt_fib = new G4SubtractionSolid("scnt_fib",
                                                          scnt_full_s,
                                                          fiber_s,
                                                          0,
                                                          G4ThreeVector(fFib1X, fFib1Y)
    );

    scnt_s = new G4SubtractionSolid("scnt_s",
                                    scnt_fib,
                                    fiber_s,
                                    0,
                                    G4ThreeVector(fFib2X, fFib2Y)
    );

    scnt_l = new G4LogicalVolume(scnt_s, G4Material::GetMaterial("liquidscintillator"), "scnt_l", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(), scnt_l, "scnt", fHousing_log, false, 0);

    //--------- fiber

    FibMaker = new WLSfiber();
    fiber_l = FibMaker->BuildStraight(fFibRadius, fFibFullLength);
    new G4PVPlacement(0, G4ThreeVector(fFib1X, fFib1Y, fFibZ), fiber_l, "outer_clading", fHousing_log, false, 0);
    new G4PVPlacement(0, G4ThreeVector(fFib2X, fFib2Y, fFibZ), fiber_l, "outer_clading", fHousing_log, false, 1);

    //****************** Build PMTs

    G4double innerRadius_pmt   = 0.*cm;
    G4double height_pmt        = fPMTHeight;
    G4double startAngle_pmt    = 0.*deg;
    G4double spanningAngle_pmt = 360.*deg;

    fPmt = new G4Tubs("pmt_tube",innerRadius_pmt,fFibRadius,
                      height_pmt,startAngle_pmt,spanningAngle_pmt);

    fPhotocath = new G4Tubs("photocath_tube",innerRadius_pmt,fFibRadius,
                            height_pmt / 2.,startAngle_pmt,spanningAngle_pmt);

    fPmt_log = new G4LogicalVolume(fPmt,G4Material::GetMaterial("Glass"),
                                   "pmt_log");
    fPhotocath_log = new G4LogicalVolume(fPhotocath,
                                         G4Material::GetMaterial("Al"),
                                         "photocath_log");

    new G4PVPlacement(0,G4ThreeVector(0, 0, -height_pmt/2),
                      fPhotocath_log, "photocath",
                      fPmt_log, false, 0);

    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    if(!fPmt_SD){
      fPmt_SD = new LXePMTSD("/LXeDet/pmtSD");
      SDman->AddNewDetector(fPmt_SD);
    }

    fPmt_SD->InitPMTs(2);
    new G4PVPlacement(0, G4ThreeVector(fFib1X, fFib1Y, fPMTZ), fPmt_log, "pmt", fHousing_log, false, 0);
    new G4PVPlacement(0, G4ThreeVector(fFib2X, fFib2Y, fPMTZ), fPmt_log, "pmt", fHousing_log, false, 1);
    fPmt_SD->SetPMTPos(0, fFib1X, fFib1Y, fPMTZ);
    fPmt_SD->SetPMTPos(1, fFib2X, fFib2Y, fPMTZ);

    //****************** Setup Sensitive Detectors

    if(!fScint_SD){
      fScint_SD = new LXeScintSD("/LXeDet/scintSD");
      SDman->AddNewDetector(fScint_SD);
    }

    scnt_l->SetSensitiveDetector(fScint_SD);
    fPhotocath_log->SetSensitiveDetector(fPmt_SD);

    VisAttributes();
    SurfaceProperties();
  }

  SetLogicalVolume(fHousing_log);
}


void NovaMainVolume::CopyValues(){
  fUpdated= fConstructor->getUpdated();

  rectangleWidth = fConstructor->getRectangleWidth();
  rectangleHeight = fConstructor->getRectangleHeight();
  cellLength = fConstructor->getCellLength();
  pvcThickness = fConstructor->getPvcThickness();
  fInner_Corner_Radius = fConstructor->getInnerCornerRadius();
  fFibRadius = fConstructor->getFiberRadius();
  fFib1X = fConstructor->getFiber1X();
  fFib1Y = fConstructor->getFiber1Y();
  fFib2X = fConstructor->getFiber2X();
  fFib2Y = fConstructor->getFiber2Y();
  fCelltoPMTL = fConstructor->getCellToPmtDistance();
  fFibTail = fConstructor->getFiberTailLength();

  fPMTHeight = fConstructor->getPmtHeight();
  fPMTZ = fConstructor->getPmtZ();
  fFibFullLength = fConstructor->getFullFiberLength();
  fFibZ = fConstructor->getFiberZ();
  fCellWidth = fConstructor->getCellWidth();
  fCellHeight = fConstructor->getCellHeight();
  fOuter_Corner_Radius = fConstructor->getOuterCornerRadius();

  UsePMT = fConstructor->getUsePMT();
}


void NovaMainVolume::VisAttributes(){

  G4VisAttributes* clear = new G4VisAttributes( G4Colour(255/255. ,255/255. ,255/255. ));
  clear->SetForceWireframe(true);
  G4VisAttributes* white = new G4VisAttributes(G4Color(1,1,1));
  G4VisAttributes* green = new G4VisAttributes(G4Colour(0,1,0));
  green->SetForceWireframe(true);
  G4VisAttributes* blue = new G4VisAttributes( G4Colour(0/255. ,0/255. ,255/255. ));

  fHousing_log->SetVisAttributes(G4VisAttributes::Invisible);
  PVC_l->SetVisAttributes(white);
  scnt_l->SetVisAttributes(blue);
  fiber_l->SetVisAttributes(green);
  fPhotocath_log->SetVisAttributes(blue);
}


void NovaMainVolume::SurfaceProperties(){

  // PMT
  {
    G4int    PMTentries = 0;
    G4double PMTenergy[500];
    G4double PMTindex[500];
    G4double PMTreflect[500];
    G4double PMTdetect[500];
    G4double indexconst = 1.49;
    G4double reflectconst = 0.;
    std::ifstream fphodet;
    if(UsePMT)
      fphodet.open("input/qe_file_2.dat");
    else
      fphodet.open("input/apd_qe_1.dat");

    while(!fphodet.eof()){
      G4String filler;
      G4double wavelength;
      fphodet >> wavelength >> filler >> PMTdetect[PMTentries];
      PMTdetect[PMTentries]  = PMTdetect[PMTentries];
      PMTenergy[PMTentries]  = (1240. / wavelength) * eV;
      PMTindex[PMTentries]   = indexconst;
      PMTreflect[PMTentries] = reflectconst;
      PMTentries++;
    }
    fphodet.close();

    opt_phodet = new G4OpticalSurface("opt_phodet");
    phodetMPT = new G4MaterialPropertiesTable();
    phodetMPT->AddProperty("EFFICIENCY",   PMTenergy, PMTdetect,  PMTentries);
    phodetMPT->AddProperty("REFLECTIVITY", PMTenergy, PMTreflect, PMTentries);
    phodetMPT->AddProperty("RINDEX",       PMTenergy, PMTindex,   PMTentries);
    opt_phodet->SetModel(glisur);
    opt_phodet->SetFinish(polished);
    opt_phodet->SetType(dielectric_metal);
    opt_phodet->SetMaterialPropertiesTable(phodetMPT);
    new G4LogicalSkinSurface("skin_pmt", fPhotocath_log, opt_phodet);
    phodetMPT->DumpTable();
  }

  //PVC
  {
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
    new G4LogicalSkinSurface("paint_b", PVC_l, op_paint);
  }
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

G4UnionSolid* NovaMainVolume::MakeCell(G4double radius){
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
