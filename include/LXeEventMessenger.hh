#ifndef LXeEventMessenger_h
#define LXeEventMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class NovaEventAction;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class LXeEventMessenger: public G4UImessenger
{
  public:

    LXeEventMessenger(NovaEventAction*);
    virtual ~LXeEventMessenger();
 
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
