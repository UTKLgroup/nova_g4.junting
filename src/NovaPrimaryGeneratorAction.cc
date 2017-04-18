#include <G4UImanager.hh>
#include "NovaPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"


NovaPrimaryGeneratorAction::NovaPrimaryGeneratorAction(const char *inputFilename)
    :fuseCRY(false)
{
  particleGun = new G4ParticleGun();
  generalParticleSource = new G4GeneralParticleSource();
  particleTable = G4ParticleTable::GetParticleTable();

  G4UImanager* ui = G4UImanager::GetUIpointer();
  ui->ApplyCommand("/control/execute general_particle_source.mac");

  std::ifstream inputFile;
  inputFile.open(inputFilename, std::ios::in);
  char buffer[1000];

  if (inputFile.fail()) {
    if (*inputFilename !=0)
      G4cout << "PrimaryGeneratorAction: Failed to open CRY input filename = " << inputFilename << G4endl;
    inputState = -1;
  }
  else {
    std::string setupString("");
    while (!inputFile.getline(buffer,1000).eof()) {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup* crySetup = new CRYSetup(setupString,"/Users/juntinghuang/Desktop/nova/cry_v1.7/data");
    cryGenerator = new CRYGenerator(crySetup);
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
    crySetup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    inputState = 0;
  }

  cryParticles = new std::vector<CRYParticle*>;
  gunMessenger = new NovaPrimaryGeneratorMessenger(this);
}

NovaPrimaryGeneratorAction::~NovaPrimaryGeneratorAction()
{
  delete particleGun;
  delete generalParticleSource;
}

void NovaPrimaryGeneratorAction::useCRY(G4bool useCry)
{
  fuseCRY = useCry;
}

void NovaPrimaryGeneratorAction::InputCRY()
{
  inputState = 1;
}

void NovaPrimaryGeneratorAction::UpdateCRY(std::string* MessInput)
{
  CRYSetup *setup=new CRYSetup(*MessInput,"/Users/juntinghuang/Desktop/nova/cry_v1.7/data");
  cryGenerator = new CRYGenerator(setup);

  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  inputState = 0;
}

void NovaPrimaryGeneratorAction::CRYFromFile(G4String newValue)
{
  std::ifstream inputFile;
  inputFile.open(newValue,std::ios::in);
  char buffer[1000];

  if (inputFile.fail()) {
    G4cout << "Failed to open input file " << newValue << G4endl;
    G4cout << "Make sure to define the cry library on the command line" << G4endl;
    inputState = -1;
  }
  else {
    std::string setupString("");
    while ( !inputFile.getline(buffer,1000).eof()) {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup *setup=new CRYSetup(setupString,"/Users/juntinghuang/Desktop/nova/cry_v1.7/data");
    cryGenerator = new CRYGenerator(setup);
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    inputState = 0;
  }
}

void NovaPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent){
  if(fuseCRY){
    if (inputState != 0) {
      G4String* str = new G4String("CRY library was not successfully initialized");
      G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, *str);
    }
    G4String particleName;
    cryParticles->clear();
    cryGenerator->genEvent(cryParticles);

    for ( unsigned j=0; j<cryParticles->size(); j++) {
      particleName = CRYUtils::partName((*cryParticles)[j]->id());

      particleGun->SetParticleDefinition(particleTable->FindParticle((*cryParticles)[j]->PDGid()));
      particleGun->SetParticleEnergy((*cryParticles)[j]->ke()*MeV);
      particleGun->SetParticlePosition(G4ThreeVector((*cryParticles)[j]->x()*m, (*cryParticles)[j]->y()*m, (*cryParticles)[j]->z()*m));
      particleGun->SetParticleMomentumDirection(G4ThreeVector((*cryParticles)[j]->u(), (*cryParticles)[j]->v(), (*cryParticles)[j]->w()));
      particleGun->SetParticleTime((*cryParticles)[j]->t());
      particleGun->GeneratePrimaryVertex(anEvent);
      delete (*cryParticles)[j];
    }
  }
  else {
    generalParticleSource->GeneratePrimaryVertex(anEvent);
  }
}
