#include "LXeStackingAction.hh"
#include "LXeUserEventInformation.hh"
#include "NovaSteppingAction.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeStackingAction::LXeStackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeStackingAction::~LXeStackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
LXeStackingAction::ClassifyNewTrack(const G4Track * aTrack){
 
  LXeUserEventInformation* eventInformation=
    (LXeUserEventInformation*)G4EventManager::GetEventManager()
    ->GetConstCurrentEvent()->GetUserInformation();
 
  //Count what process generated the optical photons
  if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()){
    // particle is optical photon
    if(aTrack->GetParentID()>0){
      // particle is secondary
      if(aTrack->GetCreatorProcess()->GetProcessName()=="Scintillation")
        eventInformation->IncPhotonCount_Scint();
      else if(aTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov"){
	//std::cout << aTrack->GetVolume()->GetName() << std::endl;
	//if(aTrack->GetVolume()->GetName() == "scintillator")
	eventInformation->IncPhotonCount_Ceren();
	//else if(aTrack->GetVolume()->GetName() == "veto")
	//eventInformation->IncVetoPhotonCount_Ceren();
      }
    }
  }
  else{
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeStackingAction::NewStage() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeStackingAction::PrepareNewEvent() {}
