//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file optical/LXe/src/LXeMainVolume.cc
/// \brief Implementation of the LXeMainVolume class
//
//
#include <G4SIunits.hh>
#include "LXeMainVolume.hh"
#include "G4SDManager.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "LXePMTSD.hh"
#include "LXeScintSD.hh"
#include "G4SystemOfUnits.hh"

LXeScintSD* LXeMainVolume::fScint_SD=NULL;
LXePMTSD* LXeMainVolume::fPmt_SD=NULL;
G4LogicalVolume* LXeMainVolume::fHousing_log = NULL;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeMainVolume::LXeMainVolume(G4RotationMatrix *pRot,
                             const G4ThreeVector &tlate,
                             G4LogicalVolume *pMotherLogical,
                             G4bool pMany,
                             G4int pCopyNo,
                             LXeDetectorConstruction* c)
  //Pass info to the G4PVPlacement constructor
  :G4PVPlacement(pRot,tlate,
                 //Temp logical volume must be created here
                 new G4LogicalVolume(new G4Box("temp",1,1,1),
                                     G4Material::GetMaterial("Vacuum"),
                                     "temp",0,0,0),
                 "housing",pMotherLogical,pMany,pCopyNo),fConstructor(c)
{
  CopyValues();

  if(!fHousing_log || fUpdated){

    //****************** Build a Cell

    //--------- housing

    fHousing_box = new G4Box("holder_s",
			     fCellWidth / 2.  + 10.*cm,
			     fCellHeight / 2. + 10.*cm,
			     fCelltoPMTL      + 10.*cm);
    fHousing_log = new G4LogicalVolume(fHousing_box, G4Material::GetMaterial("Vacuum"), "fHousing_log", 0, 0, 0);   


    //--------- PVC

    PVC_s = LXeMainVolume::MakePVC(fInner_Corner_Radius, fOuter_Corner_Radius);
    PVC_l = new G4LogicalVolume(PVC_s, G4Material::GetMaterial("PVC"), "PVC_l", 0, 0, 0);
    new G4PVPlacement(0, G4ThreeVector(0, -fCellHeight / 2. + fPVCThickness / 2.), PVC_l, "PVC", fHousing_log, false, 0);   

    //--------- scintillator

    G4UnionSolid* scnt_full_s = LXeMainVolume::MakeCell(fInner_Corner_Radius);
    G4Tubs* fiber_s = new G4Tubs("fiber_s", 0, fFibRadius, fCellLength/2, 0, twopi*rad);
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeMainVolume::CopyValues(){
  fUpdated=fConstructor->GetUpdated();

  fRectWidth = fConstructor->GetRectWidth();
  fRectHeight = fConstructor->GetRectHeight();
  fCellLength = fConstructor->GetCellLength();
  fPVCThickness = fConstructor->GetPVCThickness();
  fInner_Corner_Radius = fConstructor->GetInner_Corner_Radius();
  fFibRadius = fConstructor->GetFibRadius();
  fFib1X = fConstructor->GetFib1X();
  fFib1Y = fConstructor->GetFib1Y();
  fFib2X = fConstructor->GetFib2X();
  fFib2Y = fConstructor->GetFib2Y();
  fCelltoPMTL = fConstructor->GetCelltoPMTL();
  fFibTail = fConstructor->GetFibTail();

  fPMTHeight = fConstructor->GetPMTHeight();
  fPMTZ = fConstructor->GetPMTZ();
  fFibFullLength = fConstructor->GetFibFullLength();
  fFibZ = fConstructor->GetFibZ();
  fCellWidth = fConstructor->GetCellWidth();
  fCellHeight = fConstructor->GetCellHeight();
  fOuter_Corner_Radius = fConstructor->GetOuter_Corner_Radius();    

  UsePMT = fConstructor->GetUsePMT();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeMainVolume::VisAttributes(){

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeMainVolume::SurfaceProperties(){

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
    //paintMPT->DumpTable();
  }

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4UnionSolid* LXeMainVolume::MakePVC(G4double innerradius, G4double outerradius){

  G4Box* PVCh = new G4Box("PVCh",
			  fRectWidth / 2.,
			  fPVCThickness / 2.,
			  fCellLength / 2.);

  G4Box* PVCv = new G4Box("PVCv",
			  fRectHeight / 2.,
			  fPVCThickness / 2.,
			  fCellLength / 2.);

  G4Tubs* PVCcorner = new G4Tubs("PVCcorner",
				 innerradius,
				 outerradius,
				 fCellLength / 2.,
				 0,
				 0.5 * pi*rad);


  G4RotationMatrix Rotate90, Rotate180, Rotate270;
  Rotate90.rotateZ(90*deg);
  Rotate180.rotateZ(180*deg);
  Rotate270.rotateZ(270*deg);

  G4UnionSolid* PVCassembly1 = new G4UnionSolid("PVCassembly1",
						PVCh,
						PVCcorner,
						&Rotate90,
						G4ThreeVector(fRectWidth / 2., innerradius + fPVCThickness / 2.)
						);

  G4UnionSolid* PVCassembly2 = new G4UnionSolid("PVCassembly2",
						PVCassembly1,
						PVCv,
						&Rotate90,
						G4ThreeVector(fRectWidth / 2. + innerradius + fPVCThickness / 2., innerradius + fPVCThickness / 2. + fRectHeight / 2.)
						);

  G4UnionSolid* PVCassembly3 = new G4UnionSolid("PVCassembly3",
						PVCassembly2,
						PVCcorner,
						0,
						G4ThreeVector(fRectWidth / 2., innerradius + fPVCThickness / 2. + fRectHeight)
						);

  G4UnionSolid* PVCassembly4 = new G4UnionSolid("PVCassembly4",
						PVCassembly3,
						PVCh,
						0,
						G4ThreeVector(0, 2. * innerradius + fRectHeight + fPVCThickness)
						);

  G4UnionSolid* PVCassembly5 = new G4UnionSolid("PVCassembly5",
						PVCassembly4,
						PVCcorner,
						&Rotate270,
						G4ThreeVector(-fRectWidth / 2., innerradius + fPVCThickness / 2. + fRectHeight)
						);

  G4UnionSolid* PVCassembly6 = new G4UnionSolid("PVCassembly6",
						PVCassembly5,
						PVCv,
						&Rotate90,
						G4ThreeVector(-(fRectWidth / 2. + innerradius + fPVCThickness / 2.), innerradius + fPVCThickness / 2. + fRectHeight / 2.)
						);


  G4UnionSolid* PVCassembly7 = new G4UnionSolid("PVCassembly7",
						PVCassembly6,
						PVCcorner,
						&Rotate180,
						G4ThreeVector(-fRectWidth / 2., innerradius + fPVCThickness / 2.)
						);

  return PVCassembly7;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4UnionSolid* LXeMainVolume::MakeCell(G4double radius){

  G4double fullheight = fRectHeight + 2. * radius;
  G4double fullwidth = fRectWidth + 2. * radius;

  G4Box* box1 = new G4Box("box1",
			  fRectWidth / 2.,
			  fullheight / 2.,
			  fCellLength / 2.);

  G4Box* box2 = new G4Box("box2",
			  fullwidth / 2.,
			  fRectHeight / 2.,
			  fCellLength / 2.);

  G4UnionSolid* add_boxes = new G4UnionSolid("add_boxes",
					     box1,
					     box2,
					     0,
					     G4ThreeVector());

  G4Tubs* inner_corner = new G4Tubs("inner_corner", 0*mm, radius, fCellLength / 2., 0, twopi*rad);

  G4UnionSolid* add_corner1 = new G4UnionSolid("add_corner1",
					       add_boxes,
					       inner_corner,
					       0,
					       G4ThreeVector(fRectWidth / 2.,
							     fRectHeight / 2.,
							     0));

  G4UnionSolid* add_corner2 = new G4UnionSolid("add_corner2",
					       add_corner1,
					       inner_corner,
					       0,
					       G4ThreeVector(-fRectWidth / 2.,
							     fRectHeight / 2.,
							     0));

  G4UnionSolid* add_corner3 = new G4UnionSolid("add_corner3",
					       add_corner2,
					       inner_corner,
					       0,
					       G4ThreeVector(fRectWidth / 2.,
							     -fRectHeight / 2.,
							     0));

  G4UnionSolid* add_corner4 = new G4UnionSolid("add_corner4",
					       add_corner3,
					       inner_corner,
					       0,
					       G4ThreeVector(-fRectWidth / 2.,
							     -fRectHeight / 2.,
							     0));


  return add_corner4;
}
