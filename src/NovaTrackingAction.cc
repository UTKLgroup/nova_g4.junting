#include "LXeTrajectory.hh"
#include "NovaTrackingAction.hh"
#include "LXeUserTrackInformation.hh"
#include "NovaRecorderBase.hh"
#include "G4TrackingManager.hh"
#include "NovaTrajectory.hh"


NovaTrackingAction::NovaTrackingAction(NovaRecorderBase* r)
  : fRecorder(r) {}

void NovaTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new NovaTrajectory(aTrack));
  fpTrackingManager->SetUserTrackInformation(new LXeUserTrackInformation);
}

void NovaTrackingAction::PostUserTrackingAction(const G4Track* aTrack){

  NovaTrajectory* trajectory=(NovaTrajectory*)fpTrackingManager->GimmeTrajectory();
  LXeUserTrackInformation* trackInformation =(LXeUserTrackInformation*) aTrack->GetUserInformation();

  trajectory->SetStatus(trackInformation->GetTrackStatus());
  trajectory->SetTrkLength(aTrack->GetTrackLength());
  if(aTrack->GetParentID()>0)
    trajectory->SetProcessName(aTrack->GetCreatorProcess()->GetProcessName());
  trajectory->SetNumRefl(trackInformation->GetReflectionCount());
  trajectory->SetNumTIRefl(trackInformation->GetTotalInternalReflCount());

  if(fRecorder)
    fRecorder->RecordTrack(aTrack);
}
