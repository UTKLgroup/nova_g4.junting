#include <math.h>
#include "TGraph.h"
#include <fstream>

Double_t getHitTimeFirstPeak(TString filename);
Double_t getMeanHitTime(TString filename);
void scanRun(TString filename);
void scanEvent(TString filename);
void plotHitTime(TString filename, TString figureName);
void plotGpsEnergySpectrum(TString filename, TString figureName);
void plotSpeed();
void plotSpeedData();
void plotSpectrum();
void readCsvFile(TString filename, vector<Double_t>& wavelengths, vector<Double_t>& values, double valueUnit);
TString getFilePath(TString filename);
TH1D* getHitWavelengthHist(TString filename);
void normalizeTh1(TH1* h1);

void nova() {
//  gStyle->SetOptFit(1111);
//  gStyle->SetOptFit(0);
//  gStyle->SetOptStat("emr");
//  TString filename = "../cmake-build-debug/nova.root";
//
//  scanRun(filename);
//  scanEvent(filename);
//  plotHitTime("../cmake-build-debug/nova.1m.root", "1m");
//  plotHitTime("../cmake-build-debug/nova.2m.root", "2m");
//
 plotSpeed();
//  plotSpeedData();
 // plotSpectrum();
}

void plotSpectrum()
{
  gStyle->SetOptStat(0);
  Int_t wavelength = 470;

  vector<Double_t> data1mWavelengths;
  vector<Double_t> data1mIntensities;
  readCsvFile(TString::Format("led_%dnm_wls_spectrum_1m.csv", wavelength), data1mWavelengths, data1mIntensities, 1.0);
  TGraph* gr1m = new TGraph(data1mWavelengths.size(), &data1mWavelengths[0], &data1mIntensities[0]);

  vector<Double_t> data2mWavelengths;
  vector<Double_t> data2mIntensities;
  readCsvFile(TString::Format("led_%dnm_wls_spectrum_2m.csv", wavelength), data2mWavelengths, data2mIntensities, 1.0);
  TGraph* gr2m = new TGraph(data2mWavelengths.size(), &data2mWavelengths[0], &data2mIntensities[0]);

  vector<Double_t> data4mWavelengths;
  vector<Double_t> data4mIntensities;
  readCsvFile(TString::Format("led_%dnm_wls_spectrum_4m.csv", wavelength), data4mWavelengths, data4mIntensities, 1.0);
  TGraph* gr4m = new TGraph(data4mWavelengths.size(), &data4mWavelengths[0], &data4mIntensities[0]);

  // wavelength = 360;
  // wavelength = 470;
  // Int_t beamOnCount = 500000;
  // wavelength = 395;
  // Int_t beamOnCount = 100000;
  TH1D* h1m = getHitWavelengthHist(TString::Format("run_spectrum.%dnm.100cm.root", wavelength));
  TH1D* h2m = getHitWavelengthHist(TString::Format("run_spectrum.%dnm.200cm.root", wavelength));
  TH1D* h4m = getHitWavelengthHist(TString::Format("run_spectrum.%dnm.400cm.root", wavelength));

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  setMargin();
  Int_t integral1m = h1m->Integral();
  Int_t integral2m = h2m->Integral();
  Int_t integral4m = h4m->Integral();

  h1m->Scale(1. / integral1m * 0.21);
  h2m->Scale(1. / integral1m * 0.21);
  h4m->Scale(1. / integral1m * 0.21);

  setH1Style(h1m);
  h1m->GetXaxis()->SetTitle("Wavelength (m)");
  h1m->GetYaxis()->SetTitle("Relative Intensity (a.u.)");
  h1m->GetYaxis()->SetTitleOffset(1.6);
  h1m->SetLineColor(kBlack);
  h1m->Draw();

  h2m->SetLineColor(kBlue);
  setH1Style(h2m);
  h2m->Draw("sames");

  h4m->SetLineColor(kRed);
  setH1Style(h4m);
  h4m->Draw("sames");

  gr1m->SetLineColor(kBlack);
  gr1m->SetLineWidth(2);
  gr1m->Draw("C,sames");

  gr2m->SetLineColor(kBlue);
  gr2m->SetLineWidth(2);
  gr2m->Draw("C,sames");

  gr4m->SetLineColor(kRed);
  gr4m->SetLineWidth(2);
  gr4m->Draw("C,sames");

  TLegend* lg = new TLegend(0.63, 0.5, 0.87, 0.86);
  lg->AddEntry(h1m, "MC 1m", "le");
  lg->AddEntry(h2m, "MC 2m", "le");
  lg->AddEntry(h4m, "MC 4m", "le");

  lg->AddEntry(gr1m, "Data 1m", "l");
  lg->AddEntry(gr2m, "Data 2m", "l");
  lg->AddEntry(gr4m, "Data 4m", "l");

  setLegendStyle(lg);
  lg->Draw();

  c1->SaveAs("figures/plotSpectrum.pdf");
}

