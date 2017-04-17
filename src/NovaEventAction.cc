#include "NovaEventAction.hh"
#include "LXeScintHit.hh"
#include "NovaPmtHit.hh"
#include "NovaUserEventInformation.hh"
#include "LXeTrajectory.hh"
#include "NovaRecorderBase.hh"

#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4VVisManager.hh"
#include "G4SystemOfUnits.hh"

#include "NovaRunAction.hh"

#include "NovaTrajectory.hh"
#include "NovaTrajectoryPoint.hh"


NovaEventAction::NovaEventAction(NovaRecorderBase* r)
    : recorder(r),saveThreshold(0),scintCollectionId(-1),pmtCollectionId(-1),verbose(0), pmtThreshold(1),
      forceDrawPhotons(false),forceDrawNoPhotons(false)
{
  eventMessenger = new NovaEventMessenger(this);
}

NovaEventAction::~NovaEventAction(){}

void NovaEventAction::BeginOfEventAction(const G4Event* anEvent)
{
  G4EventManager::GetEventManager()->SetUserInformation(new NovaUserEventInformation);

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  if (scintCollectionId < 0)
    scintCollectionId = sdManager->GetCollectionID("scintCollection");
  if (pmtCollectionId < 0)
    pmtCollectionId = sdManager->GetCollectionID("pmtHitCollection");
  if (recorder)
    recorder->RecordBeginOfEvent(anEvent);
}

