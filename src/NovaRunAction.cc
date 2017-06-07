#include "NovaRunAction.hh"
#include "NovaRecorderBase.hh"


NovaRunAction::NovaRunAction(NovaRecorderBase* r) : recorder(r) {}

NovaRunAction::~NovaRunAction() {}

void NovaRunAction::BeginOfRunAction(const G4Run* aRun){
  if(recorder)
    recorder->RecordBeginOfRun(aRun);

  outputFilename = new TFile("nova.root", "recreate");

  eventTree = new TTree("eventTree", "Event Statistics");
  eventTree->Branch("eventStat",
                  &eventStat.scintillationPhotonCount,
                  "scintillationPhotonCount/I:cherenkovPhotonCount:hitCount:pmtAboveThresholdCount:absorptionCount:boundaryAbsorptionCount:photocathodeAbsorptionCount:outOfWorldCount:unacountedCount:primaryPdg:primaryX/D:primaryY:primaryZ:primaryPX:primaryPY:primaryPZ:energyDeposition:energyDepositionX:energyDepositionY:energyDepositionZ");

  hitTree = new TTree("hitTree", "Hit Statistics");
  hitTree->Branch("hitStat",
                  &hitStat.eventId,
                  "eventId/I:wlsCount:reflectionCount:totalInternalReflectionCount:beginTime/D:beginX:beginY:beginZ:beginE:beginPX:beginPY:beginPZ:beginWavelength:enterTime:enterX:enterY:enterZ:enterE:enterPX:enterPY:enterPZ:enterWavelength:hitTime:hitX:hitY:hitZ:hitE:hitPX:hitPY:hitPZ:hitWavelength:trackLength");

}

void NovaRunAction::EndOfRunAction(const G4Run* aRun)
{
  if(recorder)
    recorder->RecordEndOfRun(aRun);

  eventTree->Write();
  hitTree->Write();
  outputFilename->Close();
}

void NovaRunAction::UpdateEventStatistics(EventStat stat)
{
  eventStat = stat;
  eventTree->Fill();
}

void NovaRunAction::UpdateHitStatistics(HitStat stat)
{
  hitStat = stat;
  hitTree->Fill();
}  
