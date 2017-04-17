#include "NovaStackingAction.hh"
#include "NovaUserEventInformation.hh"
#include "NovaSteppingAction.hh"

#include "G4RunManager.hh"

NovaStackingAction::NovaStackingAction() {}

NovaStackingAction::~NovaStackingAction() {}

G4ClassificationOfNewTrack NovaStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  NovaUserEventInformation* eventInformation =
      (NovaUserEventInformation*) G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation();
 
  if (aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
    if(aTrack->GetParentID()>0){
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
        eventInformation->incrementPhotonCountScintillation();
      else if (aTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov") {
        eventInformation->incrementPhotonCountCherenkov();
      }
    }
  }
  return fUrgent;
}

void NovaStackingAction::NewStage() {}

void NovaStackingAction::PrepareNewEvent() {}
