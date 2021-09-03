{
//////////////////////////////////////////////
//        clustered hits histograms
//  This is one of the files to get clustered hits hisotgram.
//  You can draw dac amplitude(mV) or adc histograms, and N hits per event hisrogram on a Canvas.
//
//  Before drawing, you have to get four files, "readtree_adc.c", "readtree_mV.c", "readtree.c", "readtree.h", "readtree_mV.cpp", "readtree.cc" .
//  Please change root file name on "readtree.h", and copy "readtree_mV" to "readtree.c", or copy "readtree_adc.c" to "readtree.c".
//  To draw histograms, you can write
//	            $ root -l rootfilename
//      	ROOT> .x readtree_mV.cpp;
//	        ROOT> .L readtree.c;
//	        ROOT> readtree t(tree);
//	        ROOT> .x readtree.cc;
///////////////////////////////////////////////

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <TH1.h>
for(int i=0;i<500000;i++) t.next();
/*for(int ii=1; ii<27;ii++){
	c1->cd(ii);
	hnhits[ii]->Draw("hist");
}
}*/

	c1->cd(4);
	SumDoublehitAdc2->Draw("hist");
	SinglehitAdc2->Draw("histsames");
	SumTriplehitAdc2->Draw("histsames");
	Sum4hitAdc2->Draw("histsames");
	Sum5hitAdc2->Draw("histsames");
	c1->cd(2);
	nhitsnChip->Draw("colz");
	c1->cd(3);
	nclsnChip->Draw("colz");
	c1->cd(1);
	hnhits->Draw();
	hnhits->SetStats(1);
	gStyle->SetOptStat(1);
	c1->cd(5);
	SumAdc2->Draw("hist");
	c1->cd(6);
	Wadc->Draw("hist");
/*c1->cd(3);
TGraph* gr1 = new TGraph(hitmapChip,hitmapChan);
gr1->SetMarkerStyle(20);
gr1->SetMarkerSize(1.5);
gr1->Draw("P");
*/
/*
//DoublehitAdc->Draw();
//c1->cd(2);
SumhitAdc->Draw();

//c1->cd(3);
Sum34hitAdc->Draw();
*/
//c1->cd(4);
//hnhits->Draw();
/*
c1->cd(1);
SinglehitAdc2->Draw("hist");
c1->cd(2);
SumDoublehitAdc2->Draw("hist");
//SumDoublehitAdc2->Fit("gaus");
//gStyle->SetOptFit();
c1->cd(3);
SumTriplehitAdc2->Draw("hists");
//SumTriplehitAdc2->Fit("gaus");
//gStyle->SetOptFit();
c1->cd(4);
Sum4hitAdc2->Draw("hist");
//Sum4hitAdc2->Fit("gaus");
//gStyle->SetOptFit();
c1->cd(5);
Sum5hitAdc2->Draw("hist");
//Sum5hitAdc2->Fit("gaus");
//gStyle->SetOptFit();
*/
//Sum6hitAdc2->Draw("histsames");
//Sum7hitAdc2->Draw("histsames");
//Sum8hitAdc2->Draw("histsames");
//Sum9hitAdc2->Draw("histsames");
//SinglehitAdc2->Fit("gaus");
//SumDoublehitAdc2->Fit("gaus");
//SumTriplehitAdc2->Fit("gaus");
//Sum4hitAdc2->Fit("gaus");
//Sum5hitAdc2->Fit("gaus");
//Sum6hitAdc2->Fit("gaus");
//Sum7hitAdc2->Fit("gaus");
//Sum8hitAdc2->Fit("gaus");
//Sum9hitAdc2->Fit("gaus");

//c1->cd(2);
//SumhitAdc2->Draw("hist");

//c1->cd(3);
//Sum34hitAdc2->Draw();
//Sum34hitAdc2->Fit("gaus");
//c1->cd(6);
//Sum234hitAdc2->Draw();

/*c1->cd(1);
c1->Update();
TPaveStats *st1 = (TPaveStats*)SinglehitAdc2->FindObject("stats");
st1->SetTextColor(kRed);
TPaveStats *st2 = (TPaveStats*)SumDoublehitAdc2->FindObject("stats");
st2->SetTextColor(kBlue);
TPaveStats *st3 = (TPaveStats*)SumTriplehitAdc2->FindObject("stats");
st3->SetTextColor(kOrange);
TPaveStats *st4 = (TPaveStats*)Sum4hitAdc2->FindObject("stats");
st4->SetTextColor(kGreen);
TPaveStats *st5 = (TPaveStats*)Sum5hitAdc2->FindObject("stats");
st5->SetTextColor(kPink);
*/
/*TPaveStats *st6 = (TPaveStats*)Sum6hitAdc2->FindObject("stats");
st6->SetTextColor(kViolet);
TPaveStats *st7 = (TPaveStats*)Sum7hitAdc2->FindObject("stats");
st7->SetTextColor(kTeal);
TPaveStats *st8 = (TPaveStats*)Sum8hitAdc2->FindObject("stats");
st8->SetTextColor(kCyan);
TPaveStats *st9 = (TPaveStats*)Sum9hitAdc2->FindObject("stats");
st9->SetTextColor(kAzure);*/
//c1->Modified();

/*
c1->Update();
c1->cd(1);
TPaveStats *st12 = (TPaveStats*)SinglehitAdc->FindObject("stats");
st12->SetTextColor(kRed);
TPaveStats *st22 = (TPaveStats*)SumDoublehitAdc->FindObject("stats");
st22->SetTextColor(kBlue);
TPaveStats *st32 = (TPaveStats*)SumTriplehitAdc->FindObject("stats");
st32->SetTextColor(kOrange);
TPaveStats *st42 = (TPaveStats*)Sum4hitAdc->FindObject("stats");
st42->SetTextColor(kGreen);
TPaveStats *st52 = (TPaveStats*)Sum5hitAdc->FindObject("stats");
st52->SetTextColor(kBlack);
TPaveStats *st62 = (TPaveStats*)Sum6hitAdc->FindObject("stats");
st62->SetTextColor(kYellow);

c1->Modified();*/
}
