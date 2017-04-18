#include "NovaSteppingAction.hh"
#include "NovaEventAction.hh"
#include "NovaTrackingAction.hh"
#include "LXeTrajectory.hh"
#include "NovaPmtSd.hh"
#include "NovaUserTrackInformation.hh"
#include "NovaUserEventInformation.hh"
#include "LXeSteppingMessenger.hh"
#include "NovaRecorderBase.hh"

#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"

NovaSteppingAction::NovaSteppingAction(NovaRecorderBase* r)
    : fRecorder(r),fOneStepPrimaries(false)
{
  fSteppingMessenger = new LXeSteppingMessenger(this);
  fExpectedNextStatus = Undefined;
}

NovaSteppingAction::~NovaSteppingAction() {}

void NovaSteppingAction::UserSteppingAction(const G4Step* theStep){
  G4Track* theTrack = theStep->GetTrack();

  NovaUserTrackInformation* trackInformation = (NovaUserTrackInformation*) theTrack->GetUserInformation();
  NovaUserEventInformation* eventInformation
      = (NovaUserEventInformation*)G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation();

  G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();

  G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4OpBoundaryProcess* boundary = NULL;

  if (!boundary) {
    G4ProcessManager* processManager = theStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int processCount = processManager->GetProcessListLength();
    G4ProcessVector* processes = processManager->GetProcessList();
    for (G4int i = 0; i < processCount; i++) {
      if((*processes)[i]->GetProcessName() == "OpBoundary"){
        boundary = (G4OpBoundaryProcess*)(*processes)[i];
        break;
      }
    }
  }

  if (theTrack->GetParentID() == 0) {
    G4TrackVector* fSecondary = fpSteppingManager->GetfSecondary();
    G4int fN2ndariesSum = fpSteppingManager->GetfN2ndariesAtRestDoIt()
                          + fpSteppingManager->GetfN2ndariesAlongStepDoIt()
                          + fpSteppingManager->GetfN2ndariesPostStepDoIt();

    if (!eventInformation->isConversionPositionSet() && fN2ndariesSum > 0) {
      for(size_t lp1=(*fSecondary).size()-fN2ndariesSum; lp1<(*fSecondary).size(); lp1++){
        const G4VProcess* creator=(*fSecondary)[lp1]->GetCreatorProcess();
        if (creator) {
          G4String creatorName=creator->GetProcessName();
          if(creatorName == "phot"|| creatorName == "compt" || creatorName == "conv"){
            eventInformation->setConversionPosition((*fSecondary)[lp1]->GetPosition());
          }
        }
      }
    }

    if(fOneStepPrimaries && thePrePV->GetName() == "scintillator")
      theTrack->SetTrackStatus(fStopAndKill);
  }

  if (!thePostPV)
    return;

  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  if (particleType == G4OpticalPhoton::OpticalPhotonDefinition()) {
    if (thePostPoint->GetProcessDefinedStep()->GetProcessName() == "OpAbsorption") {
      eventInformation->incrementAbsorption();
      trackInformation->addTrackStatusFlag(absorbed);
    }

    boundaryStatus = boundary->GetStatus();
    if (thePostPoint->GetStepStatus() == fGeomBoundary) {
      fExpectedNextStatus=Undefined;
      switch(boundaryStatus){
        case Absorption:
          trackInformation->addTrackStatusFlag(boundaryAbsorbed);
          eventInformation->incrementBoundaryAbsorption();
          break;
        case Detection:
          G4SDManager* sdManager = G4SDManager::GetSDMpointer();
          NovaPmtSd* pmtSd = (NovaPmtSd*)sdManager->FindSensitiveDetector("/NovaDet/pmtSd");
          if (pmtSd)
            pmtSd->ProcessHits_constStep(theStep, NULL);
          trackInformation->addTrackStatusFlag(hitPmt);
          break;
        case FresnelReflection:
        case TotalInternalReflection:
          if(thePrePV->GetName() == "fibcore")
            trackInformation->incrementTotalInternalReflectionCount();
          break;
        case LambertianReflection:
        case LobeReflection:
        case SpikeReflection:
        case BackScattering:
          trackInformation->incrementReflectionCount();
          fExpectedNextStatus=StepTooSmall;
          break;
        default:
          break;
      }
      if(thePostPV->GetName()=="sphere")
        trackInformation->addTrackStatusFlag(hitSphere);
    }
  }

  if(fRecorder)fRecorder->RecordStep(theStep);
}
