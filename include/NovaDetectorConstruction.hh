#ifndef LXeDetectorConstruction_H
#define LXeDetectorConstruction_H 1

#include "G4Material.hh"
#include "LXeDetectorMessenger.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Box;
class G4Tubs;
class LXePMTSD;
class LXeScintSD;
class G4Sphere;

class NovaDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    NovaDetectorConstruction();
    virtual ~NovaDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    void setDefaults();
    void updateGeometry();
    void printSettings();

    void setMainVolumeOn(G4bool b){mainVolume=b; isUpdated=true;}
    void setMainScintYield(G4double y);
    void setRectWidth(G4double a){rectangleWidth = a; isUpdated=true;}
    void setRectHeight(G4double a){rectangleHeight = a; isUpdated=true;}
    void setCellLength(G4double a){cellLength = a; isUpdated=true;}
    void setPVCThickness(G4double a){pvcThickness = a; isUpdated=true;}
    void setInnerCornerRadius(G4double a){innerCornerRadius = a; isUpdated=true;}
    void setFiberRadius(G4double a){fiberRadius = a; isUpdated=true;}
    void setFiber1X(G4double a){fiber1X = a; isUpdated=true;}
    void setFiber1Y(G4double a){fiber1Y = a; isUpdated=true;}
    void setFiber2X(G4double a){fiber2X = a; isUpdated=true;}
    void setFiber2Y(G4double a){fiber2Y = a; isUpdated=true;}
    void setCellToPmtDistance(G4double a){cellToPmtDistance = a; isUpdated=true;}
    void setFiberTailLength(G4double a){fiberTailLength = a; isUpdated=true;}
    void setUsePMT(bool b){usePmt = b;}
    void setPmtQuantumEfficiency(G4double qe){pmtQuantumEfficiency = qe; isUpdated=true;}

    G4double getRectangleWidth(){return rectangleWidth;}
    G4double getRectangleHeight(){return rectangleHeight;}
    G4double getCellLength(){return cellLength;}
    G4double getPvcThickness(){return pvcThickness;}
    G4double getInnerCornerRadius(){return innerCornerRadius;}
    G4double getFiberRadius(){return fiberRadius;}
    G4double getFiber1X(){return fiber1X;}
    G4double getFiber1Y(){return fiber1Y;}
    G4double getFiber2X(){return fiber2X;}
    G4double getFiber2Y(){return fiber2Y;}
    G4double getCellToPmtDistance(){return cellToPmtDistance;}
    G4double getFiberTailLength(){return fiberTailLength;}
    G4double getPmtHeight(){return fiberRadius / 4.;}
    G4double getFullFiberLength(){return cellToPmtDistance + cellLength / 2. + fiberTailLength;}
    G4double getOuterCornerRadius(){return innerCornerRadius + pvcThickness;}
    G4double getCellWidth(){return rectangleWidth + 2. * getOuterCornerRadius();}
    G4double getCellHeight(){return rectangleHeight + 2. * getOuterCornerRadius();}
    G4double getPmtZ(){return cellToPmtDistance + getPmtHeight();}
    G4double getFiberZ(){return getFullFiberLength() / 2. - (fiberTailLength + cellLength / 2.);}
    bool getUsePMT(){return usePmt;}
    G4bool getIsUpdated(){return isUpdated;}
    G4double getPlanckConstant() {
      return 1239.84193;
    }

  private:
    void DefineMaterials();
    G4VPhysicalVolume* ConstructDetector();
    G4bool isUpdated;
    G4Box* experimentalHallBox;
    G4LogicalVolume* experimentalHallLog;
    G4VPhysicalVolume* experimentalHallPhy;

    G4Element* oxygen;
    G4Element* carbon;
    G4Element* hydrogen;
    G4Element* titanium;

    G4Material* vacuum;
    G4Material* glass;
    G4Material* tiO2;
    G4Material* pvc;
    G4Material* liquidScintillator;
    G4Material* polystyrene;
    G4Material* pmma;
    G4Material* fluorinatedPolymer;

    G4MaterialPropertiesTable* liquidScintillatorMpt;
    G4MaterialPropertiesTable* vacuumMpt;
    G4MaterialPropertiesTable* polystyreneMpt;
    G4MaterialPropertiesTable* pmmaMpt;
    G4MaterialPropertiesTable* fluorinatedPolymerMpt;

    G4bool mainVolume;
    G4double scintillatorLightYield;
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
    G4double pmtQuantumEfficiency;
    bool usePmt;

    G4String inputDir;
};

#endif
