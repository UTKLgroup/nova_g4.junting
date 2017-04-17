#include "LXeSteppingMessenger.hh"
#include "NovaSteppingAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeSteppingMessenger::LXeSteppingMessenger(NovaSteppingAction* step)
 : fStepping(step)
{
  fOneStepPrimariesCmd = new G4UIcmdWithABool("/LXe/oneStepPrimaries",this);
  fOneStepPrimariesCmd->SetGuidance("Only allows primaries to go one step in the scintillator volume before being killed.");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeSteppingMessenger::~LXeSteppingMessenger(){
  delete fOneStepPrimariesCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void 
LXeSteppingMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
  if( command == fOneStepPrimariesCmd ){
    fStepping->SetOneStepPrimaries(fOneStepPrimariesCmd
                                  ->GetNewBoolValue(newValue));
  }
}
