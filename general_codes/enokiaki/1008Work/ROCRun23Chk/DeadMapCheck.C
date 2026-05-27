double* DeadMapCheck(const char *infile="Intt_chan_list_run20867_0_itr_0.root")
{
  static double a[2];
  a[0] = 0.0;
  a[1] = 0.0;

  TFile *f = new TFile(infile);
  TTree *tree = (TTree*) f->Get("tree");

  TCanvas *c1[7];
  TH2I *h[7][14];
  gStyle->SetOptStat(0);
  for(int ip=0; ip<7; ip++) {
    c1[ip]  = new TCanvas(Form("c%d",ip),"",1000,600);
    c1[ip]->Divide(7,2,0.001,0.001);
    for(int im=0; im<14; im++) {
      h[ip][im] = new TH2I(Form("h[%d][%d]",ip,im),"",128,0,128,24,1,25);

      c1[ip]->cd(im+1);
      tree->Draw(Form("chip_id:chan>>h[%d][%d]",ip,im),Form("felix==%d&&module==%d&&flag==1",ip,im),"box");
      //      h[ip][im]->Draw("colz");
    }
  }
  return a;
}
