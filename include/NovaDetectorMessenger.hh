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
    G4UIdirectory* detectorDir;

    G4UIcmdWithADoubleAndUnit* rectangleWidth;
    G4UIcmdWithADoubleAndUnit* rectangleHeight;
    G4UIcmdWithADoubleAndUnit* cellLength;
    G4UIcmdWithADoubleAndUnit* pvcThickness;
    G4UIcmdWithADoubleAndUnit* innerCellCornerRadius;
    G4UIcmdWithADoubleAndUnit* fiberRadius;
    G4UIcmdWithABool* usePmt;
    G4UIcommand* update;
    G4UIcommand* defaultValue;
    G4UIcmdWithADouble* liquidScintillatorLightYield;
    G4UIcmdWithoutParameter* printSetting;
};

#endif
