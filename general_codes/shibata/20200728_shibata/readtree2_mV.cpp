{
/////////////////////////////////////////////
//        clustered hits histograms
//  This is one of the files to get clustered hits hisotgram.
//  You can draw dac amplitude(mV) or adc histograms, and N hits per event hisrogram on a Canvas.
//
//  Before drawing, you have to get four files,"readtree_adc.c", "readtree_mV.c", "readtree.c", "readtree.h", "readtree_mV.cpp", "readtree.cc" .
//  Please change root file name on "readtree.h", and copy "readtree_mV.c" to "readtree.c" or copy "readtree_adc.c" to "readtree.c".
//  You can choose adc histograms or dac histograms. 
//  To draw histograms, you can write
//	            $ root -l rootfilename
//      	ROOT> .x readtree_mV.cpp;
//	        ROOT> .L readtree.c;
//	        ROOT> readtree t(tree);
//	        ROOT> .x readtree.cc;
//////////////////////////////////////////////

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <TH1.h>
#include <fstream>
	
TCanvas *c1 = new TCanvas("c1","");
c1->Divide(3,2);


////////////for dac histograms/separated by number of hits per event///////////////////////
//you can choose bins.
///////////////////////////////////////////////////////
/*
int m=30;
float xbins[31];
xbins[0] = 0.;
xbins[1] = 270.;
xbins[2] = 300.;
xbins[8] = 1200.;
//xbins[3] = 300.;
for(int i=3; i<m+1; i++){
	xbins[i] = xbins[i-1]+150.;
}*/
//c1->cd(4);
//TH1F *f1 = c1->DrawFrame (0.,0.,27.,128.);

int m=10;
//double xbins[] = {0, 20, 23, 60, 98, 135, 173, 210, 248, 256}; 
double xbins[] = {0, 250, 302, 450, 602, 750, 902, 1050, 1202, 1202, 1234}; 
//double xbins[] = {0, 270, 290, 302, 450, 602, 750, 902, 1050, 1234}; 

c1->cd(2);
TH2F *nhitsnChip = new TH2F("nhitsnChip","Nhits/chip_id",30,0,30,27,0,27);
c1->cd(3);
TH2F *nclsnChip = new TH2F("nclsnChip","N clusters per chip",30,0,30,27,0,27);

c1->cd(1);
TH1F *hnhits = new TH1F("hnhits","N hits per event",50,0,50);
c1->cd(5);
TH1F *SumAdc2 = new TH1F("sumadc","mV",m,xbins);
c1->cd(6);
TH1F *Wadc = new TH1F("wadc", "adc with weight", 8, 0, 8);

c1->cd(4);
//c1->cd(2);
TH1F *SumDoublehitAdc2 = new TH1F("Sum2hits","mV",m,xbins);
SumDoublehitAdc2->SetLineColor(kBlue);
TH1F *SinglehitAdc2 = new TH1F("1hitmV","mV",m,xbins);
SinglehitAdc2->SetLineColor(kRed);
//c1->cd(3);
TH1F *SumTriplehitAdc2 = new TH1F("Sum3hits","mV",m,xbins);
SumTriplehitAdc2->SetLineColor(kOrange);
//c1->cd(4);
TH1F *Sum4hitAdc2 = new TH1F("Sum4hits","mV",m,xbins);
Sum4hitAdc2->SetLineColor(kGreen);
//c1->cd(5);
TH1F *Sum5hitAdc2 = new TH1F("Sum5hits","mV",m,xbins);
Sum5hitAdc2->SetLineColor(kPink);

//TH1F *Sum6hitAdc2 = new TH1F("Sum6hits","adc",17,xbins);
//Sum6hitAdc2->SetLineColor(kViolet);
//TH1F *Sum7hitAdc2 = new TH1F("Sum7hits","adc",17,xbins);
////Sum7hitAdc2->SetLineColor(kCyan);
//TH1F *Sum8hitAdc2 = new TH1F("Sum8hits","adc",17,xbins);
//Sum8hitAdc2->SetLineColor(kTeal);
//TH1F *Sum9hitAdc2 = new TH1F("Sum9hits","adc",17,xbins);
//Sum9hitAdc2->SetLineColor(kAzure);
/*
c1->cd(1);
TH1F *SinglehitAdc2 = new TH1F("1hitmV","adc",40 ,0, 4000);
SinglehitAdc2->SetLineColor(kRed);
TH1F *SumDoublehitAdc2 = new TH1F("Sum2hits","adc", 40, 0, 4000);
SumDoublehitAdc2->SetLineColor(kBlue);
TH1F *SumTriplehitAdc2 = new TH1F("Sum3hits","adc", 40, 0, 4000);
SumTriplehitAdc2->SetLineColor(kOrange);
TH1F *Sum4hitAdc2 = new TH1F("Sum4hits","adc", 40, 0, 4000);
Sum4hitAdc2->SetLineColor(kGreen);
TH1F *Sum5hitAdc2 = new TH1F("Sum5hits","adc", 40, 0, 4000);
Sum5hitAdc2->SetLineColor(kPink);
TH1F *Sum6hitAdc2 = new TH1F("Sum6hits","adc", 40, 0, 4000);
Sum6hitAdc2->SetLineColor(kViolet);
TH1F *Sum7hitAdc2 = new TH1F("Sum7hits","adc", 40, 0, 4000);
Sum7hitAdc2->SetLineColor(kCyan);
TH1F *Sum8hitAdc2 = new TH1F("Sum8hits","adc", 40, 0, 4000);
Sum8hitAdc2->SetLineColor(kTeal);
TH1F *Sum9hitAdc2 = new TH1F("Sum9hits","adc", 40, 0, 4000);
Sum9hitAdc2->SetLineColor(kAzure);
*/
/*c1->cd(1);
TH1F *SinglehitAdc2 = new TH1F("1hitmV","adc",18 ,0, 1800);
SinglehitAdc2->SetLineColor(kRed);
TH1F *SumDoublehitAdc2 = new TH1F("Sum2hits","adc",18 ,0, 1800);
SumDoublehitAdc2->SetLineColor(kBlue);
TH1F *SumTriplehitAdc2 = new TH1F("Sum3hits","adc",18 ,0, 1800);
SumTriplehitAdc2->SetLineColor(kOrange);
TH1F *Sum4hitAdc2 = new TH1F("Sum4hits","adc",18 ,0, 1800);
Sum4hitAdc2->SetLineColor(kGreen);
TH1F *Sum5hitAdc2 = new TH1F("Sum5hits","adc",18 ,0, 1800);
Sum5hitAdc2->SetLineColor(kPink);
TH1F *Sum6hitAdc2 = new TH1F("Sum6hits","adc",18 ,0, 1800);
Sum6hitAdc2->SetLineColor(kViolet);
TH1F *Sum7hitAdc2 = new TH1F("Sum7hits","adc",18 ,0, 1800);
Sum7hitAdc2->SetLineColor(kCyan);
TH1F *Sum8hitAdc2 = new TH1F("Sum8hits","adc",18 ,0, 1800);
Sum8hitAdc2->SetLineColor(kTeal);
TH1F *Sum9hitAdc2 = new TH1F("Sum9hits","adc",18 ,0, 1800);
Sum9hitAdc2->SetLineColor(kAzure);
*/

/////////////////for dac histogram/Sum of clustered hits(1~9)//////////////////////
//c1->cd(2);
//TH1F *SumhitAdc2 = new TH1F("Sumhits","mV(1hit+2hits+3hits+4hits+5hits+6hits+7hits+8hits+9hits)",17,xbins);
//TH1F *SumhitAdc2 = new TH1F("Sumhits","mV(1hit+2hits+3hits+4hits+5hits+6hits+7hits+8hits+9hits)",18 ,0, 1800);
//TH1F *SumhitAdc2 = new TH1F("Sumhits","mV(1hit+2hits+3hits+4hits+5hits+6hits+7hits+8hits+9hits)",20 ,0, 4000);

////////////////for dac hisrogram/Sum of clustered hits(2~9)//////////////////////

//c1->cd(3);
//TH1F *Sum34hitAdc2 = new TH1F("2+3+4+5+6+7+8+9hits2","mV(2hits+3hits+4hits+5hits+6hits+7hits+8hits+9hits)",17, xbins);
//TH1F *Sum34hitAdc2 = new TH1F("2+3+4+5+6+7+8+9hits2","mV(2hits+3hits+4hits+5hits+6hits+7hits+8hits+9hits)",18 ,0, 1800);
//TH1F *Sum34hitAdc2 = new TH1F("2+3+4+5+6+7+8+9hits2","mV(2hits+3hits+4hits+5hits+6hits+7hits+8hits+9hits)",20 ,0, 4000);

///////////////for adc hisrogram/original adc histogram//////////////////////////

//c1->cd(5);
//tree->Draw("adc","chip_id!=8");

/////////////for adc histograms//////////////////////////

/*c1->cd(1);
TH1F *SinglehitAdc = new TH1F("1hit","adc",8,0,8);
SinglehitAdc->SetLineColor(kRed);
TH1F *SumDoublehitAdc = new TH1F("Sum2hits","adc",8,0,8);
SumDoublehitAdc->SetLineColor(kBlue);
TH1F *SumTriplehitAdc = new TH1F("Sum3hits","adc",8,0,8);
SumTriplehitAdc->SetLineColor(kOrange);
TH1F *Sum4hitAdc = new TH1F("Sum4hits","adc",8,0,8);
Sum4hitAdc->SetLineColor(kGreen);
TH1F *Sum5hitAdc = new TH1F("Sum5hits","adc",8,0,8);
Sum4hitAdc->SetLineColor(kBlack);
TH1F *Sum6hitAdc = new TH1F("Sum6hits","adc",8,0,8);
Sum4hitAdc->SetLineColor(kYellow);

//c1->cd(2);
TH1F *SumhitAdc = new TH1F("Sumhits","adc(1hit+2hits+3hits+4hits+5hits+6hits)",8,0,8);

//c1->cd(3);
TH1F *Sum34hitAdc = new TH1F("3+4+5+6hits","adc(3hits+4hits+5hits+6hits)",8,0,8);
*/


////////////////for dac hisrogram/Sum of clustered hits(2~4)//////////////////////
//c1->cd(6);
//TH1F *Sum234hitAdc2 = new TH1F("2+3+4hits2","mV(2hits+3hits+4hits)",17, xbins);
//TH1F *Sum234hitAdc2 = new TH1F("2+3+4hits2","mV(2hits+3hits+4hits)",18 ,0, 1800);
//TH1F *Sum234hitAdc2 = new TH1F("2+3+4hits2","mV(2hits+3hits+4hits)",20 ,0, 4000);




//hnhits->SetXTitle("");
//ifstream t("~/ROOT/macros/readtree.c");
//readtree t(tree);
//for(int i=0;i<500000;i++) t.next();
//c1->cd(1);
//SinglehitAdc->Draw();
//c1->cd(2);
//SinglehitChip->Draw();
//c1->cd(3);
//SinglehitChan->Draw();
}



