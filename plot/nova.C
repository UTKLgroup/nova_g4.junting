#include <math.h>
#include "TGraph.h"
#include <fstream>

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

void scanRun(TString filename);
void scanEvent(TString filename);

void nova() {
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat("emr");
    TString filename = "../cmake-build-debug/nova.root";

    scanRun(filename);
    // scanEvent(filename);
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

