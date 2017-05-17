#include "NovaPhysicsList.hh"
#include "NovaGeneralPhysics.hh"
#include "NovaEmPhysics.hh"
#include "NovaMuonPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4SystemOfUnits.hh"


NovaPhysicsList::NovaPhysicsList() : G4VModularPhysicsList()
{
  // default cut value  (1.0mm)
  defaultCutValue = 1.0*mm;

  // General Physics
  RegisterPhysics(new NovaGeneralPhysics("general"));

  // EM Physics
  RegisterPhysics(new NovaEmPhysics("standard EM"));

  // Muon Physics
  RegisterPhysics(new NovaMuonPhysics("muon"));

  // Optical Physics
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  RegisterPhysics(opticalPhysics);

  opticalPhysics->SetWLSTimeProfile("exponential");

  opticalPhysics->SetScintillationYieldFactor(1.0);
  opticalPhysics->SetScintillationExcitationRatio(0.0);

  opticalPhysics->SetMaxNumPhotonsPerStep(100);
  opticalPhysics->SetMaxBetaChangePerStep(10.0);

  opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);
}

NovaPhysicsList::~NovaPhysicsList() {}

void NovaPhysicsList::SetCuts(){
  SetCutsWithDefault();
}
