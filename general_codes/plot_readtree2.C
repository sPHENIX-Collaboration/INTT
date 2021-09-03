{
	int	nhit_1[9]; 
	int	nhit_2[9];
	for(int ii=1;ii<10;ii++){
		nhit_1[ii]=0;	
		nhit_2[ii]=0;	
	}
	TFile *rootfile = TFile::Open("histofphx_raw_20200728-1905_0.root");
	TCanvas *c1 = new TCanvas("c1", "energy loss", 1000, 280);
	c1->Divide(4,1);
	TCanvas *c2 = new TCanvas("c2", "correlation of double hits", 800, 590);
	c2->Divide(3,2);
	TCanvas *c3 = new TCanvas("c3", "module differense", 1400, 1300);
	c3->Divide(4,4);
	TCanvas *c4 = new TCanvas("c4", "cosmic", 600, 590);
	c4->Divide(3,2);
	TCanvas *c5 = new TCanvas("c5", "bco and bco_full intervals", 800, 300);
	c5->Divide(3,1);
	TCanvas *c6 = new TCanvas("c6", "energy loss per strip number", 1000, 600);
	c6->Divide(4,2);
	TCanvas *c7 = new TCanvas("c7", "module differense", 800, 590);
	c7->Divide(3,2);
	TCanvas *c9 = new TCanvas("c9", "channel differense = 3", 1200, 500);
	c9->Divide(5,2);
	TCanvas *c10 = new TCanvas("c10", "energy loss", 500, 500);
	
	c1->cd(1);//before event selection
	Wadc->Draw("hist");
	c1->cd(2);
	singlemV->Draw("hist");//before event selection
	c1->cd(3);//before cluster adc
	//SumAdc2->Draw("hist");
	DAdc->Draw("hist");
	SingleAdc->Draw("histsames");
	TAdc->Draw("histsames");
	c1->cd(4);//after cluster mV
	SinglehitAdc2->Draw("hist");
	SumDoublehitAdc2->Draw("histsames");
	SumTriplehitAdc2->Draw("histsames");
  c2->cd(1);
  Adc_2hit->Draw("colz");   
  c2->cd(2); 
  chan_2hit->Draw("colz");  
	c2->cd(4); 
  NHITChip->Draw("colz");    
	c2->cd(5); 
  chan_1hit->Draw("colz");  
	c2->cd(6); 
  chan_1hit2->Draw();
	c7->cd(1);//module differense
	CHIPDIFF->Draw("colz");
	c7->cd(2);
	ELOSSDIFF->Draw("colz");
	c7->cd(3);
	ELOSSTRAN->Draw("colz");
	c7->cd(4);
	NCLS6->Draw("");
	NCLS8->Draw("sames");
	c7->cd(5);
	NSTRIP6->Draw("");
	NSTRIP8->Draw("sames");
	c3->cd(1);
  CHANDIFFSA1->Draw("hist"); 			
	c3->cd(2);
  CHANDIFFSA2->Draw("hist"); 			
	c3->cd(3);
  CHANDIFFSA3->Draw("hist"); 			
	c3->cd(4);
  CHANDIFFSA4->Draw("hist"); 			
	c3->cd(5);
  ELOSSDIFF1->Draw("colz"); 			
	c3->cd(6);
  ELOSSDIFF2->Draw("colz"); 			
	c3->cd(7);
  ELOSSDIFF3->Draw("colz"); 			
	c3->cd(8);
  ELOSSDIFF4->Draw("colz"); 			
	c6->cd(1);
	ELOSSTRAN1->Draw("colz");
	c6->cd(2);
	ELOSSTRAN2->Draw("colz");
	c6->cd(3);
	ELOSSTRAN3->Draw("colz");
	c6->cd(4);
	ELOSSTRAN4->Draw("colz");
	c6->cd(5);
  ELOSSCLS1_1->Draw("hist");
  ELOSSCLS2_1->Draw("histsames");
	c6->cd(6);
  ELOSSCLS1_2->Draw("hist");
  ELOSSCLS2_2->Draw("histsames");
	c6->cd(7);
  ELOSSCLS1_3->Draw("hist");
  ELOSSCLS2_3->Draw("histsames");
	c6->cd(8);
  ELOSSCLS1_4->Draw("hist");
  ELOSSCLS2_4->Draw("histsames");
	c3->cd(9);
  ELOSSCLS1_6_1->Draw("hist");
  ELOSSCLS2_6_1->Draw("histsames");
	c3->cd(10);
  ELOSSCLS1_6_2->Draw("hist");
  ELOSSCLS2_6_2->Draw("histsames");
	c3->cd(11);
  ELOSSCLS1_6_3->Draw("hist");
  ELOSSCLS2_6_3->Draw("histsames");
	c3->cd(12);
  ELOSSCLS1_6_4->Draw("hist");
  ELOSSCLS2_6_4->Draw("histsames");
	c3->cd(13);
  ELOSSCLS1_8_1->Draw("hist");
  ELOSSCLS2_8_1->Draw("histsames");
	c3->cd(14);
  ELOSSCLS1_8_2->Draw("hist");
  ELOSSCLS2_8_2->Draw("histsames");
	c3->cd(15);
  ELOSSCLS1_8_3->Draw("hist");
  ELOSSCLS2_8_3->Draw("histsames");
	c3->cd(16);
  ELOSSCLS1_8_4->Draw("hist");
  ELOSSCLS2_8_4->Draw("histsames");
	c10->cd();
	//ReNhit->Draw("hist");
	c9->cd(1);
	ELOSSTRANS->Draw("colz");
	c9->cd(2);
  ELOSSCLS1_S->Draw("hist");
  ELOSSCLS2_S->Draw("histsames");
	c9->cd(3);
  ELOSSCLS1__S->Draw("hist");
  ELOSSCLS2__S->Draw("histsames");
	c9->cd(4);
  CHIPDIFFSAh->Draw("hist");
  CHIPDIFFSAh->Fit("gaus","","",-4,11);
	c9->cd(5);
  CHANDIFFSAh->Draw("hist");
  CHANDIFFSAh->Fit("gaus");
	c9->cd(6);
  ADCDIFF->Draw("colz");
	c9->cd(7);
  RECELOSS1->Draw("hist");
  RECELOSS2->Draw("histsames");
  //RECELOSS3->Draw("histsames");
	c9->cd(8);
  CHANDIFFS->Draw("colz");
	c9->cd(9);
	//ReNhit->Draw("hist");
	gStyle->SetOptStat(1000000011);
	for(int ii=1;ii<10;ii++){
		nhit_1[ii]=	ELOSSCLS1__S->GetBinContent(ii); 
		nhit_2[ii]=	ELOSSCLS2__S->GetBinContent(ii); 
	}
	int sum1=0;
	int sum2=0;
	for(int ii=1;ii<10;ii++){
		cout<<nhit_1[ii]<<", ";
		sum1+=nhit_1[ii];
	}
	cout<<" sum="<<sum1<<endl; 
	for(int ii=1;ii<10;ii++){
		cout<<sqrt(nhit_1[ii])<<", ";
	}
	cout<<" "<<endl; 
	for(int ii=1;ii<10;ii++){
		cout<<nhit_2[ii]<<", ";
		sum2+=nhit_2[ii];
	} 
	cout<<" sum="<<sum2<<endl; 
	for(int ii=1;ii<10;ii++){
		cout<<sqrt(nhit_2[ii])<<", ";
	}
	cout<<" "<<endl; 
	c4->cd(1);//number of hits per event
	hnhits->Draw();
	c4->cd(2);//number of clusters per event
	nclsnChip->Draw();
	c4->cd(3);//multiple cluster hit chip
	multiclschip->Draw("colz");
	c4->cd(4);
	NHITChipmV->Draw("hist");
	c4->cd(5);
	adc_0hit->Draw("colz");
	c4->cd(6);
	m_noiseAdc->Draw("hist");
	c5->cd(1);//bco andbco interval
	bco_interval->Draw();
	c5->cd(2);
	bcofull_interval->Draw();
	c5->cd(3);
	adc_bcofull->Draw("hist");
	gStyle->SetOptStat(1000000010);
	c1->Modified();
	c2->Modified();
	c3->Modified();
	c4->Modified();
	c5->Modified();
	c6->Modified();
	c7->Modified();
	c9->Modified();
}
