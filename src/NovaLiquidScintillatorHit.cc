#include "NovaLiquidScintillatorHit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

G4Allocator<NovaLiquidScintillatorHit> NovaScintHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NovaLiquidScintillatorHit::NovaLiquidScintillatorHit() : fEdep(0.), fPos(0.), fPhysVol(0) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NovaLiquidScintillatorHit::NovaLiquidScintillatorHit(G4VPhysicalVolume* pVol) : fPhysVol(pVol) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NovaLiquidScintillatorHit::~NovaLiquidScintillatorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NovaLiquidScintillatorHit::NovaLiquidScintillatorHit(const NovaLiquidScintillatorHit &right) : G4VHit()
{
  fEdep = right.fEdep;
  fPos = right.fPos;
  fPhysVol = right.fPhysVol;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const NovaLiquidScintillatorHit& NovaLiquidScintillatorHit::operator=(const NovaLiquidScintillatorHit &right){
  fEdep = right.fEdep;
  fPos = right.fPos;
  fPhysVol = right.fPhysVol;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int NovaLiquidScintillatorHit::operator==(const NovaLiquidScintillatorHit&) const{
  return false;
  //returns false because there currently isnt need to check for equality yet
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NovaLiquidScintillatorHit::Draw() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NovaLiquidScintillatorHit::Print() {}
