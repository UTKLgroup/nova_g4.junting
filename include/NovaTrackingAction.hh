#ifndef LXeTrackingAction_h
#define LXeTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class NovaRecorderBase;

class NovaTrackingAction : public G4UserTrackingAction {

  public:
    NovaTrackingAction(NovaRecorderBase*);
    virtual ~NovaTrackingAction() {};

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    NovaRecorderBase* fRecorder;
};

#endif
