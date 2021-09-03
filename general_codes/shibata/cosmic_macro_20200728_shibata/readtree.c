//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//        Analysis program for INTT silicon sensor
//  This is one of the files to get clustered hits hisotgram.
//  You can draw dac amplitude(mV) histograms, and N hits per event hisrogram on a Canvas.
//
//  Before drawing, you have to get four files, "readtree.c", "run_readtree.C", "plot_readtree2.C", "readtree.h".
//  Please change root file name on "run_readtree.C" and "plot_readtree2.C".
//  To draw histograms, you can write
//	        $ root -l rootfilename
//      	  ROOT> .L readtree.c;
//	        ROOT> .x run_readtree.C;			get "histofilename.root" file
//	        ROOT> .x plot_readtree2.C;		get histograms
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define readtree_cxx
#include "readtree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <TH1.h>


double mV_adc2(double SmV){
	float	mV[9] = {270, 300, 450, 602, 750, 902, 1050, 1202, 1234};
	int Adc;
	for(int ii=0;ii<8;ii++){
		if( mV[ii]<=SmV && SmV<mV[ii+1]){
		  Adc=ii;
		}
	}
	return Adc;//*Adc;
}
double weight(int hitAdc){
	float Dac[9] = {15, 23, 60, 98, 135, 173, 210, 248, 256};
	float weight[8];
	float tmpw = Dac[1]-Dac[0];
	for(int i=0; i<8; i++){
		weight[i]	= (Dac[i+1]-Dac[i])/tmpw;
	}
	float w = 1./weight[hitAdc];
	return w;
}
double mV_weight(int hitAdc){
	float	mV[9] = {270, 302, 450, 602, 750, 902, 1050, 1202, 1234};
	float weight[8];
	float tmpw = mV[1]-mV[0];
	for(int i=0; i<8; i++){
		weight[i]	= (mV[i+1]-mV[i])/tmpw;
	}
	float w = 1./weight[hitAdc];
	return w;
}
double mV_weight2(int hitmV){
	float	mV[9] = {270, 302, 450, 602, 750, 902, 1050, 1202, 1234};
	float weight[8];
	float w;
	float tmpw = mV[1]-mV[0];
	for(int i=0; i<8; i++){
		weight[i]	= (mV[i+1]-mV[i])/tmpw;
	}
	for(int i=0;i<8;i++){
		if(hitmV>=mV[i]&&hitmV<mV[i+1]){
			int tmp = i;
			w = 1./weight[i];
		}
	}
	return w;
}
double adc_mV(int hitAdc){
	double Dac[9] = {15., 23., 60., 98., 135., 173., 210., 248., 256.};
	double  mV = ((Dac[hitAdc+1]*4.+210.)+(Dac[hitAdc]*4.+210.))/2.-2.;
	return mV;
}
void readtree::process_hits(int nhit, int* chipArray, int* chanArray, int* adcArray, int* eventArray, int* bcoArray, int* bco_fullArray, double* summV_Ret, int* Nhits_Ret, int* chan_Ret, int* chip_Ret, int nclsnChip_Ret, int(*adc_Ret)[10]){
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
    nHitChip[ichip]++;
   	hitChip[ichip][ii] = chipArray[ihit];
   	hitChan[ichip][ii] = chanArray[ihit];
   	hitAdc [ichip][ii] = adcArray[ihit];
   	hitEvent[ichip][ii]= eventArray[ihit];
   	hitBco[ichip][ii]= bcoArray[ihit];
   	hitBco_full[ichip][ii]= bco_fullArray[ihit];
	}
  double nhit_1[9], nhit_2[9], nhit_3[9];
  for(int ii=0; ii<9;ii++){
  	nhit_1[ii]=0.;
  	nhit_2[ii]=0.;
  	nhit_3[ii]=0.;
  }
  double nhit_adc1[9], nhit_adc2[9], nhit_adc3[9];
  for(int ii=0; ii<9;ii++){
  	nhit_adc1[ii]=0.;
  	nhit_adc2[ii]=0.;
  	nhit_adc3[ii]=0.;
  }
///////// sort channel order//////////////
  for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>0){
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
					}
				}
			}
		}
	}
	for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>1){
			int tmpchip[5]={-1,-1,-1,-1,-1};
			int tmpchan[5]={-1,-1,-1,-1,-1};
			int noise =1;
			int tmptmp=0;
			for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
				if(hitChip[ichip][ihit]==hitChip[ichip][ihit+1]&&hitChan[ichip][ihit+1]==hitChan[ichip][ihit]){
					noise++;
					if(tmpchan[tmptmp]!=hitChan[ichip][ihit]&&tmpchip[tmptmp]!=hitChip[ichip][ihit]){
						tmptmp++;
					}
					tmpchan[tmptmp]=hitChan[ichip][ihit];
					tmpchip[tmptmp]=hitChip[ichip][ihit];
				}
			}
			for(int ttt=0;ttt<tmptmp+1;ttt++){
				for(int ihit=0; ihit<nHitChip[ichip]+1; ihit++){
					if(hitChip[ichip][ihit]==tmpchip[ttt]&&hitChan[ichip][ihit]==tmpchan[ttt]){
					}
				}
			}
		}
	}
//+++++++++++check strip number+++++++++++++++
	int nhitmap=0;
	double summV;
	int sumChan;
	int ncls = 0;
	for(int ichip=0; ichip<27; ichip++){
		if(nHitChip[ichip]>0){
			int start = 0;
			int Nhits =1;
			int end;
			for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
				if(Nhits==nHitChip[ichip]){
					end = ihit;
					summV=0;
					sumChan =0;
					for(int i=start; i<end+1; i++){
						summV += adc_mV(hitAdc[ichip][i]);
						sumChan += hitChan[ichip][i];
					}
					if(Nhits==1){
						for(int ii=0; ii<8;ii++){
							if(mV_adc2(summV)==ii)nhit_1[ii]++;
						}				
					}
					else if(Nhits==2){
						for(int ii=0; ii<8;ii++){
							if(mV_adc2(summV)==ii)nhit_2[ii]++;
						}				
					}
					else if(Nhits==3){
						for(int ii=0; ii<8;ii++){
							if(mV_adc2(summV)==ii)nhit_3[ii]++;
						}				
					}
					start = end+1;
					ncls++;
					Nhits = 1;
				}
				else if(hitChan[ichip][ihit+1]-hitChan[ichip][ihit]==1){
					Nhits++;
				}
				else{
					end = ihit;
					summV=0;
					sumChan =0;
					for(int i=start; i<end+1; i++){
						summV += adc_mV(hitAdc[ichip][i]);
						sumChan += hitChan[ichip][i];
					}
					if(Nhits==1){
						for(int ii=0; ii<8;ii++){
							if(mV_adc2(summV)==ii)nhit_1[ii]++;
						}				
					}
					else if(Nhits==2){
						for(int ii=0; ii<8;ii++){
							if(mV_adc2(summV)==ii)nhit_2[ii]++;
						}				
					}
					else if(Nhits==3){
						for(int ii=0; ii<8;ii++){
							if(mV_adc2(summV)==ii)nhit_3[ii]++;
						}				
					}
					start = end+1;
					ncls++;
					Nhits = 1;
				}
			}
		}	
	}
//+++++++++++++++++++++++++++++++++++
	if(ncls>0){
	  for(int ichip=0; ichip<27; ichip++){
			if(nHitChip[ichip]>0){
				for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
					multiclschip->Fill(hitChan[ichip][ihit],hitChip[ichip][ihit]);
				}
			}
		}
	}
