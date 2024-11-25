void draw_ncluscorr(){
  //TFile *f = TFile::Open("beam_inttall-00010433-0000_event_base_ana.root");
  TFile *f = TFile::Open("/sphenix/lustre01/sphnxpro/commissioning/INTT/tmp/beam_inttall-00020444-0000_event_base_ana.root");

  TTree *h_evttree = (TTree*)f->Get("h_evttree");

  int base=5;
  int corr[7]={0,1,2,3,4,6,7};

  TH2F *h[7];
  for(int i=0; i<7; i++){
    h[i] = new TH2F(Form("h_%d", i), Form("corr %d vs %d",corr[i], base) , 100, 0, 2000, 100, 0, 2000);
  }

  TString s_draw[8];
  for(int i=0; i<8; i++){
    s_draw[i] = "";
    int host = (i<7)? corr[i] : base;
    for(int ilad=0; ilad<14; ilad++){
      s_draw[i] += Form("nclus[%d][%d]", host, ilad);
      if(ilad<13) s_draw[i] += "+";
    }
    cout<<s_draw[i].Data()<<endl;
  }


  TCanvas *c1 = new TCanvas("c1", "c1", 900, 900);
  c1->Divide(3,3);
  for(int i=0; i<7; i++){
    c1->cd(i+1);
    gPad->SetLogz();

    TString s_draw1 = s_draw[i]+":"+s_draw[7]+Form(">>h_%d", i);
    cout<<s_draw1.Data()<<endl;
    h_evttree->Draw(s_draw1.Data(), "","colz");
  }

}
