#include "NovaEmPhysics.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4ProcessManager.hh"


NovaEmPhysics::NovaEmPhysics(const G4String& name)
    : G4VPhysicsConstructor(name)
{
  fPhotoEffect = NULL;
  fComptonEffect = NULL;
  fPairProduction = NULL;
  fElectronMultipleScattering = NULL;
  fElectronIonisation = NULL;
  fElectronBremsstrahlung = NULL;
  fPositronMultipleScattering = NULL;
  fPositronIonisation = NULL;
  fPositronBremsstrahlung = NULL;
  fAnnihilation = NULL;
}

NovaEmPhysics::~NovaEmPhysics() {}

void NovaEmPhysics::ConstructParticle()
{
  G4Gamma::GammaDefinition();
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
}

void NovaEmPhysics::ConstructProcess()
{
  fPhotoEffect = new G4PhotoElectricEffect();
  fComptonEffect = new G4ComptonScattering();
  fPairProduction = new G4GammaConversion();

  fElectronMultipleScattering = new G4eMultipleScattering();
  fElectronIonisation = new G4eIonisation();
  fElectronBremsstrahlung = new G4eBremsstrahlung();

  fPositronMultipleScattering = new G4eMultipleScattering();
  fPositronIonisation = new G4eIonisation();
  fPositronBremsstrahlung = new G4eBremsstrahlung();
  fAnnihilation = new G4eplusAnnihilation();

  G4ProcessManager* pManager = 0;
  pManager = G4Gamma::Gamma()->GetProcessManager();
  pManager->AddDiscreteProcess(fPhotoEffect);
  pManager->AddDiscreteProcess(fComptonEffect);
  pManager->AddDiscreteProcess(fPairProduction);

  pManager = G4Electron::Electron()->GetProcessManager();
  pManager->AddProcess(fElectronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fElectronIonisation, -1, 2, 2);
  pManager->AddProcess(fElectronBremsstrahlung, -1, 3, 3);

  pManager = G4Positron::Positron()->GetProcessManager();
  pManager->AddProcess(fPositronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fPositronIonisation, -1, 2, 2);
  pManager->AddProcess(fPositronBremsstrahlung, -1, 3, 3);
  pManager->AddProcess(fAnnihilation, 0, -1, 4);
}