//++++++++++Fill per strip number+++++++++++++++
	if(ncls>0){																			//cluster number for analysis
		nhitmap=0;
		int Ncls = 0;																		//number of strips (same as ncls)
		for(int ichip=0; ichip<27; ichip++){
			int NHITCHIP=0;		//number of single hits per chip per event
			int single_ihit[20];	
			int single_0_ihit[20];
			if(nHitChip[ichip]>0){
				int start = 0;
				int Nhits =1;
				int end;
				for(int ihit=0; ihit<nHitChip[ichip]; ihit++){
					if(Nhits==nHitChip[ichip]){
						end = ihit;
						summV=0;
						sumChan =0;
						for(int i=start; i<end+1; i++){
							summV += adc_mV(hitAdc[ichip][i]);		//clustered hit's energy
							sumChan += hitChan[ichip][i];					//clustered hit's sum channel number
						}
						summV_Ret[Ncls]	=	summV;
						Nhits_Ret[Ncls]	=	Nhits;
						chip_Ret[Ncls] = hitChip[ichip][ihit];	//RET  clustered hit's channel number
						chan_Ret[Ncls] = sumChan/Nhits;					//RET  clustered hit's avarage channel number
						hnhits->Fill(Nhits);										
//+++++++++ Fill single hit ++++++++++++
						if(Nhits==1){																	
							adc_Ret[Ncls][0]	=	hitAdc[ichip][start];
							single_ihit[NHITCHIP]=start;
							NHITCHIP++;
							if(hitAdc[ichip][start]!=-1){					//ADC value for analysis
								SinglehitAdc2->Fill(summV,mV_weight2(summV));
								SingleAdc->Fill(adc_mV(hitAdc[ichip][start]), mV_weight2(adc_mV(hitAdc[ichip][start])));
								if(adc_mV(hitAdc[ichip][start])>5000.&&adc_mV(hitAdc[ichip][start])<0.)cout<<"Single mV="<<adc_mV(hitAdc[ichip][start])<<endl;
								for(int ii=0; ii<8;ii++){
									if(mV_adc2(summV)==ii)nhit_1[ii]++;
								}				
							}
							else if(hitAdc[ichip][start]==0)single_0_ihit[NHITCHIP]=start;
						}
//+++++++++ Fill double hits ++++++++++++
						else if(Nhits==2){
							adc_Ret[Ncls][0]	=	hitAdc[ichip][start];
							adc_Ret[Ncls][1]	=	hitAdc[ichip][start+1];
							SumDoublehitAdc2->Fill(summV,mV_weight2(summV));
							DAdc->Fill(adc_mV(hitAdc[ichip][start]),mV_weight2(adc_mV(hitAdc[ichip][start])));
							DAdc->Fill(adc_mV(hitAdc[ichip][start+1]), mV_weight2(adc_mV(hitAdc[ichip][start+1])));
							if(adc_mV(hitAdc[ichip][start])>5000.&&adc_mV(hitAdc[ichip][start])<0.)	cout<<"D mV="<<adc_mV(hitAdc[ichip][start])<<endl;
								if(adc_mV(hitAdc[ichip][start+1])>5000.&&adc_mV(hitAdc[ichip][start+1])<0.)cout<<"D2 mV="<<adc_mV(hitAdc[ichip][start+1])<<endl;
							Adc_2hit->Fill(hitAdc[ichip][start],hitAdc[ichip][start+1]);
							chan_2hit->Fill(hitChan[ichip][start],hitChan[ichip][start+1]);
							if(hitAdc[ichip][start]==0&&hitAdc[ichip][start+1]==0){
							}
	
							for(int ii=0; ii<8;ii++){
								if(mV_adc2(summV)==ii)nhit_2[ii]++;
							}				
						}
//+++++++++ Fill triple hits ++++++++++++
						else if(Nhits==3){
							SumTriplehitAdc2->Fill(summV,mV_weight2(summV));
							TAdc->Fill(adc_mV(hitAdc[ichip][start]), mV_weight2(adc_mV(hitAdc[ichip][start])));
							TAdc->Fill(adc_mV(hitAdc[ichip][start+1]), mV_weight2(adc_mV(hitAdc[ichip][start+1])));
							TAdc->Fill(adc_mV(hitAdc[ichip][start+2]), mV_weight2(adc_mV(hitAdc[ichip][start+2])));
								if(adc_mV(hitAdc[ichip][start])>5000.&&adc_mV(hitAdc[ichip][start])<0.)cout<<"T mV="<<adc_mV(hitAdc[ichip][start])<<endl;
								if(adc_mV(hitAdc[ichip][start+1])>5000.&&adc_mV(hitAdc[ichip][start+1])<0.)cout<<"T2 mV="<<adc_mV(hitAdc[ichip][start+1])<<endl;
								if(adc_mV(hitAdc[ichip][start+2])>5000.&&adc_mV(hitAdc[ichip][start+2])<0.)cout<<"T2 mV="<<adc_mV(hitAdc[ichip][start+1])<<endl;
							for(int ii=0; ii<8;ii++){
								if(mV_adc2(summV)==ii)nhit_3[ii]++;
							}				
						}
						start = end+1;
						Ncls++;
						Nhits = 1;			
					}
					else if(hitChan[ichip][ihit+1]-hitChan[ichip][ihit]==1){
						Nhits++;
					}
					else{
						end = ihit;
						summV=0;
						sumChan =0;
						for(int i=start; i<end+1; i++){
							summV += adc_mV(hitAdc[ichip][i]);
							sumChan += hitChan[ichip][i];
						}
						summV_Ret[Ncls]	=	summV;
						Nhits_Ret[Ncls]	=	Nhits;
						chip_Ret[Ncls] = hitChip[ichip][ihit];
						chan_Ret[Ncls] = sumChan/Nhits;
						hnhits->Fill(Nhits);
//+++++++++ Fill single hit ++++++++++++
						if(Nhits==1){
							adc_Ret[Ncls][0]	=	hitAdc[ichip][start];
							single_ihit[NHITCHIP]=start;
							NHITCHIP++;
							if(hitAdc[ichip][start]!=-1){
								SinglehitAdc2->Fill(summV,mV_weight2(summV));
								SingleAdc->Fill(adc_mV(hitAdc[ichip][start]), mV_weight2(adc_mV(hitAdc[ichip][start])));
								if(adc_mV(hitAdc[ichip][start])>5000.&&adc_mV(hitAdc[ichip][start])<0.)cout<<"single mV="<<adc_mV(hitAdc[ichip][start])<<endl;
								for(int ii=0; ii<8;ii++){
									if(mV_adc2(summV)==ii)nhit_1[ii]++;
								}				
							}
							else if(hitAdc[ichip][start]==0)single_0_ihit[NHITCHIP]=start;
						}
//+++++++++ Fill double hits ++++++++++++
						else if(Nhits==2){
							adc_Ret[Ncls][0]	=	hitAdc[ichip][start];
							adc_Ret[Ncls][1]	=	hitAdc[ichip][start+1];
					 		SumDoublehitAdc2->Fill(summV,mV_weight2(summV));
							DAdc->Fill(adc_mV(hitAdc[ichip][start]), mV_weight2(adc_mV(hitAdc[ichip][start])));
							DAdc->Fill(adc_mV(hitAdc[ichip][start+1]), mV_weight2(adc_mV(hitAdc[ichip][start+1])));
								if(adc_mV(hitAdc[ichip][start])>5000.&&adc_mV(hitAdc[ichip][start])<0.)cout<<"D mV="<<adc_mV(hitAdc[ichip][start])<<endl;
								if(adc_mV(hitAdc[ichip][start+1])>5000.&&adc_mV(hitAdc[ichip][start+1])<0.)cout<<"D2 mV="<<adc_mV(hitAdc[ichip][start+1])<<endl;
							Adc_2hit->Fill(hitAdc[ichip][start],hitAdc[ichip][start+1]);
							chan_2hit->Fill(hitChan[ichip][start],hitChan[ichip][start+1]);
							for(int ii=0; ii<8;ii++){
								if(mV_adc2(summV)==ii)nhit_2[ii]++;
							}				
						}
//+++++++++ Fill triple hits ++++++++++++
						else if(Nhits==3){
							SumTriplehitAdc2->Fill(summV,mV_weight2(summV));
							TAdc->Fill(adc_mV(hitAdc[ichip][start])	 , mV_weight2(adc_mV(hitAdc[ichip][start])));
							TAdc->Fill(adc_mV(hitAdc[ichip][start+1]), mV_weight2(adc_mV(hitAdc[ichip][start+1])));
							TAdc->Fill(adc_mV(hitAdc[ichip][start+2]), mV_weight2(adc_mV(hitAdc[ichip][start+2])));
								if(adc_mV(hitAdc[ichip][start])>5000.    &&adc_mV(hitAdc[ichip][start])<0.)cout<<"T mV="<<adc_mV(hitAdc[ichip][start])<<endl;
								if(adc_mV(hitAdc[ichip][start+1])>5000.&&adc_mV(hitAdc[ichip][start+1])<0.)cout<<"T2 mV="<<adc_mV(hitAdc[ichip][start+1])<<endl;
								if(adc_mV(hitAdc[ichip][start+2])>5000.&&adc_mV(hitAdc[ichip][start+2])<0.)cout<<"T2 mV="<<adc_mV(hitAdc[ichip][start+1])<<endl;
							for(int ii=0; ii<8;ii++){
								if(mV_adc2(summV)==ii)nhit_3[ii]++;
							}				
						}
						start = end+1;
						Ncls++;
						Nhits = 1;
					}
				}					
			}
//+++++++++++++++single and adc=0 hit++++++++++++++++++++++++++++++++++++++
			if(NHITCHIP>0){
				for(int j=0;j<NHITCHIP;j++){
					if(hitAdc[ichip][single_0_ihit[j]]==0){
						adc_0hit->Fill(hitChan[ichip][single_0_ihit[j]],hitChip[ichip][single_0_ihit[j]]);
					}
				}
			}

//+++++++++++++++separate single hits++++++++++++++++++++++++++++++++++++++++++++++++++++++
			if(NHITCHIP==2){
				for(int j=0;j<NHITCHIP;j++){
					NHITChipmV->Fill(adc_mV(hitAdc[ichip][single_ihit[j]]),mV_weight2(adc_mV(hitAdc[ichip][single_ihit[j]])));
				}
				NHITChip->Fill(hitAdc[ichip][single_ihit[0]],hitAdc[ichip][single_ihit[1]]);
				chan_1hit->Fill(hitChan[ichip][single_ihit[0]],hitChan[ichip][single_ihit[1]]);
				chan_1hit2->Fill(hitChan[ichip][single_ihit[1]]-hitChan[ichip][single_ihit[0]],hitChip[ichip][single_ihit[0]]);
			}
		}
				nclsnChip->Fill(Ncls);
				nclsnChip_Ret	=	Ncls;
	}
};
void readtree::Loop(const int entry)
{
		if (fChain == 0) return;
		Long64_t nentries = fChain->GetEntriesFast(); 
		if(entry>=0) m_jentry=entry;
		Long64_t nbytes = 0, nb = 0;
		int nhit=0;
		int prenhit=0;
		int noise=0;
		int m_nbco[50000];
		int m_nbco_full[50000];	
		int m_chanArray[50000];
		int m_chipArray[50000];
		int m_adcArray[50000];
		int m_eventArray[50000];
		int m_bcoArray[50000];
		int m_bco_fullArray[50000];
		int m_prenbco_full[50000];	
		int m_prechanArray[50000];
		int m_prechipArray[50000];
		int m_preadcArray[50000];
		int m_preeventArray[50000];
		int m_prebcoArray[50000];
		int m_prebco_fullArray[50000];
		int sa_bco;
		int sa_bcofull;
		int noisehit=0;
		int cosmichit=0;
		int prenoisehit=0;
		int precosmichit=0;
		int processhit=0;
		int nhit2=0;
		int prenhit2=0;
		int noise2=0;
		int m_nbco2[50000];
		int m_nbco_full2[50000];	
		int m_chanArray2[50000];
		int m_chipArray2[50000];
		int m_adcArray2[50000];
		int m_eventArray2[50000];
		int m_bcoArray2[50000];
		int m_bco_fullArray2[50000];
		int m_prenbco_full2[50000];	
		int m_prechanArray2[50000];
		int m_prechipArray2[50000];
		int m_preadcArray2[50000];
		int m_preeventArray2[50000];
		int m_prebcoArray2[50000];
		int m_prebco_fullArray2[50000];
		int sa_bco2;
		int sa_bcofull2;
		int noisehit2=0;
		int cosmichit2=0;
		int prenoisehit2=0;
		int precosmichit2=0;
		int processhit2=0;
		double summV_Ret[10];
		int		 Nhits_Ret[10];
		double summV_Ret8[10];
		double summV_Ret6[10];
		int		 Nhits_Ret6[10];
		int		 Nhits_Ret8[10];
		int		 chan_Ret[10]	;
		int		 chan_Ret6[10];
		int		 chan_Ret8[10];
		int		 chip_Ret[10]	;
		int		 chip_Ret6[10];
		int		 chip_Ret8[10];
		int 	 ncls_Ret;
		int 	 ncls_Ret6;
		int 	 ncls_Ret8;
		int		 adc_Ret[10][10]	;
		int		 adc_Ret6[10][10];
		int		 adc_Ret8[10][10];
		for(int i=0;i<10;i++){
			summV_Ret[i]	=	-1;
			Nhits_Ret[i]	=	-1;
			summV_Ret8[i]	=	-1;
			summV_Ret6[i]	=	-1;
			Nhits_Ret6[i]	=	-1;
			Nhits_Ret8[i]	=	-1;
			chan_Ret[i]		=	-1;
			chan_Ret6[i]	=	-1;
			chan_Ret8[i]	=	-1;
			chip_Ret[i]		=	-1;
			chip_Ret6[i]	=	-1;
			chip_Ret8[i]	=	-1;
			for(int ii=0;ii<10;ii++){
				adc_Ret[ii][i]	=	-1;
				adc_Ret6[ii][i]	=	-1;
				adc_Ret8[ii][i]	=	-1;
			}
		}
		int nhit_Ret8=0;
		int nhit_Ret6=0;
		int	nstrip8=0;
		int	nstrip6=0;
		ncls_Ret6	=	-1;
		ncls_Ret8	=	-1;
	  ncls_Ret	=	-1;
		for (; m_jentry<nentries+1;m_jentry++) {
			Long64_t ientry = LoadTree(m_jentry);
			if (ientry < 0) break;
			nb = fChain->GetEntry(m_jentry);   nbytes += nb;
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++	
			if( (m_prebco!=bco) && (m_prebcofull!=bco_full) ){	
				sa_bco	=	m_prebco-bco;
				sa_bcofull	=	m_prebcofull-bco_full;
				bco_interval->Fill(sa_bco);
				bcofull_interval->Fill(sa_bcofull);
				sa_correlation->Fill(sa_bco,sa_bcofull);
			  if(sa_bcofull!=-1){
					for(int i=0;i<nhit;i++){
								adc_bcofull->Fill(m_adcArray[i],weight(m_adcArray[i]));
					}
				}
			  m_prebco     = bco;
				m_prebcofull = bco_full;
//++++++++++++check same channel and chip data++++++++++++++++++++++++
				for(int i=0;i<nhit;i++){
					for(int ii=0;ii<prenhit;ii++){
						if(m_prechanArray[ii]==m_chanArray[i]&&m_prechipArray[ii]==m_chipArray[i]){
							noise++;
							m_noiseAdc->Fill(m_preadcArray[ii],weight(m_preadcArray[ii]));	
							m_noiseAdc->Fill(m_adcArray[i],weight(m_adcArray[i]));	  
							noisehit=1;
						}
						else {
							cosmichit=1;
						}
					}
				}
//++++++++++++check same channel and chip data++++++++++++++++++++++++
				for(int i=0;i<nhit2;i++){
					for(int ii=0;ii<prenhit2;ii++){
						if(m_prechanArray2[ii]==m_chanArray2[i]&&m_prechipArray2[ii]==m_chipArray2[i]){
							noise2++;
							m_noiseAdc->Fill(m_preadcArray2[ii],weight(m_preadcArray2[ii]));	
							m_noiseAdc->Fill(m_adcArray2[i],weight(m_adcArray2[i]));	  
							noisehit2=1;
						}
						else{
							cosmichit2=1;
						}
					}
				}
				int HIT=0;
				processhit=0;
				if(noisehit==0&&prenoisehit==0)processhit=1;
				processhit2=0;
				if(noisehit2==0&&prenoisehit2==0)processhit2=1;
				//!!!!!!!!!!!!!!!!!!for two modules!!!!!!!!!!!!!!!!!!!!
/*				if(prenhit>0&&prenhit2>0){
					if(processhit==1){
						if(processhit2==1){
*/				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//!!!!!!!!!!!!!!!!!!for one modules(8)!!!!!!!!!!!!!!!!!!!!
				if(prenhit>0){
					if(cosmichit==1&&precosmichit==1&&noisehit==0&&prenoisehit==0){
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							for(int i=0;i<10;i++){
								summV_Ret[i]	=	-1;
								summV_Ret6[i]	=	-1;
								summV_Ret8[i]	=	-1;
								Nhits_Ret[i]	=	-1;
								Nhits_Ret6[i]	=	-1;
								Nhits_Ret8[i]	=	-1;
								chan_Ret[i]		=	-1;
								chan_Ret6[i]	=	-1;
								chan_Ret8[i]	=	-1;
								chip_Ret[i]		=	-1;
								chip_Ret6[i]	=	-1;
								chip_Ret8[i]	=	-1;
								for(int ii=0;ii<10;ii++){
									adc_Ret[ii][i]	=	-1;
									adc_Ret6[ii][i]	=	-1;
									adc_Ret8[ii][i]	=	-1;
								}
							}
							nhit_Ret8=0;
							nhit_Ret6=0;
							nstrip8=0;
							nstrip6=0;
							ncls_Ret6	=	-1;
							ncls_Ret8	=	-1;
	  					ncls_Ret	=	-1;
							for(int i=0;i<prenhit;i++){
								Wadc->Fill(m_preadcArray[i], weight(m_preadcArray[i]));
								//--singlemV->Fill(adc_mV(m_preadcArray[i]),mV_weight2(adc_mV(m_preadcArray[i])));
							}
							for(int i=0;i<prenhit2;i++){
								Wadc->Fill(m_preadcArray2[i], weight(m_preadcArray2[i]));
								//--singlemV->Fill(adc_mV(m_preadcArray2[i]),mV_weight2(adc_mV(m_preadcArray2[i])));
							}
// ++++++++++ process hits+++++++++++++++++++++
							process_hits(prenhit, m_prechipArray, m_prechanArray, m_preadcArray, m_preeventArray, m_prebcoArray, m_prebco_fullArray, summV_Ret, Nhits_Ret, chan_Ret, chip_Ret, ncls_Ret, adc_Ret);
							if(ncls_Ret>0){
								NCLS8->Fill(ncls_Ret);
							}
							ncls_Ret8	=	ncls_Ret;
							for(int I=0;I<10;I++){
								if(summV_Ret[I]!=-1){
									summV_Ret8[nhit_Ret8]	=	summV_Ret[I];
									Nhits_Ret8[nhit_Ret8]	=	Nhits_Ret[I];
									chan_Ret8[nhit_Ret8]	=	chan_Ret[I];
									chip_Ret8[nhit_Ret8]	=	chip_Ret[I];
									for(int II=0;II<10;II++){
										adc_Ret8[nhit_Ret8][nstrip8]	=	adc_Ret[I][II];
										nstrip8++;
									}
									nhit_Ret8++;
								}
							}
							for(int i=0;i<10;i++){
								summV_Ret[i]	=	-1;
								Nhits_Ret[i]	=	-1;
								chan_Ret[i]		=	-1;
								chip_Ret[i]		=	-1;
								for(int ii=0;ii<10;ii++){
									adc_Ret[ii][i]	=	-1;
								}
							}
	  					ncls_Ret	=	 0;
							process_hits(prenhit2, m_prechipArray2, m_prechanArray2, m_preadcArray2, m_preeventArray2, m_prebcoArray2, m_prebco_fullArray2, summV_Ret, Nhits_Ret, chan_Ret, chip_Ret, ncls_Ret, adc_Ret);
// ++++++++++++++analysis per module+++++++++++++
							if(ncls_Ret>0){
								NCLS6->Fill(ncls_Ret);
							}
							ncls_Ret6	=	ncls_Ret;
							for(int I=0;I<10;I++){
								if(summV_Ret[I]!=-1){
									summV_Ret6[nhit_Ret6]	=	summV_Ret[I];
									Nhits_Ret6[nhit_Ret6]	=	Nhits_Ret[I];
									chan_Ret6[nhit_Ret6]	=	chan_Ret[I];
									chip_Ret6[nhit_Ret6]	=	chip_Ret[I];
									for(int II=0;II<10;II++){
										adc_Ret6[nhit_Ret6][nstrip6]	=	adc_Ret[I][II];
										nstrip6++;
									}
									nhit_Ret6++;
								}
							}
							for(int i=0;i<10;i++){
								summV_Ret[i]	=	-1;
								Nhits_Ret[i]	=	-1;
								chan_Ret[i]		=	-1;
								chip_Ret[i]		=	-1;
								for(int ii=0;ii<10;ii++){
									adc_Ret[ii][i]	=	-1;
								}
							}					
							int chipselection=0;
							int chanselection=0;
	  					ncls_Ret	=	 0;
							if(nhit_Ret8==1&&nhit_Ret6==1){
								NSTRIP6->Fill(Nhits_Ret6[0]);
								NSTRIP8->Fill(Nhits_Ret8[0]);
								CHANDIFF->Fill(chan_Ret6[0], chan_Ret8[0]);
								CHIPDIFF->Fill(chip_Ret6[0], chip_Ret8[0]);
								if(chip_Ret6[0]>2&&chip_Ret6[0]<14&&chip_Ret8[0]<12){
									CHIPDIFFSAh->Fill(chip_Ret6[0]-chip_Ret8[0]);
									CHANDIFFS->Fill(chan_Ret6[0], chan_Ret8[0]);
									if(chan_Ret6[0]>63&&chan_Ret8[0]>63){
										CHANDIFFSAh->Fill(chan_Ret6[0]-chan_Ret8[0]);
										chanselection=1;
									}
									chipselection=1;
								}
								else if(chip_Ret6[0]>15&&chip_Ret8[0]>13&&chip_Ret8[0]<25){
									CHIPDIFFSAh->Fill(chip_Ret6[0]-chip_Ret8[0]);
									CHANDIFFS->Fill(chan_Ret6[0], chan_Ret8[0]);
									if(chan_Ret6[0]>63&&chan_Ret8[0]>63){
										CHANDIFFSAh->Fill(chan_Ret6[0]-chan_Ret8[0]);
										chanselection=1;
									}
									chipselection=1;
								}
								//event selection
								if(chipselection==1&&chanselection==1&&(chip_Ret6[0]-chip_Ret8[0])<2.673+1.017&&(chip_Ret6[0]-chip_Ret8[0])>2.673-1.017&&(chan_Ret6[0]-chan_Ret8[0])<27&&(chan_Ret6[0]-chan_Ret8[0])>-19){
									for(int I=0;I<nhit_Ret6;I++){
											for(int II=0;II<nstrip6;II++){
												if(adc_Ret6[II][I]>=0)originalenergyloss->Fill(adc_mV(adc_Ret6[II][I]),mV_weight2(adc_mV(adc_Ret6[II][I])));
											}
									}
									for(int I=0;I<nhit_Ret8;I++){
											for(int II=0;II<nstrip8;II++){
												if(adc_Ret8[II][I]>=0)originalenergyloss->Fill(adc_mV(adc_Ret8[II][I]),mV_weight2(adc_mV(adc_Ret8[II][I])));
											}
									}
									double gain	=	190.;
									double RecmV1	=	0.;
									double RecmV2	=	0.;
									if(Nhits_Ret8[0]==1){
										RecmV1	=	adc_mV(adc_Ret8[0][0])-gain;//+-75mV
										RECELOSS1->Fill(RecmV1);//+-75mV
									}
									if(Nhits_Ret6[0]==1){
										RecmV1	=	adc_mV(adc_Ret6[0][0])-gain;//+-75mV
										RECELOSS1->Fill(RecmV1);//+-75mV
									}
									if(Nhits_Ret8[0]==2){
										RecmV2	+=	adc_mV(adc_Ret6[0][0])-gain;//+-75mV
										RecmV2	+=	adc_mV(adc_Ret6[0][1])-gain;//+-75mV
										RECELOSS2->Fill(RecmV2);//+-75mV
									}
									if(Nhits_Ret6[0]==2){
										RecmV2	+=	adc_mV(adc_Ret6[0][0])-gain;//+-75mV
										RecmV2	+=	adc_mV(adc_Ret6[0][1])-gain;//+-75mV
										RECELOSS2->Fill(RecmV2);//+-75mV
									}
									if(Nhits_Ret8[0]==1)ELOSSCLS1__S->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==1)ELOSSCLS1__S->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==2){
										ELOSSCLS2__S->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
										ADCDIFF->Fill(adc_mV(adc_Ret8[0][0]),adc_mV(adc_Ret8[0][1]));
									}
									if(Nhits_Ret6[0]==2){
										ELOSSCLS2__S->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
										ADCDIFF->Fill(adc_mV(adc_Ret6[0][0]),adc_mV(adc_Ret6[0][1]));
									}
									if(Nhits_Ret8[0]==1&&Nhits_Ret6[0]==2){
										ELOSSTRANS	->Fill(summV_Ret8[0], summV_Ret6[0]);
										ELOSSCLS1_S	->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
										ELOSSCLS2_S	->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									}
									if(Nhits_Ret6[0]==1&&Nhits_Ret8[0]==2){
										ELOSSTRANS	->Fill(summV_Ret6[0], summV_Ret8[0]);
										ELOSSCLS1_S	->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
										ELOSSCLS2_S	->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									}
								}
								if(chip_Ret6[0]<15&&chip_Ret8[0]<14){
									CHANDIFFSA1->Fill(TMath::Abs(chan_Ret6[0]-chan_Ret8[0]));
									ELOSSDIFF1->Fill(summV_Ret6[0], summV_Ret8[0]);
									if(Nhits_Ret8[0]==1)ELOSSCLS1_8_1->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==1)ELOSSCLS1_6_1->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==2)ELOSSCLS2_8_1->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==2)ELOSSCLS2_6_1->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==1&&Nhits_Ret6[0]==2){
										ELOSSTRAN1->Fill(summV_Ret8[0], summV_Ret6[0]);
										ELOSSCLS1_1->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
										ELOSSCLS2_1->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									}
									if(Nhits_Ret6[0]==1&&Nhits_Ret8[0]==2){
										ELOSSTRAN1->Fill(summV_Ret6[0], summV_Ret8[0]);
										ELOSSCLS1_1->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
										ELOSSCLS2_1->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									}
								}
								else if(chip_Ret6[0]<15){
									CHANDIFFSA2->Fill(TMath::Abs(chan_Ret6[0]-chan_Ret8[0]));
									ELOSSDIFF2->Fill(summV_Ret6[0], summV_Ret8[0]);
									if(Nhits_Ret8[0]==1)ELOSSCLS1_8_2->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==1)ELOSSCLS1_6_2->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==2)ELOSSCLS2_8_2->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==2)ELOSSCLS2_6_2->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==1&&Nhits_Ret6[0]==2){
										ELOSSTRAN2->Fill(summV_Ret8[0], summV_Ret6[0]);
										ELOSSCLS1_2->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
										ELOSSCLS2_2->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									}
									if(Nhits_Ret6[0]==1&&Nhits_Ret8[0]==2){
										ELOSSTRAN2->Fill(summV_Ret6[0], summV_Ret8[0]);
										ELOSSCLS1_2->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
										ELOSSCLS2_2->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									}
								}
								else if(chip_Ret6[0]>14&&chip_Ret8[0]>13){
									CHANDIFFSA3->Fill(TMath::Abs(chan_Ret6[0]-chan_Ret8[0]));
									ELOSSDIFF3->Fill(summV_Ret6[0], summV_Ret8[0]);
									if(Nhits_Ret8[0]==1)ELOSSCLS1_8_3->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==1)ELOSSCLS1_6_3->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==2)ELOSSCLS2_8_3->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==2)ELOSSCLS2_6_3->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==1&&Nhits_Ret6[0]==2){
										ELOSSTRAN3->Fill(summV_Ret8[0], summV_Ret6[0]);
										ELOSSCLS1_3->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
										ELOSSCLS2_3->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									}
									if(Nhits_Ret6[0]==1&&Nhits_Ret8[0]==2){
										ELOSSTRAN3->Fill(summV_Ret6[0], summV_Ret8[0]);
										ELOSSCLS1_3->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
										ELOSSCLS2_3->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									}
								}
								else if(chip_Ret6[0]>14){
									CHANDIFFSA4->Fill(TMath::Abs(chan_Ret6[0]-chan_Ret8[0]));
									ELOSSDIFF4->Fill(summV_Ret6[0], summV_Ret8[0]);
									if(Nhits_Ret8[0]==1)ELOSSCLS1_8_4->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==1)ELOSSCLS1_6_4->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==2)ELOSSCLS2_8_4->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									if(Nhits_Ret6[0]==2)ELOSSCLS2_6_4->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									if(Nhits_Ret8[0]==1&&Nhits_Ret6[0]==2){
										ELOSSTRAN4->Fill(summV_Ret8[0], summV_Ret6[0]);
										ELOSSCLS1_4->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
										ELOSSCLS2_4->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
									}
									if(Nhits_Ret6[0]==1&&Nhits_Ret8[0]==2){
										ELOSSTRAN4->Fill(summV_Ret6[0], summV_Ret8[0]);
										ELOSSCLS1_4->Fill(summV_Ret6[0], mV_weight2(summV_Ret6[0]));
										ELOSSCLS2_4->Fill(summV_Ret8[0], mV_weight2(summV_Ret8[0]));
									}
								}
								ELOSSDIFF->Fill(summV_Ret6[0], summV_Ret8[0]);
								ELOSSDIFFSA->Fill(TMath::Abs(summV_Ret6[0]-summV_Ret8[0]));
								
								if(Nhits_Ret8[0]==1&&Nhits_Ret6[0]==2){
									ELOSSTRAN->Fill(summV_Ret8[0], summV_Ret6[0]);
									ELOSSTRANSA->Fill(TMath::Abs(summV_Ret6[0]-summV_Ret8[0]));
								}
								if(Nhits_Ret6[0]==1&&Nhits_Ret8[0]==2){
									ELOSSTRAN->Fill(summV_Ret6[0], summV_Ret8[0]);
									ELOSSTRANSA->Fill(TMath::Abs(summV_Ret6[0]-summV_Ret8[0]));
								}
							}
						//} //!!!!!!!!!!!!!!!!!for two modules!!!!!!!!!!!!!!!!!!!!!!!!!
					}
				}
