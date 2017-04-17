#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "NovaDetectorConstruction.hh"
#include "LXePhysicsList.hh"
#include "NovaPrimaryGeneratorAction.hh"
#include "NovaEventAction.hh"
#include "LXeStackingAction.hh"
#include "NovaSteppingAction.hh"
#include "LXeTrackingAction.hh"
#include "LXeRunAction.hh"
#include "LXeSteppingVerbose.hh"

#include "LXeRecorderBase.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


int main(int argc, char** argv)
{
  G4VSteppingVerbose::SetInstance(new LXeSteppingVerbose);

  LXeRecorderBase* recorder = NULL;
  G4RunManager* runManager = new G4RunManager;
  runManager->SetUserInitialization(new NovaDetectorConstruction);
  runManager->SetUserInitialization(new LXePhysicsList);
  runManager->SetUserAction(new NovaPrimaryGeneratorAction(""));
  runManager->SetUserAction(new LXeStackingAction);
  runManager->SetUserAction(new LXeRunAction(recorder));
  runManager->SetUserAction(new NovaEventAction(recorder));
  runManager->SetUserAction(new LXeTrackingAction(recorder));
  runManager->SetUserAction(new NovaSteppingAction(recorder));

  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  if (argc==1) {
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    uiManager->ApplyCommand("/control/execute vis.mac");
    uiManager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
  }
  else{
    G4String filename = argv[1];
    G4String randomSeed = argv[2];
    uiManager->ApplyCommand("/random/setSeeds " + randomSeed + " 1");
    uiManager->ApplyCommand("/control/execute " + filename);
  }

  delete visManager;
  delete runManager;
  return 0;
}
