#include "NovaTrackingAction.hh"
#include "NovaUserTrackInformation.hh"
#include "NovaRecorderBase.hh"
#include "G4TrackingManager.hh"
#include "NovaTrajectory.hh"


NovaTrackingAction::NovaTrackingAction(NovaRecorderBase* r)
  : fRecorder(r) {}

void NovaTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new NovaTrajectory(aTrack));
  fpTrackingManager->SetUserTrackInformation(new NovaUserTrackInformation);
}

void NovaTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  NovaTrajectory* trajectory=(NovaTrajectory*)fpTrackingManager->GimmeTrajectory();
  NovaUserTrackInformation* trackInformation =(NovaUserTrackInformation*) aTrack->GetUserInformation();

  trajectory->SetStatus(trackInformation->getTrackStatus());
  trajectory->setTrackLength(aTrack->GetTrackLength());
  if(aTrack->GetParentID()>0)
    trajectory->SetProcessName(aTrack->GetCreatorProcess()->GetProcessName());
  trajectory->setReflectionCount(trackInformation->getReflectionCount());
  trajectory->setTotalInternalReflectionCount(trackInformation->getTotalInternalReflectionCount());

  if(fRecorder)
    fRecorder->RecordTrack(aTrack);
}
