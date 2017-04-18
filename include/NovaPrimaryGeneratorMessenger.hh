#ifndef NovaPrimaryGeneratorMessenger_h
#define NovaPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class NovaPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithABool;


class NovaPrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    NovaPrimaryGeneratorMessenger(NovaPrimaryGeneratorAction*);
    ~NovaPrimaryGeneratorMessenger();
    void SetNewValue(G4UIcommand*, G4String);

  private:
    NovaPrimaryGeneratorAction* primaryGeneratorAction;
    G4UIdirectory* cryDir;
    G4UIcmdWithAString* fileCmd;
    G4UIcmdWithAString* inputCmd;
    G4UIcmdWithoutParameter* updateCmd;
    G4UIcmdWithABool* useCRYCmd;
    std::string* messengerInput;
};

#endif
