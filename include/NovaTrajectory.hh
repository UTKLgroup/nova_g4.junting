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

typedef std::vector<G4VTrajectoryPoint*> TrajectoryPointContainer;

class NovaTrajectory : public G4VTrajectory
{

//--------
   public: // without description
//--------

// Constructor/Destructor

     NovaTrajectory();
     NovaTrajectory(const G4Track* aTrack);
     NovaTrajectory(NovaTrajectory&);
     virtual ~NovaTrajectory();

// Operators

     inline void* operator new(size_t);
     inline void  operator delete(void*);
     inline int operator == (const NovaTrajectory& right) const
     { return (this==&right); }

// Get/Set functions

     inline G4int GetTrackID() const { return fTrackID; }
     inline G4int GetParentID() const { return fParentID; }
     inline G4String GetParticleName() const { return fParticleName; }
     inline G4double GetCharge() const { return fPDGCharge; }
     inline G4int GetPDGEncoding() const { return fPDGEncoding; }
     inline G4ThreeVector GetInitialMomentum() const { return fInitialMomentum; }

     inline int GetStatus() const { return fTrackStatus; }
     inline void SetStatus(int a) { fTrackStatus = a; }

     inline G4double GetTrkLength() const { return fTrkLength; }
     inline void SetTrkLength(G4double a) { fTrkLength = a; }

     inline G4String GetProcessName() const { return fProcessName; }
     inline void SetProcessName(G4String a) { fProcessName = a; }

     inline G4int GetNumRefl() const { return fNumRefl; }
     inline void SetNumRefl(G4int a) { fNumRefl = a; }

     inline G4int GetNumTIRefl() const { return fNumTIRefl; }
     inline void SetNumTIRefl(G4int a) { fNumTIRefl = a; }

// Other member functions

     virtual void ShowTrajectory(std::ostream& os=G4cout) const;
     virtual void DrawTrajectory() const;
     virtual void AppendStep(const G4Step* aStep);
     virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

     G4ParticleDefinition* GetParticleDefinition();

     virtual int GetPointEntries() const
     { return fpPointsContainer->size(); }
     virtual G4VTrajectoryPoint* GetPoint(G4int i) const
     { return (*fpPointsContainer)[i]; }

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;

//---------
   private:
//---------

// Member data

     TrajectoryPointContainer* fpPointsContainer;

     G4int         fTrackID;
     G4int         fParentID;
     G4double      fPDGCharge;
     G4int         fPDGEncoding;
     G4String      fParticleName;
     G4ThreeVector fInitialMomentum;

     int fTrackStatus;
     G4double fTrkLength;
     G4String fProcessName;
     G4int fNumRefl;
     G4int fNumTIRefl;
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
