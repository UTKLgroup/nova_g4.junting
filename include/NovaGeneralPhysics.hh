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

    // This method will be invoked in the Construct() method.
    // each particle type will be instantiated
    virtual void ConstructParticle();
 
    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type
    virtual void ConstructProcess();

  protected:

    G4Decay* fDecayProcess;
};

#endif
