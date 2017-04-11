#include "LXeScintSD.hh"
#include "LXeScintHit.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeScintSD::LXeScintSD(G4String name)
  : G4VSensitiveDetector(name)
{
  fScintCollection = NULL;
  collectionName.insert("scintCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeScintSD::~LXeScintSD() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeScintSD::Initialize(G4HCofThisEvent* hitsCE){
  fScintCollection = new LXeScintHitsCollection
                      (SensitiveDetectorName,collectionName[0]);
  //A way to keep all the hits of this event in one place if needed
  static G4int hitsCID = -1;
  if(hitsCID<0){
    hitsCID = GetCollectionID(0);
  }
  hitsCE->AddHitsCollection( hitsCID, fScintCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool LXeScintSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ){
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false; //No edep so dont count as hit

  G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
  G4TouchableHistory* theTouchable =
    (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume* thePrePV = theTouchable->GetVolume();
 
  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();

  //Get the average position of the hit
  G4ThreeVector pos = thePrePoint->GetPosition() + thePostPoint->GetPosition();
  pos/=2.;

  //*********************************************************************************

  /*
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  G4String preMaterial = preStepPoint->GetMaterial()->GetName();
  G4String postMaterial = postStepPoint->GetMaterial()->GetName();
  G4String preVolume = preStepPoint->GetPhysicalVolume()->GetName();
  G4String postVolume = postStepPoint->GetPhysicalVolume()->GetName();
  G4int parentID = aStep->GetTrack()->GetParentID();
  G4String parentName = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4double globalTime = preStepPoint->GetGlobalTime();
  */

  /*
  if(parentID == 0){
  G4cout << postVolume << "  " << preMaterial << G4endl;
  if(preVolume != "veto")
  G4cout << "Entering " << G4endl;
  if(postVolume != "veto")
  G4cout << "Exiting " << G4endl;
  }
  */
    
  /*
  if(preMaterial == "Al" && postMaterial == "Vacuum" && parentID == 0){
    std::ofstream fhit;
    fhit.open("fcosmic.txt", std::ios::app);
    fhit << "hit" << " "
	 << 1 << " "
	 << std::setprecision(4)
	 << 1. << " "
	 << 1. << " "
	 << 1. << " "
	 << 1. << " "
	 << 1. << " "
	 << 1. << " "
	 << 1. << " "
	 << " " << G4endl;
    fhit.close();
  }
  */

  /*
  G4cout << preStepPoint->GetMaterial()->GetName() << " ";
  G4cout << postStepPoint->GetMaterial()->GetName() << " ";
  G4cout << aStep->GetTrack()->GetParentID() << " ";
  G4cout << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << " ";
  G4cout << preStepPoint->GetGlobalTime() << " ";
  G4cout << G4endl;
  */
  //*********************************************************************************

  LXeScintHit* scintHit = new LXeScintHit(thePrePV);

  scintHit->SetEdep(edep);
  scintHit->SetPos(pos);

  fScintCollection->insert(scintHit);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeScintSD::EndOfEvent(G4HCofThisEvent* ) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeScintSD::clear() {} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeScintSD::DrawAll() {} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeScintSD::PrintAll() {} 
