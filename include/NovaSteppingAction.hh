#ifndef NovaSteppingAction_H
#define LXeSteppingACtion_H 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"

#include "G4OpBoundaryProcess.hh"

class LXeRecorderBase;
class NovaEventAction;
class LXeTrackingAction;
class LXeSteppingMessenger;

class NovaSteppingAction : public G4UserSteppingAction
{
  public:

    NovaSteppingAction(LXeRecorderBase*);
    virtual ~NovaSteppingAction();
    virtual void UserSteppingAction(const G4Step*);

    void SetOneStepPrimaries(G4bool b){fOneStepPrimaries=b;}
    G4bool GetOneStepPrimaries(){return fOneStepPrimaries;}
 
  private:

    LXeRecorderBase* fRecorder;
    G4bool fOneStepPrimaries;
    LXeSteppingMessenger* fSteppingMessenger;

    G4OpBoundaryProcessStatus fExpectedNextStatus;
};

#endif
