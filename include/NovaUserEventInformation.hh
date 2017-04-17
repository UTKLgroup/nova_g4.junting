#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#ifndef NovaUserEventInformation_h
#define NovaUserEventInformation_h 1

class NovaUserEventInformation : public G4VUserEventInformation
{
  public:
    NovaUserEventInformation();
    virtual ~NovaUserEventInformation();

    inline virtual void Print()const{};

    void incrementPhotonCountScintillation(){photonCountScintillation++;}
    void incrementPhotonCountCherenkov(){photonCountCherenkov++;}
    void addEnergyDeposition(G4double energyDeposition){totalEnergyDeposition += energyDeposition;}
    void incrementAbsorption(){absorptionCount++;}
    void incrementBoundaryAbsorption(){boundaryAbsorptionCount++;}

    void incrementHitCount(G4int i = 1){hitCount+=i;}
    void setEnergyWeightedPosition(const G4ThreeVector &p){energyWeightedPosition=p;}
    void setConversionPosition(const G4ThreeVector &p){conversionPos = p; conversionPositionSet = true;}
    void setPositionMax(const G4ThreeVector &p, G4double eDep){positionMax = p; energyDepositionMax = eDep;}

    G4int getPhotonCountScintillation()const {return photonCountScintillation;}
    G4int getPhotonCountCerenkov()const {return photonCountCherenkov;}
    G4int getHitCount()const {return hitCount;}

    G4double getEnergyDeposition()const {return totalEnergyDeposition;}
    G4int getAbsorptionCount()const {return absorptionCount;}
    G4int getBoundaryAbsorptionCount() const {return boundaryAbsorptionCount;}

    G4ThreeVector getEnergyWeightedPosition(){return energyWeightedPosition;}
    G4double isConversionPositionSet(){return conversionPositionSet;}
    G4int getPhotonCount(){return photonCountScintillation+photonCountCherenkov;}
    void incrementPmtCountAboveThreshold(){pmtCountAboveThreshold++;}
    G4int getPmtCountAboveThreshold(){return pmtCountAboveThreshold;}

  private:
    G4int hitCount;
    G4int photonCountScintillation;
    G4int photonCountCherenkov;
    G4int absorptionCount;
    G4int boundaryAbsorptionCount;

    G4double totalEnergyDeposition;
    G4ThreeVector energyWeightedPosition;
    G4ThreeVector reconstructionPosition;
    G4ThreeVector conversionPos;
    G4bool conversionPositionSet;
    G4ThreeVector positionMax;
    G4double energyDepositionMax;
    G4int pmtCountAboveThreshold;
};

#endif
