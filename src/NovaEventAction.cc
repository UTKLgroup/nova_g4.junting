#include <NovaUserTrackInformation.hh>
#include <G4OpticalPhoton.hh>
#include "NovaEventAction.hh"
#include "NovaLiquidScintillatorHit.hh"
#include "NovaPmtHit.hh"
#include "NovaUserEventInformation.hh"
#include "NovaRecorderBase.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4VVisManager.hh"
#include "G4SystemOfUnits.hh"
#include "NovaRunAction.hh"
#include "NovaTrajectoryPoint.hh"


NovaEventAction::NovaEventAction(NovaRecorderBase* r)
    : recorder(r),saveThreshold(0),scintCollectionId(-1),
      pmtCollectionId(-1),verbose(0),pmtThreshold(1) {}

NovaEventAction::~NovaEventAction(){}

void NovaEventAction::BeginOfEventAction(const G4Event* anEvent)
{
  G4EventManager::GetEventManager()->SetUserInformation(new NovaUserEventInformation);

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  if (scintCollectionId < 0)
    scintCollectionId = sdManager->GetCollectionID("liquidScintillatorCollection");
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
    G4double energyDepositionMax = 0.0;

    for(int i = 0; i < nHit; i++) {
      energyDeposition = (*scintHitsCollection)[i]->GetEdep();
      eventInformation->addEnergyDeposition(energyDeposition);
      energyWeightedPosition += (*scintHitsCollection)[i]->GetPos() * energyDeposition;
      if (energyDeposition > energyDepositionMax) {
        energyDepositionMax = energyDeposition;
        G4ThreeVector positionMax = (*scintHitsCollection)[i]->GetPos();
        eventInformation->setPositionMax(positionMax, energyDepositionMax);
      }
    }

    if (eventInformation->getEnergyDeposition() == 0.0) {
      if(verbose > 0)
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

  NovaRunAction* runAction = (NovaRunAction*) G4RunManager::GetRunManager()->GetUserRunAction();

  runStat.scintillationPhotonCount = eventInformation->getScintillationPhotonCount();
  runStat.cherenkovPhotonCount = eventInformation->getCherenkovPhotonCount();
  runStat.hitCount = eventInformation->getHitCount();
  runStat.pmtAboveThresholdCount = eventInformation->getPmtAboveThresholdCount();
  runStat.absorptionCount = eventInformation->getAbsorptionCount();
  runStat.boundaryAbsorptionCount = eventInformation->getBoundaryAbsorptionCount();
  runStat.photocathodeAbsorptionCount = eventInformation->getPhotocathodeAbsorptionCount();
  runStat.outOfWorldCount = eventInformation->getOutOfWorldCount();
  runStat.unacountedCount = eventInformation->getCherenkovPhotonCount()
                            + eventInformation->getScintillationPhotonCount()
                            - eventInformation->getAbsorptionCount()
                            - eventInformation->getHitCount()
                            - eventInformation->getBoundaryAbsorptionCount()
                            - eventInformation->getOutOfWorldCount();
  runStat.energyDeposition  = eventInformation->getEnergyDeposition() / MeV;
  runStat.energyDepositionX = eventInformation->getEnergyWeightedPosition().getX();
  runStat.energyDepositionY = eventInformation->getEnergyWeightedPosition().getY();
  runStat.energyDepositionZ = eventInformation->getEnergyWeightedPosition().getZ();
  hitStat.eventId = anEvent->GetEventID();

  for (G4int i = 0; i < nTrajectories; i++) {
    NovaTrajectory* trajectory = (NovaTrajectory*) (*trajectoryContainer)[i];

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

    if(trajectory->GetStatus() == hitPmt){
      NovaTrajectoryPoint* lastTrajectoryPoint = (NovaTrajectoryPoint*)trajectory->GetPoint(trajectory->GetPointEntries() - 1);
      hitStat.hitTime = lastTrajectoryPoint->GetTime();
      hitStat.hitX = lastTrajectoryPoint->GetPosition().getX();
      hitStat.hitY = lastTrajectoryPoint->GetPosition().getY();
      hitStat.hitZ = lastTrajectoryPoint->GetPosition().getZ();
      hitStat.hitE = lastTrajectoryPoint->GetMomentum().getR() / eV;
      hitStat.hitPX = lastTrajectoryPoint->GetMomentum().getX() / eV;
      hitStat.hitPY = lastTrajectoryPoint->GetMomentum().getY() / eV;
      hitStat.hitPZ = lastTrajectoryPoint->GetMomentum().getZ() / eV;
      hitStat.hitWavelength = 1239.84 / (lastTrajectoryPoint->GetMomentum().getR() / eV);

      hitStat.wlsCount = 0;
      hitStat.trackLength = trajectory->getTrackLength();
      hitStat.reflectionCount = trajectory->getReflectionCount();
      hitStat.totalInternalReflectionCount = trajectory->getTotalInternalReflectionCount();
      incrementWlsCount(trajectory);

      NovaTrajectory* parentTrajectory = trajectory->getParentTrajectory(trajectoryContainer);
      while (parentTrajectory != 0 && parentTrajectory->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
        hitStat.trackLength += parentTrajectory->getTrackLength();
        hitStat.reflectionCount += parentTrajectory->getReflectionCount();
        hitStat.totalInternalReflectionCount += parentTrajectory->getTotalInternalReflectionCount();
        if (parentTrajectory->GetProcessName() == "OpWLS") {
          incrementWlsCount(parentTrajectory);
        }
        parentTrajectory = parentTrajectory->getParentTrajectory(trajectoryContainer);
      }

      runAction->UpdateHitStatistics(hitStat);
    }
  }

  printEventInfo();

  runAction->UpdateRunStatistics(runStat);
  if (saveThreshold && eventInformation->getPhotonCount() <= saveThreshold)
    G4RunManager::GetRunManager()->rndmSaveThisEvent();

  if(recorder)
    recorder->RecordEndOfEvent(anEvent);
}

void NovaEventAction::printEventInfo()
{
  G4cout << "runStat.scintillationPhotonCount = " << runStat.scintillationPhotonCount << G4endl;
  G4cout << "runStat.cherenkovPhotonCount = " << runStat.cherenkovPhotonCount << G4endl;
  G4cout << "runStat.hitCount = " << runStat.hitCount << G4endl;
}

void NovaEventAction::SetSaveThreshold(G4int save)
{
  saveThreshold = save;
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
}

void NovaEventAction::incrementWlsCount(NovaTrajectory* trajectory)
{
  if (trajectory->GetProcessName() == "OpWLS")
    hitStat.wlsCount++;
}