#include "NovaPmtSd.hh"
#include "NovaDetectorConstruction.hh"

#include "G4ParticleTypes.hh"


NovaPmtSd::NovaPmtSd(G4String name)
  : G4VSensitiveDetector(name),
    fPMTHitCollection(0),
    fPMTPositionsX(0),
    fPMTPositionsY(0),
    fPMTPositionsZ(0)
{
  collectionName.insert("pmtHitCollection");
}

NovaPmtSd::~NovaPmtSd() {}

void NovaPmtSd::Initialize(G4HCofThisEvent* hitsCE)
{
  fPMTHitCollection = new NovaPmtHitsCollection(SensitiveDetectorName,collectionName[0]);
  static G4int hitCID = -1;
  if(hitCID<0){
    hitCID = GetCollectionID(0);
  }
  hitsCE->AddHitsCollection( hitCID, fPMTHitCollection );
}

G4bool NovaPmtSd::ProcessHits(G4Step* ,G4TouchableHistory* )
{
  return false;
}

G4bool NovaPmtSd::ProcessHits_constStep(const G4Step* aStep, G4TouchableHistory*)
{
  if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;

  G4int pmtNumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
  G4VPhysicalVolume* physVol = aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

  G4int n=fPMTHitCollection->entries();
  NovaPmtHit* hit=NULL;
  for (G4int i=0;i<n;i++) {
    if ((*fPMTHitCollection)[i]->GetPMTNumber()==pmtNumber) {
      hit = (*fPMTHitCollection)[i];
      break;
    }
  }
 
  if (hit == NULL) {
    hit = new NovaPmtHit();
    hit->SetPMTNumber(pmtNumber);
    hit->SetPMTPhysVol(physVol);
    fPMTHitCollection->insert(hit);
    hit->SetPMTPos((*fPMTPositionsX)[pmtNumber], (*fPMTPositionsY)[pmtNumber], (*fPMTPositionsZ)[pmtNumber]);
  }

  hit->IncPhotonCount();
  return true;
}

void NovaPmtSd::EndOfEvent(G4HCofThisEvent* ) {}

void NovaPmtSd::clear() {}

void NovaPmtSd::DrawAll() {}

void NovaPmtSd::PrintAll() {}
