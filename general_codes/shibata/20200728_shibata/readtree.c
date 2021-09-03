////////////////////////////////////////////
//        clustered hits histograms
//  This is one of the files to get clustered hits hisotgram.
//  You can draw dac amplitude(mV) histograms, and N hits per event hisrogram on a Canvas.
//
//  Before drawing, you have to get four files, "readtree_mV.c", "readtree.c", "readtree.h", "readtree_mV.cpp", "readtree.cc" .
//  Please change root file name on "readtree.h", and copy "readtree_mV" to "readtree.c".
//  To draw histograms, you can write
//	            $ root -l rootfilename
//      	ROOT> .x readtree_mV.cpp;
//	        ROOT> .L readtree.c;
//	        ROOT> readtree t(tree);
//	        ROOT> .x readtree.cc;
/////////////////////////////////////////

#define readtree_cxx
#include "readtree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <TH1.h>

double weight(int hitAdc){
	float	Dac[9] = {10, 23, 60, 98, 135, 173, 210, 248, 256};
	float weight[8];
	float tmpw = Dac[1]-Dac[0];
  for(int i=0; i<8; i++){
		weight[i]	= (Dac[hitAdc+1]-Dac[hitAdc])/tmpw;
	}
	float w = 1./weight[hitAdc];
	return w;
}


int adc_mV(int hitAdc){
//	int Dac[9] = {20, 23, 60, 98, 135, 173, 210, 248, 256};
//	int Dac[9] = {13, 28, 43, 58, 73, 88, 103, 118, 256};
//	int Dac[9] = {13, 45, 78, 110, 143, 175, 208, 240, 256};
int Dac[9] = {10, 23, 60, 98, 135, 173, 210, 248, 256};
//	int mV = ((Dac[hitAdc+1]+Dac[hitAdc])/2)*4+210-5;
	int mV = ((Dac[hitAdc+1]*4+210-5)+(Dac[hitAdc]*4+210-5))/2;
	return mV;
}

