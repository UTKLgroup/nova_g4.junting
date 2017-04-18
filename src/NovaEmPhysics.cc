#include "NovaEmPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NovaEmPhysics::NovaEmPhysics(const G4String& name)
               :  G4VPhysicsConstructor(name)
{
  fPhotoEffect = NULL;
  fComptonEffect = NULL;
  fPairProduction = NULL;
  fElectronMultipleScattering = NULL;
  fElectronIonisation = NULL; 
  fElectronBremsStrahlung = NULL;
  fPositronMultipleScattering = NULL;
  fPositronIonisation = NULL; 
  fPositronBremsStrahlung = NULL;
  fAnnihilation = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NovaEmPhysics::~NovaEmPhysics() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4Gamma.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"

void NovaEmPhysics::ConstructParticle()
{
  // gamma
  G4Gamma::GammaDefinition();
 
  // electron
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ProcessManager.hh"

void NovaEmPhysics::ConstructProcess()
{
  fPhotoEffect = new G4PhotoElectricEffect();
  fComptonEffect = new G4ComptonScattering();
  fPairProduction = new G4GammaConversion();
 
    // Electron physics
  fElectronMultipleScattering = new G4eMultipleScattering();
  fElectronIonisation = new G4eIonisation();
  fElectronBremsStrahlung = new G4eBremsstrahlung();
 
    //Positron physics
  fPositronMultipleScattering = new G4eMultipleScattering();
  fPositronIonisation = new G4eIonisation();
  fPositronBremsStrahlung = new G4eBremsstrahlung();
  fAnnihilation = new G4eplusAnnihilation();

  G4ProcessManager* pManager = 0;

  // Gamma Physics
  pManager = G4Gamma::Gamma()->GetProcessManager();
  pManager->AddDiscreteProcess(fPhotoEffect);
  pManager->AddDiscreteProcess(fComptonEffect);
  pManager->AddDiscreteProcess(fPairProduction);

  // Electron Physics
  pManager = G4Electron::Electron()->GetProcessManager();

  pManager->AddProcess(fElectronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fElectronIonisation,         -1, 2, 2);
  pManager->AddProcess(fElectronBremsStrahlung,     -1, 3, 3);  

  //Positron Physics
  pManager = G4Positron::Positron()->GetProcessManager();
 
  pManager->AddProcess(fPositronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fPositronIonisation,         -1, 2, 2);
  pManager->AddProcess(fPositronBremsStrahlung,     -1, 3, 3);  
  pManager->AddProcess(fAnnihilation,                0,-1, 4);  

}
