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

    inline void initPmts(G4int nPmt)
    {
      if(pmtPositionXs)
        delete pmtPositionXs;
      if(pmtPositionYs)
        delete pmtPositionYs;
      if(pmtPositionZs)
        delete pmtPositionZs;
      pmtPositionXs = new G4DataVector((size_t) nPmt);
      pmtPositionYs = new G4DataVector((size_t) nPmt);
      pmtPositionZs = new G4DataVector((size_t) nPmt);
    }

    inline void setPmtPosition(G4int n, G4double x, G4double y, G4double z)
    {
      if(pmtPositionXs)
        pmtPositionXs->insertAt((size_t) n, x);
      if(pmtPositionYs)
        pmtPositionYs->insertAt((size_t) n, y);
      if(pmtPositionZs)
        pmtPositionZs->insertAt((size_t) n, z);
    }

  private:
    NovaPmtHitsCollection* pmtHitsCollection;
    G4DataVector* pmtPositionXs;
    G4DataVector* pmtPositionYs;
    G4DataVector* pmtPositionZs;
};

#endif
