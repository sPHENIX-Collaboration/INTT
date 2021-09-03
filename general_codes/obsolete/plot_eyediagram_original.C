#include<string>

void plot_eyediagram_original(const char *fname = "number.txt"){

 gStyle->SetOptStat(0);

 ifstream fin(fname);


 // ------------------------------
 // 1st step
 // 1) read data and make waveform graph
 // 2) make a histogram of voltage for finding the peak at 2nd step
 // 3) search maximum and minimum voltage
 

 TH1F *h1 = new TH1F("h1","height",1500,-0.85,0.85);
 //h1:voltage-time histogram about all
 //need to update(-0.25,0.25)

 TH1F *h2 = new TH1F("h2","height",1500,-0.85,0.85);
 //h2:voltage-time histogram about test pulse
 //need to update(-0.25,0.25)
 h2->SetLineColor(3);

 TGraph *g1 = new TGraph();
 g1->SetMarkerStyle(20);
 g1->SetMarkerSize(0.4);

 int n=0;

 double ymin, ymax;
 ymin=100;
 ymax=-100;

 string sbuf;
 while(!fin.eof()){
  std::getline(fin,sbuf);
  TString stxt(sbuf);
  stxt.ReplaceAll(","," ");
  
  istringstream iss(stxt.Data());

  float time, vol;
  iss >>time>>vol;

  //cout<<"time="<<time<<"vol="<<vol<<endl;

  h1->Fill(vol);
  
  if(time<-0.2e-6||0.2e-6<time)
   h2->Fill(vol);
   //set range of peak
 
/* if(time<-0.2e-6)
  cout<<"mintime="<<vol<<endl;

 if(0.2e-6<time)
  cout<<"maxtime="<<vol<<endl;
*/
  if(vol<ymin)
   ymin=vol;

  if(vol>ymax)
   ymax=vol;

  g1->SetPoint(n,time,vol);

  n++;
 }

//cout<<n<<endl;
 cout<<"ymin="<<ymin<<endl;
 cout<<"ymax="<<ymax<<endl;

 fin.close();


 // ------------------------------
 // 2nd step
 // 1) search the middle voltage of high and low from clock signal 
 //   i) fit the voltage histogram to find the voltage of high and low
 //   ii) take an average that is the middle voltage of the high and low


 TF1 *f1 = new TF1("f1","gaus", ymin, ymax);
// h2->Fit("f1","","",0.195,0.22);

 f1->SetParameter(0, h2->GetMaximum());
 f1->SetParameter(1, h2->GetBinCenter(h2->GetMaximumBin()));
 f1->SetParameter(2, h2->GetRMS()*0.5);

 float meanpos = h2->GetBinCenter(h2->GetMaximumBin());
 float rms     = h2->GetRMS();

 h2->Fit("f1","R","",meanpos-rms*3,meanpos+rms*3);

 float peak1  = f1->GetParameter(1);
 float sigma1 = f1->GetParameter(2);

// cout<<"peak1="<<peak1<<endl;
// cout<<"sigma1="<<sigma1<<endl;

 float range2_low  = (peak1>0) ? ymin		 : peak1+sigma1*10;
 float range2_high = (peak1>0) ? peak1-sigma1*10 : ymax;

 if(peak1<0 && range2_low<0)
  range2_low=0;
 if(peak1>0 && range2_high>0)
  range2_high=0;

 cout<<"range="<<range2_low<<"~"<<range2_high<<endl;

 h2->Fit("f1","R+","",range2_low,range2_high);
 
 float peak2  = f1->GetParameter(1);
 float sigma2 = f1->GetParameter(2);

 cout<<"peak1="<<peak1<<endl;
 cout<<"peak2="<<peak2<<endl;
 cout<<"sigma1="<<sigma1<<endl;
 cout<<"sigma2="<<sigma2<<endl;
 

// float mean = ((peak1 + peak2) / 2);
 float mean = peak1*0.4+peak2*0.6;

 cout<<"mean="<<mean<<endl;
 
 double t0, tn, t1, v0, vn, v1;
 g1->GetPoint(0, t0, v0);
 cout<<"v0="<<v0<<endl;
 g1->GetPoint(1, t1, v1);
 cout<<"v1="<<v1<<endl;
 g1->GetPoint(g1->GetN()-1, tn, vn);
 cout<<"vn="<<vn<<endl;

 cout<<"t0="<<t0<<endl;
 cout<<"t1="<<t1<<endl;
 cout<<"tn="<<tn<<endl;
 
 double t1_t0 = t1 - t0;//1 tick width
 cout<<"1tick="<<t1_t0<<endl;

 TLine *l1 = new TLine(t0, mean, tn, mean);
 // horizontal line of graph which shows a border between high and low


 // ------------------------------
 // 3rd step
 //  search the times to cross the average from high to low
 //  ->time_edge[] : time when get across the average


 int nline=0;
 double te[100]={0};
 // te = time_edge
 int tep[100]={0};
 // tep = time_edge_pos
 TLine *lv[100];

 int count_high=0, count_low=0, low_OK=0, high_OK=0;
 
 if(v0<mean){
  for(int i=0; i < g1->GetN(); i++){
   double t, v;
   g1->GetPoint(i, t, v);
 
   if(v<mean)
    count_low++;
   else
    count_high++;
 
//   cout<<"count_high="<<count_high<<endl;
//   cout<<"count_low="<<count_low<<endl;
 
   if(count_low>0 && count_high==1){
    cout<<"cross border_high : "<<"t="<<t<<"v="<<v<<endl;
    high_OK=1;
    count_low=0;
 
    if(nline<100){
     te[nline] = t;
     tep[nline] = i;
     lv[nline] = new TLine(t, peak2, t, peak1);
     nline++;
    }
   }
   
   if(high_OK==1 && count_high>0 && count_low==1){
    high_OK=0;
    count_high=0;
   }
  }
 }
    
 else{
  for(int i=0; i < g1->GetN(); i++){
   double t, v;
   g1->GetPoint(i, t, v);
 
   if(v>mean)
    count_high++;
   else
    count_low++;
 
//   cout<<"count_high="<<count_high<<endl;
//   cout<<"count_low="<<count_low<<endl;
 
   if(count_high>0 && count_low==1){
    cout<<"cross border_low : "<<"t="<<t<<"v="<<v<<endl;
    low_OK=1;
    count_high=0;
 
    if(nline<100){
     te[nline] = t;
     tep[nline] = i;
     lv[nline] = new TLine(t, peak2, t, peak1);
     nline++;
    }
   }
   
   if(low_OK==1 && count_low>0 && count_high==1){
    low_OK=0;
    count_low=0;
   }
  }
 }
 
 TCanvas *c2 = new TCanvas("c2","c2",600,450);
 h1->Draw();


 // ------------------------------
 // 4th step
 //  calculate the clock width using time_edge by truncated mean (3times iteration)
 //  ->tdiff : difference of time edge
 //  ->tdiff_mean, rms : tdiff_average and rms


 double teu[100]={0};
 // teu = time_edge_update
 int    tepu[100]={0};
 // tepu = time_edge_pos_update
 double tru_mean=100e-9, tru_rms=20e-9;
 // assume truncate_mean and truncate_rms
 // tru = truncate
 
 for(int i_iter=0; i_iter<3; i_iter++){
  double tru_range = tru_rms * pow(0.5, i_iter);
  // make smaller window by iteration

  cout<<Form("truncate_mean_%d : ", i_iter)<<tru_mean<<endl;
  cout<<Form("truncate_rms_%d : ", i_iter)<<tru_rms<<endl;
  cout<<Form("truncate_range_%d : ", i_iter)<<tru_range<<endl;
 
  double tdiff_mean=0.0, tdiff_rms=0.0;
  int nadd=0;
  for(int il=0; il<nline; il++){
   cout<<"il="<<il<<endl;
   
   double tdiff = te[il+1] - te[il];
   cout<<"tdiff="<<tdiff<<endl;

   if(fabs(tdiff-tru_mean)<tru_range){
    tdiff_mean += tdiff;
    tdiff_rms += (tdiff*tdiff);
    
    teu[nadd] = te[il];
    teu[nadd+1] = te[il+1];
    tepu[nadd] = tep[il];
    tepu[nadd+1] = tep[il+1];

    nadd++;
    
    cout<<Form("time_edge%d=",il)<<te[il]<<endl;
    cout<<Form("time_edge%d=",il+1)<<te[il+1]<<endl;
   }
  }
   
  cout<<tdiff_mean<<" "<<tdiff_rms<<endl;

  tdiff_mean /= (nadd);
  tdiff_rms = sqrt(tdiff_rms/(nadd));
  cout<<"tdiff_mean : "<<tdiff_mean<<endl;
  cout<<"tdiff_rms : "<<tdiff_rms<<endl;
  cout<<"tdiff_mean^2 : "<<tdiff_mean*tdiff_mean<<endl;
  cout<<"tdiff_rms^2 : "<<tdiff_rms*tdiff_rms<<endl;


  tru_mean = tdiff_mean;
  tru_rms = sqrt(tdiff_rms*tdiff_rms - tdiff_mean*tdiff_mean);
  
  cout<<"truncate_mean="<<tru_mean<<endl;
  cout<<"truncate_rms="<<tru_rms<<endl;

  if(fabs(tru_rms/tru_mean)<0.05) break;

 } 
 
 cout<<"clock width mean : "<<tru_mean<<endl;
 cout<<"clock width rms : "<<tru_rms<<endl;

 double clk_width = tru_mean*0.05;
 // 106ns/20->5.3ns
 int clk_q = TMath::Nint(clk_width/(t1_t0));
 // clk_q : quantity of data point per 1 clock time

 cout<<"clock width per 1 cut : "<<clk_width<<endl;
 cout<<"quantity per 1 clock : "<<clk_q<<endl;

// cout<<sqrt(tdiff_rms*tdiff_rms - tdiff_mean*tdiff_mean)<<endl;


 // ------------------------------
 // 5th step
 //  1) take multiple waveform from the graph based on the clock edge
 //   i) the waveform is defined in time from -1 clock to 2 clock (1clk = 5ns)
 //   ii) this is calculated using the time across the average (= time_edge) and clk_width
 //   iii) if the time of edge is over the next time_edge, the time is recalculated using the next time_edge to adjust the clock timing
 //   -> n_edge = number of edge time ( = number of waveform)
 //   -> g1_edge = edge time of each waveform
 //   -> g1_start = start of waveform ( gedge - 1clk)


 int nwidth = TMath::Nint((te[nline]-te[0])/(clk_width)+1);
 int nedge = TMath::Nint((te[nline]-te[0])/(tru_mean)+1);
 
 cout<<"nwidth : "<<nwidth<<endl;
 cout<<"nedge : "<<nedge<<endl;
 cout<<"from start edge to last edge : "<<(te[nline]-te[0])<<endl;
 
 TGraph *g1_eye[200];
 
 int count=0, pre_il=-1;
 
 for(int neye=0; neye<nwidth+1; neye++){
  g1_eye[neye] = new TGraph();
  
  int g1_edge = tep[0] + neye * clk_q;
  int g0 = g1_edge - clk_q;
  int gn = g1_edge + 2*clk_q;

//  double t, v;
  g1->GetPoint(g1_edge, t, v);

  for(int il=0; il<nadd-1; il++){
   if(teu[il+1] > t){
    t = teu[il];
    if(pre_il!=il){
     pre_il=il;
     count=0;

    }
    break;
   }

  }
  double t_edge = t + count * clk_width;

  for(int i=g0; i<gn; i++){
   g1->GetPoint(i, t, v);
   g1_eye[neye]->SetPoint(g1_eye[neye]->GetN(), t-t_edge, v);
  }

  count++;
 }

 double xt, yv;
 g1_eye[0]->GetPoint(0, xt, yv);
// g1->GetPoint(0, xt, yv);

 double h3_xmin = xt - 0.1 * clk_width;
 double h3_xmax = xt + 3.3 * clk_width;
 double h3_ymin = ymin;
 double h3_ymax = ymax;
 double h3_ywidth = h3_ymax - h3_ymin;
 
 h3_ymin -= h3_ywidth * 0.1;
 h3_ymax += h3_ywidth * 0.1;

 cout<<"xmin="<<h3_xmin<<endl;
 cout<<"xmax="<<h3_xmax<<endl;
 cout<<"ymin="<<h3_ymin<<endl;
 cout<<"ymax="<<h3_ymax<<endl;

 cout<<"t0="<<xt<<endl;

 TH2F *h3_eye = new TH2F("h3_eye","eye diagram", 300, h3_xmin, h3_xmax, 300, h3_ymin, h3_ymax);
// TH2F *h3_eye = new TH2F("h3_eye","eye diagram", 300, -5e-9, 12e-9, 300, -0.25, 0.25);

 for(int i=0; i<nwidth; i++){
  for(int ipos=0; ipos<g1_eye[i]->GetN(); ipos++){
   g1_eye[i]->GetPoint(ipos, xt, yv);
   h3_eye->Fill(xt, yv);
  }
 }


 // ------------------------------
 // 6th step
 //  plot each graph


 TCanvas *eye1 = new TCanvas("eye1", "eyediagram", 600, 450);
 eye1->Divide(2,2);
 eye1->cd(1);
 g1->Draw("ALP");
 l1->Draw("same");
 for(int il=0; il<nline; il++){
  lv[il]->SetLineColor(2);
  lv[il]->Draw("same");
 }

 eye1->cd(2);
 h1->Draw();
 h2->Draw("same");

 eye1->cd(3);
 for(int i=0; i<nwidth; i++){
  g1_eye[i]->SetMarkerStyle(20);
  g1_eye[i]->SetMarkerSize(0.4);
  g1_eye[i]->Draw(i==0?"ALP":"PL");
 }

 eye1->cd(4);
 h3_eye->Draw("colz");


 // ------------------------------
 // 7th step
 //  record eyediagram as root file


 TString foname(fname);
 foname.ReplaceAll(".txt",".root");
 TFile *foroot = TFile::Open(foname.Data(), "recreate");
 g1->Write();
 h1->Write();
 h2->Write();

 for(int i=0; i<nwidth; i++){
  g1_eye[i]->SetName(Form("g1_eye_%d", i));
  g1_eye[i]->Write();
 }

 h3_eye->Write();
 foroot->Close();
}
