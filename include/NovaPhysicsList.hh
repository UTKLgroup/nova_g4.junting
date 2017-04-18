#ifndef NovaPhysicsList_h
#define NovaPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"


class NovaPhysicsList: public G4VModularPhysicsList
{
  public:
    NovaPhysicsList();
    virtual ~NovaPhysicsList();

  public:
    virtual void SetCuts();
};

#endif
