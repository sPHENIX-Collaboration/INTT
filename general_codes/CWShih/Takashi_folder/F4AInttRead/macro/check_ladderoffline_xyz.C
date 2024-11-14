void check_ladderoffline_xyz(){
  gStyle->SetOptStat(0);

  TFile *f = TFile::Open("AnaTutorial.root");
  //TFile *f = TFile::Open("AnaTutorial_dummyoffline.root");
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
  
  int nladder[4] = {12, 12, 16, 16};
  int nsenz[4] = {8, 5, 8, 5};
  float xyz[4][16][3];
  float pos_z[56][4][13][3];
  float pos_phi[512][3];
  for(int ievt=1; ievt<ntp_clus->GetEntries(); ievt++){
    ntp_clus->GetEntry(ievt);

    if(evt==0) continue;

    int evtno=0;
    int pid  =0;
    int lad  =0;

    int lad_z  =0;
    int sen_z  =0;

    if(evt<25){
      evtno = evt - 1;
      pid   = (evtno)/12;
      lad   = (evtno)%12;
    }
    else if(evt<57){
      evtno = evt - 25;
      pid   = ((evtno)/16) + 2;
      lad   = ((evtno)%16);
    }
    else if(evt<57+26*56){
      int seed  = (evt - 57);
      pid   = seed/26;
      evtno = seed%26;
     
      if(evtno<8){
        lad_z = 0;
        sen_z = evtno;
      }
      else if(evtno<13){
        lad_z = 1;
        sen_z = evtno-8;
      }
      else if(evtno<21){
        lad_z = 2;
        sen_z = evtno-13;
      }
      else if(evtno<26){
        lad_z = 3;
        sen_z = evtno-21;
      }
    }
    else if(evt<57+26*56+2*256){
      pid=(evt-(57+26*56));
    }
    else {
     break;
    }

    if(evt<57){
      xyz[pid][lad][0] = x;
      xyz[pid][lad][1] = y;
      xyz[pid][lad][2] = z;
    }
    else if(evt<57+56*26){
      pos_z[pid][lad_z][sen_z][0] = x;
      pos_z[pid][lad_z][sen_z][1] = y;
      pos_z[pid][lad_z][sen_z][2] = z;
    }
    else if(evt<57+56*26+2*256){
      pos_phi[pid][0] = x;
      pos_phi[pid][1] = y;
      pos_phi[pid][2] = z;
    }
    lad=lad_z;

    cout<<"xyz: "<<ievt<<" "<<evt<<" "<<pid<<" "<<lad<<" "<<x<<" "<<y<<" "<<z<<endl;
  }

  TCanvas *c1 = new TCanvas("c1", "c1", 1000, 500);
  c1->Divide(2,1);

  //TH2* hxy = c1->DrawFrame(-12, 12, -12, 12);
  //TH2* hrz = c1->DrawFrame(-30, 30, -12, 12);

  TText * t = new TText();

  int color[4] = {kGray, kGreen+1, kBlue-4, kOrange};

  c1->cd(1);
  TH1* hxy = gPad->DrawFrame(-12, -12,  12, 12);

  for(int ilay=0; ilay<4; ilay++){
    for(int ilad=0; ilad<nladder[ilay]; ilad++){

      t->SetTextColor(color[ilay]);
      t->DrawText(xyz[ilay][ilad][0], xyz[ilay][ilad][1], Form("%d", ilad));

    }
  }

  c1->cd(2);
  TH1* hrz = gPad->DrawFrame(-30, -12,  30, 12);

  int zlad[4] = {0, 12, 24, 40};
  for(int ilayer=0; ilayer<4; ilayer++){
    for(int iladz=0; iladz<4; iladz++){
      for(int isenz=0; isenz<nsenz[iladz]; isenz++){

        float xval = pos_z[ zlad[ilayer] ][iladz][isenz][0];
        float yval = pos_z[ zlad[ilayer] ][iladz][isenz][1];
        float pos_r = ((yval<0)?-1:1)*sqrt(xval*xval+yval*yval);

        t->SetTextColor(color[iladz]);
        t->DrawText(pos_z[zlad[ilayer]][iladz][isenz][2], pos_r, Form("%d", isenz));
        //cout<<ilayer<<" "<<zlad[ilayer]<<" "<<pos_z[ zlad[ilayer] ][iladz][isenz][2]<<endl;

      }
    }
  }

  TCanvas *c2 = new TCanvas("c2", "c2", 1000, 1000);
  c2->Divide(2,2);
  int ladder_idx=0;
  for(int ilayer=0; ilayer<4; ilayer++){
    c2->cd(ilayer+1);
    TH1* hrz = gPad->DrawFrame(-30, -1,  30, 20);

    for(int ilad=0; ilad<nladder[ilayer]; ilad++){
      for(int iladz=0; iladz<4; iladz++){
        for(int isenz=0; isenz<nsenz[iladz]; isenz++){

          float pos_r = ilad;

          t->SetTextColor(color[iladz]);
          t->DrawText(pos_z[ladder_idx][iladz][isenz][2], pos_r, Form("%d", isenz));
          //cout<<ilayer<<" "<<zlad[ilayer]<<" "<<pos_z[ ladder_idx ][iladz][isenz][2]<<endl;

        }
      }
      ladder_idx++;
    }
  }

  TCanvas *c3 = new TCanvas("c3", "c3", 1000, 1000);
  c3->Divide(4,1);
  TText * t1 = new TText();
  t1->SetTextSize(0.04);
  {
     for(int ich=0; ich<512; ich++){
       int ican    = (ich/64)%4;
       int ican_ch = ich%64;

       if(ican_ch==0){
         c3->cd(ican+1);
         if(ich<256){
           gPad->DrawFrame(-30, -0.139+0.069*ican, 30, -0.139+0.069*(ican+1));
         }
       }

       float pos_r = atan2(pos_phi[ich][1], pos_phi[ich][0]);

       //t->SetTextColor(color[iladz]);
       t1->DrawText(pos_phi[ich][2], pos_r, Form("%d", ich%256));
       //cout<<ilayer<<" "<<zlad[ilayer]<<" "<<pos_z[ ladder_idx ][iladz][isenz][2]<<endl;

     }
  }

}
