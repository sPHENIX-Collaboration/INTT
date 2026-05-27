#include "GetBtoERatio.C"
void ScanRuns(const char* inlist="timescanlist_240520.txt")
{

  //  string nst[4] = {"total peak","adjusted peak","even/total","odd/total"};
  string nst[4] = {"peak base","fixed even/total","peak base","fixed even/total"};
  int npt[4] = {0, 2, 0, 2};
  const int nfx = 8; //
  ifstream ifs;
  string str;
  ifs.open(inlist);
  if(ifs.fail()) {
    cerr << "File do not exist.\n";
    exit(0);
  }

  TGraph *g[4][nfx+1];
  for(int ig=0; ig<4; ig++) {
    for(int ifx=0; ifx<nfx+1; ifx++) {
      g[ig][ifx] = new TGraph(255);
      g[ig][ifx]->SetLineColor(ifx+1);
    }
  }
  double ratt[4];
  int fnum = 0;
  int run;
  float l1d, fdl;
  while(getline(ifs, str)) {

    if( strstr(str.data(),"#")!=0 ) continue;
    if( strstr(str.data(),"RETURN")!=0 ) break;
    TString s = str.data();
    TObjArray *tsa = s.Tokenize(",");

    run=0;
    l1d=fdl=0.;
    for(int i=0; i<tsa->GetEntries(); i++) {
      TString ts1 = ((TObjString *)(tsa->At(i)))->String();
      if(i==0) run = ts1.Atoi();
      if(i==1) l1d = ts1.Atof();
      if(i==2) fdl = ts1.Atof();
    }
    printf("%d %f %fn",run,l1d,fdl);
    ratt[0]=ratt[1]=ratt[2]=ratt[3]=0.;
    double *rati;    
    for( int ifx=0; ifx<nfx; ifx++) {
      cout<<"Process Run#"<<run<<" intt"<<ifx<<" L1 Delay="<<l1d<<" Fine Delay="<<fdl<<endl;
      rati = GetBtoERatio(Form("/sphenix/tg/tg01/commissioning/INTT/data/root_files/2024/beam_intt%d-000%d-0000_hist.root",ifx,run),ifx);
      cout<<"Set : "<<fnum+1<<" "<<l1d<<" "<<fdl<<" "<<rati[15]<<endl;
      cout<<endl;
      for(int ig=0; ig<4; ig++) {
	g[ig][ifx]->SetPoint(fnum,l1d+(fdl/200),rati[14+ig]);
	ratt[ig]+=rati[14+ig];
      }
    }
    for(int ig=0; ig<4; ig++) {
      ratt[ig] = ratt[ig]/nfx;
      g[ig][8]->SetPoint(fnum,l1d+(fdl/200),ratt[ig]);
    }

    fnum++;
  }

  ifs.close();

  TH2F *h[4];
  TCanvas *cc = new TCanvas("cc","",1400,1000);
  cc->Divide(2,2);
  TLegend *leg = new TLegend(0.1, 0.8, 0.9, 0.9);
  leg->SetNColumns(8);
  TLegend *leg2 = new TLegend(0.1, 0.8, 0.9, 0.9);
  for(int ig=0; ig<4; ig++) {
    cc->cd(ig+1);
    h[ig] = new TH2F(Form("h%d",ig),Form("L1 delay vs ratio : %s",nst[ig].c_str()),1,112,133,1,0.,1.0);
    h[ig]->Draw();
    h[ig]->SetXTitle("L1+fine delay [L1 delay Unit]");
    h[ig]->SetYTitle("ratio");
    gStyle->SetOptStat(0);

    if(ig<2) {
      for(int ifx=0; ifx<nfx; ifx++) {
	g[npt[ig]][ifx]->Set(fnum);
	g[npt[ig]][ifx]->SetLineWidth(3.);
	g[npt[ig]][ifx]->Draw("L");
	if(ig==0) leg->AddEntry(g[npt[ig]][ifx], Form("intt%d",ifx), "l");
      }
      leg->Draw("same");
    } else {
      g[npt[ig]][8]->Set(fnum);
      g[npt[ig]][8]->SetLineWidth(3.);
      g[npt[ig]][8]->SetMarkerSize(1.);
      g[npt[ig]][8]->SetMarkerStyle(20);
      g[npt[ig]][8]->Draw("PL");
      if(ig==2) leg2->AddEntry(g[npt[ig]][8], "All intt average", "l");
      leg2->Draw("same");
    }

  }
  return;  
}
