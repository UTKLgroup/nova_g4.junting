#ifndef NovaMuonPhysics_h
#define NovaMuonPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4MuMultipleScattering.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4MuIonisation.hh"
#include "G4hIonisation.hh"
#include "G4MuonMinusCapture.hh"


class NovaMuonPhysics : public G4VPhysicsConstructor
{
  public:
    NovaMuonPhysics(const G4String& name="muon");
    virtual ~NovaMuonPhysics();
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  protected:
   G4MuIonisation* fMuPlusIonisation;
   G4MuMultipleScattering* fMuPlusMultipleScattering;
   G4MuBremsstrahlung* fMuPlusBremsstrahlung;
   G4MuPairProduction* fMuPlusPairProduction;

   G4MuIonisation* fMuMinusIonisation;
   G4MuMultipleScattering* fMuMinusMultipleScattering;
   G4MuBremsstrahlung* fMuMinusBremsstrahlung;
   G4MuPairProduction* fMuMinusPairProduction;
    G4MuonMinusCapture* fMuMinusCaptureAtRest;
};

#endif
