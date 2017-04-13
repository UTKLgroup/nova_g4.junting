#ifndef NovaMainVolume_H
#define NovaMainVolume_H 1

#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh" 
#include "NovaWlsFiber.hh"
#include "NovaDetectorConstruction.hh"

class NovaMainVolume : public G4PVPlacement
{
  public:
    NovaMainVolume(G4RotationMatrix *pRot,
                   const G4ThreeVector &tlate,
                   G4LogicalVolume *pMotherLogical,
                   G4bool pMany,
                   G4int pCopyNo,
                   NovaDetectorConstruction* c);

  private:
    void VisAttributes();
    void SurfaceProperties();

    void CopyValues();

    G4UnionSolid* makeCell(G4double radius);
    G4UnionSolid* makePvc(G4double innerRadius, G4double outerRadius);

    G4bool isUpdated;

    NovaDetectorConstruction* fConstructor;


    G4double rectangleWidth;
    G4double rectangleHeight;
    G4double cellLength;
    G4double pvcThickness;
    G4double innerCornerRadius;
    G4double fiberRadius;
    G4double fiber1X;
    G4double fiber1Y;
    G4double fiber2X;
    G4double fiber2Y;
    G4double cellToPmtDistance;
    G4double fiberTailLength;
    bool usePMT;

    G4double pmtHeight;
    G4double pmtZ;
    G4double fullFiberLength;
    G4double fiberZ;
    G4double cellWidth;
    G4double cellHeight;
    G4double outerCornerRadius;

    G4Box* housingSolid;
    static G4LogicalVolume* housingLog;

    G4Tubs* pmt;
    G4Tubs* photoCathode;

    G4LogicalVolume* pmtLog;
    G4LogicalVolume* photocathodeLog;

    G4UnionSolid* pvcSolid ;
    G4LogicalVolume* pvcLog ;

    G4SubtractionSolid* scintillator;
    G4LogicalVolume* scintillatorLog;

    NovaWlsFiber* wlsFiber;
    G4LogicalVolume* fiberLog;

    G4Tubs* pmt_s;
    G4LogicalVolume* pmt_l;

    G4MaterialPropertiesTable* phodetMPT;
    G4OpticalSurface* opt_phodet;

    G4MaterialPropertiesTable* paintMPT;
    G4OpticalSurface* op_paint;

    static LXeScintSD* scintSd;
    static LXePMTSD* pmtSd;
};

#endif
