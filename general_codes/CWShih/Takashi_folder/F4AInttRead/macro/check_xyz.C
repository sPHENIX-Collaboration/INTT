void check_xyz(){
  gStyle->SetOptStat(0);

  TFile *f = TFile::Open("AnaTutorial.root");
  //TFile *f = TFile::Open("AnaTutorial_before.root");


  TNtuple *ntp_clus = (TNtuple*)f->Get("ntp_clus");

  TCanvas *c1 = new TCanvas("c1", "c1", 1000, 500);
  c1->Divide(2,1);

  TH2* hxy = new TH2F("hxy", "hxy", 240, -12, 12, 240, -12,12);
  TH2* hrz = new TH2F("hrz", "hrz",  90, -30, 30, 240, -12,12);

  c1->cd(1);
  ntp_clus->Draw("y:x>>hxy", "", "colz");
  c1->cd(2);
  ntp_clus->Draw("(y<0?-1:1)*sqrt(y*y+x*x):z>>hrz", "", "colz");
}
