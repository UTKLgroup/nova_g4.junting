#include "NovaPmtHit.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<NovaPmtHit> NovaPmtHitAllocator;

NovaPmtHit::NovaPmtHit()
    : pmtNumber(-1),photons(0),physicalVolume(0),drawIt(false) {}

NovaPmtHit::~NovaPmtHit() {}

NovaPmtHit::NovaPmtHit(const NovaPmtHit &right) : G4VHit()
{
  pmtNumber = right.pmtNumber;
  photons = right.photons;
  physicalVolume = right.physicalVolume;
  drawIt = right.drawIt;
}

const NovaPmtHit& NovaPmtHit::operator=(const NovaPmtHit &right){
  pmtNumber = right.pmtNumber;
  photons = right.photons;
  physicalVolume = right.physicalVolume;
  drawIt = right.drawIt;
  return *this;
}

G4int NovaPmtHit::operator==(const NovaPmtHit &right) const{
  return (pmtNumber==right.pmtNumber);
}

void NovaPmtHit::Draw(){
  if(drawIt && physicalVolume){
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager) {
      G4VisAttributes attributes(G4Colour(1.0, 0.0, 0.0));
      attributes.SetForceSolid(true);
      G4RotationMatrix rot;
      if (physicalVolume->GetRotation())
        rot = *(physicalVolume->GetRotation());
      G4Transform3D trans(rot, physicalVolume->GetTranslation());
    }
  }
}

void NovaPmtHit::Print() {
  G4cout << "hits" << G4endl;
}
