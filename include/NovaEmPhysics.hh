#ifndef NovaEmPhysics_h
#define NovaEmPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

class NovaEmPhysics : public G4VPhysicsConstructor
{
  public:
    NovaEmPhysics(const G4String& name ="EM");
    virtual ~NovaEmPhysics();

  public:
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  protected:
    G4PhotoElectricEffect* fPhotoEffect;
    G4ComptonScattering* fComptonEffect;
    G4GammaConversion* fPairProduction;
 
    G4eMultipleScattering* fElectronMultipleScattering;
    G4eIonisation* fElectronIonisation;
    G4eBremsstrahlung* fElectronBremsstrahlung;

    G4eMultipleScattering* fPositronMultipleScattering;
    G4eIonisation* fPositronIonisation;
    G4eBremsstrahlung* fPositronBremsstrahlung;
    G4eplusAnnihilation* fAnnihilation;
};

#endif
