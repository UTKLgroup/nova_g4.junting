#include "LXeUserEventInformation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeUserEventInformation::LXeUserEventInformation()
  :fHitCount(0),fVetoHitCount(0),fPhotonCount_Scint(0),fPhotonCount_Ceren(0),fVetoPhotonCount_Ceren(0),fAbsorptionCount(0),fVetoAbsorptionCount(0),
   fBoundaryAbsorptionCount(0),fVetoBoundaryAbsorptionCount(0),fTotE(0.),fEWeightPos(0.),fReconPos(0.),fConvPos(0.),
   fConvPosSet(false),fPosMax(0.),fEdepMax(0.),fPMTsAboveThreshold(0),fVetoPMTsAboveThreshold(0) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeUserEventInformation::~LXeUserEventInformation() {}
