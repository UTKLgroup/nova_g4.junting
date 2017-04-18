#ifndef NovaGeneralPhysics_h
#define NovaGeneralPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4Decay.hh"


class NovaGeneralPhysics : public G4VPhysicsConstructor
{
  public:
    NovaGeneralPhysics(const G4String& name = "general");
    virtual ~NovaGeneralPhysics();
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  protected:
    G4Decay* fDecayProcess;
};

#endif
