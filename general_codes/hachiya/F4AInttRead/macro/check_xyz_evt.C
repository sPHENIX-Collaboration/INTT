void check_xyz_evt(int ievent=0){
  gStyle->SetOptStat(0);

  TFile *f = TFile::Open("AnaTutorial_run20869ZF_lo_BCyp5mm.root");
  //TFile *f = TFile::Open("AnaTutorial.root");
  //TFile *f = TFile::Open("AnaTutorial_run20864all.root");
  //TFile *f = TFile::Open("AnaTutorial_run20864.root");
  //TFile *f = TFile::Open("AnaTutorial.root");
  //TFile *f = TFile::Open("AnaTutorial_5000.root");
  //TFile *f = TFile::Open("AnaTutorial_before.root");


  TNtuple *ntp_clus = (TNtuple*)f->Get("ntp_clus");

  TCanvas *c1 = new TCanvas("c1", "c1", 1500, 500);
  c1->Divide(3,1);


  TH2* hxy = new TH2F("hxy", Form("y vs x : %d", ievent), 60, -12, 12, 60, -12,12);
  hxy->SetMarkerStyle(20);
  hxy->SetMarkerSize(1);
  TH2* hrz = new TH2F("hrz", Form("r vs z : %d", ievent), 60, -30, 30, 60, -12,12);
  hrz->SetMarkerStyle(20);
  hrz->SetMarkerSize(1);
  TH3* hxyz = new TH3F("hxyz", Form("x vs y vs z : %d", ievent), 60, -30, 30, 60, -12, 12, 60, -12,12);
  hxyz->SetMarkerStyle(20);
  hxyz->SetMarkerSize(1);

  c1->cd(1);
  ntp_clus->Draw("y:x>>hxy",                        Form("size<5&&40<adc&&adc<200&&evt==%d", ievent));
  c1->cd(2);                                                    
  ntp_clus->Draw("(y<0?-1:1)*sqrt(y*y+x*x):z>>hrz", Form("size<5&&40<adc&&adc<200&&evt==%d", ievent));
  c1->cd(3);                                                    
  ntp_clus->Draw("y:x:z>>hxyz", Form("size<5&&40<adc&&adc<200&&evt==%d", ievent));
}
