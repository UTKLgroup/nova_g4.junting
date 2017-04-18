#ifndef NovaUserTrackInformation_h
#define NovaUserTrackInformation_h 1

#include "G4VUserTrackInformation.hh"
#include "globals.hh"


enum LXeTrackStatus
{
  active=1,
  hitPmt=2,
  absorbed=4,
  boundaryAbsorbed=8,
  hitSphere=16,
  inactive=14
};

class NovaUserTrackInformation : public G4VUserTrackInformation
{
  public:
    NovaUserTrackInformation();
    virtual ~NovaUserTrackInformation();
    void setTrackStatusFlag(int s){trackStatus=s;}
    void addTrackStatusFlag(int s);
    int getTrackStatus()const {return trackStatus;}
    void incrementReflectionCount(){reflectionCount++;}
    G4int getReflectionCount()const {return reflectionCount;}
    void incrementTotalInternalReflectionCount(){totalInternalReflectionCount++;}
    G4int getTotalInternalReflectionCount()const {return totalInternalReflectionCount;}
    void setForceDrawTrajectory(G4bool b){forceDraw=b;}
    G4bool getForceDrawTrajectory(){return forceDraw;}
    inline virtual void Print() const{};

  private:
    int trackStatus;
    G4int reflectionCount;
    G4int totalInternalReflectionCount;
    G4bool forceDraw;
};

#endif