void normalizeTh1(TH1* h1)
{
  h1->Scale(1. / h1->Integral());
}

TH1D* getHitWavelengthHist(TString filename)
{
  TFile* fileEvent = new TFile(getFilePath(filename));
  TTree* treeEvent = (TTree*) fileEvent->Get("eventTree");
  TBranch* branchEvent = treeEvent->GetBranch("eventStat");
  TLeaf* leafHitWavelength = branchEvent->GetLeaf("hitWavelength");

  TH1D* hHitWavelength = new TH1D("hHitWavelength", "hHitWavelength", 220, 480, 700);
  for (Int_t j = 0; j < branchEvent->GetEntries(); j++) {
    branchEvent->GetEntry(j);
    hHitWavelength->Fill(leafHitWavelength->GetValue());
  }
  return hHitWavelength;
}

void readCsvFile(TString filename,
                 vector<Double_t>& wavelengths,
                 vector<Double_t>& values,
                 double valueUnit)
{
  string wavelengthString;
  string valueString;

  ifstream fCsv(getFilePath(filename));
  if (fCsv.is_open()) {
    while (getline(fCsv, wavelengthString, ',')) {
      getline(fCsv, valueString);
      wavelengths.push_back(stof(wavelengthString));
      values.push_back(stof(valueString) * valueUnit);
    }
  }
}

TString getFilePath(TString filename)
{
  return "../cmake-build-debug/" + filename;
}

void plotSpeedData()
{
  Double_t lengths[5] = {1., 2., 3., 4., 5.};
  Double_t times[5] = {68.4189, 73.7123, 79.6922, 85.5795, 91.3367};
  TGraph* gr = new TGraph(5, times, lengths);

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);
  gr->GetXaxis()->SetTitle("Time (ns)");
  gr->GetYaxis()->SetTitle("Length (m)");
  gr->Draw("AP");
  setGraphStyle(gr);
  gr->Fit("pol1");
  c1->SaveAs("figures/speed.data.pdf");
}

