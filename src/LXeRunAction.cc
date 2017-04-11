#include "LXeRunAction.hh"
#include "LXeRecorderBase.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeRunAction::LXeRunAction(LXeRecorderBase* r) : fRecorder(r) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeRunAction::~LXeRunAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeRunAction::BeginOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordBeginOfRun(aRun);

  out=new TFile("nova.root","update");

  runTree   = new TTree("runTree","Run Statistics");
  runBranch = runTree->Branch("runStat",
			      &runStat.PhotonCount_Scint, 
			      "PhotonCount_Scint/I:PhotonCount_Ceren:HitCount:PMTsAboveThreshold:AbsorptionCount:BoundaryAbsorptionCount:Unacounted:primaryPDG:primaryX/D:primaryY:primaryZ:primaryPX:primaryPY:primaryPZ:EDep:EDepX:EDepY:EDepZ");


  evtTree   = new TTree("evtTree","PE Statistics");
  evtBranch = evtTree->Branch("evtStat",
			      &evtStat.EvtNum,
			      "EvtNum/I:numWLS:numRefl:numTIRefl:BeginTime/D:BeginX:BeginY:BeginZ:BeginE:BeginPX:BeginPY:BeginPZ:BeginWL:EnterTime:EnterX:EnterY:EnterZ:EnterE:EnterPX:EnterPY:EnterPZ:EnterWL:HitTime:HitX:HitY:HitZ:HitE:HitPX:HitPY:HitPZ:HitWL:TrkLength");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeRunAction::EndOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordEndOfRun(aRun);

  runTree->Write();
  evtTree->Write();
  out->Close();
}

void LXeRunAction::UpdateRunStatistics(RunStat stat){
  runStat = stat;
  runTree->Fill();
}  

void LXeRunAction::UpdateEvtStatistics(EvtStat stat){
  evtStat = stat;
  evtTree->Fill();
}  
