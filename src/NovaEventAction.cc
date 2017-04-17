#include "NovaEventAction.hh"
#include "LXeScintHit.hh"
#include "NovaPmtHit.hh"
#include "NovaUserEventInformation.hh"
#include "LXeTrajectory.hh"
#include "NovaRecorderBase.hh"

#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

#include "fstream"
#include "NovaRunAction.hh"

#include "NovaTrajectory.hh"
#include "NovaTrajectoryPoint.hh"


NovaEventAction::NovaEventAction(NovaRecorderBase* r)
  : recorder(r),fSaveThreshold(0),scintCollectionId(-1),pmtCollectionId(-1),verbose(0), pmtThreshold(1),
    forceDrawPhotons(false),forceDrawNoPhotons(false)
{
  fEventMessenger = new LXeEventMessenger(this);
}
 
NovaEventAction::~NovaEventAction(){}

void NovaEventAction::BeginOfEventAction(const G4Event* anEvent)
{
  G4EventManager::GetEventManager()->SetUserInformation(new NovaUserEventInformation);

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  if(scintCollectionId<0)
    scintCollectionId = sdManager->GetCollectionID("scintCollection");
  if(pmtCollectionId<0)
    pmtCollectionId = sdManager->GetCollectionID("pmtHitCollection");
  if(recorder)
    recorder->RecordBeginOfEvent(anEvent);
}
 
void NovaEventAction::EndOfEventAction(const G4Event* anEvent){

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

  if(scintHitsCollection){
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

    if (eventInformation->getEnergyDeposition()==0.) {
      if(verbose>0)G4cout<<"No hits in the scintillator this event."<<G4endl;
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
      G4ThreeVector pt = trajectoryPoint->GetPosition();
      runStat.primaryPDG = trajectory->GetPDGEncoding();
      runStat.primaryPX  = trajectory->GetInitialMomentum().getX();
      runStat.primaryPY  = trajectory->GetInitialMomentum().getY();
      runStat.primaryPZ  = trajectory->GetInitialMomentum().getZ();
      runStat.primaryX   = pt.getX();
      runStat.primaryY   = pt.getY();
      runStat.primaryZ   = pt.getZ();     
    }

    // get info of the photons that hit a PMT 
    if(trajectory->GetStatus() == 2){

      //G4cout << "had a hit on PMT" << G4endl;
      eventStat.TrkLength = 0.;
      eventStat.numWLS    = 0;
      eventStat.numRefl   = 0;
      eventStat.numTIRefl = 0;

      // hit info      
      NovaTrajectoryPoint* hitpt = (NovaTrajectoryPoint*)trajectory->GetPoint(trajectory->GetPointEntries() - 1);
      eventStat.HitTime = hitpt->GetTime();
      eventStat.HitX    = hitpt->GetPosition().getX();
      eventStat.HitY    = hitpt->GetPosition().getY();
      eventStat.HitZ    = hitpt->GetPosition().getZ();
      eventStat.HitE    = hitpt->GetMomentum().getR() / eV;
      eventStat.HitPX   = hitpt->GetMomentum().getX() / eV;
      eventStat.HitPY   = hitpt->GetMomentum().getY() / eV;
      eventStat.HitPZ   = hitpt->GetMomentum().getZ() / eV;
      eventStat.HitWL   = 1239.84 / (hitpt->GetMomentum().getR() / eV);

      G4int CurrentID;
      NovaTrajectory* CurrentTrj;
      G4int ParentID;
      NovaTrajectory* ParentTrj;
      CurrentTrj = trajectory;
      CurrentID = trajectory->GetTrackID();
      while(CurrentTrj->GetParticleName() == "opticalphoton"){

	G4double trklength = CurrentTrj->GetTrkLength() / cm;
	eventStat.TrkLength += trklength;

	G4String processname = CurrentTrj->GetProcessName();
	if(processname == "OpWLS")
	  eventStat.numWLS++;

	eventStat.numRefl   += CurrentTrj->GetNumRefl();
	eventStat.numTIRefl += CurrentTrj->GetNumTIRefl();

	//G4cout << CurrentID << "   " << CurrentTrj->GetParticleName() << "   " << trklength << " cm     " << processname << G4endl;

	for(G4int j = CurrentTrj->GetPointEntries() - 1; j >= 0; j--){
	  NovaTrajectoryPoint* trjpt = (NovaTrajectoryPoint*)CurrentTrj->GetPoint(j);
	  G4ThreeVector position = trjpt->GetPosition();
	  G4ThreeVector momentum = trjpt->GetMomentum();
	  G4String vol = trjpt->GetVolumeName();
	  G4double time = trjpt->GetTime() / ns;

	  // begin info
	  if(j == 0){
	    eventStat.BeginTime = time;
	    eventStat.BeginX    = position.getX();
	    eventStat.BeginY    = position.getY();
	    eventStat.BeginZ    = position.getZ();
	    eventStat.BeginE    = momentum.getR()  / eV;
	    eventStat.BeginPX   = momentum.getX()  / eV;
	    eventStat.BeginPY   = momentum.getY()  / eV;
	    eventStat.BeginPZ   = momentum.getZ()  / eV;
	    eventStat.BeginWL    = 1239.84 / (momentum.getR()  / eV);
	  }

	  // enter info
	  if(j > 0){
	    NovaTrajectoryPoint* pretrjpt = (NovaTrajectoryPoint*)CurrentTrj->GetPoint(j-1);
	    G4String prevol = pretrjpt->GetVolumeName();
	    if(vol == "outer_clading" && prevol == "scnt"){
	      eventStat.EnterTime = time;
	      eventStat.EnterX    = position.getX();
	      eventStat.EnterY    = position.getY();
	      eventStat.EnterZ    = position.getZ();
	      eventStat.EnterE    = momentum.getR();
	      eventStat.EnterPX   = momentum.getX();
	      eventStat.EnterPY   = momentum.getY();
	      eventStat.EnterPZ   = momentum.getZ();
	      eventStat.EnterWL   = 1239.84 / (momentum.getR()  / eV);
	    }
	  }
	  
	  //G4cout << position << "   " << time << "   " << vol << G4endl;   
	}

	// find parent trajectory
	ParentID = CurrentTrj->GetParentID();
	for(G4int j = 0; j < nTrajectories; j++){
	  ParentTrj = (NovaTrajectory*) ((*(anEvent->GetTrajectoryContainer()))[j]);
	  if(ParentTrj->GetTrackID() == ParentID)
	    break;
	}
	CurrentTrj = ParentTrj;
	CurrentID = ParentID;
      }

      runAction->UpdateEvtStatistics(eventStat);

    }      
  }

  //-------------------------------------

  runAction->UpdateRunStatistics(runStat);
    
  //-------------
  
  //If we have set the flag to save 'special' events, save here
  if(fSaveThreshold&& eventInformation->getPhotonCount() <= fSaveThreshold)
    G4RunManager::GetRunManager()->rndmSaveThisEvent();
  
  if(recorder)recorder->RecordEndOfEvent(anEvent);


}
  
void NovaEventAction::SetSaveThreshold(G4int save){
/*Sets the save threshold for the random number seed. If the number of photons
generated in an event is lower than this, then save the seed for this event
in a file called run###evt###.rndm
*/
  fSaveThreshold=save;
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
  //  G4UImanager::GetUIpointer()->ApplyCommand("/random/setSavingFlag 1");
}
