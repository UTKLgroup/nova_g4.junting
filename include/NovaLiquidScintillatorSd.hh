#ifndef LXeScintSD_h
#define LXeScintSD_h 1

#include "NovaLiquidScintillatorHit.hh"

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

class NovaLiquidScintillatorSd : public G4VSensitiveDetector
{
  public:

    NovaLiquidScintillatorSd(G4String name);
    virtual ~NovaLiquidScintillatorSd();

    virtual void Initialize(G4HCofThisEvent* );
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );
    virtual void EndOfEvent(G4HCofThisEvent* );
    virtual void clear();
    virtual void DrawAll();
    virtual void PrintAll();
 
  private:

    NovaScintHitsCollection* fScintCollection;
 
};

#endif
