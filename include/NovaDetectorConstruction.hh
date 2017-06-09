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

    void setStraightWidth(G4double a) {straightWidth = a; isUpdated = true;}
    void setStraightHeight(G4double a) {straightHeight = a; isUpdated = true;}
    void setDetectorLength(G4double a) {detectorLength = a; isUpdated = true;}
    void setSnoutLength(G4double a) {snoutLength = a; isUpdated = true;}
    void setPvcThickness(G4double a) {pvcThickness = a; isUpdated = true;}
    void setInnerCellCornerRadius(G4double a) {innerCellCornerRadius = a; isUpdated = true;}
    void setFiberRadius(G4double a) {fiberRadius = a; isUpdated = true;}
    void setFiberCurveRadius(G4double a) {fiberCurveRadius = a; isUpdated = true;}
    void setSimulationMode(G4String a) {simulationMode = a; isUpdated = true;}
    void setPhotodetectorType(G4String a) {photodetectorType = a; isUpdated = true;}
    void setLiquidScintillatorLightYield(G4double a) {liquidScintillatorLightYield = a; isUpdated = true;};
    void setDefaults();

    G4double getOuterCellCornerRadius() {return innerCellCornerRadius + pvcThickness;}
    G4double getCellHeight() {return straightHeight + 2.0 * getOuterCellCornerRadius();}
    G4double getCellWidth() { return straightWidth + 2.0 * getOuterCellCornerRadius();}
    G4double getFiberCoreRadius() {return fiberRadius * (1.0 - 2.0 * fiberCladdingFraction);};
    G4double getFiberInnerCladdingRadius() {return fiberRadius * (1.0 - fiberCladdingFraction);};

    void updateDetector();
    void printSetting();

    void readCsvFile(G4String filename,
                     std::vector<G4double>& energies,
                     std::vector<G4double>& values,
                     double valueUnit);
    G4double convertWavelengthToEnergy(G4double wavelength) {
      return PLANK_CONSTANT / wavelength * eV;
    }

  private:
    G4double PLANK_CONSTANT = 1239.84193;
    G4double FIBER_CORE_REFRACTION_INDEX = 1.59;
    G4double FIBER_FIRST_CLADDING_REFRACTION_INDEX = 1.49;
    G4double FIBER_SECOND_CLADDING_REFRACTION_INDEX = 1.42;

    G4String INPUT_DIR = "input";
    G4String PVC_REFLECTIVITY_FILENAME = "avg_ext.txt";
    G4String LIQUID_SCINTILLATOR_EMISSION_FILENAME = "ppo_emission.txt";
    G4String LIQUID_SCINTILLATOR_ABSORPTION_LENGTH_FILENAME = "PSTBulkAbsorb.cfg";
    G4String LIQUID_SCINTILLATOR_WLS_ABSORPTION_LENGTH_FILENAME = "UPS923.cfg";
    G4String LIQUID_SCINTILLATOR_WLS_EMISSION_FILENAME = "bisMSB_emission.txt";
    G4String FIBER_CORE_ABSORPTION_LENGTH_FILENAME = "fiberPSTabsorb.dat";
    G4String FIBER_CORE_WLS_ABSORPTION_LENGTH_FILENAME = "y11_abs_length.dat";
    G4String FIBER_CORE_WLS_EMISSION_FILENAME = "k27_emission.csv";
    G4String FIBER_CLADDING_ABSORPTION_LENGTH_FILENAME = "PMMABulkAbsorb.dat";
    G4String FIBER_CORE_REFRACTION_INDEX_FILENAME = "refraction_index_polystyrene_k27.csv";
    G4String FIBER_FIRST_CLADDING_REFRACTION_INDEX_FILENAME = "refraction_index_pmma.csv";
    G4String FIBER_SECOND_CLADDING_REFRACTION_INDEX_FILENAME = "refraction_index_fluorinated_polymer.csv";
    G4String APD_QUANTUM_EFFICIENCY_FILENAME = "apd_qe_1.dat";
    G4String PMT_QUANTUM_EFFICIENCY_FILENAME = "qe_file_2.dat";
    G4String LIQUID_SCINTILLATOR_SENSITIVE_DETECTOR_NAME = "/NovaDet/liquidScintillatorSd";
    G4String PMT_SENSITIVE_DETECTOR_NAME = "/NovaDet/pmtSd";

    G4String getFilePath(G4String filename) {return INPUT_DIR + "/" + filename;}
    void defineMaterials();
    void setMaterialProperties();
    void setGlassProperty();
    void setLiquidScintillatorProperty();
    void setFiberCoreProperty();
    void setPmmaProperty();
    void setFluorinatedPolymerProperty();
    void setGalacticProperty();
    void setPvcSurfaceProperty(G4LogicalVolume* pvcLogicalVolume, G4bool turnOffReflectivity = false);

    G4UnionSolid* makeCellSolid(G4double deltaSize, G4double length);
    G4UnionSolid* makePvcCellSolid();
    G4UnionSolid* makeLiquidScintillatorSolid();
    G4UnionSolid* makeFiberLoopSolid(G4double radius, G4double curveRadius, G4double straightLength);
    G4LogicalVolume* makeWlsFiberLoopLogicalVolume(G4double straightLength);
    G4LogicalVolume* makeWlsFiberStraightLogicalVolume(G4double straightLength);
    G4LogicalVolume* makePmtLogicalVolume();
    G4VPhysicalVolume* makeDetectorPhysicalVolume();
    G4VPhysicalVolume* makeNovaCellPhysicalVolume();
    G4VPhysicalVolume* makeSingleWlsFiberPhysicalVolume();
    G4VPhysicalVolume* makeBenchtopNovaCellPhysicalVolume();
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
    G4Material* glass;
    G4Material* aluminum;
    G4Material* galactic;

    G4MaterialPropertiesTable* liquidScintillatorMpt;
    G4MaterialPropertiesTable* fiberCoreMpt;
    G4MaterialPropertiesTable* pmmaMpt;
    G4MaterialPropertiesTable* fluorinatedPolymerMpt;

    G4double inputWavelength;
    G4double inputVariable;
    G4String filler;

    G4double liquidScintillatorLightYield;
    G4double straightWidth;
    G4double straightHeight;
    G4double detectorLength;
    G4double snoutLength;
    G4double fiberCurveToEndPlateDistance;
    G4double pvcThickness;
    G4double innerCellCornerRadius;
    G4double fiberRadius;
    G4double fiberCladdingFraction;
    G4double fiberCurveRadius;
    G4double pmtThickness;
    G4String photodetectorType;
    G4String simulationMode;
};

#endif