/*int gdacw(int hitAdc){
				
  float dac[9] = {13,45,78,110,143,175,208,240,256};
	float dacw[8];
	for (int i=0;i<8;i++){
					dacw[i] = (dac[i+1] - dac[i])/13.;
	}
	double ndacw = dacw[hitAdc];
	return ndacw;
}*/
/*double* process_hits2(int nhit, int* chipArray, int* chanArray, int* adcArray, int* eventArray){

  int hitChip[27][800];
  int hitChan[27][800];
  int hitAdc [27][800];
  int hitEvent[27][800];

  int nchan=0, nchip=0;
	int nHitChip[5];
	for(int i=0;i<5;i++){
					nHitChip[i]=0;
	}
  int prechip[5]={-1,-1,-1,-1,-1};
  int tmp1, tmp2, tmp3, tmp4;
	int tmp5=0;
  
  // fill to chip-by-chip
  for(int ihit=0; ihit<nhit; ihit++){
		int ichip = chipArray[ihit];

		for(int i=0; i<5;i++){
			if(prechip[i]==ichip){
				tmp5=1;
				nHitChip[i]++;
			}
		}
		if(tmp5==0){
			prechip[nchip]=ichip;
			nchip++;
		}
    hitChip[ichip][ihit] = chipArray[ihit];
    hitChan[ichip][ihit] = chanArray[ihit];
    hitAdc [ichip][ihit] = adcArray[ihit];
    hitEvent[ichip][ihit]= eventArray[ihit];
    nHitChip[nchip]++;
  }
  ///////// sort channel order//////////////
  for(int ichip=0; ichip<nchip; ichip++){
    
		for(int ihit=0; ihit<nHitChip[nchip]; ihit++){

			for(int jhit=ihit+1;jhit<nHitChip[nchip];jhit++){
				int jchip = chipArray[jhit];
				if(ichip==jchip){
					if(hitChan[ichip][ihit]>hitChan[ichip][jhit]){
						tmp1  = hitChan[ichip][ihit];
						tmp2 = hitChip[ichip][ihit];
  				 	tmp3 = hitAdc[ichip][ihit];
  					tmp4 = hitEvent[ichip][ihit];
  				 	hitChan[ichip][ihit] = hitChan[ichip][jhit];
  				 	hitChip[ichip][ihit] = hitChip[ichip][jhit];
  				 	hitAdc[ichip][ihit]  = hitAdc[ichip][jhit];
  				 	hitEvent[ichip][ihit]  = hitEvent[ichip][jhit];
  				 	hitChan[ichip][jhit] = tmp1;
   				 	hitChip[ichip][jhit] = tmp2;
   				 	hitAdc[ichip][jhit]  = tmp3;
   				 	hitEvent[ichip][jhit]  = tmp4;
					}
				}
			}
		}
	}
	
   //////// check single hit/////////////////
  double hitmapChip[5000];
  double hitmapChan[5000];
	
	int nhitmap=0;
	for(int ichip=0; ichip<nchip; ichip++){
		int Nhits=1;
		for(int ihit=0;ihit<nHitChip[nchip];ihit++){
			if(ihit==nHitChip[nchip]-2){
				if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
					Nhits++;	
					if(Nhits==2){
						int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit+1])/Nhits;
						nhitmap++;
					}
					if(Nhits==3){
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitAdc[ichip][ihit-1]+hitChip[ichip][ihit+1])/Nhits;
						nhitmap++;
						int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]);
					}
					if(Nhits==4){
						int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2]+hitChip[ichip][ihit+1])/Nhits;
						nhitmap++;
					}		
					if(Nhits==5){
	 	      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit+1]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitAdc[ichip][ihit-2]+hitAdc[ichip][ihit-3]+hitAdc[ichip][ihit+1])/Nhits;
						nhitmap++;
		 	    }
				}
				if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){	
					
					hitmapChip[nhitmap] = hitChip[ichip][ihit+1];
					hitmapChan[nhitmap] = hitChip[ichip][ihit+1];
					nhitmap++;
			    if(adc_mV(hitAdc[ichip][ihit+1])>2000){
						cout<<hitAdc[ichip][ihit+1]<<endl;
					}
					
					if(Nhits==1){
						
						int summV = adc_mV(hitAdc[ichip][ihit]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = hitChip[ichip][ihit];
						nhitmap++;
			    	if(adc_mV(hitAdc[ichip][ihit])>2000){
							cout<<hitAdc[ichip][ihit]<<endl;
						}
					}
					if(Nhits==2){
						int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1])/Nhits;
						nhitmap++;
					}
					if(Nhits==3){
	 	      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2])/Nhits;
						nhitmap++;
 					}
					if(Nhits==4){
	 	    		int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2]+hitChip[ichip][ihit-3])/Nhits;
						nhitmap++;
					}	
					if(Nhits==5){
	 	      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2]+hitChip[ichip][ihit-3]+hitChip[ichip][ihit-4])/Nhits;
						nhitmap++;
					}
					Nhits=1;
				}
			}
			if(ihit<nHitChip[nchip]-2){
				if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
					Nhits++;
				}
				if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){
					if(Nhits==1){
						int summV = adc_mV(hitAdc[ichip][ihit]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = hitChip[ichip][ihit];
						nhitmap++;
					}
					if(Nhits==2){
						int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1])/Nhits;
						nhitmap++;
					}
					if(Nhits==3){
	 	      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2])/Nhits;
						nhitmap++;
					}
					if(Nhits==4){
	 	    		int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2]+hitChip[ichip][ihit-3])/Nhits;
						nhitmap++;
					}
					if(Nhits==5){
	 	      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChip[ichip][ihit]+hitChip[ichip][ihit-1]+hitChip[ichip][ihit-2]+hitChip[ichip][ihit-3]+hitChip[ichip][ihit-4])/Nhits;
						nhitmap++;
		 	  	}
					Nhits=1;
				}
			}
		}
	}

c1->cd(3);
TGraph* gr1 = new TGraph(nhitmap,hitmapChip,hitmapChan);
gr1->SetMarkerStyle(20);
gr1->SetMarkerSize(1.5);
gr1->Draw("AP");
};
*/
void process_hits(int nhit, int* chipArray, int* chanArray, int* adcArray, int* eventArray, int* bcoArray, int* bco_fullArray){

  int hitChip[27][800];
  int hitChan[27][800];
  int hitAdc [27][800];
  int hitEvent[27][800];
  int hitBco[27][800];
  int hitBco_full[27][800];

	int nchip=0;
	int nHitChip[27];
	for(int i=0;i<27;i++){
		nHitChip[i] = 0;
	}
  int tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
  
	
	
	
	for(int ihit=0; ihit<nhit; ihit++){
		int ichip = chipArray[ihit];
    int ii = nHitChip[ichip];
   	hitChip[ichip][ii] = chipArray[ihit];
   	hitChan[ichip][ii] = chanArray[ihit];
   	hitAdc [ichip][ii] = adcArray[ihit];
   	hitEvent[ichip][ii]= eventArray[ihit];
   	hitBco[ichip][ii]= bcoArray[ihit];
   	hitBco_full[ichip][ii]= bco_fullArray[ihit];
    nHitChip[ichip]++;
//		cout<<" clk:bco="<<hitBco[ichip][ii]<<" bco_full="<<hitBco_full[ichip][ii]<<" data:chip_id="<<hitChip[ichip][ii]<<" chan_id="<<hitChan[ichip][ii]<<" abc="<<hitAdc[ichip][ii]<<endl;;
	}
  
	
	
///////// sort channel order//////////////
  for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>0){
// 		cout<<"nHitChip="<<nHitChip[ichip]<<endl;
			for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
				for(int jhit=ihit+1;jhit<nHitChip[ichip];jhit++){
					if(hitChan[ichip][ihit]>hitChan[ichip][jhit]){
						tmp1  = hitChan[ichip][ihit];
						tmp2 = hitChip[ichip][ihit];
  				 	tmp3 = hitAdc[ichip][ihit];
  					tmp4 = hitEvent[ichip][ihit];
   					tmp5 = hitBco[ichip][ihit];;
   					tmp6 = hitBco_full[ichip][ihit];
  				 	hitChan[ichip][ihit] = hitChan[ichip][jhit];
  				 	hitChip[ichip][ihit] = hitChip[ichip][jhit];
  				 	hitAdc[ichip][ihit]  = hitAdc[ichip][jhit];
  				 	hitEvent[ichip][ihit]  = hitEvent[ichip][jhit];
   					hitBco[ichip][ihit]= hitBco[ichip][jhit];
   					hitBco_full[ichip][ihit]= hitBco_full[ichip][jhit];
  				 	hitChan[ichip][jhit] = tmp1;
   				 	hitChip[ichip][jhit] = tmp2;
   				 	hitAdc[ichip][jhit]  = tmp3;
   				 	hitEvent[ichip][jhit]  = tmp4;
   					hitBco[ichip][jhit]= tmp5;
   					hitBco_full[ichip][jhit]= tmp6;
//    		  cout<<hitChan[ichip][jhit]<<endl;
//    		  cout<<hitChip[ichip][jhit]<<endl;
//    		  cout<<hitAdc[ichip][jhit] <<endl;
					}
				}
			}
		}
	}
	
