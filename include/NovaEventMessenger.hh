#ifndef NovaEventMessenger_h
#define NovaEventMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class NovaEventAction;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class NovaEventMessenger: public G4UImessenger
{
  public:

    NovaEventMessenger(NovaEventAction*);
    virtual ~NovaEventMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);
 
  private:

    NovaEventAction*        fLXeEvent;
    G4UIcmdWithAnInteger*  fSaveThresholdCmd;
    G4UIcmdWithAnInteger*  fVerboseCmd;
    G4UIcmdWithAnInteger*  fPmtThresholdCmd;
    G4UIcmdWithABool*      fForceDrawPhotonsCmd;
    G4UIcmdWithABool*      fForceDrawNoPhotonsCmd;
};

#endif
