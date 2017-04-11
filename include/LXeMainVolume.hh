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