/*	for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>0){
//			cout<<"nHitChip="<<nHitChip[ichip]<<endl;
			for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
				cout<<" clk:bco="<<hitBco[ichip][ihit]<<" bco_full="<<hitBco_full[ichip][ihit]<<" data:chip_id="<<hitChip[ichip][ihit]<<" chan_id="<<hitChan[ichip][ihit]<<" abc="<<hitAdc[ichip][ihit]<<endl;;
			}
		}
	}*/

   //////// check single hit/////////////////
	double hitmapChip[5000];
  double hitmapChan[5000];
	
	int nhitmap=0;
	for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>0){
			int Ncls = 0;
			int start = 0;
			int Nhits =1;
			int end;
//   		cout<<"nHitChip="<<nHitChip[ichip]<<endl;
			for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
	
//   		  cout<<"hitChan="<<hitChan[ichip][ihit]+1<<"  hitChip="<<hitChip[ichip][ihit]<<endl;
				if(ihit+1==nHitChip[ichip]){
					if(hitChan[ichip][ihit+1]-hitChan[ichip][ihit]==1){
								Nhits++;
					}
// 			  	cout<<"Nhits="<<Nhits<<endl;
					end = ihit;
					int summV=0;
					int sumChan =0;
					for(int i=start; i<end+1; i++){
					summV += adc_mV(hitAdc[ichip][i]);
					sumChan += hitChan[ichip][i];
					}
					hitmapChip[Ncls] = hitChip[ichip][ihit];
					hitmapChan[Ncls] = sumChan/Nhits;
					c1->cd(1);
					hnhits->Fill(Nhits);
					c1->cd(2);
					nhitsnChip->Fill(Nhits,hitChip[ichip][ihit]);
					
					if(Nhits>20){
						//	for(int ii=0;ii<Nhits;ii++){
			   		  //	if(hitAdc[ichip][ihit]!=1){
									for(int ii=0;ii<Nhits;ii++){
//											cout<<"hitChan="<<hitChan[ichip][ihit-ii]<<"  hitChip="<<hitChip[ichip][ihit-ii]<<"  hitAdc="<<hitAdc[ichip][ihit-ii]<<"  hitEvent[ichip][jhit]="<<hitEvent[ichip][ihit-ii]<<endl;
									}
							//	}
						//	}
						}
					c1->cd(4);
					if(Nhits==1)SinglehitAdc2->Fill(summV);
		 	 		if(Nhits==2)SumDoublehitAdc2->Fill(summV);
		 	 		if(Nhits==3)SumTriplehitAdc2->Fill(summV);
 		 	 		if(Nhits==4)Sum4hitAdc2->Fill(summV);
		 	 		if(Nhits==5)Sum5hitAdc2->Fill(summV);
					c1->cd(5);
					SumAdc2->Fill(summV);
					Nhits = 1;
					Ncls++;
				}
				else{
					if(hitChan[ichip][ihit+1]-hitChan[ichip][ihit]==1){
							Nhits++;
// 				  	cout<<"Nhits="<<Nhits<<endl;
					}
					else{
//						cout<<"Nhits="<<Nhits<<endl;
						end = ihit;
						int summV=0;
						int sumChan =0;
						for(int i=start; i<end+1; i++){
						summV += adc_mV(hitAdc[ichip][i]);
						sumChan += hitChan[ichip][i];
						}
						hitmapChip[Ncls] = hitChip[ichip][ihit];
						hitmapChan[Ncls] = sumChan/Nhits;
						c1->cd(1);
						hnhits->Fill(Nhits);
						c1->cd(2);
						nhitsnChip->Fill(Nhits, hitChip[ichip][ihit]);
						if(Nhits>30){
//			   		  cout<<"hitChan="<<hitChan[ichip][ihit]+1<<"  hitChip="<<hitChip[ichip][ihit]<<"  hitAdc="<<hitAdc[ichip][ihit]<<endl;
						}

						c1->cd(4);
						if(Nhits==1)SinglehitAdc2->Fill(summV);
		 		 		if(Nhits==2)SumDoublehitAdc2->Fill(summV);
		 		 		if(Nhits==3)SumTriplehitAdc2->Fill(summV);
 		 		 		if(Nhits==4)Sum4hitAdc2->Fill(summV);
		 		 		if(Nhits==5)Sum5hitAdc2->Fill(summV);
					c1->cd(5);
					SumAdc2->Fill(summV);
						start = ihit+1;
						Ncls++;
						Nhits = 1;
					}
				}
			}
			c1->cd(3);
			nclsnChip->Fill(Ncls, ichip);
		}
	}


	/*c1->cd(5);
	TGraph* gr1 = new TGraph(nhitmap,hitmapChip,hitmapChan);
	gr1->SetMarkerStyle(2);
	gr1->GetXaxis()->SetLimits(0.,27.);
	gr1->GetYaxis()->SetLimits(0.,128);
	gr1->SetMarkerSize(1.5);
	gr1->Draw("AP");
	gr1->GetXaxis()->SetRangeUser(0.,27.);
	gr1->GetYaxis()->SetRangeUser(0.,128.);
	gr1->Draw("AP");
	c1->Update();
	*/


