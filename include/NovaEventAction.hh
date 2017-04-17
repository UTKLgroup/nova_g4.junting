#ifndef NovaEventAction_h
#define NovaEventAction_h 1

#include "LXeEventMessenger.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Event;
class LXeRecorderBase;


struct EvtStat{
  
  G4int EvtNum;
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
  G4int PhotonCount_Scint;

  G4int PhotonCount_Ceren;

  G4int HitCount;
  G4int PMTsAboveThreshold;
  G4int AbsorptionCount;
  G4int BoundaryAbsorptionCount;
  G4int Unacounted;

  G4int primaryPDG;
  G4double primaryX;
  G4double primaryY;
  G4double primaryZ;
  G4double primaryPX;
  G4double primaryPY;
  G4double primaryPZ;

  G4double EDep;
  G4double EDepX;
  G4double EDepY;
  G4double EDepZ;


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

    NovaEventAction(LXeRecorderBase*);
    virtual ~NovaEventAction();

  public:

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void SetSaveThreshold(G4int );

    void SetEventVerbose(G4int v){fVerbose=v;}

    void SetPMTThreshold(G4int t){fPMTThreshold=t;}

    void SetForceDrawPhotons(G4bool b){fForcedrawphotons=b;}
    void SetForceDrawNoPhotons(G4bool b){fForcenophotons=b;}

  private:

    LXeRecorderBase* recorder;
    LXeEventMessenger* fEventMessenger;
    G4int              fSaveThreshold;
    G4int              scintCollectionId;
    G4int              pmtCollectionId;
    G4int              fVerbose;
    G4int              fPMTThreshold;

    RunStat runStat; 
    EvtStat evtStat;

    G4bool fForcedrawphotons;
    G4bool fForcenophotons;

};

#endif
