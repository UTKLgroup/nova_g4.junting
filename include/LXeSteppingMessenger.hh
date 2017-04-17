#ifndef LXeSteppingMessenger_h
#define LXeSteppingMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class NovaSteppingAction;
class G4UIcmdWithABool;

class LXeSteppingMessenger: public G4UImessenger
{
  public:
    LXeSteppingMessenger(NovaSteppingAction*);
    virtual ~LXeSteppingMessenger();
 
    virtual void SetNewValue(G4UIcommand*, G4String);

  private:

    NovaSteppingAction*        fStepping;
    G4UIcmdWithABool*  fOneStepPrimariesCmd;
 
};

#endif