void plotSpeed()
{
  gStyle->SetOptStat("emr");
  // gStyle->SetOptFit(1111);
  vector<Double_t> lengths;
  vector<Double_t> meanHitTimes;
  // TString configuration = "400nm";
  // TString configuration = "430nm";
  // TString configuration = "2deg.430nm";
  // TString configuration = "0deg.470nm_spectrum";
  // TString configuration = "0deg.430nm_spectrum";
  // TString configuration = "0deg.395nm_spectrum";
  // TString configuration = "0deg.360nm_spectrum";
  // TString configuration = "0deg.360nm_spectrum.seed2";
  // TString configuration = "0deg.395nm_spectrum.seed2";
  // TString configuration = "0deg.430nm_spectrum.seed2";
  TString configuration = "395nm";

  for (Int_t i = 1; i <= 5; i++) {
    lengths.push_back((Double_t) i);
    TString filename = TString::Format("../cmake-build-debug/run_speed.395nm.%dcm.random_seed_1.root", i * 100);
    // meanHitTimes.push_back(getMeanHitTime(filename));
    meanHitTimes.push_back(getHitTimeFirstPeak(filename));
    // plotHitTime(filename, TString::Format("%s.%dm", configuration.Data(), i));
    plotHitTime(filename, TString::Format("%s.%dm", configuration.Data(), i));
    // if (i == 1)
    // plotGpsEnergySpectrum(filename, TString::Format("%s.%dm.gps_energy_spectrum", configuration.Data(), i));
  }

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  setMargin();

  const Int_t lengthCount = 5;
  Double_t times[lengthCount] = {68.4189, 73.7123, 79.6922, 85.5795, 91.3367};
  Double_t offset = times[0] - meanHitTimes[0];
  for (int i = 0; i < lengthCount; i++) {
    times[i] -= offset;
  }

  TGraph* grData = new TGraph(lengths.size(), times, &lengths[0]);
  grData->GetXaxis()->SetTitle("Time (ns)");
  grData->GetYaxis()->SetTitle("Length (m)");
  setGraphStyle(grData);
  grData->SetMarkerStyle(20);
  grData->Draw("AP");
  grData->Fit("pol1");
  // grData->GetXaxis()->SetLimits(20, 60);
  TF1* pol1Fit = grData->GetFunction("pol1");

  TGraph* gr = new TGraph(lengths.size(), &meanHitTimes[0], &lengths[0]);
  gr->GetXaxis()->SetTitle("Time (ns)");
  gr->GetYaxis()->SetTitle("Length (m)");
  setGraphStyle(gr);
  gr->SetMarkerStyle(26);
  gr->SetMarkerSize(1.8);
  gr->Draw("P");

  TLegend* lg = new TLegend(0.18, 0.7, 0.5, 0.85);
  lg->AddEntry(grData, "Data", "p");
  lg->AddEntry(gr, "MC", "p");
  lg->AddEntry(pol1Fit, "Fit to data, 0.173 m/ns", "l");
  setLegendStyle(lg);
  lg->SetMargin(0.3);
  lg->Draw();

  c1->Update();
  c1->Modified();
  // c1->SaveAs(TString::Format("figures/speed.%s.pdf", configuration.Data()));
  c1->SaveAs(TString::Format("figures/plotSpeed.pdf"));
}

Double_t getHitTimeFirstPeak(TString filename)
{
  TFile* fileEvent = new TFile(filename);
  TTree* treeEvent = (TTree*) fileEvent->Get("eventTree");
  TBranch* branchEvent = treeEvent->GetBranch("eventStat");
  TLeaf* leafHitTime = branchEvent->GetLeaf("hitTime");

  TH1D* hHitTime = new TH1D("hHitTime", "hHitTime", 100, 0, 100);
  for (Int_t j = 0; j < branchEvent->GetEntries(); j++) {
    branchEvent->GetEntry(j);
    hHitTime->Fill(leafHitTime->GetValue());
  }
  return hHitTime->GetBinCenter(hHitTime->GetMaximumBin());
}

Double_t getMeanHitTime(TString filename)
{
  TFile* fileEvent = new TFile(filename);
  TTree* treeEvent = (TTree*) fileEvent->Get("eventTree");
  TBranch* branchEvent = treeEvent->GetBranch("eventStat");
  TLeaf* leafHitTime = branchEvent->GetLeaf("hitTime");

  Double_t meanHitTime = 0.0;
  for (Int_t j = 0; j < branchEvent->GetEntries(); j++) {
    branchEvent->GetEntry(j);
    meanHitTime += leafHitTime->GetValue();
  }
  meanHitTime /= branchEvent->GetEntries();
  return meanHitTime;
}