//++++++++++++++copy previous data++++++++++++++++++++++
				precosmichit	=	cosmichit;
				precosmichit2	=	cosmichit2;
				prenoisehit	=	noisehit;
				prenoisehit2	=	noisehit2;
				cosmichit=0;
				cosmichit2=0;
				noisehit=0;
				noisehit2=0;
				prenhit	=	nhit;
				prenhit2	=	nhit2;
				processhit=0;
				processhit2=0;
				nhit=0;
				nhit2=0;
        for(int jhit=0; jhit<prenhit; jhit++){
          m_prechipArray[jhit]    = m_chipArray[jhit]    ;
          m_prechanArray[jhit]    = m_chanArray[jhit]    ;
          m_preadcArray[jhit]     = m_adcArray[jhit]     ;
          m_preeventArray[jhit]   = m_eventArray[jhit]   ;
          m_prebcoArray[jhit]     = m_bcoArray[jhit]     ;
          m_prebco_fullArray[jhit]= m_bco_fullArray[jhit];
				}
        for(int jhit=0; jhit<prenhit2; jhit++){
          m_prechipArray2[jhit]    = m_chipArray2[jhit]    ;
          m_prechanArray2[jhit]    = m_chanArray2[jhit]    ;
          m_preadcArray2[jhit]     = m_adcArray2[jhit]     ;
          m_preeventArray2[jhit]   = m_eventArray2[jhit]   ;
          m_prebcoArray2[jhit]     = m_bcoArray2[jhit]     ;
          m_prebco_fullArray2[jhit]= m_bco_fullArray2[jhit];
				}
			}
			singlemV->Fill(adc_mV(adc),mV_weight2(adc_mV(adc)));
