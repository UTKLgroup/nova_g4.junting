
void plotSliceCount(TH1D* hSliceCountMc, TH1D* hSliceCountData);
void nomalizeBySliceCount(TH1D* hSliceDurationMc, TH1D* hSliceDurationData,
                           TH1D* hSliceCountMc, TH1D* hSliceCountData);
void nomalizeByEventCount(TH1D* hSliceDurationMc, TH1D* hSliceDurationData,
                          TH1D* hSliceCountMc, TH1D* hSliceCountData);
void plot(TH1D* hSliceDurationMc, TH1D* hSliceDurationData, TString outputFilename);

void timing()
{
  gStyle->SetOptStat("imrn");

  TFile* fData = new TFile("photon_timing_data_1.root");  
  TH1D* hSliceDurationData = (TH1D*) fData->Get("photontimingana/fSliceDuration");
  TH1D* hSliceCountData = (TH1D*) fData->Get("photontimingana/fSliceCount");

  // TFile* fMc = new TFile("photon_timing_mc_1.root");
  TFile* fMc = new TFile("photon_timing_mc.noiseless_rsim.root");
  TH1D* hSliceDurationMc = (TH1D*) fMc->Get("photontimingana/fSliceDuration");
  TH1D* hSliceCountMc = (TH1D*) fMc->Get("photontimingana/fSliceCount");

  nomalizeByEventCount(hSliceDurationMc, hSliceDurationData, hSliceCountMc, hSliceCountData);
  plot(hSliceDurationMc, hSliceDurationData, "nomalizeByEventCount.noiseless_rsim.pdf");

  // nomalizeBySliceCount(hSliceDurationMc, hSliceDurationData, hSliceCountMc, hSliceCountData);
  // plot(hSliceDurationMc, hSliceDurationData, "nomalizeBySliceCount.noiseless_rsim.pdf");

  // plotSliceCount(hSliceCountMc, hSliceCountData);
}

void plotSliceCount(TH1D* hSliceCountMc, TH1D* hSliceCountData)
{
  Int_t dataEventCount = hSliceCountData->Integral();
  Int_t mcEventCount = hSliceCountMc->Integral();
  hSliceCountData->Scale((Double_t) mcEventCount / dataEventCount);

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  setMargin();
  setH1Style(hSliceCountData);
  setH1Style(hSliceCountMc);

  hSliceCountMc->GetXaxis()->SetTitle("Slice Count (ns)");
  hSliceCountMc->GetYaxis()->SetTitle("Event Count");
  hSliceCountMc->SetName("MC");
  hSliceCountMc->SetLineColor(kBlue + 2);
  hSliceCountMc->GetXaxis()->SetRangeUser(0, 20);
  hSliceCountMc->GetYaxis()->SetRangeUser(0, 200);
  hSliceCountMc->Draw("hist");
  c1->Update();
  TPaveStats *pMc = (TPaveStats*) hSliceCountMc->GetListOfFunctions()->FindObject("stats");
  // hSliceCountMc->GetListOfFunctions()->Remove(pMc);
  // hSliceCountMc->SetStats(0);
  pMc->SetTextColor(kBlue + 2);
  pMc->SetLineColor(kBlue + 2);
  pMc->SetX1NDC(0.72);
  pMc->SetY1NDC(0.75);
  pMc->SetX2NDC(0.95);
  pMc->SetY2NDC(0.95);
  pMc->Draw();

  hSliceCountData->SetName("Data");
  hSliceCountData->SetLineColor(kRed + 2);
  hSliceCountData->Draw("sames");
  c1->Update();
  TPaveStats *pData = (TPaveStats*) hSliceCountData->GetListOfFunctions()->FindObject("stats");
  // hSliceCountData->GetListOfFunctions()->Remove(pData);
  // hSliceCountData->SetStats(0);
  pData->SetTextColor(kRed + 2);
  pData->SetLineColor(kRed + 2);
  pData->SetX1NDC(0.72);
  pData->SetY1NDC(0.51);
  pData->SetX2NDC(0.95);
  pData->SetY2NDC(0.71);
  pData->Draw();

  c1->SaveAs("figures/plotSliceCount.noiseless_rsim.pdf");
}

void nomalizeBySliceCount(TH1D* hSliceDurationMc, TH1D* hSliceDurationData,
                          TH1D* hSliceCountMc, TH1D* hSliceCountData)
{
  Int_t dataSliceCount = 0;
  Int_t mcSliceCount = 0;
  for (Int_t i = 1; i < hSliceCountMc->GetXaxis()->GetNbins(); i++) {
    mcSliceCount += hSliceCountMc->GetBinLowEdge(i) * hSliceCountMc->GetBinContent(i);
    dataSliceCount += hSliceCountData->GetBinLowEdge(i) * hSliceCountData->GetBinContent(i);    
  }

  hSliceDurationData->Scale((Double_t) mcSliceCount / dataSliceCount);
}

void nomalizeByEventCount(TH1D* hSliceDurationMc, TH1D* hSliceDurationData,
                          TH1D* hSliceCountMc, TH1D* hSliceCountData)
{
  Int_t dataEventCount = hSliceCountData->Integral();
  Int_t mcEventCount = hSliceCountMc->Integral();
  hSliceDurationData->Scale((Double_t) mcEventCount / dataEventCount);
}

void plot(TH1D* hSliceDurationMc, TH1D* hSliceDurationData, TString outputFilename)
{
  hSliceDurationMc->Rebin(2);
  hSliceDurationData->Rebin(2);

  TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
  setMargin();
  setH1Style(hSliceDurationData);
  setH1Style(hSliceDurationMc);

  hSliceDurationMc->GetXaxis()->SetTitle("Slice Duration (ns)");
  hSliceDurationMc->GetYaxis()->SetTitle("Slice Count");
  hSliceDurationMc->SetName("MC");
  hSliceDurationMc->SetLineColor(kBlue + 2);
  hSliceDurationMc->Draw("hist");
  c1->Update();
  TPaveStats *pMc = (TPaveStats*) hSliceDurationMc->GetListOfFunctions()->FindObject("stats");
  // hSliceDurationMc->GetListOfFunctions()->Remove(pMc);
  // hSliceDurationMc->SetStats(0);
  pMc->SetTextColor(kBlue + 2);
  pMc->SetLineColor(kBlue + 2);
  pMc->SetX1NDC(0.72);
  pMc->SetY1NDC(0.75);
  pMc->SetX2NDC(0.95);
  pMc->SetY2NDC(0.95);
  pMc->Draw();

  hSliceDurationData->SetName("Data");
  hSliceDurationData->SetLineColor(kRed + 2);
  hSliceDurationData->Draw("sames");
  c1->Update();
  TPaveStats *pData = (TPaveStats*) hSliceDurationData->GetListOfFunctions()->FindObject("stats");
  // hSliceDurationData->GetListOfFunctions()->Remove(pData);
  // hSliceDurationData->SetStats(0);
  pData->SetTextColor(kRed + 2);
  pData->SetLineColor(kRed + 2);
  pData->SetX1NDC(0.72);
  pData->SetY1NDC(0.51);
  pData->SetX2NDC(0.95);
  pData->SetY2NDC(0.71);
  pData->Draw();

  c1->SaveAs("figures/" + outputFilename);
}
