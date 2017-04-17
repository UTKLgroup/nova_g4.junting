#include "NovaRunAction.hh"
#include "NovaRecorderBase.hh"


NovaRunAction::NovaRunAction(NovaRecorderBase* r) : fRecorder(r) {}

NovaRunAction::~NovaRunAction() {}

void NovaRunAction::BeginOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordBeginOfRun(aRun);

  outputFilename=new TFile("nova.root", "update");

  runTree   = new TTree("runTree", "Run Statistics");
  runBranch = runTree->Branch("runStat",
                              &runStat.PhotonCount_Scint,
                              "PhotonCountScintillation/I:PhotonCountCherenkov:HitCount:PMTsAboveThreshold:AbsorptionCount:BoundaryAbsorptionCount:UnAccounted:primaryPDG:primaryX/D:primaryY:primaryZ:primaryPX:primaryPY:primaryPZ:EDep:EDepX:EDepY:EDepZ");


  eventTree   = new TTree("eventTree","Event Statistics");
  evtBranch = eventTree->Branch("eventStat",
                                &eventStat.EvtNum,
                                "EvtNum/I:wlsCount:reflectionCount:totalInternalReflectionCount:BeginTime/D:BeginX:BeginY:BeginZ:BeginE:BeginPX:BeginPY:BeginPZ:BeginWL:EnterTime:EnterX:EnterY:EnterZ:EnterE:EnterPX:EnterPY:EnterPZ:EnterWL:HitTime:HitX:HitY:HitZ:HitE:HitPX:HitPY:HitPZ:HitWL:TrkLength");

}

void NovaRunAction::EndOfRunAction(const G4Run* aRun){
  if(fRecorder)
    fRecorder->RecordEndOfRun(aRun);

  runTree->Write();
  eventTree->Write();
  outputFilename->Close();
}

void NovaRunAction::UpdateRunStatistics(RunStat stat){
  runStat = stat;
  runTree->Fill();
}

void NovaRunAction::UpdateEvtStatistics(EventStat stat){
  eventStat = stat;
  eventTree->Fill();
}  
