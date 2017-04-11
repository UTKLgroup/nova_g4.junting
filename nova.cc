#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "LXeDetectorConstruction.hh"
#include "LXePhysicsList.hh"
#include "LXePrimaryGeneratorAction.hh"
#include "LXeEventAction.hh"
#include "LXeStackingAction.hh"
#include "LXeSteppingAction.hh"
#include "LXeTrackingAction.hh"
#include "LXeRunAction.hh"
#include "LXeSteppingVerbose.hh"

#include "LXeRecorderBase.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc, char** argv)
{
  G4VSteppingVerbose::SetInstance(new LXeSteppingVerbose);
  G4RunManager* runManager = new G4RunManager;

  runManager->SetUserInitialization(new LXeDetectorConstruction);
  runManager->SetUserInitialization(new LXePhysicsList);

#ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  LXeRecorderBase* recorder = NULL;
  runManager->SetUserAction(new LXePrimaryGeneratorAction(""));
  runManager->SetUserAction(new LXeStackingAction);
  runManager->SetUserAction(new LXeRunAction(recorder));
  runManager->SetUserAction(new LXeEventAction(recorder));
  runManager->SetUserAction(new LXeTrackingAction(recorder));
  runManager->SetUserAction(new LXeSteppingAction(recorder));

  G4UImanager* uiManager = G4UImanager::GetUIpointer();

  if (argc==1) {
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    uiManager->ApplyCommand("/control/execute vis.mac");
#endif
    ui->SessionStart();
    delete ui;
#endif
  }
  else{
    G4String filename = argv[1];
    G4String randomSeed = argv[2];

    uiManager->ApplyCommand("/random/setSeeds " + randomSeed + " 1");
    uiManager->ApplyCommand("/control/execute " + filename);
  }

#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;
  return 0;
}
