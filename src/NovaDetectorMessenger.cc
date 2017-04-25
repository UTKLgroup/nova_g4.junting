#include "NovaDetectorMessenger.hh"
#include "NovaDetectorConstruction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithoutParameter.hh"


NovaDetectorMessenger::NovaDetectorMessenger(NovaDetectorConstruction* detector)
    : fLXeDetector(detector)
{
  detectorDir = new G4UIdirectory("/Nova/detector/");
  detectorDir->SetGuidance("Detector geometry control");

  rectangleHeight = new G4UIcmdWithADoubleAndUnit("/Nova/detector/rectangleHeight", this);
  rectangleHeight->SetGuidance("Set the rectangle height.");
  rectangleHeight->SetParameterName("rectangleHeight", false);
  rectangleHeight->SetDefaultUnit("mm");

  rectangleWidth = new G4UIcmdWithADoubleAndUnit("/Nova/detector/rectangleWidth", this);
  rectangleWidth->SetGuidance("Set the rectangle width.");
  rectangleWidth->SetParameterName("rectangleWidth", false);
  rectangleWidth->SetDefaultUnit("mm");

  innerCellCornerRadius = new G4UIcmdWithADoubleAndUnit("/Nova/detector/innerCellCornerRadius", this);
  innerCellCornerRadius->SetGuidance("Set the inner cell corner radius.");
  innerCellCornerRadius->SetParameterName("innerCellCornerRadius", false);
  innerCellCornerRadius->SetDefaultUnit("mm");

  pvcThickness = new G4UIcmdWithADoubleAndUnit("/Nova/detector/pvcThickness", this);
  pvcThickness->SetGuidance("Set the thickness of the PVC.");
  pvcThickness->SetParameterName("pvcThickness", false);
  pvcThickness->SetDefaultUnit("mm");

  fiberRadius = new G4UIcmdWithADoubleAndUnit("/Nova/detector/fiberRadius", this);
  fiberRadius->SetGuidance("Set the fiber radius.");
  fiberRadius->SetParameterName("fiberRadius", false);
  fiberRadius->SetDefaultUnit("mm");

  cellLength = new G4UIcmdWithADoubleAndUnit("/Nova/detector/cellLength", this);
  cellLength->SetGuidance("Set the cell length.");
  cellLength->SetParameterName("cellLength", false);
  cellLength->SetDefaultUnit("cm");

  usePmt = new G4UIcmdWithABool("/Nova/detector/volumes/usePmt", this);
  usePmt->SetGuidance("Use PMT, else use APD");

  update = new G4UIcommand("/Nova/detector/update", this);
  update->SetGuidance("Update the detector geometry with new values.");
  update->SetGuidance("Must be run before beamOn if detector has been updated.");

  defaultValue = new G4UIcommand("/Nova/detector/defaultValue", this);
  defaultValue->SetGuidance("Set all detector geometry values to defaults.");

  liquidScintillatorLightYield=new G4UIcmdWithADouble("/Nova/detector/liquidScintillatorLightYield", this);
  liquidScintillatorLightYield->SetGuidance("Set the liquid scintillator light yield in number of photons per MeV.");

  printSetting = new G4UIcmdWithoutParameter("/Nova/detector/printSetting", this);
  printSetting->SetGuidance("Print the detector setting.");
}

NovaDetectorMessenger::~NovaDetectorMessenger()
{
  delete rectangleWidth;
  delete rectangleHeight;
  delete cellLength;
  delete pvcThickness;
  delete innerCellCornerRadius;
  delete fiberRadius;
  delete usePmt;
  delete update;
  delete defaultValue;
  delete liquidScintillatorLightYield;
  delete printSetting;
}

void NovaDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == rectangleWidth) {
    fLXeDetector->setRectWidth(rectangleWidth->GetNewDoubleValue(newValue));
  }
  else if (command == rectangleHeight) {
    fLXeDetector->setRectHeight(rectangleHeight->GetNewDoubleValue(newValue));
  }
  else if (command == cellLength) {
    fLXeDetector->setCellLength(cellLength->GetNewDoubleValue(newValue));
  }
  else if (command == pvcThickness) {
    fLXeDetector->setPVCThickness(pvcThickness->GetNewDoubleValue(newValue));
  }
  else if (command == innerCellCornerRadius) {
    fLXeDetector->setInnerCornerRadius(innerCellCornerRadius->GetNewDoubleValue(newValue));
  }
  else if (command == fiberRadius) {
    fLXeDetector->setFiberRadius(fiberRadius->GetNewDoubleValue(newValue));
  }
  else if (command == usePmt) {
    fLXeDetector->setUsePMT(usePmt->GetNewBoolValue(newValue));
  }
  else if (command == update) {
    fLXeDetector->updateGeometry();
  }
  else if (command == defaultValue) {
    fLXeDetector->setDefaults();
  }
  else if (command == liquidScintillatorLightYield) {
    fLXeDetector->setMainScintYield(liquidScintillatorLightYield->GetNewDoubleValue(newValue));
  }
  else if (command == printSetting) {
    fLXeDetector->printSettings();
  }
}
