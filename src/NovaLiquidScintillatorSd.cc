#include "NovaLiquidScintillatorSd.hh"
#include "G4VProcess.hh"

NovaLiquidScintillatorSd::NovaLiquidScintillatorSd(G4String name)
    : G4VSensitiveDetector(name)
{
  fScintCollection = NULL;
  collectionName.insert("liquidScintillatorCollection");
}

NovaLiquidScintillatorSd::~NovaLiquidScintillatorSd() {}

void NovaLiquidScintillatorSd::Initialize(G4HCofThisEvent* hitsCE){
  fScintCollection = new NovaScintHitsCollection(SensitiveDetectorName, collectionName[0]);
  static G4int hitsCID = -1;
  if(hitsCID<0){
    hitsCID = GetCollectionID(0);
  }
  hitsCE->AddHitsCollection( hitsCID, fScintCollection );
}

G4bool NovaLiquidScintillatorSd::ProcessHits(G4Step* aStep,G4TouchableHistory*){
  G4double totalEnergyDeposit = aStep->GetTotalEnergyDeposit();
  if(totalEnergyDeposit==0.0)
    return false;

  G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume* thePrePV = theTouchable->GetVolume();
  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();

  G4ThreeVector position = (thePrePoint->GetPosition() + thePostPoint->GetPosition()) / 2.0;
  NovaLiquidScintillatorHit* liquidScintillatorHit = new NovaLiquidScintillatorHit(thePrePV);
  liquidScintillatorHit->SetEdep(totalEnergyDeposit);
  liquidScintillatorHit->SetPos(position);

  fScintCollection->insert(liquidScintillatorHit);

  return true;
}

void NovaLiquidScintillatorSd::EndOfEvent(G4HCofThisEvent* ) {}

void NovaLiquidScintillatorSd::clear() {}

void NovaLiquidScintillatorSd::DrawAll() {}

void NovaLiquidScintillatorSd::PrintAll() {}
