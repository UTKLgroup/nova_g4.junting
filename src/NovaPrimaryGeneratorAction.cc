#include "NovaPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"

NovaPrimaryGeneratorAction::NovaPrimaryGeneratorAction(const char *inputFilename)
    :fuseCRY(false)
{
  particleGun = new G4ParticleGun();
  particleTable = G4ParticleTable::GetParticleTable();

  std::ifstream inputFile;
  inputFile.open(inputFilename, std::ios::in);
  char buffer[1000];

  if (inputFile.fail()) {
    if (*inputFilename !=0)
      G4cout << "PrimaryGeneratorAction: Failed to open CRY input file= " << inputFilename << G4endl;
    InputState = -1;
  }
  else {
    std::string setupString("");
    while (!inputFile.getline(buffer,1000).eof()) {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup *setup=new CRYSetup(setupString,"/Users/juntinghuang/Desktop/nova/cry_v1.7/data");
    gen = new CRYGenerator(setup);
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    InputState = 0;
  }

  vect=new std::vector<CRYParticle*>;
  gunMessenger = new PrimaryGeneratorMessenger(this);
}

NovaPrimaryGeneratorAction::~NovaPrimaryGeneratorAction()
{
  delete particleGun;
}

void NovaPrimaryGeneratorAction::useCRY(G4bool useCry)
{
  fuseCRY = useCry;
}

void NovaPrimaryGeneratorAction::InputCRY()
{
  InputState = 1;
}

void NovaPrimaryGeneratorAction::UpdateCRY(std::string* MessInput)
{
  CRYSetup *setup=new CRYSetup(*MessInput,"/Users/juntinghuang/Desktop/nova/cry_v1.7/data");
  gen = new CRYGenerator(setup);

  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  InputState = 0;
}

void NovaPrimaryGeneratorAction::CRYFromFile(G4String newValue)
{
  std::ifstream inputFile;
  inputFile.open(newValue,std::ios::in);
  char buffer[1000];

  if (inputFile.fail()) {
    G4cout << "Failed to open input file " << newValue << G4endl;
    G4cout << "Make sure to define the cry library on the command line" << G4endl;
    InputState = -1;
  }
  else {
    std::string setupString("");
    while ( !inputFile.getline(buffer,1000).eof()) {
      setupString.append(buffer);
      setupString.append(" ");
    }

    CRYSetup *setup=new CRYSetup(setupString,"/Users/juntinghuang/Desktop/nova/cry_v1.7/data");
    gen = new CRYGenerator(setup);
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    InputState = 0;
  }
}

void NovaPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent){
  if(fuseCRY){
    if (InputState != 0) {
      G4String* str = new G4String("CRY library was not successfully initialized");
      G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, *str);
    }
    G4String particleName;
    vect->clear();
    gen->genEvent(vect);

    for ( unsigned j=0; j<vect->size(); j++) {
      particleName = CRYUtils::partName((*vect)[j]->id());

      particleGun->SetParticleDefinition(particleTable->FindParticle((*vect)[j]->PDGid()));
      particleGun->SetParticleEnergy((*vect)[j]->ke()*MeV);
      particleGun->SetParticlePosition(G4ThreeVector((*vect)[j]->x()*m, (*vect)[j]->y()*m, (*vect)[j]->z()*m));
      particleGun->SetParticleMomentumDirection(G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
      particleGun->SetParticleTime((*vect)[j]->t());
      particleGun->GeneratePrimaryVertex(anEvent);
      delete (*vect)[j];
    }
  }
  else
    particleGun->GeneratePrimaryVertex(anEvent);

}
