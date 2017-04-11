#include "LXeMuonPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include "G4PhysicalConstants.hh"

#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeMuonPhysics::LXeMuonPhysics(const G4String& name)
                   :  G4VPhysicsConstructor(name) {
  fMuPlusIonisation = NULL;
  fMuPlusMultipleScattering = NULL;
  fMuPlusBremsstrahlung = NULL;
  fMuPlusPairProduction = NULL;

  fMuMinusIonisation = NULL;
  fMuMinusMultipleScattering = NULL;
  fMuMinusBremsstrahlung = NULL;
  fMuMinusPairProduction = NULL;

  fMuMinusCaptureAtRest = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeMuonPhysics::~LXeMuonPhysics() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"

//without the following header, muon runs report errors
#include "G4ParticleTypes.hh"

void LXeMuonPhysics::ConstructParticle()
{
  //without the following particle definition, muon runs reports errors
  G4Deuteron::DeuteronDefinition();
  G4Triton::TritonDefinition();
  G4He3::He3Definition();
  G4Alpha::AlphaDefinition();
  G4GenericIon::GenericIonDefinition();
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

  //for CRY
  G4PionMinus::PionMinus();
  G4PionPlus::PionPlus();
  G4PionZero::PionZero();

  // Mu
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ProcessManager.hh"

void LXeMuonPhysics::ConstructProcess()
{
  fMuPlusIonisation = new G4MuIonisation();
  fMuPlusMultipleScattering = new G4MuMultipleScattering();
  fMuPlusBremsstrahlung=new G4MuBremsstrahlung();
  fMuPlusPairProduction= new G4MuPairProduction();

  fMuMinusIonisation = new G4MuIonisation();
  fMuMinusMultipleScattering = new G4MuMultipleScattering;
  fMuMinusBremsstrahlung = new G4MuBremsstrahlung();
  fMuMinusPairProduction = new G4MuPairProduction();

  fMuMinusCaptureAtRest = new G4MuonMinusCapture();

  G4ProcessManager * pManager = 0;

  // Muon Plus Physics
  pManager = G4MuonPlus::MuonPlus()->GetProcessManager();

  pManager->AddProcess(fMuPlusMultipleScattering,-1,  1, 1);
  pManager->AddProcess(fMuPlusIonisation,        -1,  2, 2);
  pManager->AddProcess(fMuPlusBremsstrahlung,    -1,  3, 3);
  pManager->AddProcess(fMuPlusPairProduction,    -1,  4, 4);

  // Muon Minus Physics
  pManager = G4MuonMinus::MuonMinus()->GetProcessManager();

  pManager->AddProcess(fMuMinusMultipleScattering,-1,  1, 1);
  pManager->AddProcess(fMuMinusIonisation,        -1,  2, 2);
  pManager->AddProcess(fMuMinusBremsstrahlung,    -1,  3, 3);
  pManager->AddProcess(fMuMinusPairProduction,    -1,  4, 4);

  pManager->AddRestProcess(fMuMinusCaptureAtRest);

}
