#include "NovaPrimaryGeneratorMessenger.hh"
#include "NovaPrimaryGeneratorAction.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"


NovaPrimaryGeneratorMessenger::NovaPrimaryGeneratorMessenger(NovaPrimaryGeneratorAction* Gun)
    : primaryGeneratorAction(Gun)
{
  cryDir = new G4UIdirectory("/CRY/");
  cryDir->SetGuidance("CRY initialization");

  fileCmd = new G4UIcmdWithAString("/CRY/file",this);
  fileCmd->SetGuidance("This reads the CRY definition from a file");
  fileCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  inputCmd = new G4UIcmdWithAString("/CRY/input",this);
  inputCmd->SetGuidance("CRY input lines");
  inputCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  updateCmd = new G4UIcmdWithoutParameter("/CRY/update",this);
  updateCmd->SetGuidance("Update CRY definition.");
  updateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  updateCmd->SetGuidance("if you changed the CRY definition.");
  updateCmd->AvailableForStates(G4State_Idle);

  useCRYCmd = new G4UIcmdWithABool("/CRY/useCRY",this);
  useCRYCmd->SetGuidance("Enable/Disable CRY, 1 for using CRY, 0 for using a pencil beam");

  messengerInput = new std::string;
}

NovaPrimaryGeneratorMessenger::~NovaPrimaryGeneratorMessenger()
{
  delete cryDir;
  delete inputCmd;
  delete updateCmd;
  delete fileCmd;
}

void NovaPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == inputCmd) {
    primaryGeneratorAction->InputCRY();
    (*messengerInput).append(newValue);
    (*messengerInput).append(" ");
  }

  if(command == updateCmd) {
    primaryGeneratorAction->UpdateCRY(messengerInput);
    *messengerInput = "";
  }

  if(command == fileCmd) {
    primaryGeneratorAction->CRYFromFile(newValue);
  }

  if(command == useCRYCmd) {
    primaryGeneratorAction->useCRY(useCRYCmd->GetNewBoolValue(newValue));
  }
}
