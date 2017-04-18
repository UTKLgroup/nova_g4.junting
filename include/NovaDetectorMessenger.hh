#ifndef NovaDetectorMessenger_h
#define NovaDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class NovaDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcommand;
class G4UIcmdWithABool;
class G4UIcmdWithADouble;
class G4UIcmdWithoutParameter;

class NovaDetectorMessenger: public G4UImessenger
{
  public:
    NovaDetectorMessenger(NovaDetectorConstruction*);
    virtual ~NovaDetectorMessenger();
    virtual void SetNewValue(G4UIcommand*, G4String);

  private:
    NovaDetectorConstruction* fLXeDetector;
    G4UIdirectory* fDetectorDir;
    G4UIdirectory* fVolumesDir;

    G4UIcmdWithADoubleAndUnit* fRectWidthCmd;
    G4UIcmdWithADoubleAndUnit* fRectHeightCmd;
    G4UIcmdWithADoubleAndUnit* fCellLengthCmd;
    G4UIcmdWithADoubleAndUnit* fPVCThicknessCmd;
    G4UIcmdWithADoubleAndUnit* fInner_Corner_RadiusCmd;
    G4UIcmdWithADoubleAndUnit* fFibRadiusCmd;
    G4UIcmdWithADoubleAndUnit* fFib1XCmd;
    G4UIcmdWithADoubleAndUnit* fFib1YCmd;
    G4UIcmdWithADoubleAndUnit* fFib2XCmd;
    G4UIcmdWithADoubleAndUnit* fFib2YCmd;
    G4UIcmdWithADoubleAndUnit* fCelltoPMTLCmd;
    G4UIcmdWithADoubleAndUnit* fFibTailCmd;
    G4UIcmdWithABool* fUsePMTCmd;
    G4UIcmdWithADouble* fScintReflectivityCmd;
    G4UIcmdWithADouble* fVetoReflectivityCmd;
    G4UIcmdWithABool* fLxeCmd;
    G4UIcommand* fUpdateCmd;
    G4UIcommand* fDefaultsCmd;
    G4UIcmdWithADouble* fMainScintYield;
    G4UIcmdWithADouble* fpmtQECmd;
    G4UIcmdWithoutParameter* fPrintSettings;
};

#endif