void plotHitTime(TString filename, TString figureName)
{
  TFile* fileEvent = new TFile(filename);
  TTree* treeEvent = (TTree*) fileEvent->Get("eventTree");
  TBranch* branchEvent = treeEvent->GetBranch("eventStat");
  TLeaf* leafHitTime = branchEvent->GetLeaf("hitTime");

  TH1D* hHitTime = new TH1D("hHitTime", "hHitTime", 100, 0, 100);
  for (Int_t j = 0; j < branchEvent->GetEntries(); j++) {
    branchEvent->GetEntry(j);
    hHitTime->Fill(leafHitTime->GetValue());
  }

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);
  gPad->SetLogy();

  setH1Style(hHitTime);
  hHitTime->GetXaxis()->SetTitle("Time (ns)");
  hHitTime->GetYaxis()->SetTitle("Hit Count");
  hHitTime->SetName("");
  hHitTime->SetTitle(figureName);
  hHitTime->Draw();
  c1->SaveAs(TString::Format("figures/%s.pdf", figureName.Data()));
}

void plotGpsEnergySpectrum(TString filename, TString figureName)
{
  TFile* fileRun = new TFile(filename);
  TTree* treeRun = (TTree*) fileRun->Get("runTree");
  TBranch* branchRun = treeRun->GetBranch("runStat");
  TLeaf* leafPrimaryZ = branchRun->GetLeaf("primaryPZ");

  TH1D* hPrimaryZ = new TH1D("hPrimaryZ", "hPrimaryZ", 200, 300, 500);
  for (Int_t j = 0; j < branchRun->GetEntries(); j++) {
    branchRun->GetEntry(j);
    Double_t primaryPZ = -leafPrimaryZ->GetValue();
    hPrimaryZ->Fill(1239.84193 / primaryPZ);
  }

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  gPad->SetLeftMargin(0.15);
  gPad->SetBottomMargin(0.15);

  setH1Style(hPrimaryZ);
  hPrimaryZ->GetXaxis()->SetTitle("Energy (eV)");
  hPrimaryZ->GetYaxis()->SetTitle("Primary Photon Count");
  hPrimaryZ->Draw();
  c1->SaveAs(TString::Format("figures/%s.pdf", figureName.Data()));
}

void scanRun(TString filename)
{
  TFile* fileRun = new TFile(filename);
  TTree* treeRun = (TTree*) fileRun->Get("runTree");
  TBranch* branchRun = treeRun->GetBranch("runStat");
  TLeaf* leafHitCount = branchRun->GetLeaf("hitCount");
  TLeaf* leafAbsorptionCount = branchRun->GetLeaf("absorptionCount");

  for (Int_t j = 0; j < branchRun->GetEntries(); j++) {
    branchRun->GetEntry(j);
    double hitCount = leafHitCount->GetValue();
    double absorptionCount = leafAbsorptionCount->GetValue();

    cout << hitCount << endl;
    cout << absorptionCount << endl;
  }
}

void scanEvent(TString filename)
{
  TFile* fileEvent = new TFile(filename);
  TTree* treeEvent = (TTree*) fileEvent->Get("eventTree");
  TBranch* branchEvent = treeEvent->GetBranch("eventStat");
  TLeaf* leafHitTime = branchEvent->GetLeaf("hitTime");
  TLeaf* leafHitZ = branchEvent->GetLeaf("hitZ");
  TLeaf* leafHitWavelength = branchEvent->GetLeaf("hitWavelength");

  for (Int_t j = 0; j < branchEvent->GetEntries(); j++) {
    branchEvent->GetEntry(j);
    double hitTime = leafHitTime->GetValue();
    double hitZ = leafHitZ->GetValue();
    double hitWavelength = leafHitWavelength->GetValue();

    cout << hitTime << endl;
    cout << hitZ << endl;
    cout << hitWavelength << endl;
  }
}