void NovaEventAction::EndOfEventAction(const G4Event* anEvent)
{
  NovaUserEventInformation* eventInformation = (NovaUserEventInformation*) anEvent->GetUserInformation();
  G4TrajectoryContainer* trajectoryContainer = anEvent->GetTrajectoryContainer();
  G4int nTrajectories = 0;
  if (trajectoryContainer)
    nTrajectories = trajectoryContainer->entries();

  NovaScintHitsCollection* scintHitsCollection = 0;
  NovaPmtHitsCollection* pmtHitsCollection = 0;
  G4HCofThisEvent* hcOfThisEvent = anEvent->GetHCofThisEvent();

  if (hcOfThisEvent) {
    if (scintCollectionId >= 0)
      scintHitsCollection = (NovaScintHitsCollection*)(hcOfThisEvent->GetHC(scintCollectionId));
    if (pmtCollectionId >= 0)
      pmtHitsCollection = (NovaPmtHitsCollection*)(hcOfThisEvent->GetHC(pmtCollectionId));
  }

  if (scintHitsCollection) {
    int nHit = scintHitsCollection->entries();
    G4ThreeVector energyWeightedPosition(0.0);
    G4double energyDeposition;
    G4double energyDepositionMax = 0;

    for(int i = 0; i < nHit; i++) {
      energyDeposition = (*scintHitsCollection)[i]->GetEdep();
      eventInformation->addEnergyDeposition(energyDeposition);
      energyWeightedPosition += (*scintHitsCollection)[i]->GetPos() * energyDeposition;
      if (energyDeposition > energyDepositionMax) {
        energyDepositionMax = energyDeposition;
        G4ThreeVector positionMax = (*scintHitsCollection)[i]->GetPos();
        eventInformation->setPositionMax(positionMax, energyDeposition);
      }
    }

    if (eventInformation->getEnergyDeposition()==0.0) {
      if(verbose>0)
        G4cout << "No hits in the scintillator this event." << G4endl;
    }
    else {
      energyWeightedPosition /= eventInformation->getEnergyDeposition();
      eventInformation->setEnergyWeightedPosition(energyWeightedPosition);
    }
  }

  if(pmtHitsCollection){
    G4int pmtHitCount = pmtHitsCollection->entries();
    for (G4int i = 0; i < pmtHitCount; i++) {
      eventInformation->incrementHitCount((*pmtHitsCollection)[i]->getPhotonCount());
      if ((*pmtHitsCollection)[i]->getPhotonCount() >= pmtThreshold) {
        eventInformation->incrementPmtCountAboveThreshold();
      }
      else
        (*pmtHitsCollection)[i]->setDrawIt(false);
    }
    pmtHitsCollection->DrawAllHits();
  }

  NovaRunAction* runAction = (NovaRunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());

  runStat.scintillationPhotonCount = eventInformation->getScintillationPhotonCount();
  runStat.cherenkovPhotonCount = eventInformation->getCherenkovPhotonCount();
  runStat.hitCount = eventInformation->getHitCount();
  runStat.pmtAboveThresholdCount = eventInformation->getPmtAboveThresholdCount();
  runStat.absorptionCount = eventInformation->getAbsorptionCount();
  runStat.boundaryAbsorptionCount = eventInformation->getBoundaryAbsorptionCount();
  runStat.unacountedCount = eventInformation->getCherenkovPhotonCount()
                            - eventInformation->getAbsorptionCount()
                            - eventInformation->getHitCount()
                            - eventInformation->getBoundaryAbsorptionCount();
  runStat.energyDeposition  = eventInformation->getEnergyDeposition() / MeV;
  runStat.energyDepositionX = eventInformation->getEnergyWeightedPosition().getX();
  runStat.energyDepositionY = eventInformation->getEnergyWeightedPosition().getY();
  runStat.energyDepositionZ = eventInformation->getEnergyWeightedPosition().getZ();

  eventStat.eventId = anEvent->GetEventID();
  NovaTrajectory* trajectory;
  for (G4int i = 0; i < nTrajectories; i++) {
    trajectory = (NovaTrajectory*) ((*(anEvent->GetTrajectoryContainer()))[i]);

    if (trajectory->GetParentID() == 0) {
      NovaTrajectoryPoint* trajectoryPoint = (NovaTrajectoryPoint*) trajectory->GetPoint(0);
      G4ThreeVector position = trajectoryPoint->GetPosition();
      runStat.primaryPdg = trajectory->GetPDGEncoding();
      runStat.primaryPX = trajectory->GetInitialMomentum().getX();
      runStat.primaryPY = trajectory->GetInitialMomentum().getY();
      runStat.primaryPZ = trajectory->GetInitialMomentum().getZ();
      runStat.primaryX = position.getX();
      runStat.primaryY = position.getY();
      runStat.primaryZ = position.getZ();
    }

    if(trajectory->GetStatus() == 2){
      eventStat.trackLength = 0.;
      eventStat.wlsCount = 0;
      eventStat.reflectionCount = 0;
      eventStat.totalInternalReflectionCount = 0;

      NovaTrajectoryPoint* lastTrajectoryPoint = (NovaTrajectoryPoint*)trajectory->GetPoint(trajectory->GetPointEntries() - 1);
      eventStat.hitTime = lastTrajectoryPoint->GetTime();
      eventStat.hitX = lastTrajectoryPoint->GetPosition().getX();
      eventStat.hitY = lastTrajectoryPoint->GetPosition().getY();
      eventStat.hitZ = lastTrajectoryPoint->GetPosition().getZ();
      eventStat.hitE = lastTrajectoryPoint->GetMomentum().getR() / eV;
      eventStat.hitPX = lastTrajectoryPoint->GetMomentum().getX() / eV;
      eventStat.hitPY = lastTrajectoryPoint->GetMomentum().getY() / eV;
      eventStat.hitPZ = lastTrajectoryPoint->GetMomentum().getZ() / eV;
      eventStat.hitWavelength = 1239.84 / (lastTrajectoryPoint->GetMomentum().getR() / eV);

      NovaTrajectory* currentTrajectory = trajectory;
      G4int parentTrackId;
      NovaTrajectory* parentTrajectory;
      while (currentTrajectory->GetParticleName() == "opticalphoton") {
        G4double trkLength = currentTrajectory->GetTrkLength() / cm;
        eventStat.trackLength += trkLength;

        G4String processName = currentTrajectory->GetProcessName();
        if(processName == "OpWLS")
          eventStat.wlsCount++;
        eventStat.reflectionCount += currentTrajectory->getReflectionCount();
        eventStat.totalInternalReflectionCount += currentTrajectory->getTotalInternalReflectionCount();

        for(G4int j = currentTrajectory->GetPointEntries() - 1; j >= 0; j--){
          NovaTrajectoryPoint* trajectoryPoint = (NovaTrajectoryPoint*) currentTrajectory->GetPoint(j);
          G4ThreeVector position = trajectoryPoint->GetPosition();
          G4ThreeVector momentum = trajectoryPoint->GetMomentum();
          G4String vol = trajectoryPoint->GetVolumeName();
          G4double time = trajectoryPoint->GetTime() / ns;

          if (j == 0) {
            eventStat.beginTime = time;
            eventStat.beginX = position.getX();
            eventStat.beginY = position.getY();
            eventStat.beginZ = position.getZ();
            eventStat.beginE = momentum.getR() / eV;
            eventStat.beginPX = momentum.getX() / eV;
            eventStat.beginPY = momentum.getY() / eV;
            eventStat.beginPZ = momentum.getZ() / eV;
            eventStat.beginWavelength = 1239.84 / (momentum.getR() / eV);
          }

          if (j > 0) {
            NovaTrajectoryPoint* previousTrajectoryPoint = (NovaTrajectoryPoint*)currentTrajectory->GetPoint(j-1);
            G4String previousVolume = previousTrajectoryPoint->GetVolumeName();
            if (vol == "wlsFiber" && previousVolume == "liquidScintillator") {
              eventStat.enterTime = time;
              eventStat.enterX = position.getX();
              eventStat.enterY = position.getY();
              eventStat.enterZ = position.getZ();
              eventStat.enterE = momentum.getR();
              eventStat.enterPX = momentum.getX();
              eventStat.enterPY = momentum.getY();
              eventStat.enterPZ = momentum.getZ();
              eventStat.enterWavelength = 1239.84 / (momentum.getR() / eV);
            }
          }
        }

        parentTrackId = currentTrajectory->GetParentID();
        for (G4int j = 0; j < nTrajectories; j++) {
          parentTrajectory = (NovaTrajectory*) ((*(anEvent->GetTrajectoryContainer()))[j]);
          if (parentTrajectory->GetTrackID() == parentTrackId) {
            currentTrajectory = parentTrajectory;
            break;
          }
        }
      }
      runAction->UpdateEventStatistics(eventStat);
    }
  }

  runAction->UpdateRunStatistics(runStat);
  if (saveThreshold && eventInformation->getPhotonCount() <= saveThreshold)
    G4RunManager::GetRunManager()->rndmSaveThisEvent();

  if(recorder)
    recorder->RecordEndOfEvent(anEvent);
}

void NovaEventAction::SetSaveThreshold(G4int save)
{
  saveThreshold = save;
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
}
