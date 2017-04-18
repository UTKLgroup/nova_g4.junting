#include "NovaLiquidScintillatorHit.hh"
#include "G4VVisManager.hh"

G4Allocator<NovaLiquidScintillatorHit> NovaScintHitAllocator;

NovaLiquidScintillatorHit::NovaLiquidScintillatorHit() : fEdep(0.), fPos(0.), fPhysVol(0) {}

NovaLiquidScintillatorHit::NovaLiquidScintillatorHit(G4VPhysicalVolume* pVol) : fPhysVol(pVol) {}

NovaLiquidScintillatorHit::~NovaLiquidScintillatorHit() {}

NovaLiquidScintillatorHit::NovaLiquidScintillatorHit(const NovaLiquidScintillatorHit &right) : G4VHit()
{
  fEdep = right.fEdep;
  fPos = right.fPos;
  fPhysVol = right.fPhysVol;
}

const NovaLiquidScintillatorHit& NovaLiquidScintillatorHit::operator=(const NovaLiquidScintillatorHit &right)
{
  fEdep = right.fEdep;
  fPos = right.fPos;
  fPhysVol = right.fPhysVol;
  return *this;
}

G4int NovaLiquidScintillatorHit::operator==(const NovaLiquidScintillatorHit&) const
{
  return false;
}

void NovaLiquidScintillatorHit::Draw() {}

void NovaLiquidScintillatorHit::Print() {}
