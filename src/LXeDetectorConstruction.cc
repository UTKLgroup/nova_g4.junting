#include "LXeDetectorConstruction.hh"
#include "LXePMTSD.hh"
#include "LXeMainVolume.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4UImanager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4NistManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeDetectorConstruction::LXeDetectorConstruction()
{
    fScintMPT = NULL;
    fExperimentalHall_box = NULL;
    fExperimentalHall_log = NULL;
    fExperimentalHall_phys = NULL;
    fTiO2 = fPVC = flscint = NULL;
    fO = fC = fH = fTi = NULL;
    fUpdated = false;

    SetDefaults();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeDetectorConstruction::~LXeDetectorConstruction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::DefineMaterials(){
    //***Elements

    fH = new G4Element("H", "H", 1., 1.01*g/mole);
    fC = new G4Element("C", "C", 6., 12.01*g/mole);
    fO = new G4Element("O", "O", 8., 16.00*g/mole);
    fTi = new G4Element( "Ti", "Ti", 22., 47.87*g/mole);

    //Vacuum
    fVacuum = new G4Material("Vacuum",
                             1.,
                             1.01*g/mole,
                             universe_mean_density,
                             kStateGas,
                             0.1*kelvin,
                             1.e-19*pascal);

    // TiO2
    fTiO2 = new G4Material("TiO2", 4.23*g/cm3, 2, kStateSolid);
    fTiO2->AddElement(fO, 2);
    fTiO2->AddElement(fTi, 1);


    // liquid scintillator
    flscint = new G4Material("liquid_scintillator",  1.032*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
    flscint->AddElement(fH, 0.666);
    flscint->AddElement(fC, 0.334);

    // Polystyrene
    Polystyrene = new G4Material("Polystyrene",  1.05*g/cm3, 2, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
    Polystyrene->AddElement(fH, 0.498);
    Polystyrene->AddElement(fC, 0.502);

    // PMMA
    PMMA = new G4Material("PMMA", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
    PMMA->AddElement(fH, 0.532);
    PMMA->AddElement(fC, 0.336);
    PMMA->AddElement(fO, 0.132);

    // Fluorinated Polymer
    Fluorinated_Polymer = new G4Material("Fluorinated_Polymer", 1.19*g/cm3, 3, kStateSolid, 273.15*kelvin, 1.0*atmosphere);
    Fluorinated_Polymer->AddElement(fH, 0.532);
    Fluorinated_Polymer->AddElement(fC, 0.336);
    Fluorinated_Polymer->AddElement(fO, 0.132);

    // PVC
    fPVC = new G4Material("PVC", 1.4316*g/cm3, 2, kStateSolid);
    fPVC->AddMaterial(fTiO2, 0.15);
    fPVC->AddMaterial(Polystyrene, 0.85);

    //Glass
    fGlass = new G4Material("Glass", 1.032*g/cm3, 2);
    fGlass->AddElement(fC, 91.533*perCent);
    fGlass->AddElement(fH, 8.467*perCent);

    //***Material properties tables

    // glass
    const G4int lxenum = 2;
    G4double lxe_Energy[lxenum] = {1240. / 200. * eV, 1240. / 700. * eV};
    G4double glass_RIND[lxenum] = {1.49, 1.49};
    G4double glass_AbsLength[lxenum] = {420.*cm, 420.*cm};
    G4MaterialPropertiesTable *glass_mt = new G4MaterialPropertiesTable();
    glass_mt->AddProperty("ABSLENGTH", lxe_Energy, glass_AbsLength, lxenum);
    glass_mt->AddProperty("RINDEX", lxe_Energy, glass_RIND, lxenum);
    fGlass->SetMaterialPropertiesTable(glass_mt);

    // liquid scintillator

    fScintMPT = new G4MaterialPropertiesTable();
    G4double ScintRIndexConst = 1.47;
    G4double VacAbsConst = 100.*m;
    G4double wavelength;
    G4double variable;
    G4String filler;

    std::vector<G4double> VacIndexE;
    std::vector<G4double> VacIndex;

    std::vector<G4double> VacAbsE;
    std::vector<G4double> VacAbs;

    std::vector<G4double> ScintRIndexE;
    std::vector<G4double> ScintRIndex;

    std::vector<G4double> ScintAbsE;
    std::vector<G4double> ScintAbs;

    std::vector<G4double> ScintEmitE;
    std::vector<G4double> ScintEmitFast;
    std::vector<G4double> ScintEmitSlow;

    std::vector<G4double> ScintWLSabsE;
    std::vector<G4double> ScintWLSabs;

    std::vector<G4double> ScintWLSemitE;
    std::vector<G4double> ScintWLSemit;

    // scintillator emission (fast and slow)
    std::ifstream ReadScint;
    ReadScint.open(InputDir + "ppo_emission.txt");
    if(ReadScint.is_open()){
        while(!ReadScint.eof()){
            ReadScint >> wavelength >> filler >> variable;
            ScintEmitE.push_back(1240 / wavelength * eV);
            ScintEmitFast.push_back(variable);
            ScintEmitSlow.push_back(0.);

            ScintRIndexE.push_back(1240 / wavelength * eV);
            ScintRIndex.push_back(ScintRIndexConst);

            VacIndexE.push_back(1240 / wavelength * eV);
            VacIndex.push_back(1.);

            VacAbsE.push_back(1240 / wavelength * eV);
            VacAbs.push_back(VacAbsConst);
        }
    }
    else
        G4cout << "Error opening file: "  << G4endl;
    ReadScint.close();

    // vacuum
    vacMPT = new G4MaterialPropertiesTable();
    vacMPT->AddProperty("RINDEX", &VacIndexE[0], &VacIndex[0], (G4int) VacIndexE.size());
    vacMPT->AddProperty("ABSLENGTH", &VacAbsE[0], &VacAbs[0], (G4int) VacAbsE.size());
    fVacuum->SetMaterialPropertiesTable(vacMPT);

    // bulk absorption  
    std::ifstream Readabsorb;
    Readabsorb.open(InputDir + "PSTBulkAbsorb.cfg");
    if(Readabsorb.is_open()){
        while(!Readabsorb.eof()){
            Readabsorb >> wavelength >> filler >> variable;
            ScintAbsE.push_back(1240 / wavelength * eV);
            ScintAbs.push_back(1.8 * variable * m);
        }
    }
    else
        G4cout << "Error opening file: " << G4endl;
    Readabsorb.close();

    // WLS absorption  
    std::ifstream ReadWLSa;
    ReadWLSa.open(InputDir + "UPS923.cfg");
    if (ReadWLSa.is_open()){
        while (!ReadWLSa.eof()){
            ReadWLSa >> wavelength >> filler >> variable;
            ScintWLSabsE.push_back(1240 / wavelength * eV);
            ScintWLSabs.push_back(1.8 * variable * m);
        }
    }
    else
        G4cout << "Error opening file: " << G4endl;
    ReadWLSa.close();

    // WLS emission  
    std::ifstream ReadWLSe;
    ReadWLSe.open(InputDir + "bisMSB_emission.txt");
    if(ReadWLSe.is_open()){
        while (!ReadWLSe.eof()){
            ReadWLSe >> wavelength >> filler >> variable;
            ScintWLSemitE.push_back(1240 / wavelength * eV);
            ScintWLSemit.push_back(variable);
        }
    }
    else
        G4cout << "Error opening file: " << G4endl;
    ReadWLSe.close();

    fScintMPT->AddProperty(     "RINDEX",        &ScintRIndexE[0],  &ScintRIndex[0],   ScintRIndexE.size());
    fScintMPT->AddProperty(     "ABSLENGTH",     &ScintAbsE[0],     &ScintAbs[0],      ScintAbsE.size());
    fScintMPT->AddProperty(     "FASTCOMPONENT", &ScintEmitE[0],    &ScintEmitFast[0], ScintEmitE.size());
    fScintMPT->AddProperty(     "SLOWCOMPONENT", &ScintEmitE[0],    &ScintEmitSlow[0], ScintEmitE.size());
    fScintMPT->AddProperty(     "WLSABSLENGTH",  &ScintWLSabsE[0],  &ScintWLSabs[0],   ScintWLSabsE.size());
    fScintMPT->AddProperty(     "WLSCOMPONENT",  &ScintWLSemitE[0], &ScintWLSemit[0],  ScintWLSemitE.size());
    fScintMPT->AddConstProperty("SCINTILLATIONYIELD",   fScintLightYield / MeV);
    fScintMPT->AddConstProperty("CONSTANTQUANTUMYIELD", 0.9);
    fScintMPT->AddConstProperty("RESOLUTIONSCALE",      1.);
    fScintMPT->AddConstProperty("FASTTIMECONSTANT",     2.1*ns);
    fScintMPT->AddConstProperty("SLOWTIMECONSTANT",     10.*ns);
    fScintMPT->AddConstProperty("WLSTIMECONSTANT",      1.*ns);
    fScintMPT->AddConstProperty("YIELDRATIO",           1.);
    flscint->SetMaterialPropertiesTable(fScintMPT);
    flscint->GetIonisation()->SetBirksConstant(0.126*mm/MeV);



    // fiber

    G4double coreIndexconst = 1.59;
    G4double InCladRIndexConst = 1.49;
    G4double OutCladRIndexConst = 1.42;

    std::vector < G4double > CoreWLSabsE;
    std::vector < G4double > CoreWLSabs;

    std::vector < G4double > CoreWLSemitE;
    std::vector < G4double > CoreWLSemit;

    std::vector < G4double > RIndexE;
    std::vector < G4double > CoreRIndex;
    std::vector < G4double > InCladRIndex;
    std::vector < G4double > OutCladRIndex;

    std::vector < G4double > CoreAbsE;
    std::vector < G4double > CoreAbs;

    std::vector < G4double > CladAbsE;
    std::vector < G4double > CladAbs;

    // WLS absorption
    ReadWLSa.open(InputDir + "y11_abs_length.dat");
    if (ReadWLSa.is_open()){
        while(!ReadWLSa.eof()){
            ReadWLSa >> wavelength >> filler >> variable;

            CoreWLSabsE.push_back(1240 / wavelength * eV);
            CoreWLSabs.push_back(variable * m);

            RIndexE.push_back(1240 / wavelength * eV);
            CoreRIndex.push_back(coreIndexconst);
            InCladRIndex.push_back(InCladRIndexConst);
            OutCladRIndex.push_back(OutCladRIndexConst);
        }
    }
    else
        G4cout<<"Error opening file: "<< "y11_abs_length.dat" <<G4endl;
    ReadWLSa.close();

    // WLS emission
    ReadWLSe.open(InputDir + "WLSemit.cfg");
    if(ReadWLSe.is_open()){
        while(!ReadWLSe.eof()){
            ReadWLSe >> wavelength >> filler >> variable;
            CoreWLSemitE.push_back(1240 / wavelength * eV);
            CoreWLSemit.push_back(variable);
        }
    }
    else
        G4cout<<"Error opening file: "<< "WLSemit.cfg" <<G4endl;
    ReadWLSe.close();

    // core bulk absorption
    std::ifstream ReadBulk;
    ReadBulk.open(InputDir + "fiberPSTabsorb.dat");
    if(ReadBulk.is_open()){
        while(!ReadBulk.eof()){
            ReadBulk >> wavelength >> filler >> variable;
            CoreAbsE.push_back(1240 / wavelength * eV);
            CoreAbs.push_back(variable * m);
        }
    }
    else
        G4cout<<"Error opening file: "<< "fiberPSTabsorb.dat" <<G4endl;
    ReadBulk.close();

    // first cladding bulk absorption
    std::ifstream Read_pmma_Bulk;
    Read_pmma_Bulk.open(InputDir + "PMMABulkAbsorb.dat");
    if(Read_pmma_Bulk.is_open()){
        while(!Read_pmma_Bulk.eof()){
            Read_pmma_Bulk >> wavelength >> filler >> variable;
            CladAbsE.push_back(1240 / wavelength * eV);
            CladAbs.push_back(variable * m);
        }
    }
    else
        G4cout << "Error opening file: " << "PMMABulkAbsorb.dat" << G4endl;
    Read_pmma_Bulk.close();

    //---------- set MPT    
    wlsMPT = new G4MaterialPropertiesTable();
    wlsMPT->AddProperty("RINDEX",       &RIndexE[0],      &CoreRIndex[0],  RIndexE.size());
    wlsMPT->AddProperty("ABSLENGTH",    &CoreAbsE[0],     &CoreAbs[0],     CoreAbsE.size());
    wlsMPT->AddProperty("WLSABSLENGTH", &CoreWLSabsE[0],  &CoreWLSabs[0],  CoreWLSabsE.size());
    wlsMPT->AddProperty("WLSCOMPONENT", &CoreWLSemitE[0], &CoreWLSemit[0], CoreWLSemitE.size());
    wlsMPT->AddConstProperty("WLSTIMECONSTANT",      11.8*ns);
    wlsMPT->AddConstProperty("CONSTANTQUANTUMYIELD", 0.7);
    Polystyrene->SetMaterialPropertiesTable(wlsMPT);
    //wlsMPT->DumpTable();

    innercladMPT = new G4MaterialPropertiesTable();
    innercladMPT->AddProperty("RINDEX",    &RIndexE[0],  &InCladRIndex[0], RIndexE.size());
    innercladMPT->AddProperty("ABSLENGTH", &CladAbsE[0], &CladAbs[0],      CladAbsE.size());
    PMMA->SetMaterialPropertiesTable(innercladMPT);

    outercladMPT = new G4MaterialPropertiesTable();
    outercladMPT->AddProperty("RINDEX",    &RIndexE[0],  &OutCladRIndex[0], RIndexE.size());
    outercladMPT->AddProperty("ABSLENGTH", &CladAbsE[0], &CladAbs[0],       CladAbsE.size());
    Fluorinated_Polymer->SetMaterialPropertiesTable(outercladMPT);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* LXeDetectorConstruction::Construct(){
    DefineMaterials();
    return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* LXeDetectorConstruction::ConstructDetector()
{
    //The experimental hall walls are all 1m away from housing walls
    G4double expHall_x = GetCellWidth() / 2.  + 20.*cm;
    G4double expHall_y = GetCellHeight() / 2.  + 20.*cm;
    G4double expHall_z = GetCelltoPMTL() + 20.*cm;

    //Create experimental hall
    fExperimentalHall_box  = new G4Box("expHall_box", expHall_x, expHall_y, expHall_z);
    fExperimentalHall_log  = new G4LogicalVolume(fExperimentalHall_box, fVacuum, "expHall_log", 0, 0, 0);
    fExperimentalHall_phys = new G4PVPlacement(0, G4ThreeVector(), fExperimentalHall_log, "expHall", 0, false, 0);
    fExperimentalHall_log->SetVisAttributes(G4VisAttributes::Invisible);

    //Place the main volume
    if(fMainVolume){
        new LXeMainVolume(0, G4ThreeVector(), fExperimentalHall_log,false,0,this);
    }

    return fExperimentalHall_phys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::PrintSettings(){
    G4cout << std::setw(25) << "fScintLightYield = "     << std::setw(10) << fScintMPT->GetConstProperty("SCINTILLATIONYIELD") << std::setw(10) << " MeV-1" << G4endl;
    G4cout << std::setw(25) << "fCellLength = "          << std::setw(10) << fCellLength          / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "fRectWidth = "           << std::setw(10) << fRectWidth           / mm << std::setw(10) << " mm" << G4endl;
    G4cout << std::setw(25) << "fRectHeight = "          << std::setw(10) << fRectHeight          / mm << std::setw(10) << " mm" << G4endl;
    G4cout << std::setw(25) << "fInner_Corner_Radius = " << std::setw(10) << fInner_Corner_Radius / mm << std::setw(10) << " mm" << G4endl;
    G4cout << std::setw(25) << "fPVCThickness = "        << std::setw(10) << fPVCThickness        / mm << std::setw(10) << " mm" << G4endl;
    G4cout << std::setw(25) << "fFibRadius = "           << std::setw(10) << fFibRadius           / mm << std::setw(10) << " mm" << G4endl;
    G4cout << std::setw(25) << "fFib1X = "               << std::setw(10) << fFib1X               / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "fFib1Y = "               << std::setw(10) << fFib1Y               / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "fFib2X = "               << std::setw(10) << fFib2X               / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "fFib2Y = "               << std::setw(10) << fFib2Y               / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "fCelltoPMTL = "          << std::setw(10) << fCelltoPMTL          / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "fFibTail = "             << std::setw(10) << fFibTail             / cm << std::setw(10) << " cm" << G4endl;
    G4cout << std::setw(25) << "UsePMT = "               << std::setw(10) << UsePMT << G4endl;
    G4cout << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::SetDefaults(){

    fScintLightYield     = 100.;
    fRectHeight          = 40.0*mm;
    fRectWidth           = 17.7*mm;
    fInner_Corner_Radius = 9.7*mm;
    fPVCThickness        = 3.3*mm;
    fFibRadius           = 0.35*mm;
    fCellLength          = 120.*cm;
    fFib1X               = 1*cm;
    fFib1Y               = 0;
    fFib2X               = -1*cm;
    fFib2Y               = 0;
    fCelltoPMTL          = 100.*cm;
    fFibTail             = 10.*cm;
    UsePMT               = true;
    fMainVolume          = true;
    InputDir             = "/Users/juntinghuang/Desktop/nova/input/";

    G4UImanager::GetUIpointer()->ApplyCommand("/LXe/detector/scintYieldFactor 1.");

    fUpdated=true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeDetectorConstruction::UpdateGeometry(){

    // clean-up previous geometry
    G4GeometryManager::GetInstance()->OpenGeometry();

    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
    G4SurfaceProperty::CleanSurfacePropertyTable();

    //define new one
    G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());
    G4RunManager::GetRunManager()->GeometryHasBeenModified();

    fUpdated=false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void LXeDetectorConstruction::SetMainScintYield(G4double y){
    fScintMPT->AddConstProperty("SCINTILLATIONYIELD",y/MeV);
}
