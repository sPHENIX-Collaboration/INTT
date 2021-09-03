{
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <TH1.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "TNtuple.h"
#include "TStyle.h"
#include "TPad.h"
#include <fstream>
	

int chip_order[26]={26,25,24,23,22,21,20,19,18,17,16,15,14,
	            13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
TCanvas *c1=new TCanvas("c1","event vs chan_id",1625,250);
//TH2D* his1[26];
      
c1->Divide(13,2);
//for(int i=0;i<26;i++){
//his1[i] = new TH2D("his1[i]", "", 2500, 0, 2500, 128, 0, 128);
//}
/*gPad->SetTopMargin(0.10);
gPad->SetBottomMargin(0.08);
gPad->SetRightMargin(0.03);
gPad->SetLeftMargin(0.08);*/
//  std::string stmp=Form("ampl<70&&module==%d&&chip_id==%d&&chan_id<127",3,chip_order[i]);
c1->cd(1);
tree->Draw("event:chan_id","chip_id==1","colz");
c1->cd(2);
tree->Draw("event:chan_id","chip_id==2","colz");
c1->cd(3);
tree->Draw("event:chan_id","chip_id==3","colz");
c1->cd(4);
tree->Draw("event:chan_id","chip_id==4","colz");
c1->cd(5);
tree->Draw("event:chan_id","chip_id==5","colz");
c1->cd(6);
tree->Draw("event:chan_id","chip_id==6","colz");
c1->cd(7);
tree->Draw("event:chan_id","chip_id==7","colz");
c1->cd(8);
tree->Draw("event:chan_id","chip_id==8","colz");
c1->cd(9);
tree->Draw("event:chan_id","chip_id==9","colz");
c1->cd(10);
tree->Draw("event:chan_id","chip_id==10","colz");
c1->cd(11);
tree->Draw("event:chan_id","chip_id==11","colz");
c1->cd(12);
tree->Draw("event:chan_id","chip_id==12","colz");
c1->cd(13);
tree->Draw("event:chan_id","chip_id==13","colz");
c1->cd(14);
tree->Draw("event:chan_id","chip_id==14","colz");
c1->cd(15);
tree->Draw("event:chan_id","chip_id==15","colz");
c1->cd(16);
tree->Draw("event:chan_id","chip_id==16","colz");
c1->cd(17);
tree->Draw("event:chan_id","chip_id==17","colz");
c1->cd(18);
tree->Draw("event:chan_id","chip_id==18","colz");
c1->cd(19);
tree->Draw("event:chan_id","chip_id==19","colz");
c1->cd(20);
tree->Draw("event:chan_id","chip_id==20","colz");
c1->cd(21);
tree->Draw("event:chan_id","chip_id==21","colz");
c1->cd(22);
tree->Draw("event:chan_id","chip_id==22","colz");
c1->cd(23);
tree->Draw("event:chan_id","chip_id==23","colz");
c1->cd(24);
tree->Draw("event:chan_id","chip_id==24","colz");
c1->cd(25);
tree->Draw("event:chan_id","chip_id==25","colz");
c1->cd(26);
tree->Draw("event:chan_id","chip_id==26","colz");
//  stmp=Form("chip_id=%d",chip_order[i]);   
//	TPaveText *tx1b = new TPaveText(10,8.3,60,8.3,"br");
	
//	tx1b->AddText(stmp.c_str());  	
//	tx1b->SetTextSize(0.1);
//	tx1b->SetBorderSize(0);
//	tx1b->Draw("same"); 
c1->Update();   
}