/*
   //////// check single hit/////////////////
  double hitmapChip[5000];
  double hitmapChan[5000];
	
	int nhitmap=0;
	for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>0){
			for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
				int Nhits=1;
				if(nHitChip[ichip]==1){
					int ihit=0;
					int summV = adc_mV(hitAdc[ichip][ihit]);
					hitmapChip[nhitmap] = hitChip[ichip][ihit];
					hitmapChan[nhitmap] = hitChan[ichip][ihit];
//				cout<<"nhitmap1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
					nhitmap++;	
					c1->cd(4);
					hnhits->Fill(Nhits);
					c1->cd(1);
 				 	SinglehitAdc2->Fill(summV);
					c1->cd(2);
//				nhitsnChip->Fill(hitChip[ichip][ihit],Nhits,summV);
					nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
				}


				if(nHitChip[ichip]==2){
					int ihit=0;
					if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
						Nhits++;	
						int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit+1])/Nhits;
//					cout<<"nhitmap2"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
						nhitmap++;	
						c1->cd(2);
						nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
						c1->cd(1);
	 				 	SumDoublehitAdc2->Fill(summV);		
					}
					if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){	
						int summV = adc_mV(hitAdc[ichip][ihit]);
						hitmapChip[nhitmap] = hitChip[ichip][ihit];
						hitmapChan[nhitmap] = hitChan[ichip][ihit];
//					cout<<"nhitmap11"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
						c1->cd(1);
 					 	SinglehitAdc2->Fill(summV);
						c1->cd(2);
						nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
						nhitmap++;
						
					  hitmapChip[nhitmap] = hitChip[ichip][ihit+1];
						hitmapChan[nhitmap] = hitChan[ichip][ihit+1];
//					cout<<"nhitmap11+1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
						c1->cd(1);
						summV = adc_mV(hitAdc[ichip][ihit+1]);
 					 	SinglehitAdc2->Fill(summV);
//					cout<<"nhitmap111"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
						c1->cd(2);
						nhitsnChip->Fill(hitChip[ichip][ihit+1],Nhits);
						nhitmap++;
					}
						c1->cd(4);
						hnhits->Fill(Nhits);
						hnhits->Fill(Nhits);
					Nhits=1;
				}
	


		
				if(nHitChip[ichip]>2){
					for(int ihit=0;ihit<nHitChip[ichip];ihit++){
						if(ihit==nHitChip[ichip]-1){
							if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
								Nhits++;	
								if(Nhits==2){
									int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]);
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									c1->cd(1);
			  	 			 	SumDoublehitAdc2->Fill(summV);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit+1])/Nhits;
									nhitmap++;
								}
								if(Nhits==3){
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit+1])/Nhits;
									nhitmap++;
									int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]);
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									c1->cd(1);
			 		      	SumTriplehitAdc2->Fill(summV);
								}
								if(Nhits==4){
									int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit+1])/Nhits;
									nhitmap++;
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									c1->cd(1);
			 		    		Sum4hitAdc2->Fill(summV);
								}		
								if(Nhits==5){
			 		      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit+1]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3]+hitChan[ichip][ihit+1])/Nhits;
									nhitmap++;
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									c1->cd(1);
			 		    		Sum5hitAdc2->Fill(summV);
					 	   	}
									c1->cd(4);
									hnhits->Fill(Nhits);
						 	  Nhits=1;
							}
							if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){	
								if(Nhits==1){
					 	 			int summV = adc_mV(hitAdc[ichip][ihit]);
						 			hitmapChip[nhitmap] = hitChip[ichip][ihit];
				 		 			hitmapChan[nhitmap] = hitChan[ichip][ihit];
									c1->cd(1);
 				 		 			SinglehitAdc2->Fill(summV);
//								cout<<"nhitmap1111"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
					 	 			c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
				 		 			nhitmap++;
					 	 		}
					 	 		if(Nhits==2){
					 	 			int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]);
					 	 			hitmapChip[nhitmap] = hitChip[ichip][ihit];
					 	 			hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1])/Nhits;
					 	 			c1->cd(1);
			   	 	 			SumDoublehitAdc2->Fill(summV);
					 	 			c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
					 	 			nhitmap++;
					 	 		}
					 	 		if(Nhits==3){
			 	   	 	  	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]);
					 	 			hitmapChip[nhitmap] = hitChip[ichip][ihit];
					 	 			hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2])/Nhits;
					 	 			c1->cd(1);
			 	   	 	  	SumTriplehitAdc2->Fill(summV);
					 	 			c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
					 	 			nhitmap++;
 					 	 		}
					 	 		if(Nhits==4){
			 	   	 			int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]);
					 	 			hitmapChip[nhitmap] = hitChip[ichip][ihit];
					 	 			hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3])/Nhits;
					 	 			c1->cd(1);
			 	   	 			Sum4hitAdc2->Fill(summV);
					 	 			c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
					 	 			nhitmap++;
					 	 		}	
					 	 		if(Nhits==5){
			 	   	 	  	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]);
					 	 			hitmapChip[nhitmap] = hitChip[ichip][ihit];
					 	 			hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3]+hitChan[ichip][ihit-4])/Nhits;
					 	 			c1->cd(1);
			 	   	 	  	Sum5hitAdc2->Fill(summV);
					 	 			c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
					 	 			nhitmap++;
					 	 		}
					 	 		hitmapChip[nhitmap] = hitChip[ichip][ihit+1];
					 	 		hitmapChan[nhitmap] = hitChan[ichip][ihit+1];
					 	 		c1->cd(1);
					 	 		SinglehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit+1]));
//							cout<<"nhitmap1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
					 	 		c1->cd(2);
								nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
					 	 		nhitmap++; 
									c1->cd(4);
									hnhits->Fill(Nhits);
					 	 		Nhits=1;
									c1->cd(4);
									hnhits->Fill(Nhits);
							}
						}		
				
				
						if(ihit<nHitChip[ichip]-2){
							if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
								Nhits++;
							}
							if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){
								if(Nhits==1){
									int summV = adc_mV(hitAdc[ichip][ihit]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = hitChan[ichip][ihit];
									c1->cd(1);
									SinglehitAdc2->Fill(summV);
//								cout<<"nhitmap1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
						 			nhitmap++;
								}
								if(Nhits==2){
									int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1])/Nhits;
									c1->cd(1);
		  			 			SumDoublehitAdc2->Fill(summV);
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									nhitmap++;
								}
								if(Nhits==3){
		 				     	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2])/Nhits;
									c1->cd(1);
		 				 			SumTriplehitAdc2->Fill(summV);
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									nhitmap++;
								}
								if(Nhits==4){
		 				   		int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3])/Nhits;
									c1->cd(1);
		 				 			Sum4hitAdc2->Fill(summV);
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									nhitmap++;
		 				 		}	
								if(Nhits==5){
		 				     	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]);
									hitmapChip[nhitmap] = hitChip[ichip][ihit];
									hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3]+hitChan[ichip][ihit-4])/Nhits;
									c1->cd(1);
		 				 			Sum5hitAdc2->Fill(summV);
									c1->cd(2);
									nhitsnChip->Fill(hitChip[ichip][ihit],Nhits);
									nhitmap++;
								}
									c1->cd(4);
									hnhits->Fill(Nhits);
								Nhits=1;
							}
						}
					}	
				}
			}
		}
	}
*/
/*c1->cd(5);
TGraph* gr1 = new TGraph(nhitmap,hitmapChip,hitmapChan);
gr1->SetMarkerStyle(2);
gr1->GetXaxis()->SetLimits(0.,27.);
gr1->GetYaxis()->SetLimits(0.,128);
gr1->SetMarkerSize(1.5);
gr1->Draw("AP");
gr1->GetXaxis()->SetRangeUser(0.,27.);
gr1->GetYaxis()->SetRangeUser(0.,128.);
gr1->Draw("AP");
c1->Update();
*/
/*   //////// check single hit/////////////////
  float hitmapChip[5000];
  float hitmapChan[5000];

	int nhitmap=0;
  for(int ii=1; ii<nchip+1; ii++){
//		cout<<"nchip="<<nchip<<endl;
		int ichip = prechip[ii];
		int Nhits=1;
		
		if(nHitChip[nchip]==1){
			int ihit=0;
			int summV = adc_mV(hitAdc[ichip][ihit]);
			hitmapChip[nhitmap] = hitChip[ichip][ihit];
			hitmapChan[nhitmap] = hitChan[ichip][ihit];
//    	cout<<hitChan[ichip][ihit]<<endl;
//			cout<<"nhitmap1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
				nhitmap++;
		}
		
	
		if(nHitChip[nchip]==2){
			int ihit=0;
			
			if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
				Nhits++;	
				int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]);
				hitmapChip[nhitmap] = hitChip[ichip][ihit];
				hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit+1])/Nhits;
//				cout<<"nhitmap2"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
				nhitmap++;
				Nhits=1;
			}
			if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){	
				int summV = adc_mV(hitAdc[ichip][ihit]);
				hitmapChip[nhitmap] = hitChip[ichip][ihit];
				hitmapChan[nhitmap] = hitChan[ichip][ihit];
				nhitmap++;
//    		cout<<hitChan[ichip][ihit]<<endl;
//				cout<<"nhitmap11"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
				hitmapChip[nhitmap] = hitChip[ichip][ihit+1];
				hitmapChan[nhitmap] = hitChan[ichip][ihit+1];
//    	  cout<<hitChan[ichip][ihit+1]<<endl;
//				cout<<"nhitmap1-1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
				nhitmap++;
				Nhits=1;
			}
		}
		
		
		if(nHitChip[nchip]>2){
			for(int ihit=0;ihit<nHitChip[nchip];ihit++){
				
				if(ihit==nHitChip[nchip]-2){
					
					if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
						Nhits++;	
						if(Nhits==2){
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit+1])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//    		  	cout<<hitChip[ichip][ihit]<<endl;
//							cout<<"nhitmap2"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						if(Nhits==3){
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit+1])/Nhits;
//    		  		cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap3"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]);
						}
						if(Nhits==4){
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit+1])/Nhits;
//	    		  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap4"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}		
						if(Nhits==5){
		 	      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit+1]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3]+hitChan[ichip][ihit+1])/Nhits;
//    		  		cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap5"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
		 	    	}
						Nhits=1;
					}
					if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){	
						
						if(Nhits==1){
							int summV = adc_mV(hitAdc[ichip][ihit]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = hitChan[ichip][ihit];
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap11"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						if(Nhits==2){
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1])/Nhits;
//							cout<<"nhitmap22"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
							nhitmap++;
						}
						if(Nhits==3){
	 		      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap33"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
 						}
						if(Nhits==4){
	 		    		int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap44"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}	
						if(Nhits==5){
	 		      	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3]+hitChan[ichip][ihit-4])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap55"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						hitmapChip[nhitmap] = hitChip[ichip][ihit+1];
						hitmapChan[nhitmap] = hitChan[ichip][ihit+1];
//    			  cout<<hitChan[ichip][ihit+1]<<endl;
//						cout<<"nhitmap1-1"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
						nhitmap++;
						Nhits=1;
					}
				}
				if(ihit<nHitChip[nchip]-2){
					if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
						Nhits++;
					}
					if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){
						if(Nhits==1){
							int summV = adc_mV(hitAdc[ichip][ihit]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = hitChan[ichip][ihit];
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap111"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						if(Nhits==2){
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap222"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						if(Nhits==3){
	 	    	  	int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap333"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						if(Nhits==4){
	 	  	  		int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3])/Nhits;
//    			  	cout<<hitChan[ichip][ihit]<<endl;
//							cout<<"nhitmap444"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
						}
						if(Nhits==5){
	 	      		int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]);
							hitmapChip[nhitmap] = hitChip[ichip][ihit];
							hitmapChan[nhitmap] = (hitChan[ichip][ihit]+hitChan[ichip][ihit-1]+hitChan[ichip][ihit-2]+hitChan[ichip][ihit-3]+hitChan[ichip][ihit-4])/Nhits;
    		  		cout<<hitChan[ichip][ihit]<<endl;
							cout<<"nhitmap555"<<nhitmap<<" hitmapChip"<<hitmapChip[nhitmap]<<" hitmapChan"<<hitmapChan[nhitmap]<<endl;
							nhitmap++;
		 	  		}
						Nhits=1;
					}
				}
			}
		}
	}
	

c1->cd(4);
TGraph* gr1 = new TGraph(nhitmap,hitmapChip,hitmapChan);
cout<<nhitmap<<endl;
for(int i=0;i<nhitmap;i++){
	cout<<"nhitmap"<<1<<" hitmapChip"<<hitmapChip[i]<<" hitmapChan"<<hitmapChan[i]<<endl;
}
gr1->SetMarkerStyle(2);
gr1->GetXaxis()->SetLimits(0.,27.);
gr1->GetYaxis()->SetLimits(0.,128);
gr1->SetMarkerSize(1.5);
gr1->Draw("AP");
gr1->GetXaxis()->SetRangeUser(0.,27.);
gr1->GetYaxis()->SetRangeUser(0.,128.);
gr1->Draw("AP");
c1->Update();
*/


