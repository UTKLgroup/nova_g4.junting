#ifndef LXeStackingAction_H
#define LXeStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"

class NovaStackingAction : public G4UserStackingAction
{
  public:
    NovaStackingAction();
    virtual ~NovaStackingAction();
 
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();
 };

#endif
