#include "G4UserRunAction.hh"
#include "NovaEventAction.hh"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#ifndef NovaRunAction_h
#define NovaRunAction_h 1

class NovaRecorderBase;

class NovaRunAction : public G4UserRunAction
{
  public:
    NovaRunAction(NovaRecorderBase*);
    virtual ~NovaRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    virtual void UpdateRunStatistics(RunStat stat);
    virtual void UpdateHitStatistics(HitStat stat);

  private:
    NovaRecorderBase* recorder;
    TFile* outputFilename;
    RunStat  runStat;
    TTree*   runTree;

    HitStat  hitStat;
    TTree*   hitTree;
};

#endif