//++++++++++++++copy new data+++++++++++++++++++++++++++++
			if(module==8&&ampl==0&&adc>-1.&&adc<8&&chip_id>0&&chip_id<27&&chan_id>-1&&chan_id<128){
				m_chipArray[nhit] = chip_id;
				m_chanArray[nhit] = chan_id;
				m_adcArray[nhit]  = adc;
				m_eventArray[nhit]= event;
				m_bcoArray[nhit]  = bco;
				m_bco_fullArray[nhit]=bco_full;
				nhit++;
			}
			if(module==6&&ampl==0&&adc>-1.&&adc<8&&chip_id>0&&chip_id<27&&chan_id>-1&&chan_id<128){
				m_chipArray2[nhit2] = chip_id;
				m_chanArray2[nhit2] = chan_id;
				m_adcArray2[nhit2]  = adc;
				m_eventArray2[nhit2]= event;
				m_bcoArray2[nhit2]  = bco;
				m_bco_fullArray2[nhit2]=bco_full;
				nhit2++;
			}
		}
		if(chan_id<0||128<=chan_id) { 
			cout<<"exceed chan_id"<<endl;
			cout<<event<<" clk:"<<bco<<" "<<bco_full<<" : "<<chip_id<<" "<<chan_id<<" : "<<module<<" "<<adc<<(adc>0?"hit":" ")<<endl;;
		}
	cout<<"noise"<<noise<<endl;
};
void readtree::init_histo(){
  int m=9;
  double xbins[] = {0, 270, 302, 450, 602, 750, 902, 1050, 1202, 1234};   
  singlemV  = new TH1F("singlemV","energy loss without clustering with weight",m,xbins);
  hnhits    = new TH1F("hnhits","N hits per event",7,0,7);
  SingleAdc = new TH1F("SingleAdc","energyloss before clustering;mV",m,xbins);
  DAdc      = new TH1F("DAdc","energyloss before clustering;mV",m,xbins);
  TAdc      = new TH1F("TAdc","energyloss before clustering;mV",m,xbins);
  SingleAdc->SetLineColor(kRed);
  DAdc->SetLineColor(kBlue);
  TAdc->SetLineColor(kGreen);
  m_noiseAdc       = new TH1F("m_noiseAdc", "ADC of noise data;adc", 8, 0, 8);
  Wadc             = new TH1F("Wadc", "ADC with weight;adc", 8, 0, 8);
  NHITChip         = new TH2F("NHITChip","ADC correlation of separate double hits;ch1 ADC;ch2 ADC",12,-2.,10.,12,-2.,10.);
  chan_1hit        = new TH2F("chan_1hit","channel correlation of separate double hits;ch1 channel;ch2 channel",130,0.,130.,130,0.,130.);
  chan_1hit2       = new TH2F("chan_1hit2","channel difference of single vs chip;channel difference;chip_id",128,0.,128.,27,0,27);
  adc_0hit         = new TH2F("adc_0hit","single hit of ADC=0;chan_id;chip_id",128,0,128,26,0,26);
  multiclschip     = new TH2F("multiclschip","chip vs channel in multiple clsters event;chan_id;chip_id",128,0,128,26,0,26);
  nclsnChip     	 = new TH1F("nclsnChip","Number of clusters per event;number of clusters",5,0,5);
  NCLS6			     	 = new TH1F("NCLS6","Number of clusters per event;number of clusters",5,0,5);
  NCLS8     			 = new TH1F("NCLS8","Number of clusters per event;number of clusters",5,0,5);
  NSTRIP6			     = new TH1F("NSTRIP6","Number of strips per event;number of strips",5,0,5);
  NSTRIP8     		 = new TH1F("NSTRIP8","Number of strips per event;number of strips",5,0,5);
  NHITChipmV       = new TH1F("NHITChipmV","2 single hits in same chip;mV",m,xbins);
  SumDoublehitAdc2 = new TH1F("SumDoublehitAdc2","energy loss per cluster;mV",m,xbins);
  SinglehitAdc2    = new TH1F("SinglehitAdc2","energy loss per cluster;mV",m,xbins);
  SumTriplehitAdc2 = new TH1F("SumTriplehitAdc2","energy loss per cluster;mV",m,xbins);
  Adc_2hit         = new TH2F("Adc_2hit","adc correlation of 2 hits;ch1 adc;ch2 adc",12,-2.,10.,12,-2.,10.);
  chan_2hit        = new TH2F("chan_2hit","channel correlation of 2 hits;ch1 channel;ch2 channel",130,0.,130.,130,0.,130.);
  sa_correlation   = new TH2F("sa_correlation","correlation of bco and bcofull interval;bco interval;bcofull interval",1000,-500.,500.,10000,-5000,5000.);
  SumTriplehitAdc2 = new TH1F("SumTriplehitAdc2","energy loss per cluster;mV",m,xbins);
  bco_interval		 = new TH1F("bco_interval","bco interval;bco interval",300,-150,150);
  bcofull_interval = new TH1F("bcofull_interval","bcofull interval;bco interval",140000,-70000,70000);
  adc_bcofull 		 = new TH1F("adc_bcofull","ADC (bcofull interval==-1);adc",8,0,8);
  ADCDIFF      	   = new TH2F("ADCDIFF","energyloss correlation of 2 hits;channel1 mV;channel2 mV",m,xbins,m,xbins);
  ELOSSDIFF        = new TH2F("ELOSSDIFF","energy loss correlation of modules;moudle6 mV;module8 mV",m,xbins,m,xbins);
  ELOSSDIFFSA      = new TH1F("ELOSSDIFFSA","energy loss  difference between modules;mV",130,0.,1300.);
  ELOSSTRAN        = new TH2F("ELOSSTRAN","energy loss correlarion between number of hits;1 hit mV;2 hits mV",m,xbins,m,xbins);
  ELOSSTRANSA      = new TH1F("ELOSSTRANSA","energy loss  difference between number of hits;mV",130,0.,1300.);
  ELOSSTRAN1       = new TH2F("ELOSSTRAN1","energy loss differense between number of hits(1);1 hit mV;2 hits mV",m,xbins,m,xbins);
  ELOSSTRAN2       = new TH2F("ELOSSTRAN2","energy loss differense between number of hits(2);1 hit mV;2 hits mV",m,xbins,m,xbins);
  ELOSSTRAN3       = new TH2F("ELOSSTRAN3","energy loss differense between number of hits(3);1 hit mV;2 hits mV",m,xbins,m,xbins);
  ELOSSTRAN4       = new TH2F("ELOSSTRAN4","energy loss differense between number of hits(4);1 hit mV;2 hits mV",m,xbins,m,xbins);
  ELOSSTRANS       = new TH2F("ELOSSTRANS","energy loss differense between number of hits(hit position);1 hit mV;2 hits mV",m,xbins,m,xbins);
  CHANDIFF         = new TH2F("CHANDIFF","channel correlation of modules;moudle6 channel;module8 channel",65,-1,129,65,-1,129);
  CHANDIFFS        = new TH2F("CHANDIFFS","channel correlation of modules(hit position);moudle6 channel;module8 channel",8,0,128,8,0,128);
  CHANDIFFSA1      = new TH1F("CHANDIFFSA1","channel difference between chips;chan (1)",130,-1,129);
  CHANDIFFSA2      = new TH1F("CHANDIFFSA2","channel difference between chips;chan (2)",130,-1,129);
  CHANDIFFSA3      = new TH1F("CHANDIFFSA3","channel difference between chips;chan (3)",130,-1,129);
  CHANDIFFSA4      = new TH1F("CHANDIFFSA4","channel difference between chips;chan (4)",130,-1,129);
  CHANDIFFSAh      = new TH1F("CHANDIFFSAh","channel difference between chips(hit position);channel difference",16,-128,128);
  CHIPDIFFSAh      = new TH1F("CHIPDIFFSAh","chip difference between modules(hit position) ;chip difference",52,-26,26);
  ELOSSDIFF1     	 = new TH2F("ELOSSDIFF1","energy loss correlation of modules (1);moudle6 mV;module8 mV",m,xbins,m,xbins);
  ELOSSDIFF2     	 = new TH2F("ELOSSDIFF2","energy loss correlation of modules (2);moudle6 mV;module8 mV",m,xbins,m,xbins);
  ELOSSDIFF3     	 = new TH2F("ELOSSDIFF3","energy loss correlation of modules (3);moudle6 mV;module8 mV",m,xbins,m,xbins);
  ELOSSDIFF4     	 = new TH2F("ELOSSDIFF4","energy loss correlation of modules (4);moudle6 mV;module8 mV",m,xbins,m,xbins);
	ELOSSCLS1_8_1		 = new TH1F("ELOSSCLS1_8_1","clusterd energy loss (1);mV",m,xbins);					
	ELOSSCLS1_6_1		 = new TH1F("ELOSSCLS1_6_1","clusterd energy loss (1);mV",m,xbins);					
	ELOSSCLS2_8_1		 = new TH1F("ELOSSCLS2_8_1","clusterd energy loss (1);mV",m,xbins);					
	ELOSSCLS2_6_1		 = new TH1F("ELOSSCLS2_6_1","clusterd energy loss (1);mV",m,xbins);					
	ELOSSCLS1_8_2		 = new TH1F("ELOSSCLS1_8_2","clusterd energy loss (2);mV",m,xbins);					
	ELOSSCLS1_6_2		 = new TH1F("ELOSSCLS1_6_2","clusterd energy loss (2);mV",m,xbins);					
	ELOSSCLS2_8_2		 = new TH1F("ELOSSCLS2_8_2","clusterd energy loss (2);mV",m,xbins);					
	ELOSSCLS2_6_2		 = new TH1F("ELOSSCLS2_6_2","clusterd energy loss (2);mV",m,xbins);					
	ELOSSCLS1_8_3		 = new TH1F("ELOSSCLS1_8_3","clusterd energy loss (3);mV",m,xbins);					
	ELOSSCLS1_6_3		 = new TH1F("ELOSSCLS1_6_3","clusterd energy loss (3);mV",m,xbins);					
	ELOSSCLS2_8_3		 = new TH1F("ELOSSCLS2_8_3","clusterd energy loss (3);mV",m,xbins);					
	ELOSSCLS2_6_3		 = new TH1F("ELOSSCLS2_6_3","clusterd energy loss (3);mV",m,xbins);					
	ELOSSCLS1_8_4		 = new TH1F("ELOSSCLS1_8_4","clusterd energy loss (4);mV",m,xbins);					
	ELOSSCLS1_6_4		 = new TH1F("ELOSSCLS1_6_4","clusterd energy loss (4);mV",m,xbins);					
	ELOSSCLS2_8_4		 = new TH1F("ELOSSCLS2_8_4","clusterd energy loss (4);mV",m,xbins);					
	ELOSSCLS2_6_4		 = new TH1F("ELOSSCLS2_6_4","clusterd energy loss (4);mV",m,xbins);					
  CHIPDIFF         = new TH2F("CHIPDIFF","chip correlation of modules;moudle6 chip_id;module8 chip_id",28,-1,27,28,-1,27);  
  ELOSSCLS1_1      = new TH1F("ELOSSCLS1_1","clusterd energy loss (1);mV",m,xbins);
  ELOSSCLS2_1      = new TH1F("ELOSSCLS2_1","clusterd energy loss (1);mV",m,xbins);
  ELOSSCLS1_2      = new TH1F("ELOSSCLS1_2","clusterd energy loss (2);mV",m,xbins);
  ELOSSCLS2_2      = new TH1F("ELOSSCLS2_2","clusterd energy loss (2);mV",m,xbins);
  ELOSSCLS1_3      = new TH1F("ELOSSCLS1_3","clusterd energy loss (3);mV",m,xbins);
  ELOSSCLS2_3      = new TH1F("ELOSSCLS2_3","clusterd energy loss (3);mV",m,xbins);
  ELOSSCLS1_4      = new TH1F("ELOSSCLS1_4","clusterd energy loss (4);mV",m,xbins);
  ELOSSCLS2_4      = new TH1F("ELOSSCLS2_4","clusterd energy loss (4);mV",m,xbins);
  ELOSSCLS1_S      = new TH1F("ELOSSCLS1_S","clusterd energy loss (diff=3, different hits);mV",m,xbins);
  ELOSSCLS2_S      = new TH1F("ELOSSCLS2_S","clusterd energy loss (diff=3, different hits);mV",m,xbins);
  ELOSSCLS1__S     = new TH1F("ELOSSCLS1__S","clusterd energy loss (diff=3);mV",m,xbins);
  ELOSSCLS2__S     = new TH1F("ELOSSCLS2__S","clusterd energy loss (diff=3);mV",m,xbins);
	RECELOSS1				 = new TH1F("RECELOSS1","reconstract energy loss;mV",m,xbins);
	RECELOSS2				 = new TH1F("RECELOSS2","reconstract energy loss;mV",m,xbins);
	originalenergyloss				 = new TH1F("originalenergyloss","energyloss before clustering;mV",m,xbins);
	ELOSSCLS1_1		  ->SetLineColor(kRed);  
	ELOSSCLS2_1		  ->SetLineColor(kBlue); 
	ELOSSCLS1_2		  ->SetLineColor(kRed);  
	ELOSSCLS2_2		  ->SetLineColor(kBlue); 
	ELOSSCLS1_3		  ->SetLineColor(kRed);  
	ELOSSCLS2_3		  ->SetLineColor(kBlue); 
	ELOSSCLS1_4		  ->SetLineColor(kRed);  
	ELOSSCLS2_4		  ->SetLineColor(kBlue); 
	ELOSSCLS1_S		  ->SetLineColor(kRed);  
	ELOSSCLS2_S		  ->SetLineColor(kBlue); 
	ELOSSCLS1__S	  ->SetLineColor(kRed);  
	ELOSSCLS2__S	  ->SetLineColor(kBlue); 
	RECELOSS1			  ->SetLineColor(kRed);
	RECELOSS2			  ->SetLineColor(kBlue);
	NCLS6		 				->SetLineColor(kRed);  
	NCLS8		 				->SetLineColor(kBlue); 
	NSTRIP6		 			->SetLineColor(kRed);  
	NSTRIP8		 			->SetLineColor(kBlue); 
  SumDoublehitAdc2->SetLineColor(kBlue);
  SinglehitAdc2		->SetLineColor(kRed);
  SumTriplehitAdc2->SetLineColor(kGreen);
	ELOSSCLS1_8_1		->SetLineColor(kRed); 
	ELOSSCLS1_6_1		->SetLineColor(kRed);  
	ELOSSCLS2_8_1		->SetLineColor(kBlue); 
	ELOSSCLS2_6_1		->SetLineColor(kBlue); 
	ELOSSCLS1_8_2		->SetLineColor(kRed);  
	ELOSSCLS1_6_2		->SetLineColor(kRed);  
	ELOSSCLS2_8_2		->SetLineColor(kBlue); 
	ELOSSCLS2_6_2		->SetLineColor(kBlue); 
	ELOSSCLS1_8_3		->SetLineColor(kRed);  
	ELOSSCLS1_6_3		->SetLineColor(kRed);  
	ELOSSCLS2_8_3		->SetLineColor(kBlue); 
	ELOSSCLS2_6_3		->SetLineColor(kBlue); 
	ELOSSCLS1_8_4		->SetLineColor(kRed);  
	ELOSSCLS1_6_4		->SetLineColor(kRed);  
	ELOSSCLS2_8_4		->SetLineColor(kBlue); 
	ELOSSCLS2_6_4		->SetLineColor(kBlue); 
}
void readtree::end(){
  m_froot->cd(); 	 
  singlemV 					->Write();
  hnhits   					->Write();
  SingleAdc					->Write();
  DAdc     					->Write();
  TAdc     					->Write();
  singlemV 					->Write();
  originalenergyloss->Write();
  m_noiseAdc	      ->Write();
  Wadc              ->Write();
  NHITChip          ->Write();
  chan_1hit         ->Write();
  chan_1hit2        ->Write();
  adc_0hit          ->Write();
  multiclschip      ->Write();
  NHITChipmV        ->Write();
  SumDoublehitAdc2  ->Write();
  SinglehitAdc2     ->Write();
  SumTriplehitAdc2  ->Write();
  Adc_2hit          ->Write();
  chan_2hit         ->Write();
  sa_correlation	  ->Write();
  bco_interval	 	  ->Write();
  bcofull_interval  ->Write();
	adc_bcofull			  ->Write();
	nclsnChip			 	  ->Write();
	NCLS6					 	  ->Write();
	NCLS8					 	  ->Write();
	NSTRIP6				 	  ->Write();
	NSTRIP8				 	  ->Write();
	ELOSSDIFF			 	  ->Write();
	ELOSSDIFFSA		 	  ->Write();
	ELOSSTRAN			 	  ->Write();
	ELOSSTRANSA		 	  ->Write();
	ELOSSTRANS		 	  ->Write();
	CHANDIFF			 	  ->Write();
	CHANDIFFS			 	  ->Write();
	CHIPDIFF			 	  ->Write();
	ADCDIFF				 	  ->Write();
  CHANDIFFSA1			  ->Write();
  CHANDIFFSA2       ->Write();
  CHANDIFFSA3       ->Write();
  CHANDIFFSA4       ->Write();
  CHANDIFFSAh       ->Write();
  CHIPDIFFSAh       ->Write();
  CHIPDIFFSAh       ->Write();
  ELOSSDIFF1     	  ->Write();
  ELOSSDIFF2     	  ->Write();
  ELOSSDIFF3     	  ->Write();
  ELOSSDIFF4     	  ->Write();
  ELOSSCLS1_8_1     ->Write();
  ELOSSCLS1_6_1     ->Write();
  ELOSSCLS2_8_1     ->Write();
  ELOSSCLS2_6_1     ->Write();
  ELOSSCLS1_8_2     ->Write();
  ELOSSCLS1_6_2     ->Write();
  ELOSSCLS2_8_2     ->Write();
  ELOSSCLS2_6_2     ->Write();
  ELOSSCLS1_8_3     ->Write();
  ELOSSCLS1_6_3     ->Write();
  ELOSSCLS2_8_3     ->Write();
  ELOSSCLS2_6_3     ->Write();
  ELOSSCLS1_8_4     ->Write();
  ELOSSCLS1_6_4     ->Write();
  ELOSSCLS2_8_4     ->Write();
  ELOSSCLS2_6_4     ->Write();
  ELOSSTRAN1        ->Write();
  ELOSSTRAN2        ->Write();
  ELOSSTRAN3        ->Write();
  ELOSSTRAN4        ->Write();
  ELOSSCLS1_1       ->Write();
  ELOSSCLS2_1       ->Write();
  ELOSSCLS1_2       ->Write();
  ELOSSCLS2_2       ->Write();
  ELOSSCLS1_3       ->Write();
  ELOSSCLS2_3       ->Write();
  ELOSSCLS1_4       ->Write();
  ELOSSCLS2_4       ->Write();
  ELOSSCLS2_S       ->Write();
  ELOSSCLS1_S       ->Write();
  ELOSSCLS2__S      ->Write();
  ELOSSCLS1__S      ->Write();
	RECELOSS1				  ->Write(); 
	RECELOSS2			 	  ->Write();
}
