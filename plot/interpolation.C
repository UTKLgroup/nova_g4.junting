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

void setRooPlotStyle(RooPlot* frame)
{
  frame->SetMarkerStyle(24);
  frame->SetMarkerSize(1.2);
  frame->SetMarkerColor(kBlack);
  frame->GetYaxis()->SetTitleOffset(1.2);
  frame->GetXaxis()->SetTitleOffset(1.2);
  frame->GetXaxis()->CenterTitle();
  frame->GetYaxis()->CenterTitle();
  frame->GetXaxis()->SetTitleFont(43);
  frame->GetYaxis()->SetTitleFont(43);
  frame->GetXaxis()->SetLabelFont(43);
  frame->GetYaxis()->SetLabelFont(43);
  frame->GetXaxis()->SetLabelSize(28);
  frame->GetYaxis()->SetLabelSize(28);
  frame->GetXaxis()->SetTitleSize(28);
  frame->GetYaxis()->SetTitleSize(28);
  frame->SetTitle("");
}

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
  TH1D* hHitTime3m = getHitTimeHist("../cmake-build-debug/run_cell.4.0m.3.0m.root");
  hHitTime1m->Scale(1. / hHitTime1m->Integral());
  hHitTime2m->Scale(1. / hHitTime2m->Integral());
  hHitTime3m->Scale(1. / hHitTime3m->Integral());

  RooRealVar realVar("realVar" , "realVar", 0, 200);
  RooDataHist dataHist1m("dataHist1m", "dataHist1m", realVar, hHitTime1m);
  RooHistPdf histPdf1m("histPdf1m","histPdf1m", realVar, dataHist1m, 5);
  RooDataHist dataHist2m("dataHist2m", "dataHist2m", realVar, hHitTime2m);
  RooHistPdf histPdf2m("histPdf2m","histPdf2m", realVar, dataHist2m, 5);
  RooDataHist dataHist3m("dataHist3m", "dataHist3m", realVar, hHitTime3m);
  RooHistPdf histPdf3m("histPdf3m","histPdf3m", realVar, dataHist3m, 5);

  RooRealVar alpha("alpha", "alpha", 0, 1.0);
  realVar.setBins(100, "cache");
  alpha.setBins(10, "cache");
  RooIntegralMorph lmorph("lmorph", "lmorph", histPdf3m, histPdf1m, realVar, alpha);

  // hist
  // TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  // gPad->SetLeftMargin(0.15);
  // gPad->SetBottomMargin(0.15);
  // setRooPlotStyle(frameHist);

  // frameHist
  // RooPlot* frameHist = realVar.frame(Title("realVar"), Bins(50));
  // dataHist1m.plotOn(frameHist, Name(dataHist1m.GetName()), LineColor(kBlack), LineWidth(3), MarkerColor(kBlack));
  // histPdf1m.plotOn(frameHist, LineColor(kBlack));
  // dataHist2m.plotOn(frameHist, Name(dataHist2m.GetName()), LineColor(kRed), LineWidth(3), MarkerColor(kRed));
  // histPdf2m.plotOn(frameHist, LineColor(kRed));
  // dataHist3m.plotOn(frameHist, Name(dataHist3m.GetName()), LineColor(kBlue), LineWidth(3), MarkerColor(kBlue));
  // histPdf3m.plotOn(frameHist, LineColor(kBlue));

  // TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  // gPad->SetLeftMargin(0.15);
  // gPad->SetBottomMargin(0.15);
  // setRooPlotStyle(frameHist);

  // frameHist->GetXaxis()->SetTitle("Photon Hit Time (ns)");
  // frameHist->GetYaxis()->SetTitle("Photon Count (area norm.)");
  // frameHist->GetYaxis()->SetTitleOffset(1.5);
  // frameHist->GetXaxis()->SetRangeUser(0, 120);
  // frameHist->Draw();

  // TLegend* lg1 = new TLegend(0.6, 0.6, 0.89, 0.89);
  // setLegendStyle(lg1);
  // lg1->AddEntry(frameHist->findObject("dataHist1m"), "1 m","lpe");
  // lg1->AddEntry(frameHist->findObject("dataHist2m"), "2 m","lpe");
  // lg1->AddEntry(frameHist->findObject("dataHist3m"), "3 m","lpe");
  // lg1->Draw();

  // c1->SaveAs("figures/interpolation_hist.pdf");

  // frameMorph
  // RooPlot* frameMorph = realVar.frame(Title("realVar"), Bins(50));
  // histPdf1m.plotOn(frameMorph, Name(histPdf1m.GetName()), LineColor(kRed));
  // histPdf3m.plotOn(frameMorph, Name(histPdf3m.GetName()), LineColor(kRed + 4));
  // alpha.setVal(0.25);
  // lmorph.plotOn(frameMorph, LineColor(kRed + 1));
  // alpha.setVal(0.5);
  // lmorph.plotOn(frameMorph, LineColor(kRed + 2));
  // alpha.setVal(0.75);
  // lmorph.plotOn(frameMorph, LineColor(kRed + 3));

  // TCanvas* c2 = new TCanvas("c2", "c2", 800, 600);
  // gPad->SetLeftMargin(0.15);
  // gPad->SetBottomMargin(0.15);
  // setRooPlotStyle(frameMorph);

  // frameMorph->GetXaxis()->SetTitle("Photon Hit Time (ns)");
  // frameMorph->GetYaxis()->SetTitle("Photon Count (area norm.)");
  // frameMorph->GetYaxis()->SetTitleOffset(1.5);
  // frameMorph->GetXaxis()->SetRangeUser(0, 120);
  // frameMorph->Draw();

  // TLegend* lg2 = new TLegend(0.65, 0.6, 0.85, 0.85);
  // setLegendStyle(lg2);
  // lg2->SetMargin(0.5);
  // lg2->AddEntry(frameMorph->findObject(histPdf1m.GetName()), "1 m","l");
  // lg2->AddEntry(frameMorph->findObject(histPdf3m.GetName()), "3 m","l");
  // lg2->Draw();

  // c2->SaveAs("figures/interpolation_morph.pdf");

  // frameCompare
  // RooPlot* frameCompare = realVar.frame(Title("realVar"), Bins(50));
  // dataHist2m.plotOn(frameCompare, Name(dataHist2m.GetName()), LineColor(kBlack), LineWidth(3), MarkerColor(kBlack));
  // histPdf2m.plotOn(frameCompare, Name(histPdf2m.GetName()), LineColor(kBlack));
  // alpha.setVal(0.5);
  // lmorph.plotOn(frameCompare, Name(lmorph.GetName()), LineColor(kRed));

  // TCanvas* c3 = new TCanvas("c2", "c2", 800, 600);
  // gPad->SetLeftMargin(0.15);
  // gPad->SetBottomMargin(0.15);
  // setRooPlotStyle(frameCompare);

  // frameCompare->GetXaxis()->SetTitle("Photon Hit Time (ns)");
  // frameCompare->GetYaxis()->SetTitle("Photon Count (area norm.)");
  // frameCompare->GetYaxis()->SetTitleOffset(1.5);
  // frameCompare->GetXaxis()->SetRangeUser(0, 120);
  // frameCompare->Draw();

  // TLegend* lg3 = new TLegend(0.5, 0.7, 0.77, 0.87);
  // setLegendStyle(lg3);
  // lg3->SetMargin(0.5);
  // lg3->AddEntry(frameCompare->findObject(lmorph.GetName()), "2 m, morphed","l");
  // lg3->AddEntry(frameCompare->findObject(histPdf2m.GetName()), "2 m, simulation","l");
  // lg3->Draw();

  // c3->SaveAs("figures/interpolation_compare.pdf");
}
