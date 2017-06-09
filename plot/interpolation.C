#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooPolynomial.h"
#include "RooHistPdf.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
using namespace RooFit;

TH1D* getHitTimeHist(TString filename)
{
  TFile* fileEvent = new TFile(filename);
  TTree* treeEvent = (TTree*) fileEvent->Get("hitTree");
  TBranch* branchEvent = treeEvent->GetBranch("hitStat");
  TLeaf* leafHitTime = branchEvent->GetLeaf("hitTime");
  TH1D* hHitTime = new TH1D("hHitTime", "hHitTime", 50, 0, 200);
  for (Int_t j = 0; j < branchEvent->GetEntries(); j++) {
    branchEvent->GetEntry(j);
    hHitTime->Fill(leafHitTime->GetValue());
  }
  return hHitTime;
}

void interpolation()
{
  TH1D* hHitTime1m = getHitTimeHist("../cmake-build-debug/run_cell.4.0m.1.0m.root");
  TH1D* hHitTime2m = getHitTimeHist("../cmake-build-debug/run_cell.2m.root");
  hHitTime1m->Scale(1. / hHitTime1m->Integral());
  hHitTime2m->Scale(1. / hHitTime2m->Integral());

  RooRealVar realVar("realVar" , "realVar", 0, 200);
  RooDataHist dataHist1m("dataHist1m", "dataHist1m", realVar, hHitTime1m);
  RooHistPdf histPdf1m("histPdf1m","histPdf1m", realVar, dataHist1m, 5);
  RooDataHist dataHist2m("dataHist2m", "dataHist2m", realVar, hHitTime2m);
  RooHistPdf histPdf2m("histPdf2m","histPdf2m", realVar, dataHist2m, 5);

  RooPlot* frame = realVar.frame(Title("realVar"), Bins(50));
  dataHist1m.plotOn(frame);
  histPdf1m.plotOn(frame);
  dataHist2m.plotOn(frame);
  histPdf2m.plotOn(frame);

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  // hHitTime1m->SetLineColor(kRed);
  // hHitTime1m->Draw();
  // hHitTime2m->Draw("sames");
  frame->Draw();
}
