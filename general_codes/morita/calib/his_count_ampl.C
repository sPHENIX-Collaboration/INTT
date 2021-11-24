//20200217
//MiuMorita
//

 using namespace std;

void his_count_ampl(const char *fname = "fphx.root"){

 TFile *f = TFile::Open(fname);

 TH1D *H[128][26];

 TCanvas *c1 = new TCanvas("c1","count vs amplitude",600,450);

 for(int j=0; j<26; j++){
  for(int i=0; i<128; i++){
   H[i][j] = new TH1D(Form("H[%d][%d]",i,j),Form("chan_id==%d",i),70,0,70);
   tree->Draw(Form("ampl>>H[%d][%d]",i,j),Form("chip_id==%d&&chan_id==%d&&fem_id==4&&module==8",j+1,i));
   if(i==0)cout << "chip chan : " << j + 1 << "_" << i << endl;
  }
 }

 TString sname = gSystem->BaseName(fname);
 sname.Prepend("cahis_");
 cout<<sname<<endl;

 TFile *frootout = TFile::Open(sname.Data(),"recreate");
 
 for(int j=0; j<26; j++){
  for(int i=0; i<128; i++){
   H[i][j]->Write();
  }
 }            

 frootout->Close();
}







