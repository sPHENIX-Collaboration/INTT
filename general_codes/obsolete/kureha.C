#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "TNtuple.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"

using namespace std;

void kureha(char* fname="C:\root_v5.34.36\macros",int usemod=3,int maxbuf=0,int n_meas=64,float maxscale=200.)
{
 TH1F* hist_data = new TH1F("hist_data","amplitude",67,0,67);
 TH1F* hist_line = new TH1F("hist_line","",67,0,67);//hist for line
 TH1F* hist_final = new TH1F("hist_final","",67,0,67);//hist with error

 TF1* z1=new TF1("z1","0",0,67);//make line of y=0
 z1->SetLineColor(kBlack);

 for(int i=0;i<hist_line->GetNbinsX();i++){
   hist_line->SetBinContent(i+1,20);       //make hist of y=20
 }


 //////////////////////////
 // file open
 TFile* f1 = new TFile("fphx_raw_20190204-1502_0.root"); //data of calib(DAC20.25.30--)
 TTree* tree = (TTree*)f1->Get("tree");

 //////////////////////////
 // get 1ch data from tree
 {
 tree->Draw("ampl>>hist_data","chip_id==1 && chan_id==0","e");//Set chip_id&chan_id
 TH1F* hist_data = (TH1F*)gDirectory -> Get("hist_data");//treehist into hist_data


 hist_data->SetMinimum(-2);//set minimum of y
 hist_data->SetXTitle("ampl");
 hist_data->SetYTitle("entry");
 hist_data->SetLineColor(kBlack);
 hist_data->Draw();

 z1->Draw("same");
 hist_line->Draw("same");

 hist_final->Divide(hist_data,hist_line,1,1,"B");//(hist_data)/(hist_line)+(binomial error)
 hist_final->Scale(20);
 hist_final->SetLineColor(kRed);
 hist_final->Draw("same");


 //cout<<hist_data->GetNbinsX()<<endl;
 //cout<<hist_line->GetNbinsX()<<endl;

 TF1* f2=new TF1("f2","(TMath::Erf([1]*(x-[0]))+1)*10",0,67);
 //TF1* f2=new TF1("f2","(TMath::Erf((x-[0]))+1)*10",0,67);
 f2->SetLineColor(kGreen);
 f2->SetParameter(0,28);
 f2->SetParameter(1,1);
 //gStyle->SetOptFit();
 hist_final->Fit("f2");
 //cout<<f2->GetXaxis()->GetBinCenter("30")<<endl;
 //TF1* f3=new TF1("f3","(TMath::Erfinv((x-[0]))+1)*10",0,67);
}

 
}
