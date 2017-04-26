#ifndef NovaDetectorConstruction_H
#define NovaDetectorConstruction_H 1

#include <G4NistManager.hh>
#include "G4SystemOfUnits.hh"
#include <G4UnionSolid.hh>
#include "G4Material.hh"
#include "NovaDetectorMessenger.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Box;
class G4Tubs;
class NovaPmtSd;
class NovaLiquidScintillatorSd;
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

    void setLiquidScintillatorLightYield(G4double y);
    void setRectWidth(G4double a){rectangleWidth = a; isUpdated=true;}
    void setRectHeight(G4double a){rectangleHeight = a; isUpdated=true;}
    void setCellLength(G4double a){cellLength = a; isUpdated=true;}
    void setPVCThickness(G4double a){pvcThickness = a; isUpdated=true;}
    void setInnerCornerRadius(G4double a){innerCornerRadius = a; isUpdated=true;}
    void setFiberRadius(G4double a){fiberRadius = a; isUpdated=true;}
    void setUsePMT(bool b){usePmt = b;}

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
    G4double convertWavelengthToEnergy(G4double wavelength) {
      return PLANK_CONSTANT / wavelength * eV;
    }

  private:
    G4double PLANK_CONSTANT = 1239.84193;
    G4String INPUT_DIR = "input";
    G4String LIQUID_SCINTILLATOR_EMISSION_FILENAME = "ppo_emission.txt";
    G4String LIQUID_SCINTILLATOR_ABSORPTION_LENGTH_FILENAME = "PSTBulkAbsorb.cfg";
    G4String LIQUID_SCINTILLATOR_WLS_ABSORPTION_LENGTH_FILENAME = "UPS923.cfg";
    G4String LIQUID_SCINTILLATOR_WLS_EMISSION_FILENAME = "bisMSB_emission.txt";
    G4String FIBER_CORE_ABSORPTION_LENGTH_FILENAME = "fiberPSTabsorb.dat";
    G4String FIBER_CORE_WLS_ABSORPTION_LENGTH_FILENAME = "y11_abs_length.dat";
    G4String FIBER_CORE_WLS_EMISSION_FILENAME = "WLSemit.cfg";
    G4String FIBER_CLADDING_ABSORPTION_LENGTH_FILENAME = "PMMABulkAbsorb.dat";
    G4String APD_QUANTUM_EFFICIENCY_FILENAME = "apd_qe_1.dat";
    G4String PMT_QUANTUM_EFFICIENCY_FILENAME = "qe_file_2.dat";

    G4String getFilePath(G4String filename) {return INPUT_DIR + "/" + filename;}
    void defineMaterials();
    void defineGlass();
    void defineLiquidScintillator(G4String materialName);
    void defineFiberCore(G4String materialName);
    void definePmma(G4String materialName);
    void defineFluorinatedPolymer(G4String materialName);
    void definePvc(G4String materialName);
    void defineGalactic();
    G4UnionSolid* makePvcCell();
    G4UnionSolid* makeLiquidScintillator();
    G4LogicalVolume* makeWlsFiber();
    G4LogicalVolume* makePmt();
    G4VPhysicalVolume* constructDetector();
    G4VPhysicalVolume* constructLiquidScintillatorCell();
    G4VPhysicalVolume* constructSingleWlsFiber();
    G4bool isUpdated;
    G4Box* experimentalHallSolid;
    G4LogicalVolume* experimentalHallLogicalVolume;
    G4VPhysicalVolume* experimentalHallPhysicalVolume;
    G4NistManager* nistManager;

    G4Element* H;
    G4Element* C;
    G4Element* O;
    G4Element* Ti;

    G4Material* tiO2;
    G4Material* polystyrene;
    G4Material* pvc;
    G4Material* liquidScintillator;
    G4Material* fiberCore;
    G4Material* pmma;
    G4Material* fluorinatedPolymer;

    G4MaterialPropertiesTable* liquidScintillatorMpt;
    G4MaterialPropertiesTable* fiberCoreMpt;
    G4MaterialPropertiesTable* pmmaMpt;
    G4MaterialPropertiesTable* fluorinatedPolymerMpt;

    G4double inputWavelength;
    G4double inputVariable;
    G4String filler;

    G4double liquidScintillatorLightYield;
    G4double rectangleWidth;
    G4double rectangleHeight;
    G4double cellLength;
    G4double pvcThickness;
    G4double innerCornerRadius;
    G4double fiberRadius;
    G4double fiberCladdingFraction;
    G4double fiber1X;
    G4double fiber1Y;
    G4double fiber2X;
    G4double fiber2Y;
    G4double cellToPmtDistance;
    G4double fiberTailLength;
    G4double pmtThickness;
    bool usePmt;
};

#endif
