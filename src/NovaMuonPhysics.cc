#include "NovaMuonPhysics.hh"
#include "G4ProcessManager.hh"


NovaMuonPhysics::NovaMuonPhysics(const G4String& name)
    : G4VPhysicsConstructor(name)
{
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

NovaMuonPhysics::~NovaMuonPhysics() {}

void NovaMuonPhysics::ConstructParticle()
{
  G4Deuteron::DeuteronDefinition();
  G4Triton::TritonDefinition();
  G4He3::He3Definition();
  G4Alpha::AlphaDefinition();
  G4GenericIon::GenericIonDefinition();
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

  G4PionMinus::PionMinus();
  G4PionPlus::PionPlus();
  G4PionZero::PionZero();

  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

void NovaMuonPhysics::ConstructProcess()
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
  pManager = G4MuonPlus::MuonPlus()->GetProcessManager();
  pManager->AddProcess(fMuPlusMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fMuPlusIonisation, -1, 2, 2);
  pManager->AddProcess(fMuPlusBremsstrahlung, -1, 3, 3);
  pManager->AddProcess(fMuPlusPairProduction, -1, 4, 4);

  pManager = G4MuonMinus::MuonMinus()->GetProcessManager();
  pManager->AddProcess(fMuMinusMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fMuMinusIonisation, -1,  2, 2);
  pManager->AddProcess(fMuMinusBremsstrahlung, -1, 3, 3);
  pManager->AddProcess(fMuMinusPairProduction, -1, 4, 4);

  pManager->AddRestProcess(fMuMinusCaptureAtRest);
}
