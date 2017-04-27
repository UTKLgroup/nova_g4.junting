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

    void setRectangleWidth(G4double a){rectangleWidth = a; isUpdated=true;}
    void setRectangleHeight(G4double a){rectangleHeight = a; isUpdated=true;}
    void setCellLength(G4double a){cellLength = a; isUpdated=true;}
    void setPvcThickness(G4double a){pvcThickness = a; isUpdated=true;}
    void setInnerCellCornerRadius(G4double a){innerCellCornerRadius = a; isUpdated=true;}
    void setFiberRadius(G4double a){fiberRadius = a; isUpdated=true;}
    G4double getOuterCellCornerRadius(){return innerCellCornerRadius + pvcThickness;}
    G4double getCellHeight(){return rectangleHeight + 2.0 * getOuterCellCornerRadius();}
    void setUsePMT(bool b){usePmt = b;}
    void setLiquidScintillatorLightYield(G4double y);
    void setDefaults();
    void updateGeometry();
    void printSetting();

    void readCsvFile(G4String filename,
                     std::vector<G4double>* energies,
                     std::vector<G4double>* values,
                     double valueUnit);
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
    G4double innerCellCornerRadius;
    G4double fiberRadius;
    G4double fiberCladdingFraction;
    G4double pmtThickness;
    bool usePmt;
};

#endif