////////check single hit/////////////////
/*  for(int ichip=0; ichip<nchip; ichip++){
			int Nhits=1;
				
			for(int ihit=0;ihit<nHitChip;ihit++){
//								cout<<adc_mV(hitAdc[ichip][ihit])<<endl;
				if(ihit==nHitChip-2){
					if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
						Nhits++;	
						//c1->cd(4);
						//hnhits->Fill(Nhits);
						if(Nhits==2){
							//int sumadc = hitAdc[ichip][ihit]+hitAdc[ichip][ihit+1];
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]);
							c1->cd(2);
							SumDoublehitAdc2->Fill(summV);
							//c1->cd(2);
							//SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]));
							//c1->cd(3);
							//Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]));
							//c1->cd(6);
							//Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit+1]));
						}

						if(Nhits==3){
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]);
										c1->cd(3);
										SumTriplehitAdc2->Fill(summV);
										//c1->cd(2);
										//SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]));
										//c1->cd(3);
//	 	      					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]));
//	 	      					c1->cd(6);
//	 	      					Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]));
//						}
						if(Nhits==4){
							int summV = adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit+1]);
									c1->cd(4);
									Sum4hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]));
//									c1->cd(2);
//	 	    					SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]));
//	 	    					c1->cd(3);
//	 	    					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]));
//	 	    					c1->cd(6);
//	 	    					Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit+1]));
						}	
						if(Nhits==5){
										c1->cd(5);
										Sum5hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit+1]));
//	 	      					c1->cd(2);
//	 	      					SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit+1]));
//	 	      					c1->cd(3);
//	 	      					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit+1]));
//						}
						if(Nhits==6){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit+1]));
						}
						if(Nhits==7){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit+1]));
						}
						if(Nhits==8){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit+1]));
						}
						if(Nhits==9){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit+1]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit+1]));
						}
          }
					if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){	
//						c1->cd(4);
//						hnhits->Fill(Nhits);
//						hnhits->Fill(1);

							c1->cd(1);
							SinglehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit+1]));
//							c1->cd(2);
//							SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit+1]));
						if(Nhits==1){
							c1->cd(1);
							SinglehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit]));
//							c1->cd(2);
//							SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit]));
						}
						if(Nhits==2){
								c1->cd(2);
									SumDoublehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
//									c1->cd(2);
//	   						SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
//	 	      					c1->cd(3);
//	 	      					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
// 	      					c1->cd(6);
//	 	      					Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
						}

						if(Nhits==3){
										c1->cd(3);
										SumTriplehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
//	 	      					c1->cd(2);
//	 	      					SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
//	 	      					c1->cd(3);
//	 	      					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
//	 	      					c1->cd(6);
//	 	      					Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
						}
						if(Nhits==4){
									c1->cd(4);
									Sum4hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
//	 	    					c1->cd(2);
//	 	    					SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
//	 	    					c1->cd(3);
//	 	    					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
//	 	    					c1->cd(6);
//	 	    					Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
						}	
						if(Nhits==5){
										c1->cd(5);
										Sum5hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]));
//	 	      					c1->cd(2);
//	 	      					SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]));
//	 	      					c1->cd(3);
//	 	      					Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]));
						}
						if(Nhits==6){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5]));
						}
						if(Nhits==7){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6]));
						}
						if(Nhits==8){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7]));
						}
						if(Nhits==9){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit-8]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit-8]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit-8]));
						}
					}
				}
				if(ihit<nHitChip-2){
					if(hitChan[ichip][ihit]==hitChan[ichip][ihit+1]-1){
						Nhits++;
					}
					if(hitChan[ichip][ihit]!=hitChan[ichip][ihit+1]-1){
//						c1->cd(4);
//						hnhits->Fill(Nhits);
						if(Nhits==1){
							c1->cd(1);
							SinglehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit]));
//							c1->cd(2);
//							SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit]));
						}
						if(Nhits==2){
								c1->cd(2);
									SumDoublehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
								c1->cd(2);
								SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
										c1->cd(3);
										Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
										c1->cd(6);
										Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1]));
						}
						if(Nhits==3){
										c1->cd(3);
										SumTriplehitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
										c1->cd(2);
										SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
										c1->cd(3);
										Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
										c1->cd(6);
										Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2]));
						}
						if(Nhits==4){
									c1->cd(4);
									Sum4hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
									c1->cd(6);
									Sum234hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3]));
						}	
						if(Nhits==5){
										c1->cd(5);
										Sum5hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]));
										c1->cd(2);
										SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]));
										c1->cd(3);
										Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4]));
						}
						if(Nhits==6){
									c1->cd(1);
									Sum6hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5]));
						}
						if(Nhits==7){
									c1->cd(1);
									Sum7hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6]));
						}
						if(Nhits==8){
									c1->cd(1);
									Sum8hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7]));
						}
						if(Nhits==9){
									c1->cd(1);
									Sum9hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit-8]));
									c1->cd(2);
									SumhitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit-8]));
									c1->cd(3);
									Sum34hitAdc2->Fill(adc_mV(hitAdc[ichip][ihit])+adc_mV(hitAdc[ichip][ihit-1])+adc_mV(hitAdc[ichip][ihit-2])+adc_mV(hitAdc[ichip][ihit-3])+adc_mV(hitAdc[ichip][ihit-4])+adc_mV(hitAdc[ichip][ihit-5])+adc_mV(hitAdc[ichip][ihit-6])+adc_mV(hitAdc[ichip][ihit-7])+adc_mV(hitAdc[ichip][ihit-8]));
						}
						Nhits=1;
					}
				}
//				cout<<Nhits<<endl;
//				process_nhits(chipArray, hitChan, NChan, NChip, Nhits);
			}*/
