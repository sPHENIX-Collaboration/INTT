void comp_phi_data_sim(){
  TDirectory *gDir = gDirectory;

  TH1* hphi_data[4];
  TH1* hphi_sim[4];
  for(int ilay=0; ilay<4; ilay++){
    hphi_data[ilay]= new TH1F(Form("hphi_data_%d", ilay), Form("hphi_data_%d", ilay), 700, -3.5, 3.5);
    hphi_sim[ilay] = new TH1F(Form("hphi_sim_%d",  ilay), Form("hphi_sim_%d",  ilay), 700, -3.5, 3.5);
  }
  

  TFile *fsim  = TFile::Open("AnaTutorial_SimZF.root");
  TFile *fdata = TFile::Open("AnaTutorial_run20869_newgeo_chinv.root");

  gDirectory = gDir;

  TFile* f[2] = {fdata, fsim};

  for(int ifile=0; ifile<2; ifile++){
    TTree *ntp_clus = (TTree*)f[ifile]->Get("ntp_clus");
    TString sflag = (ifile==0?"data":"sim");

    for(int ilay=0; ilay<4; ilay++){
      ntp_clus->Draw(Form("atan2(y,x)>>hphi_%s_%d", sflag.Data(), ilay), Form("lay==%d", ilay));
    }
  }

  TCanvas* c1 = new TCanvas("c1", "c1", 1200, 1000);
  c1->Divide(1,4);
  for(int ilay=0; ilay<4; ilay++){
    c1->cd(ilay+1);
    float max_data = hphi_data[ilay]->GetMaximum();
    float max_sim  = hphi_sim[ilay]->GetMaximum();

    float scale = max_data/max_sim;
    hphi_sim[ilay]->Scale(scale*0.7);

    hphi_sim[ilay]->SetLineColor(2);

    hphi_data[ilay]->Draw();
    hphi_sim[ilay]->Draw("same");
  }

/*
  ntp_clus->Draw("atan2(y,x)>>h(350,-3.5,3.5)", "lay==0")
  ntp_clus->Draw("atan2(y,x)>>h1(350,-3.5,3.5)", "lay==1")
  h->SetLineWidth(2)
  h1->SetLineColor(2)
  h->Draw()
  h1->Draw("same")
  ntp_clus->Draw("atan2(y,x)>>h2(350,-3.5,3.5)", "lay==2")
  ntp_clus->Draw("atan2(y,x)>>h3(350,-3.5,3.5)", "lay==3")
  h2->SetLineWidth(2)
  h3->SetLineWidth(3)
  h2->Draw()
  h3->Draw("same")
  h3->SetLineWidth(1)
  h3->SetLineColor(2)
  ntp_clus->Draw("lay:atan2(y,x)>>h3(350,-3.5,3.5,4,0,4)","","colz")
  h->Draw()
  ntp_clus->Draw("atan2(y,x)>>h(700,-3.5,3.5)", "lay==0")
  h->SetLineWidth(2)
  TFile *_file0 = TFile::Open("AnaTutorial_SimZF.root")
  ntp_clus->Draw("atan2(y,x)>>h(700,-3.5,3.5)", "lay==0")
  h->SetLineWidth(2)
  .q
  ntp_clus->Draw("atan2(y,x)>>h(700,-3.5,3.5)", "lay==0")
  h->SetLineColor(2)
  ntp_clus->Draw("atan2(y,x)>>h1(700,-3.5,3.5)", "lay==1")
  h1->SetLineColor(2)
*/
}
