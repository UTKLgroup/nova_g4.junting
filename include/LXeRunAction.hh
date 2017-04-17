#include "G4UserRunAction.hh"
#include "NovaEventAction.hh"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#ifndef LXeRunAction_h
#define LXeRunAction_h 1

class LXeRecorderBase;

class LXeRunAction : public G4UserRunAction
{
  public:

    LXeRunAction(LXeRecorderBase*);
    virtual ~LXeRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    virtual void UpdateRunStatistics(RunStat stat);
    virtual void UpdateEvtStatistics(EvtStat stat);

  private:

    LXeRecorderBase* fRecorder;

    TFile *out;

    RunStat  runStat;
    TTree*   runTree;
    TBranch* runBranch;

    EvtStat  evtStat;
    TTree*   evtTree;
    TBranch* evtBranch;

};

#endif
