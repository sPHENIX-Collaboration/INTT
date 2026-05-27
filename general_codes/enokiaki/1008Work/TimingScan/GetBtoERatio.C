double* GetBtoERatio(const char *infile, int fxn=0)
{

  int cflg=0; // Want to see plot?
  static double ratio[18]; //0-13: each module, 14:total, 15:peak adjusted total 16: even/(odd+even) 17:odd/(odd+even)
  TString smod;
  int mbin, nmax[16], nmin[16], neve, nodd;

  TFile *f = new TFile(infile);

  TCanvas *c;
  if(cflg) { c = new TCanvas("c","",2500,1500); c->Divide(5,3,0.01,0.01); }

  TH1D *h2, *hm[14];
  TH1D *ht = new TH1D("ht","All modules",128,0,128);

  for(int im=0; im<16; im++) {
    
    nmax[im]=nmin[im]=0;
    if(
       (fxn==0&&im==6) ||
       (fxn==1&&im==1) ||
       (fxn==6&&im==10) 
       )  continue;
    

    if(im<14) {

      smod = TString(Form("module%d",im));
      hm[im] = new TH1D(Form("hm%d",im),smod,128,0,128);

      for(int ic=0; ic<24; ic++) {
	h2 = (TH1D*) gROOT->FindObject(Form("hist_bco_diff_module%d_chip%d",im,ic));
	hm[im]->Add(h2);
	ht->Add(h2);
	delete h2;
      }

      if(cflg) { c->cd(im+1); hm[im]->Draw(); }

      mbin = hm[im]->GetMaximumBin();
      for(int ibc=0; ibc<64; ibc++) {
	nmax[im] += hm[im]->GetBinContent(ibc*2 + (mbin+1)%2 + 1);
	nmin[im] += hm[im]->GetBinContent(ibc*2 + mbin%2 + 1);
	nodd += hm[im]->GetBinContent(ibc*2 + 1);
	neve += hm[im]->GetBinContent(ibc*2 + 2);
      }

    } else if(im==14) {

      smod = TString("total");
      if(cflg) { c->cd(15); ht->Draw(); }

      mbin = ht->GetMaximumBin();
      for(int ibc=0; ibc<64; ibc++) {
	nmax[14] += ht->GetBinContent(ibc*2 + (mbin+1)%2 + 1);
	nmin[14] += ht->GetBinContent(ibc*2 + mbin%2 + 1);
      }

    } else if(im==15) {

      smod = TString("adjusted total");
      for(int im=0; im<14; im++) {
	nmax[15] += nmax[im];
	nmin[15] += nmin[im];
      }
    }

    ratio[im] = (double)nmin[im]/(nmax[im]+nmin[im]);
    cout<<"Maxbin = "<<mbin<<", nmax : nmin : ratio = "<<nmax[im]<<" : "<<nmin[im]<<
      " : "<<ratio[im]<<" @"<<smod<<endl;


  }
  ratio[16] = (double) neve/(nodd+neve);
  ratio[17] = (double) nodd/(nodd+neve);
  cout<<"neve = "<<neve<<" nodd = "<<nodd<<endl;
  cout<<"neve/total = "<<ratio[16]<<" nodd/total = "<<ratio[17]<<endl;
  
  return ratio;
}
  
