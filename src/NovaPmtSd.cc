#include "NovaPmtSd.hh"
#include "NovaDetectorConstruction.hh"

#include "G4ParticleTypes.hh"


NovaPmtSd::NovaPmtSd(G4String name)
  : G4VSensitiveDetector(name),
    pmtHitsCollection(0),
    pmtPositionXs(0),
    pmtPositionYs(0),
    pmtPositionZs(0)
{
  collectionName.insert("pmtHitCollection");
}

NovaPmtSd::~NovaPmtSd() {}

void NovaPmtSd::Initialize(G4HCofThisEvent* hitsCE)
{
  pmtHitsCollection = new NovaPmtHitsCollection(SensitiveDetectorName,collectionName[0]);
  static G4int hitCID = -1;
  if(hitCID<0){
    hitCID = GetCollectionID(0);
  }
  hitsCE->AddHitsCollection( hitCID, pmtHitsCollection );
}

G4bool NovaPmtSd::ProcessHits(G4Step* ,G4TouchableHistory* )
{
  return false;
}

G4bool NovaPmtSd::ProcessHits_constStep(const G4Step* aStep, G4TouchableHistory*)
{
  if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;

  NovaPmtHit* hit = NULL;
  G4int pmtNumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
  G4VPhysicalVolume* physicalVolume = aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

  G4int n = pmtHitsCollection->entries();
  for (G4int i = 0; i < n; i++) {
    if ((*pmtHitsCollection)[i]->getPmtNumber() == pmtNumber) {
      hit = (*pmtHitsCollection)[i];
      break;
    }
  }
 
  if (hit == NULL) {
    hit = new NovaPmtHit();
    hit->SetPmtNumber(pmtNumber);
    hit->setPmtPhysicalVolume(physicalVolume);
    pmtHitsCollection->insert(hit);
    hit->setPmtPosition((*pmtPositionXs)[pmtNumber], (*pmtPositionYs)[pmtNumber], (*pmtPositionZs)[pmtNumber]);
  }

  hit->incrementPhotonCount();
  return true;
}

void NovaPmtSd::EndOfEvent(G4HCofThisEvent*) {}

void NovaPmtSd::clear() {}

void NovaPmtSd::DrawAll() {}

void NovaPmtSd::PrintAll() {}
