#ifndef NovaTrajectoryPoint_h
#define NovaTrajectoryPoint_h 1

#include "globals.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4TrajectoryPoint.hh"
#include "G4StepStatus.hh"


class G4Track;
class G4Step;
class G4VProcess;

class NovaTrajectoryPoint : public G4TrajectoryPoint
{
  public:
    NovaTrajectoryPoint();
    NovaTrajectoryPoint(const G4Track* aTrack);
    NovaTrajectoryPoint(const G4Step* aStep);
    NovaTrajectoryPoint(const NovaTrajectoryPoint &right);
    virtual ~NovaTrajectoryPoint();

    inline void *operator new(size_t);
    inline void operator delete(void *aTrajectoryPoint);
    inline int operator==(const NovaTrajectoryPoint& right) const { return (this==&right); };

    inline G4double GetTime() const { return fTime; };
    inline const G4ThreeVector GetMomentum() const { return fMomentum; };
    inline G4StepStatus GetStepStatus() const { return fStepStatus; };
    inline G4String GetVolumeName() const { return fVolumeName; };

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;

  private:
    G4double fTime;
    G4ThreeVector fMomentum;
    G4StepStatus fStepStatus;
    G4String fVolumeName;
};

extern G4DLLEXPORT G4Allocator<NovaTrajectoryPoint> aTrajPointAllocator;

inline void* NovaTrajectoryPoint::operator new(size_t)
{
  void *aTrajectoryPoint = (void*) aTrajPointAllocator.MallocSingle();
  return aTrajectoryPoint;
}

inline void NovaTrajectoryPoint::operator delete(void *aTrajectoryPoint)
{
  aTrajPointAllocator.FreeSingle((NovaTrajectoryPoint*) aTrajectoryPoint);
}

#endif
