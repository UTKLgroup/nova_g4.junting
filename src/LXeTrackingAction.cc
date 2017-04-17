#include "LXeTrajectory.hh"
#include "LXeTrackingAction.hh"
#include "LXeUserTrackInformation.hh"
#include "NovaDetectorConstruction.hh"
#include "NovaRecorderBase.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4ParticleTypes.hh"

#include "F04Trajectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeTrackingAction::LXeTrackingAction(NovaRecorderBase* r)
  : fRecorder(r) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  //Let this be up to the user via vis.mac
  //  fpTrackingManager->SetStoreTrajectory(true);

  //Use custom trajectory class
  //fpTrackingManager->SetTrajectory(new LXeTrajectory(aTrack));
  fpTrackingManager->SetTrajectory(new F04Trajectory(aTrack));

  //This user track information is only relevant to the photons
  fpTrackingManager->SetUserTrackInformation(new LXeUserTrackInformation);

  /*  const G4VProcess* creator = aTrack->GetCreatorProcess();
  if(creator)
    G4cout<<creator->GetProcessName()<<G4endl;
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeTrackingAction::PostUserTrackingAction(const G4Track* aTrack){

  F04Trajectory* trajectory=(F04Trajectory*)fpTrackingManager->GimmeTrajectory();
  //LXeTrajectory* trajectory=(LXeTrajectory*)fpTrackingManager->GimmeTrajectory();
  LXeUserTrackInformation*
    trackInformation=(LXeUserTrackInformation*)aTrack->GetUserInformation();

  trajectory->SetStatus(trackInformation->GetTrackStatus());
  trajectory->SetTrkLength(aTrack->GetTrackLength());
  if(aTrack->GetParentID()>0)
    trajectory->SetProcessName(aTrack->GetCreatorProcess()->GetProcessName());
  trajectory->SetNumRefl(trackInformation->GetReflectionCount());
  trajectory->SetNumTIRefl(trackInformation->GetTotalInternalReflCount());

  //if(trackInformation->GetTrackStatus()&hitPMT)
  //G4cout << "a hit" << G4endl;


  /*
  //Lets choose to draw only the photons that hit the sphere and a pmt
  if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()){

    const G4VProcess* creator=aTrack->GetCreatorProcess();
    if(creator && creator->GetProcessName()=="OpWLS"){
      trajectory->WLS();
      trajectory->SetDrawTrajectory(true);
    }

    if(NovaDetectorConstruction::GetSphereOn()){
      if((trackInformation->GetTrackStatus()&hitPMT)&&
         (trackInformation->GetTrackStatus()&hitSphere)){
        trajectory->SetDrawTrajectory(true);
      }
    }
    else{
      if(trackInformation->GetTrackStatus()&hitPMT)
        trajectory->SetDrawTrajectory(true);
    }
  }
  else //draw all other trajectories
    trajectory->SetDrawTrajectory(true);

  if(trackInformation->GetForceDrawTrajectory())
    trajectory->SetDrawTrajectory(true);

  */

  if(fRecorder)fRecorder->RecordTrack(aTrack);
}
