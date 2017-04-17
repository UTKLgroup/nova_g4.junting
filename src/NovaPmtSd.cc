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
  fPMTHitCollection = new NovaPmtHitsCollection
                      (SensitiveDetectorName,collectionName[0]);
  //Store collection with event and keep ID
  static G4int hitCID = -1;
  if(hitCID<0){
    hitCID = GetCollectionID(0);
  }
  hitsCE->AddHitsCollection( hitCID, fPMTHitCollection );
}

G4bool NovaPmtSd::ProcessHits(G4Step* ,G4TouchableHistory* ){
  return false;
}

//Generates a hit and uses the postStepPoint's mother volume replica number
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photocathode

G4bool NovaPmtSd::ProcessHits_constStep(const G4Step* aStep,
                                       G4TouchableHistory* ){

  //need to know if this is an optical photon
  if(aStep->GetTrack()->GetDefinition()
     != G4OpticalPhoton::OpticalPhotonDefinition()) return false;
 
  //User replica number 1 since photocathode is a daughter volume
  //to the pmt which was replicated
  G4int pmtNumber=
    aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
  G4VPhysicalVolume* physVol=
    aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

  //Find the correct hit collection
  G4int n=fPMTHitCollection->entries();
  NovaPmtHit* hit=NULL;
  for(G4int i=0;i<n;i++){
    if((*fPMTHitCollection)[i]->GetPMTNumber()==pmtNumber){
      hit=(*fPMTHitCollection)[i];
      break;
    }
  }
 
  if(hit==NULL){//this pmt wasnt previously hit in this event
    hit = new NovaPmtHit(); //so create new hit
    hit->SetPMTNumber(pmtNumber);
    hit->SetPMTPhysVol(physVol);
    fPMTHitCollection->insert(hit);
    hit->SetPMTPos((*fPMTPositionsX)[pmtNumber],(*fPMTPositionsY)[pmtNumber],
                   (*fPMTPositionsZ)[pmtNumber]);
  }

  hit->IncPhotonCount(); //increment hit for the selected pmt

  /*
  if(!NovaDetectorConstruction::GetSphereOn()){
    hit->SetDrawit(true);
    //If the sphere is disabled then this hit is automaticaly drawn
  }
  else{//sphere enabled
    LXeUserTrackInformation* trackInfo=
      (LXeUserTrackInformation*)aStep->GetTrack()->GetUserInformation();
    if(trackInfo->GetTrackStatus()&hitSphere)
      //only draw this hit if the photon has hit the sphere first
      hit->SetDrawit(true);
  }
  */
  return true;
}

void NovaPmtSd::EndOfEvent(G4HCofThisEvent* ) {}

void NovaPmtSd::clear() {}

void NovaPmtSd::DrawAll() {}

void NovaPmtSd::PrintAll() {}
