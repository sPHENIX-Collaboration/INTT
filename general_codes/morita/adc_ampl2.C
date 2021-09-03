//MiuMorita
//20200221
//

#include<string>

 using namespace std;

 void adc_ampl2(const char *fname = "fphx.root"){

   TFile *file = TFile::Open(fname);
   TH1F *h1[8];
   TF1 *f1[8];

   TGraph *g1 = new TGraph();
   g1->SetMarkerStyle(20);
   g1->SetMarkerSize(0.3);

   int n = 7;
   int j = 0;
   float mean_buf=0.0;

   double y[8]={290., 310., 330., 350., 370., 390., 410., 430.};
   double yj[8]={0.0};
//   int y[8]={290, 310, 330, 350, 370, 390, 410, 430};
   double mean[8]={0.};
   double mean2[8]={0.};

   for(int i=0; i<n; i++){
     h1[i] = new TH1F(Form("h1[%d]",i),"his",70,0,70);
     f1[i] = new TF1(Form("f1[%d]",i),"gaus",0,64);
     tree->Draw(Form("ampl>>h1[%d]",i),Form("adc==%d&&fem_id==4&&module==8&&ampl<64",i));

     h1[i]->Fit(Form("f1[%d]",i),"q");
     mean[i] = f1[i]->GetParameter(1);
     if(mean_buf<mean[i]){
        mean2[j] = mean[i];
	yj[j] = y[i];
	j++;
     }
     mean_buf=mean[i];

/*
     if(25<mean[i]&&mean[i]<64){
        cout<<mean[i]<<endl;
        g1->SetPoint(i, mean[i], y[i]);

        cout<<"i x y : "<<i<<" "<<mean[i]<<" "<<y[i]<<endl;
     }
*/

   }
   

   TCanvas *c2 = new TCanvas("c2", "gsel", 600, 450);
//   g1->Draw("AP");


   TGraph *g2 = new TGraph(j, mean2, yj);
   g2->SetMarkerStyle(20);
   g2->SetMarkerSize(0.4);
   g2->GetXaxis()->SetLimits(0, 64);
   g2->GetYaxis()->SetLimits(270, 440);
   g2->Draw("AP");


/*   int sum_y = 2880;
   double sum_x = 0.0;
   double sum_sq = 0.0;
   double sum_xy = 0.0;

   
   TH2F *h2 = new TH2F("h2", "", 70, 0,70, 8, 0, 8);

   tree->Draw("adc:ampl>>h2","fem_id==4&&module==8");
   h2->Draw("colz");

   TProfile *prf_x = h2->ProfileX();
   prf_x->Draw("same");

   TF1 *fit = new TF1("fit", "pol1", 30, 64);
   prf_x->Fit("fit", "", "", 35, 64);
   
*/
   /*
     sum_x += mean[i];
     sum_sq += mean[i]*mean[i];
     sum_xy += mean[i]*y[i];

   cout<<sum_x<<" "<<sum_sq<<" "<<sum_xy<<endl;

   double sum_x_sq = sum_x*sum_x;
   double a=0, b=0;

   cout<<sum_x_sq<<endl;

   a=(n*sum_xy-sum_x*sum_y)/(n*sum_sq-sum_x_sq);
   b=(sum_sq*sum_y-sum_xy*sum_x)/(n*sum_sq-sum_x_sq);

   cout<<"a = "<<a<<" "<<"b = "<<b<<" "<<"b/a = "<<b/a<<endl;
   */

 }
