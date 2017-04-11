#ifndef F04TrajectoryPoint_h
#define F04TrajectoryPoint_h 1

#include "globals.hh"

#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4TrajectoryPoint.hh"

#include "G4StepStatus.hh"

class G4Track;
class G4Step;
class G4VProcess;

class F04TrajectoryPoint : public G4TrajectoryPoint
{
public:
    F04TrajectoryPoint();
    F04TrajectoryPoint(const G4Track* aTrack);
    F04TrajectoryPoint(const G4Step* aStep);
    F04TrajectoryPoint(const F04TrajectoryPoint &right);
    virtual ~F04TrajectoryPoint();

    inline void *operator new(size_t);
    inline void operator delete(void *aTrajectoryPoint);
    inline int operator==(const F04TrajectoryPoint& right) const
    { return (this==&right); };

    inline G4double GetTime() const { return fTime; };
    inline const G4ThreeVector GetMomentum() const { return fMomentum; };
    inline G4StepStatus GetStepStatus() const { return fStepStatus; };
    inline G4String GetVolumeName() const { return fVolumeName; };

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;


private:
    G4double      fTime;
    G4ThreeVector fMomentum;
    G4StepStatus  fStepStatus;
    G4String      fVolumeName;
};

extern G4DLLEXPORT G4Allocator<F04TrajectoryPoint> aTrajPointAllocator;

inline void* F04TrajectoryPoint::operator new(size_t)
{
    void *aTrajectoryPoint = (void *) aTrajPointAllocator.MallocSingle();
    return aTrajectoryPoint;
}

inline void F04TrajectoryPoint::operator delete(void *aTrajectoryPoint)
{
    aTrajPointAllocator.FreeSingle(
        (F04TrajectoryPoint *) aTrajectoryPoint);
}

#endif
