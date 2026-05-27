void RunByRunCheck()
{
  int hcflg = 1;

  ifstream ifs;
  string str;
  ifs.open("FelixROCMap.txt");
  if(ifs.fail()) {
    cerr << "FelixROCMap.txt do not exist.\n";
    exit(0);
  }
  int flxid, flxch;
  TString ladn[8][14];
  TString prtn[8][14];
  TString rocp[8][14];
  TString rocn[8][14];
  while(getline(ifs, str)) {
    TString s = str.data();
    TObjArray *tsa = s.Tokenize(",");
    //    tsa->Print();
    for(int i=0; i<tsa->GetEntries(); i++) {
      TString ts1 = ((TObjString *)(tsa->At(i)))->String();
      if(i==0) flxid = ts1.Atoi() - 3001;
      if(i==1) flxch = ts1.Atoi();
      if(i==2) rocp[flxid][flxch] = ts1;
      if(i==3) prtn[flxid][flxch] = ts1;
      if(i==4) ladn[flxid][flxch] = ts1;
      if(i==5) rocn[flxid][flxch] = ts1;
    }
  }

  TString runs[] = {
    "20444", "6/29", 
    "20867", "7/8",
    "20868", "7/8",
    "20869", "7/8",
    "20885", "7/8",
    "21506", "7/14",
    "22058", "7/20",
    "22986", "7/21",
    "24767", "7/31"
  };
  int nft = sizeof(runs)/sizeof(runs[0]);
  const int nf = (int)nft/2;

  TCanvas *c1[8];
  for(int ip=0; ip<8; ip++) {
    c1[ip] = new TCanvas(Form("c1%d",ip),"",1600,1000);
    c1[ip]->Divide(15,nf+1,0.001,0.001);
  }


  TH2F *hd[8][14*nf];
  TH2F *hh[8][14*nf];
  TH2F *hc[8][14*nf];
  TGraph *g[8][14];
  TH2I *h2[8];
  for(int ip=0; ip<8; ip++) {
    h2[ip] = new TH2I(Form("h2[%d]",ip),"",1,-1,nf,1,-100,3400);
    for(int im=0; im<14; im++) {
      g[ip][im] = new TGraph(nf);
      g[ip][im]->SetLineColor(im);
      g[ip][im]->SetLineWidth(2.);
    }
  }
  gStyle->SetOptStat(0);
  TText txt1, txt2, txt3, txt4, txt5;
  txt1.SetTextSize(0.2); // ROC Port Ladder
  txt1.SetTextFont(1);
  txt2.SetTextSize(0.20); // Run
  txt2.SetTextFont(1);
  txt3.SetTextSize(0.18); // Date
  txt3.SetTextFont(1);
  txt4.SetTextSize(0.12); // channel
  txt5.SetTextSize(0.12); // chipID
  txt5.SetTextAngle(90);

  TFile *f[nf];
  TTree *t[nf];
  for(int ir=0; ir<nf+1; ir++) {
    if(ir<nf) {
      f[ir] = new TFile(Form("Intt_chan_list_run%s_0_itr_0.root",runs[int(ir*2)].Data()));
      t[ir] = (TTree*) f[ir]->Get("tree");
    }

    for(int ip=0; ip<8; ip++) {
      for(int im=0; im<14; im++) {
	c1[ip]->cd(15*ir+im+2);
	//	c1[ip]->SetFrameLineWidth(0);
	if(ir==nf) {
	  txt1.DrawTextNDC(0.1,0.7,ladn[ip][im]);
	  txt1.DrawTextNDC(0.1,0.5,rocp[ip][im]);
	  txt1.DrawTextNDC(0.1,0.3,Form("(ROC#%s)",rocn[ip][im].Data()));
	  txt1.DrawTextNDC(0.1,0.1,Form("Port:%s",prtn[ip][im].Data()));
	  txt4.DrawTextNDC(0.3,0.92,"channel");
	} else {
	  //	  hd[ip][im+ir*14] = new TH2I(Form("hd[%d][%d]",ip,im+ir*14),"",142,-7,135,28,0,28);
	  //	  hh[ip][im+ir*14] = new TH2I(Form("hh[%d][%d]",ip,im+ir*14),"",142,-7,135,28,0,28);
	  //	  hc[ip][im+ir*14] = new TH2I(Form("hc[%d][%d]",ip,im+ir*14),"",142,-7,135,28,0,28);
	  hd[ip][im+ir*14] = new TH2F(Form("hd[%d][%d]",ip,im+ir*14),"",142,-6.5,135.5,28,-0.5,27.5);
	  hh[ip][im+ir*14] = new TH2F(Form("hh[%d][%d]",ip,im+ir*14),"",142,-6.5,135.5,28,-0.5,27.5);
	  hc[ip][im+ir*14] = new TH2F(Form("hc[%d][%d]",ip,im+ir*14),"",142,-6.5,135.5,28,-0.5,27.5);
	  hd[ip][im+ir*14]->SetFillColor(4);
	  hh[ip][im+ir*14]->SetFillColor(2);
	  hc[ip][im+ir*14]->SetFillColor(7);
	  if( hcflg == 0 ) {
	    t[ir]->Draw(Form("chip_id:chan>>hd[%d][%d]",ip,im+ir*14),
			Form("felix==%d&&module==%d&&flag==%d",ip,im,1),"box");
	    g[ip][im]->SetPoint(ir,ir,hd[ip][im+ir*14]->GetEntries());
	  } else {
	    t[ir]->Draw(Form("chip_id:chan>>hh[%d][%d]",ip,im+ir*14),
			Form("felix==%d&&module==%d&&flag==%d",ip,im,3),"box");
	    t[ir]->Draw(Form("chip_id:chan>>hc[%d][%d]",ip,im+ir*14),
			Form("felix==%d&&module==%d&&flag==%d",ip,im,2),"box&same");
	    g[ip][im]->SetPoint(ir,ir,hh[ip][im+ir*14]->GetEntries()+hc[ip][im+ir*14]->GetEntries());
	  }
	  cout<<"Run:"<<runs[ir*2]<<" Felix:"<<ip<<" Module:"<<im<<endl;
	  gPad->SetTopMargin(0.00);
	  gPad->SetBottomMargin(0.00);
	  gPad->SetLeftMargin(0.00);
	  gPad->SetRightMargin(0.00);
	  if(im==0) {
	    c1[ip]->cd(15*ir+im+1);
	    txt2.DrawTextNDC(0.1,0.85,Form("Run%s",runs[ir*2].Data()));
	    txt3.DrawTextNDC(0.1,0.65,Form("Date:%s",runs[ir*2+1].Data()));
	    txt5.DrawTextNDC(0.98,0.4,"chipID");
	  }

	}
	c1[ip]->Update();
      }
    }
  } // End of run loop

  TCanvas *c2 = new TCanvas("c2","",1600,1000);
  c2->Divide(4,2,0.001,0.001);
  for(int ip=0; ip<8; ip++) {
    c2->cd(ip+1);
    h2[ip]->Draw();
    for(int im=0; im<14; im++) {
      g[ip][im]->Draw("L");
    }
  }

  c1[0]->Print("plots/plots.pdf(","pdf");
  for(int ip=1; ip<7; ip++) {
    c1[ip]->Print("plots/plots.pdf","pdf");
  }
  c1[7]->Print("plots/plots.pdf)","pdf");
  if(hcflg==0) gSystem->Exec("convert -quality 100 -density 400 -trim plots/plots.pdf plots/plot_hd_page%d.png");
  if(hcflg==1) gSystem->Exec("convert -quality 100 -density 400 -trim plots/plots.pdf plots/plot_hh_page%d.png");
}

