#ifndef NovaEventAction_h
#define NovaEventAction_h 1

#include "LXeEventMessenger.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Event;
class NovaRecorderBase;


struct EventStat{
  
  G4int eventId;
  G4int numWLS;
  G4int numRefl;
  G4int numTIRefl;

  G4double BeginTime;
  G4double BeginX;
  G4double BeginY;
  G4double BeginZ;
  G4double BeginE;
  G4double BeginPX;
  G4double BeginPY;
  G4double BeginPZ;
  G4double BeginWL;

  G4double EnterTime;
  G4double EnterX;
  G4double EnterY;
  G4double EnterZ;
  G4double EnterE;
  G4double EnterPX;
  G4double EnterPY;
  G4double EnterPZ;
  G4double EnterWL;

  G4double HitTime;
  G4double HitX;
  G4double HitY;
  G4double HitZ;
  G4double HitE;
  G4double HitPX;
  G4double HitPY;
  G4double HitPZ; 
  G4double HitWL; 

  G4double TrkLength;
};

struct RunStat {
  G4int scintillationPhotonCount;

  G4int cherenkovPhotonCount;

  G4int hitCount;
  G4int pmtAboveThresholdCount;
  G4int absorptionCount;
  G4int boundaryAbsorptionCount;
  G4int unacountedCount;

  G4int primaryPDG;
  G4double primaryX;
  G4double primaryY;
  G4double primaryZ;
  G4double primaryPX;
  G4double primaryPY;
  G4double primaryPZ;

  G4double energyDeposition;
  G4double energyDepositionX;
  G4double energyDepositionY;
  G4double energyDepositionZ;


  /*
  G4int VetoPhotonCount_Ceren;
  G4int VetoHitCount;
  G4int VetoPMTsAboveThreshold;
  G4int VetoAbsorptionCount;
  G4int VetoBoundaryAbsorptionCount;
  G4int VetoUnacounted;
  */

  /*
  G4double inX;
  G4double inY;
  G4double inZ;

  G4double outX;
  G4double outY;
  G4double outZ;

  G4double VetoinX;
  G4double VetoinY;
  G4double VetoinZ;

  G4double VetooutX;
  G4double VetooutY;
  G4double VetooutZ;
  */

};  


class NovaEventAction : public G4UserEventAction
{
  public:

    NovaEventAction(NovaRecorderBase*);
    virtual ~NovaEventAction();

  public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void SetSaveThreshold(G4int );

    void SetEventVerbose(G4int v){verbose=v;}

    void SetPMTThreshold(G4int t){pmtThreshold=t;}

    void SetForceDrawPhotons(G4bool b){forceDrawPhotons=b;}
    void SetForceDrawNoPhotons(G4bool b){forceDrawNoPhotons=b;}

  private:
    NovaRecorderBase* recorder;
    LXeEventMessenger* fEventMessenger;
    G4int              fSaveThreshold;
    G4int              scintCollectionId;
    G4int              pmtCollectionId;
    G4int              verbose;
    G4int              pmtThreshold;

    RunStat runStat; 
    EventStat eventStat;

    G4bool forceDrawPhotons;
    G4bool forceDrawNoPhotons;
};

#endif
