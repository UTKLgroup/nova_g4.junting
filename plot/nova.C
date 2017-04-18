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

void nova() {
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat("emr");
        
    TFile* f1 = new TFile("nova.root");
    TTree* t1 = (TTree*) f1->Get("eventTree");
    TBranch* branch = t1->GetBranch("eventStat");
    TLeaf* lHitTime = branch->GetLeaf("hitTime");
    TLeaf* lHitZ = branch->GetLeaf("hitZ");
    TLeaf* lHitWavelength = branch->GetLeaf("hitWavelength");

    for(Int_t j = 0; j < branch->GetEntries(); j++) {
        branch->GetEntry(j);
        double fHitTime = lHitTime->GetValue();
        double fHitZ = lHitZ->GetValue();
        double fHitWavelength = lHitWavelength->GetValue();

        cout << fHitTime << endl;
        cout << fHitZ << endl;
        cout << fHitWavelength << endl;
    }
}
