#include <math.h>
#include "TGraph.h"
#include <fstream>

void lgstyle(TLegend* lg){
  lg->SetTextFont(43);
  lg->SetTextSize(28);
  lg->SetFillStyle(0);
  lg->SetMargin(0.4);
  lg->SetBorderSize(0);
}

void grstyle(TGraph* gr){
    gr->SetMarkerStyle(24);
    gr->SetMarkerSize(1.2);
    gr->SetMarkerColor(kBlack);
    gr->GetYaxis()->SetTitleOffset(1.2);
    gr->GetXaxis()->SetTitleOffset(1.2);
    gr->GetXaxis()->CenterTitle();
    gr->GetYaxis()->CenterTitle();
    gr->GetXaxis()->SetTitleFont(43);
    gr->GetYaxis()->SetTitleFont(43);
    gr->GetXaxis()->SetLabelFont(43);
    gr->GetYaxis()->SetLabelFont(43);
    gr->GetXaxis()->SetLabelSize(28);
    gr->GetYaxis()->SetLabelSize(28);
    gr->GetXaxis()->SetTitleSize(28);
    gr->GetYaxis()->SetTitleSize(28);
    gr->SetTitle("");
}

void h1style(TH1* h1){
    h1->GetYaxis()->SetTitleOffset(1.3);
    h1->GetXaxis()->SetTitleOffset(1.2);
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetXaxis()->SetTitleFont(43);
    h1->GetYaxis()->SetTitleFont(43);
    h1->GetXaxis()->SetLabelFont(43);
    h1->GetYaxis()->SetLabelFont(43);
    h1->GetXaxis()->SetLabelSize(28);
    h1->GetYaxis()->SetLabelSize(28);
    h1->GetXaxis()->SetTitleSize(28);
    h1->GetYaxis()->SetTitleSize(28);
    h1->GetYaxis()->SetNdivisions(505, 1);
    h1->GetXaxis()->SetNdivisions(505, 1);
    h1->SetLineWidth(2);
    h1->SetTitle("");
}

Double_t getHitTimeFirstPeak(TString filename);
void scanRun(TString filename);
void scanEvent(TString filename);
Double_t getMeanHitTime(TString filename);
void plotHitTime(TString filename, TString figureName);
void plotSpeed();
void plotSpeedData();

void nova() {
    // gStyle->SetOptFit(1111);
    // gStyle->SetOptFit(0);
    // gStyle->SetOptStat("emr");
    // TString filename = "../cmake-build-debug/nova.root";

    // scanRun(filename);
    // scanEvent(filename);
    // plotHitTime("../cmake-build-debug/nova.1m.root", "1m");
    // plotHitTime("../cmake-build-debug/nova.2m.root", "2m");
    
    plotSpeed();
    // plotSpeedData();
    // cout << getHitTimeFirstPeak("../cmake-build-debug/nova.2deg.430nm.3m.root") << endl;
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
    grstyle(gr);
    gr->Fit("pol1");
    c1->SaveAs("figures/speed.data.pdf");
}

void plotSpeed() 
{
    vector<Double_t> lengths;
    vector<Double_t> meanHitTimes;
    TString configuration = "2deg.430nm";

    for (Int_t i = 1; i <= 5; i++) {
        lengths.push_back((Double_t) i);
        TString filename = TString::Format("../cmake-build-debug/nova.%s.%dm.root", configuration.Data(), i);
        meanHitTimes.push_back(getMeanHitTime(filename));
        // meanHitTimes.push_back(getHitTimeFirstPeak(filename));
        // plotHitTime(filename, TString::Format("%s.%dm", configuration.Data(), i));
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.15);

    const Int_t lengthCount = 5;
    Double_t times[lengthCount] = {68.4189, 73.7123, 79.6922, 85.5795, 91.3367};
    Double_t offset = times[1] - meanHitTimes[1];
    for (int i = 0; i < lengthCount; i++) {
        times[i] -= offset;
    }
    
    TGraph* grData = new TGraph(lengths.size(), times, &lengths[0]);
    grData->GetXaxis()->SetTitle("Time (ns)");
    grData->GetYaxis()->SetTitle("Length (m)");
    grstyle(grData);
    grData->SetMarkerStyle(20);
    grData->Draw("AP");
    grData->Fit("pol1");
    grData->GetXaxis()->SetLimits(20, 60);

    TGraph* gr = new TGraph(lengths.size(), &meanHitTimes[0], &lengths[0]);
    gr->GetXaxis()->SetTitle("Time (ns)");
    gr->GetYaxis()->SetTitle("Length (m)");
    grstyle(gr);
    gr->SetMarkerStyle(26);
    gr->SetMarkerSize(1.8);
    gr->Draw("P");

    TLegend* lg = new TLegend(0.15, 0.7, 0.5, 0.85);
    lg->AddEntry(grData, "Data", "p");
    lg->AddEntry(gr, "MC", "p");
    lgstyle(lg);
    lg->Draw();

    c1->Update();
    c1->Modified();
    c1->SaveAs(TString::Format("figures/speed.%s.pdf", configuration.Data()));
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

    h1style(hHitTime);
    hHitTime->GetXaxis()->SetTitle("Time (ns)");
    hHitTime->GetYaxis()->SetTitle("Hit Count");
    hHitTime->Draw();
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

