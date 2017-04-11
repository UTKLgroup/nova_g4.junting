#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#ifndef LXeUserEventInformation_h
#define LXeUserEventInformation_h 1

class LXeUserEventInformation : public G4VUserEventInformation
{
  public:

    LXeUserEventInformation();
    virtual ~LXeUserEventInformation();

    inline virtual void Print()const{};

    void IncPhotonCount_Scint(){fPhotonCount_Scint++;}
    void IncPhotonCount_Ceren(){fPhotonCount_Ceren++;}
    void IncVetoPhotonCount_Ceren(){fVetoPhotonCount_Ceren++;}

    void IncEDep(G4double dep){fTotE+=dep;}
    void IncAbsorption(){fAbsorptionCount++;}
    void IncVetoAbsorption(){fVetoAbsorptionCount++;}
    void IncBoundaryAbsorption(){fBoundaryAbsorptionCount++;}
    void IncVetoBoundaryAbsorption(){fVetoBoundaryAbsorptionCount++;}

    void IncHitCount(G4int i=1){fHitCount+=i;}
    void IncVetoHitCount(G4int i=1){fVetoHitCount+=i;}

    void SetEWeightPos(const G4ThreeVector& p){fEWeightPos=p;}
    void SetReconPos(const G4ThreeVector& p){fReconPos=p;}
    void SetConvPos(const G4ThreeVector& p){fConvPos=p;fConvPosSet=true;}
    void SetPosMax(const G4ThreeVector& p,G4double edep){fPosMax=p;fEdepMax=edep;}

    G4int GetPhotonCount_Scint()const {return fPhotonCount_Scint;}
    G4int GetPhotonCount_Ceren()const {return fPhotonCount_Ceren;}
    G4int GetVetoPhotonCount_Ceren()const {return fVetoPhotonCount_Ceren;}

    G4int GetHitCount()const {return fHitCount;}
    G4int GetVetoHitCount()const {return fVetoHitCount;}
    G4double GetEDep()const {return fTotE;}
    G4int GetAbsorptionCount()const {return fAbsorptionCount;}
    G4int GetVetoAbsorptionCount()const {return fVetoAbsorptionCount;}
    G4int GetBoundaryAbsorptionCount() const {return fBoundaryAbsorptionCount;}
    G4int GetVetoBoundaryAbsorptionCount() const {return fVetoBoundaryAbsorptionCount;}

    G4ThreeVector GetEWeightPos(){return fEWeightPos;}
    G4ThreeVector GetReconPos(){return fReconPos;}
    G4ThreeVector GetConvPos(){return fConvPos;}
    G4ThreeVector GetPosMax(){return fPosMax;}
    G4double GetEDepMax(){return fEdepMax;}
    G4double IsConvPosSet(){return fConvPosSet;}

    //Gets the total photon count produced
    G4int GetPhotonCount(){return fPhotonCount_Scint+fPhotonCount_Ceren;}

    void IncPMTSAboveThreshold(){fPMTsAboveThreshold++;}
    void IncVetoPMTSAboveThreshold(){fVetoPMTsAboveThreshold++;}
    G4int GetPMTSAboveThreshold(){return fPMTsAboveThreshold;}
    G4int GetVetoPMTSAboveThreshold(){return fVetoPMTsAboveThreshold;}

  private:

    G4int fHitCount;
    G4int fVetoHitCount;
    G4int fPhotonCount_Scint;
    G4int fPhotonCount_Ceren;
    G4int fVetoPhotonCount_Ceren;
    G4int fAbsorptionCount;
    G4int fVetoAbsorptionCount;
    G4int fBoundaryAbsorptionCount;
    G4int fVetoBoundaryAbsorptionCount;

    G4double fTotE;

    //These only have meaning if totE > 0
    //If totE = 0 then these wont be set by EndOfEventAction
    G4ThreeVector fEWeightPos;
    G4ThreeVector fReconPos; //Also relies on hitCount>0
    G4ThreeVector fConvPos;//true (initial) converstion position
    G4bool fConvPosSet;
    G4ThreeVector fPosMax;
    G4double fEdepMax;

    G4int fPMTsAboveThreshold;
    G4int fVetoPMTsAboveThreshold;

};

#endif
