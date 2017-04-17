#ifndef NovaPmtSd_h
#define NovaPmtSd_h 1

#include "G4DataVector.hh"
#include "G4VSensitiveDetector.hh"
#include "NovaPmtHit.hh"

class G4Step;
class G4HCofThisEvent;

class NovaPmtSd : public G4VSensitiveDetector
{

  public:
    NovaPmtSd(G4String name);
    virtual ~NovaPmtSd();

    virtual void Initialize(G4HCofThisEvent*);
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);

    G4bool ProcessHits_constStep(const G4Step*, G4TouchableHistory*);
    virtual void EndOfEvent(G4HCofThisEvent*);
    virtual void clear();
    void DrawAll();
    void PrintAll();

    inline void InitPMTs(G4int nPMTs){
      if(fPMTPositionsX)delete fPMTPositionsX;
      if(fPMTPositionsY)delete fPMTPositionsY;
      if(fPMTPositionsZ)delete fPMTPositionsZ;
      fPMTPositionsX=new G4DataVector(nPMTs);
      fPMTPositionsY=new G4DataVector(nPMTs);
      fPMTPositionsZ=new G4DataVector(nPMTs);
    }

    inline void SetPMTPos(G4int n,G4double x,G4double y,G4double z){
      if(fPMTPositionsX)fPMTPositionsX->insertAt(n,x);
      if(fPMTPositionsY)fPMTPositionsY->insertAt(n,y);
      if(fPMTPositionsZ)fPMTPositionsZ->insertAt(n,z);
    }

  private:
    NovaPmtHitsCollection* fPMTHitCollection;
    G4DataVector* fPMTPositionsX;
    G4DataVector* fPMTPositionsY;
    G4DataVector* fPMTPositionsZ;
};

#endif
