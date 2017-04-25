#ifndef NovaTrajectory_h
#define NovaTrajectory_h 1

#include <vector>
#include <stdlib.h>
#include "globals.hh"
#include "G4ios.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4VTrajectory.hh"
#include "G4ParticleDefinition.hh"
#include "G4TrajectoryPoint.hh"
#include "G4TrajectoryContainer.hh"


typedef std::vector<G4VTrajectoryPoint*> TrajectoryPointContainer;


class NovaTrajectory : public G4VTrajectory
{
  public:
    NovaTrajectory();
    NovaTrajectory(const G4Track* aTrack);
    NovaTrajectory(NovaTrajectory&);
    virtual ~NovaTrajectory();

    inline void* operator new(size_t);
    inline void  operator delete(void*);
    inline int operator == (const NovaTrajectory& right) const { return (this==&right);}

    inline G4int GetTrackID() const { return trackId; }
    inline G4int GetParentID() const { return parentID; }
    inline G4String GetParticleName() const { return particleName; }
    inline G4double GetCharge() const { return pdgCharge; }
    inline G4int GetPDGEncoding() const { return pdgEncoding; }
    inline G4ThreeVector GetInitialMomentum() const { return initialMomentum; }

    inline int GetStatus() const { return trackStatus; }
    inline void SetStatus(int a) { trackStatus = a; }

    inline G4double getTrackLength() const { return trackLength; }
    inline void setTrackLength(G4double a) { trackLength = a; }

    inline G4String GetProcessName() const { return processName; }
    inline void SetProcessName(G4String a) { processName = a; }

    inline G4int getReflectionCount() const { return reflectionCount; }
    inline void setReflectionCount(G4int a) { reflectionCount = a; }

    inline G4int getTotalInternalReflectionCount() const { return totalInternalReflectionCount; }
    inline void setTotalInternalReflectionCount(G4int a) { totalInternalReflectionCount = a; }

    virtual void ShowTrajectory(std::ostream& os=G4cout) const;
    virtual void DrawTrajectory() const;
    virtual void AppendStep(const G4Step* aStep);
    virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

    G4ParticleDefinition* GetParticleDefinition();

    virtual int GetPointEntries() const { return (int) trajectoryPointContainer->size(); }
    virtual G4VTrajectoryPoint* GetPoint(G4int i) const { return (*trajectoryPointContainer)[i]; }

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    NovaTrajectory* getParentTrajectory(G4TrajectoryContainer* trajectoryContainer);

  private:
    TrajectoryPointContainer* trajectoryPointContainer;
    G4int trackId;
    G4int parentID;
    G4double pdgCharge;
    G4int pdgEncoding;
    G4String particleName;
    G4ThreeVector initialMomentum;
    int trackStatus;
    G4double trackLength;
    G4String processName;
    G4int reflectionCount;
    G4int totalInternalReflectionCount;
};

extern G4Allocator<NovaTrajectory> myTrajectoryAllocator;

inline void* NovaTrajectory::operator new(size_t) {
  void* aTrajectory = (void*) myTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void NovaTrajectory::operator delete(void* aTrajectory) {
  myTrajectoryAllocator.FreeSingle((NovaTrajectory*)aTrajectory);
}

#endif
