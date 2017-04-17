#include "NovaEventAction.hh"
#include "LXeScintHit.hh"
#include "NovaPmtHit.hh"
#include "NovaUserEventInformation.hh"
#include "LXeTrajectory.hh"
#include "LXeRecorderBase.hh"

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
#include "LXeRunAction.hh"

#include "F04Trajectory.hh"
#include "F04TrajectoryPoint.hh"


NovaEventAction::NovaEventAction(LXeRecorderBase* r)
  : recorder(r),fSaveThreshold(0),scintCollectionId(-1),pmtCollectionId(-1),fVerbose(0), fPMTThreshold(1),
    fForcedrawphotons(false),fForcenophotons(false)
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

    if(eventInformation->getEnergyDeposition()==0.){
      if(fVerbose>0)G4cout<<"No hits in the scintillator this event."<<G4endl;
    }
    else{
      //Finish calculation of energy weighted position
      energyWeightedPosition /= eventInformation->getEnergyDeposition();
      eventInformation->setEnergyWeightedPosition(energyWeightedPosition);
    }    
  }   

  if(pmtHitsCollection){
    G4int pmts = pmtHitsCollection->entries();
    for(G4int i=0;i<pmts;i++){
      eventInformation->incrementHitCount((*pmtHitsCollection)[i]->getPhotonCount());
      if((*pmtHitsCollection)[i]->getPhotonCount()>=fPMTThreshold){
        eventInformation->incrementPmtCountAboveThreshold();
      }
      else
        (*pmtHitsCollection)[i]->setDrawIt(false);
    }
    pmtHitsCollection->DrawAllHits();
  }
  
  //-------------------------------------

  LXeRunAction* runact = (LXeRunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());

  //-------------------------------------

  if(fVerbose>0){

    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl << G4endl;

    G4cout << "\tTotal energy deposition in scintillator : "
	   << eventInformation->getEnergyDeposition() / MeV << " (MeV)" << G4endl;

    G4cout << "\tEnergy weighted position of hits in LXe : "
	   << eventInformation->getEnergyWeightedPosition() / cm << " (cm)" << G4endl;

    G4cout << "\tNumber of photons produced by scintillation in this event : "
	   << eventInformation->getPhotonCountScintillation() << G4endl;

    G4cout << "\tNumber of photons produced by cerenkov in ID in this event : "
	   << eventInformation->getPhotonCountCerenkov() << G4endl;

    G4cout << "\tNumber of photons that hit PMTs : "
	   << eventInformation->getHitCount() << G4endl;

    G4cout << "\tNumber of PMTs above threshold("<<fPMTThreshold<<") : "
	   << eventInformation->getPmtCountAboveThreshold() << G4endl;

    G4cout << "\tNumber of photons absorbed (OpAbsorption) : "
	   << eventInformation->getAbsorptionCount() << G4endl;

    G4cout << "\tNumber of photons absorbed at boundaries (OpBoundary) : "
	   << "this event : " << eventInformation->getBoundaryAbsorptionCount() << G4endl;

    G4cout << "\tUnacounted for photons in this event : "
	   << (eventInformation->getPhotonCountScintillation() -
         eventInformation->getHitCount() -
         eventInformation->getBoundaryAbsorptionCount() -
         eventInformation->getAbsorptionCount())
	   << G4endl;
  }    

  //-------------------------------------
  
  runStat.PhotonCount_Scint       = eventInformation->getPhotonCountScintillation();
  runStat.PhotonCount_Ceren       = eventInformation->getPhotonCountCerenkov();
  runStat.HitCount                = eventInformation->getHitCount();
  runStat.PMTsAboveThreshold      = eventInformation->getPmtCountAboveThreshold();
  runStat.AbsorptionCount         = eventInformation->getAbsorptionCount();
  runStat.BoundaryAbsorptionCount = eventInformation->getBoundaryAbsorptionCount();
  runStat.Unacounted              = (eventInformation->getPhotonCountCerenkov() -
      eventInformation->getAbsorptionCount() -
      eventInformation->getHitCount() -
      eventInformation->getBoundaryAbsorptionCount());
  
  runStat.EDep  = eventInformation->getEnergyDeposition() / MeV;
  runStat.EDepX = eventInformation->getEnergyWeightedPosition().getX();
  runStat.EDepY = eventInformation->getEnergyWeightedPosition().getY();
  runStat.EDepZ = eventInformation->getEnergyWeightedPosition().getZ();

  //-------------------------------------

  evtStat.EvtNum = anEvent->GetEventID();
  F04Trajectory* trj;
  for(G4int i = 0; i < nTrajectories; i++){
    trj = (F04Trajectory*) ((*(anEvent->GetTrajectoryContainer()))[i]);

    // get info of the primary
    if(trj->GetParentID() == 0){
      F04TrajectoryPoint* trjpt = (F04TrajectoryPoint*)trj->GetPoint(0);
      G4ThreeVector pt = trjpt->GetPosition(); 
      runStat.primaryPDG = trj->GetPDGEncoding();
      runStat.primaryPX  = trj->GetInitialMomentum().getX();
      runStat.primaryPY  = trj->GetInitialMomentum().getY();
      runStat.primaryPZ  = trj->GetInitialMomentum().getZ();   
      runStat.primaryX   = pt.getX();
      runStat.primaryY   = pt.getY();
      runStat.primaryZ   = pt.getZ();     
    }

    // get info of the photons that hit a PMT 
    if(trj->GetStatus() == 2){

      //G4cout << "had a hit on PMT" << G4endl;
      evtStat.TrkLength = 0.;
      evtStat.numWLS    = 0;
      evtStat.numRefl   = 0;
      evtStat.numTIRefl = 0;

      // hit info      
      F04TrajectoryPoint* hitpt = (F04TrajectoryPoint*)trj->GetPoint(trj->GetPointEntries() - 1);
      evtStat.HitTime = hitpt->GetTime();
      evtStat.HitX    = hitpt->GetPosition().getX();
      evtStat.HitY    = hitpt->GetPosition().getY();
      evtStat.HitZ    = hitpt->GetPosition().getZ();
      evtStat.HitE    = hitpt->GetMomentum().getR() / eV;
      evtStat.HitPX   = hitpt->GetMomentum().getX() / eV;
      evtStat.HitPY   = hitpt->GetMomentum().getY() / eV;
      evtStat.HitPZ   = hitpt->GetMomentum().getZ() / eV;
      evtStat.HitWL   = 1239.84 / (hitpt->GetMomentum().getR() / eV);

      G4int CurrentID;
      F04Trajectory* CurrentTrj;
      G4int ParentID;
      F04Trajectory* ParentTrj;
      CurrentTrj = trj;
      CurrentID = trj->GetTrackID();
      while(CurrentTrj->GetParticleName() == "opticalphoton"){

	G4double trklength = CurrentTrj->GetTrkLength() / cm;
	evtStat.TrkLength += trklength;

	G4String processname = CurrentTrj->GetProcessName();
	if(processname == "OpWLS")
	  evtStat.numWLS++;

	evtStat.numRefl   += CurrentTrj->GetNumRefl();
	evtStat.numTIRefl += CurrentTrj->GetNumTIRefl();

	//G4cout << CurrentID << "   " << CurrentTrj->GetParticleName() << "   " << trklength << " cm     " << processname << G4endl;

	for(G4int j = CurrentTrj->GetPointEntries() - 1; j >= 0; j--){
	  F04TrajectoryPoint* trjpt = (F04TrajectoryPoint*)CurrentTrj->GetPoint(j);
	  G4ThreeVector position = trjpt->GetPosition();
	  G4ThreeVector momentum = trjpt->GetMomentum();
	  G4String vol = trjpt->GetVolumeName();
	  G4double time = trjpt->GetTime() / ns;

	  // begin info
	  if(j == 0){
	    evtStat.BeginTime = time;
	    evtStat.BeginX    = position.getX();
	    evtStat.BeginY    = position.getY();
	    evtStat.BeginZ    = position.getZ();
	    evtStat.BeginE    = momentum.getR()  / eV;
	    evtStat.BeginPX   = momentum.getX()  / eV;
	    evtStat.BeginPY   = momentum.getY()  / eV;
	    evtStat.BeginPZ   = momentum.getZ()  / eV;
	    evtStat.BeginWL    = 1239.84 / (momentum.getR()  / eV);
	  }

	  // enter info
	  if(j > 0){
	    F04TrajectoryPoint* pretrjpt = (F04TrajectoryPoint*)CurrentTrj->GetPoint(j-1);
	    G4String prevol = pretrjpt->GetVolumeName();
	    if(vol == "outer_clading" && prevol == "scnt"){
	      evtStat.EnterTime = time;
	      evtStat.EnterX    = position.getX();
	      evtStat.EnterY    = position.getY();
	      evtStat.EnterZ    = position.getZ();
	      evtStat.EnterE    = momentum.getR();
	      evtStat.EnterPX   = momentum.getX();
	      evtStat.EnterPY   = momentum.getY();
	      evtStat.EnterPZ   = momentum.getZ();
	      evtStat.EnterWL   = 1239.84 / (momentum.getR()  / eV);
	    }
	  }
	  
	  //G4cout << position << "   " << time << "   " << vol << G4endl;   
	}

	// find parent trajectory
	ParentID = CurrentTrj->GetParentID();
	for(G4int j = 0; j < nTrajectories; j++){
	  ParentTrj = (F04Trajectory*) ((*(anEvent->GetTrajectoryContainer()))[j]);  
	  if(ParentTrj->GetTrackID() == ParentID)
	    break;
	}
	CurrentTrj = ParentTrj;
	CurrentID = ParentID;
      }

      runact->UpdateEvtStatistics(evtStat);

    }      
  }

  //-------------------------------------

  runact->UpdateRunStatistics(runStat);
    
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
