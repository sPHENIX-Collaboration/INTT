//20200217
//MiuMorita
//

 using namespace std;

 //void erf(){
void erf3(const char *fname = "fphx.root"){

 //TFile *f = TFile::Open("E:/INTT/desktop_pc_in_NWU/data/busextender_calib/fphx_raw_20191216-1736_0.root");
 TFile *f = TFile::Open(fname);
 //tree->Draw("ampl","chip_id==1&&chan_id==0");

 TH1D *H[128][26];
 TH1F *h1 = new TH1F("h1","sigma",60,0,6);

 double sigma[128][26]=0.0;
 double sigma_sum=0.0;

// TF1 *f1 = new TF1("f1","[0]*TMath::Erf((x-[1])/[2])+10",0, 64);
// TCanvas *c1 = new TCanvas("c1","count vs amplitude",1200,800);
 TCanvas *c1 = new TCanvas("c1","count vs amplitude",600,450);
// c1->Divide(16,8);
 TF1 *fit[128][26];

 for(int j=0; j<26; j++){
 for(int i=0; i<128; i++){
  H[i][j] = new TH1D(Form("H[%d][%d]",i,j),Form("chan_id==%d",i),70,0,70);
//  c1->cd(i+1);
  tree->Draw(Form("ampl>>H[%d][%d]",i,j),Form("chip_id==%d&&chan_id==%d&&fem_id==4",j+1,i));
  

  fit[i][j] = new TF1(Form("fit[%d][%d]",i,j),"[0]*TMath::Erf((x-[1])/[2])+10",0, 64);
  fit[i][j]->SetParameter(0, 10);
  fit[i][j]->SetParameter(1, 30);
  fit[i][j]->SetParameter(2, 2);
  H[i][j]->Fit(Form("fit[%d][%d]",i,j));

  sigma[i][j] = fit[i][j]->GetParameter(2);
  h1->Fill(sigma[i][j]);
  cout<<Form("sigma%d-%d=",j+1,i)<<sigma[i][j]<<endl;
  sigma_sum += sigma[i][j];
 }
 }
 
 cout<<"sigma_average : "<<sigma_sum/(128*26)<<endl;

 TF1 *fgaus = new TF1("fgaus","gaus",0,6);

 TCanvas *c2 = new TCanvas("c2","sigma",600,450);
 h1->Draw();
 h1->Fit("fgaus");



 /*
 TH1D*  H= new TH1D("H","counts vs amplitude", 70, 0, 70);
 //tree->Draw("ampl>>H","chip_id==1&&chan_id==3");
 tree->Draw("ampl>>H","chip_id==1&&chan_id==0&&fem_id==4");
 TF1 *f1 = new TF1("f1","TMath::Erf(x)",0,64);
 TF1 *f1 = new TF1("f1","[0]*TMath::Erf((x-[1])/[2])+10",0, 64);
 f1->SetParameter(0, 10);
 f1->SetParameter(1, 35);
 f1->SetParameter(2, 2);
 H->Fit("f1"); 
 cout<<"sigma : "<<f1->GetParameter(2)<<endl;
 */
} 
