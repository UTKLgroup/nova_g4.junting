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
/// \file optical/LXe/include/LXeMainVolume.hh
/// \brief Definition of the LXeMainVolume class
//
#ifndef LXeMainVolume_H
#define LXeMainVolume_H 1

#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh" 
#include "WLSfiber.hh"
#include "LXeDetectorConstruction.hh"

class LXeMainVolume : public G4PVPlacement
{
  public:

    LXeMainVolume(G4RotationMatrix *pRot,
                 const G4ThreeVector &tlate,
                 G4LogicalVolume *pMotherLogical,
                 G4bool pMany,
                 G4int pCopyNo,
                 LXeDetectorConstruction* c);

  private:

  void VisAttributes();
  void SurfaceProperties();
  
  void CopyValues();
  
  G4UnionSolid* MakeCell(G4double radius);
  G4UnionSolid* MakePVC(G4double innerradius, G4double outerradius);    
  
  G4bool fUpdated;
  
  LXeDetectorConstruction* fConstructor;
  
  // required
  G4double fRectWidth;
  G4double fRectHeight;
  G4double fCellLength;
  G4double fPVCThickness;
  G4double fInner_Corner_Radius;
  G4double fFibRadius;
  G4double fFib1X;
  G4double fFib1Y;
  G4double fFib2X;
  G4double fFib2Y;
  G4double fCelltoPMTL;
  G4double fFibTail;
  bool UsePMT;  

  // derived
  G4double fPMTHeight;
  G4double fPMTZ;
  G4double fFibFullLength;
  G4double fFibZ;
  G4double fCellWidth;
  G4double fCellHeight;
  G4double fOuter_Corner_Radius;  
  
  G4Box* fHousing_box;
  static G4LogicalVolume* fHousing_log;

  // geometries  
  G4Tubs* fPmt;
  G4Tubs* fPhotocath;
  
  G4LogicalVolume* fPmt_log;
  G4LogicalVolume* fPhotocath_log;
  
  G4UnionSolid* PVC_s ; 
  G4LogicalVolume* PVC_l ;
  
  G4SubtractionSolid* scnt_s; 
  G4LogicalVolume* scnt_l;  
  
  WLSfiber* FibMaker;
  G4LogicalVolume* fiber_l;
    
  G4Tubs* pmt_s;
  G4LogicalVolume* pmt_l;
  
  G4MaterialPropertiesTable* phodetMPT;
  G4OpticalSurface* opt_phodet;
  
  G4MaterialPropertiesTable* paintMPT;
  G4OpticalSurface* op_paint;
  
  //Sensitive Detectors
  static LXeScintSD* fScint_SD;
  static LXePMTSD* fPmt_SD;
};

#endif
