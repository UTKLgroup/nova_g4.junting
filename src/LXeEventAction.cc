#include "LXeEventAction.hh"
#include "LXeScintHit.hh"
#include "NovaPmtHit.hh"
#include "LXeUserEventInformation.hh"
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


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeEventAction::LXeEventAction(LXeRecorderBase* r)
  : fRecorder(r),fSaveThreshold(0),fScintCollID(-1),fPMTCollID(-1),fVerbose(0),
   fPMTThreshold(1),fForcedrawphotons(false),fForcenophotons(false)
{
  fEventMessenger = new LXeEventMessenger(this);
}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeEventAction::~LXeEventAction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeEventAction::BeginOfEventAction(const G4Event* anEvent){
 
  //New event, add the user information object
  G4EventManager::
    GetEventManager()->SetUserInformation(new LXeUserEventInformation);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if(fScintCollID<0)
    fScintCollID=SDman->GetCollectionID("scintCollection");
  if(fPMTCollID<0)
    fPMTCollID=SDman->GetCollectionID("pmtHitCollection");

  if(fRecorder)fRecorder->RecordBeginOfEvent(anEvent);

}
 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeEventAction::EndOfEventAction(const G4Event* anEvent){

  LXeUserEventInformation* eventInformation
    =(LXeUserEventInformation*)anEvent->GetUserInformation();
  
  G4TrajectoryContainer* trajectoryContainer=anEvent->GetTrajectoryContainer();
  
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  // extract the trajectories and draw them
  /*
  if (G4VVisManager::GetConcreteInstance()){
    for (G4int i=0; i<n_trajectories; i++){
      LXeTrajectory* trj = (LXeTrajectory*)
        ((*(anEvent->GetTrajectoryContainer()))[i]);

      if(trj->GetParticleName()=="opticalphoton"){
        trj->SetForceDrawTrajectory(fForcedrawphotons);
        trj->SetForceNoDrawTrajectory(fForcenophotons);
      }
      trj->DrawTrajectory(50);
    }
  }
  */
 
  LXeScintHitsCollection* scintHC = 0;
  LXePMTHitsCollection* pmtHC = 0;

  G4HCofThisEvent* hitsCE = anEvent->GetHCofThisEvent();
 
  //Get the hit collections
  if(hitsCE){
    if(fScintCollID>=0)scintHC = (LXeScintHitsCollection*)(hitsCE->GetHC(fScintCollID));
    if(fPMTCollID>=0)pmtHC = (LXePMTHitsCollection*)(hitsCE->GetHC(fPMTCollID));
  }

  //Hits in scintillator
  if(scintHC){
    int n_hit = scintHC->entries();
    G4ThreeVector  eWeightPos(0.);
    G4double edep;
    G4double edepMax=0;

    for(int i=0;i<n_hit;i++){ //gather info on hits in scintillator
      edep=(*scintHC)[i]->GetEdep();
      eventInformation->IncEDep(edep); //sum up the edep
      eWeightPos += (*scintHC)[i]->GetPos()*edep;//calculate energy weighted pos
      if(edep>edepMax){
	edepMax=edep;//store max energy deposit
	G4ThreeVector posMax=(*scintHC)[i]->GetPos();
	eventInformation->SetPosMax(posMax,edep);
      }
    }

    if(eventInformation->GetEDep()==0.){
      if(fVerbose>0)G4cout<<"No hits in the scintillator this event."<<G4endl;
    }
    else{
      //Finish calculation of energy weighted position
      eWeightPos /= eventInformation->GetEDep();
      eventInformation->SetEWeightPos(eWeightPos);
    }    
  }   

  if(pmtHC){
    G4int pmts = pmtHC->entries();
    for(G4int i=0;i<pmts;i++){
      eventInformation->IncHitCount((*pmtHC)[i]->GetPhotonCount());
      if((*pmtHC)[i]->GetPhotonCount()>=fPMTThreshold){
	eventInformation->IncPMTSAboveThreshold();
      }
      else
        (*pmtHC)[i]->SetDrawit(false);
    }
    pmtHC->DrawAllHits();
  }
  
  //-------------------------------------

  LXeRunAction* runact = (LXeRunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());

  //-------------------------------------

  if(fVerbose>0){

    G4cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << G4endl << G4endl;

    G4cout << "\tTotal energy deposition in scintillator : "
	   << eventInformation->GetEDep() / MeV << " (MeV)" << G4endl;    

    G4cout << "\tEnergy weighted position of hits in LXe : "
	   << eventInformation->GetEWeightPos() / cm << " (cm)" << G4endl;    

    G4cout << "\tNumber of photons produced by scintillation in this event : "
	   << eventInformation->GetPhotonCount_Scint() << G4endl;

    G4cout << "\tNumber of photons produced by cerenkov in ID in this event : "
	   << eventInformation->GetPhotonCount_Ceren() << G4endl;

    G4cout << "\tNumber of photons that hit PMTs : "
	   << eventInformation->GetHitCount() << G4endl;

    G4cout << "\tNumber of PMTs above threshold("<<fPMTThreshold<<") : "
	   << eventInformation->GetPMTSAboveThreshold() << G4endl;

    G4cout << "\tNumber of photons absorbed (OpAbsorption) : "
	   << eventInformation->GetAbsorptionCount() << G4endl;

    G4cout << "\tNumber of photons absorbed at boundaries (OpBoundary) : "
	   << "this event : " << eventInformation->GetBoundaryAbsorptionCount() << G4endl;

    G4cout << "\tUnacounted for photons in this event : "
	   << (eventInformation->GetPhotonCount_Scint() -
	       eventInformation->GetHitCount() -
	       eventInformation->GetBoundaryAbsorptionCount() -
	       eventInformation->GetAbsorptionCount())
	   << G4endl;
  }    

  //-------------------------------------
  
  runStat.PhotonCount_Scint       = eventInformation->GetPhotonCount_Scint();
  runStat.PhotonCount_Ceren       = eventInformation->GetPhotonCount_Ceren();
  runStat.HitCount                = eventInformation->GetHitCount();
  runStat.PMTsAboveThreshold      = eventInformation->GetPMTSAboveThreshold();
  runStat.AbsorptionCount         = eventInformation->GetAbsorptionCount();
  runStat.BoundaryAbsorptionCount = eventInformation->GetBoundaryAbsorptionCount();
  runStat.Unacounted              = (eventInformation->GetPhotonCount_Ceren() -
				  eventInformation->GetAbsorptionCount() -
				  eventInformation->GetHitCount() -
				  eventInformation->GetBoundaryAbsorptionCount());
  
  runStat.EDep  = eventInformation->GetEDep() / MeV;
  runStat.EDepX = eventInformation->GetEWeightPos().getX();
  runStat.EDepY = eventInformation->GetEWeightPos().getY();
  runStat.EDepZ = eventInformation->GetEWeightPos().getZ();

  //-------------------------------------

  evtStat.EvtNum = anEvent->GetEventID();
  F04Trajectory* trj;
  for(G4int i = 0; i < n_trajectories; i++){
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
	for(G4int j = 0; j < n_trajectories; j++){
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
  if(fSaveThreshold&&eventInformation->GetPhotonCount() <= fSaveThreshold)
    G4RunManager::GetRunManager()->rndmSaveThisEvent();
  
  if(fRecorder)fRecorder->RecordEndOfEvent(anEvent);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
void LXeEventAction::SetSaveThreshold(G4int save){
/*Sets the save threshold for the random number seed. If the number of photons
generated in an event is lower than this, then save the seed for this event
in a file called run###evt###.rndm
*/
  fSaveThreshold=save;
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
  //  G4UImanager::GetUIpointer()->ApplyCommand("/random/setSavingFlag 1");
}
