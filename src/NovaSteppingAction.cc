#include "NovaSteppingAction.hh"
#include "NovaEventAction.hh"
#include "NovaTrackingAction.hh"
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

void NovaSteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track* theTrack = theStep->GetTrack();

  NovaUserTrackInformation* trackInformation = (NovaUserTrackInformation*) theTrack->GetUserInformation();
  NovaUserEventInformation* eventInformation
      = (NovaUserEventInformation*)G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation();

  G4StepPoint* preStepPoint = theStep->GetPreStepPoint();
  G4VPhysicalVolume* prePhysicalVolume = preStepPoint->GetPhysicalVolume();
  G4StepPoint* postStepPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* postPhysicalVolume = postStepPoint->GetPhysicalVolume();

  static G4OpBoundaryProcess* boundary = NULL;

  if (!boundary) {
    G4ProcessManager* processManager = theStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int processCount = processManager->GetProcessListLength();
    G4ProcessVector* processes = processManager->GetProcessList();
    for (G4int i = 0; i < processCount; i++) {
      if ((*processes)[i]->GetProcessName() == "OpBoundary") {
        boundary = (G4OpBoundaryProcess*)(*processes)[i];
        break;
      }
    }
  }

  if (!postPhysicalVolume)
    return;

  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  if (particleType == G4OpticalPhoton::OpticalPhotonDefinition()) {
    if (postStepPoint->GetProcessDefinedStep()->GetProcessName() == "OpAbsorption") {
      trackInformation->addTrackStatusFlag(absorbed);
      eventInformation->incrementAbsorption();
    }

    G4OpBoundaryProcessStatus boundaryStatus = boundary->GetStatus();
    if (postStepPoint->GetStepStatus() == fGeomBoundary) {
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
          if(prePhysicalVolume->GetName() == "fiberCore")
            trackInformation->incrementTotalInternalReflectionCount();
          break;
        case LambertianReflection:
        case LobeReflection:
        case SpikeReflection:
        case BackScattering:
          trackInformation->incrementReflectionCount();
          fExpectedNextStatus = StepTooSmall;
          break;
        default:
          break;
      }
    }
  }

  if(fRecorder)
    fRecorder->RecordStep(theStep);
}
