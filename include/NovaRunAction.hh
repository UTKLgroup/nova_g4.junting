#include "G4UserRunAction.hh"
#include "NovaEventAction.hh"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#ifndef NovaRunAction_h
#define NovaRunAction_h 1

class LXeRecorderBase;

class NovaRunAction : public G4UserRunAction
{
  public:

    NovaRunAction(LXeRecorderBase*);
    virtual ~NovaRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    virtual void UpdateRunStatistics(RunStat stat);
    virtual void UpdateEvtStatistics(EventStat stat);

  private:

    LXeRecorderBase* fRecorder;

    TFile *outputFilename;

    RunStat  runStat;
    TTree*   runTree;
    TBranch* runBranch;

    EventStat  eventStat;
    TTree*   eventTree;
    TBranch* evtBranch;

};

#endif