//	}
							//    if(ihit==nHitChip-1&&ichip==26){
								//    } 
	};


	void readtree::Loop(const int entry)
	{
	//   In a ROOT session, you can do:
	//      Root > .L readtree.C
	//      Root > readtree t
	//      Root > t.GetEntry(12); // Fill t data members with entry number 12
	//      Root > t.Show();       // Show values of entry 12
	//      Root > t.Show(16);     // Read and show values of entry 16
	//      Root > t.Loop();       // Loop on all entries
	//

	//     This is the loop skeleton where:
	//    jentry is the global entry number in the chain
	//    ientry is the entry number in the current Tree
	//  Note that the argument to GetEntry must be:
	//    jentry for TChain::GetEntry
	//    ientry for TTree::GetEntry and TBranch::GetEntry
	//
	//       To read only selected branches, Insert statements like:
	// METHOD1:
	//    fChain->SetBranchStatus("*",0);  // disable all branches
	//    fChain->SetBranchStatus("branchname",1);  // activate branchname
	// METHOD2: replace line
	//    fChain->GetEntry(jentry);       //read all branches
	//by  b_branchname->GetEntry(ientry); //read only this branch
		if (fChain == 0) return;

		Long64_t nentries = fChain->GetEntriesFast(); 
//		c1->cd(3);
//		m_hitmap->Reset();
		if(entry>=0) m_jentry=entry;
		Long64_t nbytes = 0, nb = 0;
		//int pre_bco = -1;
		//int pre_bcofull = -1;
		int nhit=0;
		int m_chanArray[50000];
		int m_chipArray[50000];
		int m_adcArray[50000];
		int m_eventArray[50000];
		int m_bcoArray[50000];
		int m_bco_fullArray[50000];
		for (; m_jentry<nentries;m_jentry++) {
			Long64_t ientry = LoadTree(m_jentry);
			if (ientry < 0) break;
			nb = fChain->GetEntry(m_jentry);   nbytes += nb;
			// if (Cut(ientry) < 0) continue;
	
			if( (m_prebco!=bco) && (m_prebcofull!=bco_full) ){
				m_prebco     = bco;
				m_prebcofull = bco_full;
	
				///////// process hits/////////////////
				process_hits(nhit, m_chipArray, m_chanArray, m_adcArray, m_eventArray, m_bcoArray, m_bco_fullArray);
				nhit=0;
				return;
			}
//			if(chip_id==17||chip_id==16){
//			if(chip_id==6||chip_id==7||chip_id==10||chip_id==11||chip_id==12||chip_id==13||chip_id==19||chip_id==20||chip_id==23||chip_id==24||chip_id==25||chip_id==26){
//			if(chip_id==1||chip_id==2||chip_id==3||chip_id==4||chip_id==5||chip_id==14||chip_id==15||chip_id==18){
			if(chip_id==8||chip_id==9||chip_id==21||chip_id==22){
					if(ampl==0&&adc>-1&&adc<8&&chip_id>0&&chip_id<27&&chan_id>-1&&chan_id<128){
						m_chipArray[nhit] = chip_id;
						m_chanArray[nhit] = chan_id;
						m_adcArray[nhit]  = adc;
						m_eventArray[nhit]= event;
						m_bcoArray[nhit]  = bco;
						m_bco_fullArray[nhit]=bco_full;
						nhit++;
						c1->cd(6);
						
						Wadc->Fill(m_adcArray[nhit], weight(m_adcArray[nhit]));
					}
			}

/*			else{*/
//				if(chip_id!=17){
/*				if(chip_id>6&&chip_id<8){
					if(adc>-1&&adc<8&&chip_id>0&&chip_id<27&&chan_id>-1&&chan_id<128){
						m_chipArray[nhit] = chip_id;
						m_chanArray[nhit] = chan_id;
						m_adcArray[nhit]  = adc;
						m_eventArray[nhit]= event;
						m_bcoArray[nhit]  = bco;
						m_bco_fullArray[nhit]=bco_full;
						nhit++;
					}
			}

			if(chip_id>19&&chip_id<21){
				if(adc>-1&&adc<8&&chip_id>0&&chip_id<27&&chan_id>-1&&chan_id<128){
					m_chipArray[nhit] = chip_id;
					m_chanArray[nhit] = chan_id;
					m_adcArray[nhit]  = adc;
					m_eventArray[nhit]= event;
					m_bcoArray[nhit]  = bco;
					m_bco_fullArray[nhit]=bco_full;
					nhit++;
					}
				}*/
		/*	}*/


				//cout<<event<<" clk:"<<bco<<" "<<bco_full<<" : "<<chip_id<<" "<<chan_id<<" : "<<module<<" "<<adc<<(adc>0?"hit":" ")<<endl;;
			if(chan_id<0||128<=chan_id) { 
				cout<<"exceed chan_id"<<endl;
				cout<<event<<" clk:"<<bco<<" "<<bco_full<<" : "<<chip_id<<" "<<chan_id<<" : "<<module<<" "<<adc<<(adc>0?"hit":" ")<<endl;;
			}
//			c1->cd(3);
//			m_hitmap->Fill(chip_id-1, chan_id, adc+1);
		}
	};

	void readtree::initHist()
	{
//	c1->cd(3);
//	m_hitmap = new TH2F("hitmap", "hitmap", 26, 0, 26, 128, 0, 128);
//	m_hitmap->SetMaximum(8);
	}

	void readtree::next()
	{
		Loop();
//	c1->cd(3);
//	m_hitmap->Draw("colz");
//  hitmapplot();
	}


