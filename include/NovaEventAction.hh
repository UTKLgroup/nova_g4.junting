#ifndef NovaEventAction_h
#define NovaEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "NovaTrajectory.hh"


class G4Event;
class NovaRecorderBase;

struct EventStat
{
  G4int eventId;
  G4int wlsCount;
  G4int reflectionCount;
  G4int totalInternalReflectionCount;

  G4double beginTime;
  G4double beginX;
  G4double beginY;
  G4double beginZ;
  G4double beginE;
  G4double beginPX;
  G4double beginPY;
  G4double beginPZ;
  G4double beginWavelength;

  G4double enterTime;
  G4double enterX;
  G4double enterY;
  G4double enterZ;
  G4double enterE;
  G4double enterPX;
  G4double enterPY;
  G4double enterPZ;
  G4double enterWavelength;

  G4double hitTime;
  G4double hitX;
  G4double hitY;
  G4double hitZ;
  G4double hitE;
  G4double hitPX;
  G4double hitPY;
  G4double hitPZ;
  G4double hitWavelength;

  G4double trackLength;
};

struct RunStat
{
  G4int scintillationPhotonCount;
  G4int cherenkovPhotonCount;

  G4int hitCount;
  G4int pmtAboveThresholdCount;
  G4int absorptionCount;
  G4int boundaryAbsorptionCount;
  G4int photocathodeAbsorptionCount;
  G4int outOfWorldCount;
  G4int unacountedCount;

  G4int primaryPdg;
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
    void incrementWlsCount(NovaTrajectory* trajectory);
    void printEventInfo();

  private:
    NovaRecorderBase* recorder;
    G4int saveThreshold;
    G4int scintCollectionId;
    G4int pmtCollectionId;
    G4int verbose;
    G4int pmtThreshold;

    RunStat runStat; 
    EventStat eventStat;
};

#endif
