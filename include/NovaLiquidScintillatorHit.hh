#ifndef LXeScintHit_h
#define LXeScintHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"

class NovaLiquidScintillatorHit : public G4VHit
{
  public:
 
    NovaLiquidScintillatorHit();
    NovaLiquidScintillatorHit(G4VPhysicalVolume* pVol);
    virtual ~NovaLiquidScintillatorHit();
    NovaLiquidScintillatorHit(const NovaLiquidScintillatorHit &right);
    const NovaLiquidScintillatorHit& operator=(const NovaLiquidScintillatorHit &right);
    G4int operator==(const NovaLiquidScintillatorHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
 
    virtual void Draw();
    virtual void Print();

    inline void SetEdep(G4double de) { fEdep = de; }
    inline void AddEdep(G4double de) { fEdep += de; }
    inline G4double GetEdep() { return fEdep; }

    inline void SetPos(G4ThreeVector xyz) { fPos = xyz; }
    inline G4ThreeVector GetPos() { return fPos; }

    inline const G4VPhysicalVolume * GetPhysV() { return fPhysVol; }

  private:
    G4double fEdep;
    G4ThreeVector fPos;
    const G4VPhysicalVolume* fPhysVol;

};

typedef G4THitsCollection<NovaLiquidScintillatorHit> NovaScintHitsCollection;

extern G4Allocator<NovaLiquidScintillatorHit> NovaScintHitAllocator;

inline void* NovaLiquidScintillatorHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) NovaScintHitAllocator.MallocSingle();
  return aHit;
}

inline void NovaLiquidScintillatorHit::operator delete(void *aHit)
{
  NovaScintHitAllocator.FreeSingle((NovaLiquidScintillatorHit*) aHit);
}

#endif
