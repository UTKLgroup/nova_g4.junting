#ifndef NovaUserTrackInformation_h
#define NovaUserTrackInformation_h 1

#include "G4VUserTrackInformation.hh"
#include "globals.hh"


enum NovaTrackStatus
{
  hitPmt=1,
  absorbed=2,
  boundaryAbsorbed=4,
};

class NovaUserTrackInformation : public G4VUserTrackInformation
{
  public:
    NovaUserTrackInformation();
    virtual ~NovaUserTrackInformation();
    void setTrackStatusFlag(int s){trackStatus=s;}
    void incrementReflectionCount(){reflectionCount++;}
    void incrementTotalInternalReflectionCount(){totalInternalReflectionCount++;}
    int getTrackStatus()const {return trackStatus;}
    G4int getReflectionCount()const {return reflectionCount;}
    G4int getTotalInternalReflectionCount()const {return totalInternalReflectionCount;}

  private:
    int trackStatus;
    G4int reflectionCount;
    G4int totalInternalReflectionCount;
    G4bool forceDraw;
};

#endif
