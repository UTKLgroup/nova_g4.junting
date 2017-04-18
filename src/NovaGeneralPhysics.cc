#include "NovaGeneralPhysics.hh"
#include "G4ProcessManager.hh"
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"


NovaGeneralPhysics::NovaGeneralPhysics(const G4String& name)
    : G4VPhysicsConstructor(name) {}

NovaGeneralPhysics::~NovaGeneralPhysics() {
  fDecayProcess = NULL;
}

void NovaGeneralPhysics::ConstructParticle()
{
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();
}

void NovaGeneralPhysics::ConstructProcess()
{
  fDecayProcess = new G4Decay();
  aParticleIterator->reset();

  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (fDecayProcess->IsApplicable(*particle)) {
      pmanager ->AddProcess(fDecayProcess);
      pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);
    }
  }
}
