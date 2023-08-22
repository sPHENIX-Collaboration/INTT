void check_ladder_xyz(){
  gStyle->SetOptStat(0);

  TDirectory* gDir = gDirectory;
  TH2* hgeo = new TH2F("hgeo", "ladder", 400, -12, 12, 400, -12,12);
  //TFile *f = TFile::Open("AnaTutorial_dummy_shift.root");
  TFile *f1 = TFile::Open("AnaTutorial_run20869_newgeo.root");
  gDirectory = gDir;
  TTree* ntp_clus1 = (TNtuple*)f1->Get("ntp_clus");
  ntp_clus1->Draw("y:x>>hgeo", "evt<10", "colz");
  f1->Close();

  TFile *f = TFile::Open("AnaTutorial.root");
  //TFile *f = TFile::Open("AnaTutorial_before.root");


  TNtuple *ntp_clus = (TNtuple*)f->Get("ntp_clus");

  float bco_full, evt, size, adc, x,y,z, lay, lad, sen;

  ntp_clus->SetBranchAddress("bco_full", &bco_full);
  ntp_clus->SetBranchAddress("evt",      &evt);
  ntp_clus->SetBranchAddress("size",     &size);
  ntp_clus->SetBranchAddress("adc",      &adc);
  ntp_clus->SetBranchAddress("x",        &x);
  ntp_clus->SetBranchAddress("y",        &y);
  ntp_clus->SetBranchAddress("z",        &z);
  ntp_clus->SetBranchAddress("lay",      &lay);
  ntp_clus->SetBranchAddress("lad",      &lad);
  ntp_clus->SetBranchAddress("sen",      &sen);
  //h_ntp_clus = new TNtuple("ntp_clus", "Cluster Ntuple", "nclus:bco_full:evt:size:adc:x:y:z:lay:lad:sen");
  //
  //
  //
  //
  
  float xyz[8][14][3];
  for(int ievt=0; ievt<ntp_clus->GetEntries(); ievt++){
    ntp_clus->GetEntry(ievt);

    if(evt<8) continue;
    int evtno = evt - 8;
    int pid = (evtno)/14;
    int lad = (evtno)%14;

    xyz[pid][lad][0] = x;
    xyz[pid][lad][1] = y;
    xyz[pid][lad][2] = z;

    cout<<"xyz: "<<evt<<" "<<x<<" "<<y<<" "<<z<<endl;
  }

  TCanvas *c1 = new TCanvas("c1", "c1", 1000, 1000);
  c1->Divide(2,2);

  //TH2* hxy = c1->DrawFrame(-12, 12, -12, 12);
  //TH2* hrz = c1->DrawFrame(-30, 30, -12, 12);

  TText * t = new TText();

  int color[8] = {kGray, kGreen-8, kGreen+1, kBlue-4, kOrange, kOrange+5, kPink+1, kGreen+4};

  for(int ipid=0; ipid<8; ipid++){
    int pad = (ipid<4 ? 0 : 1);

    c1->cd(pad+1);
    if(ipid==0||ipid==4) {
      TH1* hxy = gPad->DrawFrame(-12, -12,  12, 12);
      hgeo->Draw("samecolz");
    }
    for(int ilad=0; ilad<14; ilad++){

      t->SetTextColor(color[ipid]);
      t->DrawText(xyz[ipid][ilad][0], xyz[ipid][ilad][1], Form("%d", ilad));

    }
  }
}
