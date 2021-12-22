
// 2021/12/22 Mika Shibata Nara Women's University

// ************** README ************************************
// This macro is for the first INTT hit with a CAMAC trigger signal.
// (There are multiple INTT hits with different bcofull in a CAMAC trigger signal)
// In this macro, you can change same variables and situations.
//   INTT hit cutting window for efficiency calculation: 
//    1.adc
//      now-> adc>2
//    2.offset (from the mean of fit gaus function of residual distribution)
//      now-> (l.464)  residual_chan_13[3] = {7.5652730,-3.6684312,7.3548750};
//    3.chip
//      now-> chip+-1
//    4.channel (from the sigma of fit gaus function of residual distribution)
//      now-> (l.576, 656, 739)  if(m_boolMC)  cutwindow = 1.14710e+00*3.
//    5.number of clusters in the ladder to calculate the detection efficiency. 
//      now-> (l.513) if(COSMIC[0][0]==1&&COSMIC[1][0]==1&&COSMIC[2][0]==1) 
// In a ROOT session, you can do:
//   Root > .L readtree.C
//   Root > readtree t
//   Root > t.next();          // Show and fill t data members per bco and bcofull (The first INTT hit with a CAMAC trigger signal)
//   Root > t.next_continue(); // Loop next() on all entries (The first INTT hit with a CAMAC trigger signal)


#define readtree_cxx
#include "readtree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph2D.h>
#include <TText.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TF1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraph2DErrors.h>
#include <TLegend.h>
#include <TLatex.h>
#include "TNtuple.h"
#include "TFile.h"
#include <TAxis.h>
#include <TMath.h>
#include <TRandom.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <math.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <TH2.h>
#include <TPaveStats.h>
#include <TText.h>

using namespace std;

double mV_adc2(double SmV){
  //float  mV[9] = {250, 300, 450, 602, 750, 902, 1050, 1202, 1234};
  float  mV[9] = {270, 300, 450, 602, 750, 902, 1050, 1202, 1234};
  int Adc;
  for(int ii=0;ii<8;ii++)
  {
    if( mV[ii]<=SmV && SmV<mV[ii+1])
    {
      Adc=ii;
    }
  }
  return Adc;//*Adc;
}
double weight(int hitAdc){
  //float Dac[9] = {10, 23, 60, 98, 135, 173, 210, 248, 256};
  float Dac[9] = {15, 23, 60, 98, 135, 173, 210, 248, 256};
  float weight[8];
  float tmpw = Dac[1]-Dac[0];
  for(int i=0; i<8; i++){
    weight[i]  = (Dac[i+1]-Dac[i])/tmpw;
  }
  float w = 1./weight[hitAdc];
  return w;
}
double mV_weight(int hitAdc){
  //float  mV[9] = {250, 302, 450, 602, 750, 902, 1050, 1202, 1234};
  float  mV[9] = {270, 302, 450, 602, 750, 902, 1050, 1202, 1234};
  float weight[8];
  float tmpw = mV[1]-mV[0];
  for(int i=0; i<8; i++){
    weight[i]  = (mV[i+1]-mV[i])/tmpw;
  }
  float w = 1./weight[hitAdc];
  return w;
}
double mV_weight2(int hitmV){
  //float  mV[9] = {250, 302, 450, 602, 750, 902, 1050, 1202, 1234};
  float  mV[9] = {270, 302, 450, 602, 750, 902, 1050, 1202, 1234};
  float weight[8];
  float w;
  float tmpw = mV[1]-mV[0];
  for(int i=0; i<8; i++){
    weight[i]  = (mV[i+1]-mV[i])/tmpw;
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
  //double Dac[9] = {10., 23., 60., 98., 135., 173., 210., 248., 256.};
  double Dac[9] = {15., 23., 60., 98., 135., 173., 210., 248., 256.};
  double  mV = ((Dac[hitAdc+1]*4.+210.)+(Dac[hitAdc]*4.+210.))/2.-2.;
  return mV;
}

void readtree::process_hits(
             int nhit, 
             int* chipArray, 
             int* chanArray, 
             int* adcArray, 
             int* Nhits_Ret, 
             double* chan_Ret, 
             double* chip_Ret, 
             int* nclsnChip_Ret, 
             int  (*adc_Ret)[10],
             int* COSMIC)
{        
  
  int nhitPerModule = nhit;
  int nhitChipPerModule = 0;
  int nhitPerChip[26];
  int hitChip[26][255];
  int hitChan[26][255];
  int hitAdc[26][255];
  int hitModule[26][255];
  int isHitChipChan[26][255];
  for(int i1=0;i1<26;i1++){
    nhitPerChip[i1] = 0;
    for(int i2=0;i2<255;i2++){
      isHitChipChan[i1][i2] = 0;
    }
  }
  int PreChip = 0;
  for(int i=0;i<nhitPerModule;i++)
  {
    hitChip[chipArray[i]-1][nhitPerChip[chipArray[i]-1]] = chipArray[i];
    hitChan[chipArray[i]-1][nhitPerChip[chipArray[i]-1]] = chanArray[i];
    hitAdc [chipArray[i]-1][nhitPerChip[chipArray[i]-1]] = adcArray[i];
    PreChip = chipArray[i];
    nhitPerChip[chipArray[i]-1]++;
  }

  int tmp1, tmp2, tmp3, tmp4;
  
  // sort channel order per chip_id
  for(int ichip=0; ichip<26; ichip++)
  {
    Nhits_Ret[ichip] = nhitPerChip[ichip];
    for(int ihit=0; ihit<nhitPerChip[ichip]; ihit++)
    {
      for(int jhit=ihit+1;jhit<nhitPerChip[ichip];jhit++)
      {
        bool isLargeChan = hitChan[ichip][ihit] > hitChan[ichip][jhit];
        if(isLargeChan){
          tmp1 = hitChan[ichip][ihit];
          tmp2 = hitChip[ichip][ihit];
          tmp3 = hitAdc[ichip][ihit];
          hitChan[ichip][ihit]    = hitChan[ichip][jhit];
          hitChip[ichip][ihit]    = hitChip[ichip][jhit];
          hitAdc[ichip][ihit]     = hitAdc[ichip][jhit];
          hitChan[ichip][jhit]    = tmp1;
          hitChip[ichip][jhit]    = tmp2;
          hitAdc[ichip][jhit]     = tmp3;
        }
      }
    }
  }
  
  for(int ichip=0; ichip<26; ichip++)
  {
    if(nhitPerChip[ichip]==0)continue;
    for(int ihit=0; ihit<nhitPerChip[ichip]; ihit++)
    {
      //cout<<"chip:"<<hitChip[ichip][ihit]
      //    <<"chan:"<<hitChan[ichip][ihit]
      //    <<"adc:"<<hitAdc[ichip][ihit]<<endl;
    }
  }
  
  // count hits per module per chip
  for(int ichip=0;ichip<26;ichip++){
    if(nhitPerChip[ichip]>0)nhitChipPerModule++;
  }


  // count number of strips per event
  int    sumChan;
  int    ncls = 0;
  double summV;
  
  for(int ichip=0; ichip<26; ichip++)
  {  
    int NHITCHIP=0;  //number of single hits per chip per event
    int single_ihit[20];  
    int start = 0;
    int end;
    int Nhits = 1; //number of hits per event(cluster)
    for(int ihit=0; ihit<nhitPerChip[ichip]; ihit++)
    {
      bool isEndHit = Nhits==nhitPerChip[ichip]||ihit==nhitPerChip[ichip]-1;
      if(isEndHit)
      {
        end = ihit;
        sumChan = 0;
        for(int i=start; i<end+1; i++){
          sumChan += hitChan[ichip][i];
        }
        nclsnChip_Ret[0] = ncls;
        Nhits_Ret[ncls] = Nhits;
        chan_Ret[ncls] = (double)sumChan/Nhits;
        chip_Ret[ncls] = (double)ichip+1;
            
        bool isSingleHit = Nhits==1;
        if(isSingleHit)
        { 
          single_ihit[NHITCHIP] = start;
          NHITCHIP++;
        }
        
        for(int I=0;I<Nhits;I++)
        {
          adc_Ret[ncls][I]  =  hitAdc[ichip][start+I];
        }
        
        start = end+1;  //start=1, end=0
        ncls++; // number of cluster per bco
        Nhits = 1;
      }

      bool isMidHit = hitChan[ichip][ihit+1]-hitChan[ichip][ihit]==1;
      if(!isEndHit)if(isMidHit){
        Nhits++;
      }
       
      if(!isMidHit)if(!isEndHit){ // there are multiple single hits per bco
        end = ihit;
        sumChan = 0;
        for(int i=start; i<end+1; i++){
          sumChan += hitChan[ichip][i];
        }
        

        nclsnChip_Ret[ncls] = ncls;
        Nhits_Ret[ncls] = Nhits;
        chan_Ret[ncls] = (double)sumChan/Nhits;
        chip_Ret[ncls] = (double)ichip+1;
            
        bool isSingleHit = Nhits==1;
        if(isSingleHit)
        { 
          single_ihit[NHITCHIP] = start;
          NHITCHIP++;
        }

        for(int I=0;I<Nhits;I++)
        {
          adc_Ret[ncls][I]  =  hitAdc[ichip][start+I];
        }
        start = end+1;
        ncls++;
        Nhits = 1;
      }
      nclsnChip_Ret[0] = ncls;
    }
  }
  COSMIC[0] = 1;
  if(ncls>1){COSMIC[0]=0;}
  else if(ncls==0){COSMIC[0]=0;}
};

void readtree::Loop(Long64_t entry)
//void readtree::Loop(const int entry)
{

  int    nhit=0,
         prenhit=0,
         noise=0;
  int    m_nbco[50000],
         m_nbco_full[50000],
         m_chanArray[50000],
         m_chipArray[50000],
         m_adcArray[50000],
         m_moduleArray[50000],
         m_eventArray[50000],
         m_bcoArray[50000],
         m_bco_fullArray[50000],
         m_RowArray[50000],
         m_prenbco_full[50000],
         m_prechanArray[50000],
         m_prechipArray[50000],
         m_preadcArray[50000],
         m_preeventArray[50000],
         m_premoduleArray[50000],
         m_prebcoArray[50000],
         m_prebco_fullArray[50000];
  int    sa_bco, sa_bcofull;
  int    noisehit    = 0, cosmichit   = 0,
         prenoisehit = 0, precosmichit= 0, processhit  = 0;
  m_NextEventInOneTrigger = false;

  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();

  for(int i=0;i<4;i++) m_hitmap_perLadder[i]->Reset();
  
  m_hitmap->Reset();
  m_hitLaddermap->Reset();
  m_NHIT = 0;
  
  if(entry>=0) m_jentry=entry;

  Long64_t nbytes = 0, nb = 0;
  for (; m_jentry<nentries;m_jentry++) 
  {
    Long64_t ientry = LoadTree(m_jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(m_jentry);   nbytes += nb;
    
    for(int ins=0;ins<module->size();ins++){
      m_NextEventInOneTrigger=0;

      bool boolCUTDATA;
      if(m_boolMC) boolCUTDATA = ins==module->size()-1;
      if(!m_boolMC)boolCUTDATA = ins!=0 && (m_prebco!=bco->at(ins)) && (m_prebcofull!=bco_full->at(ins));
      if(boolCUTDATA) // for data
       {
        m_NextEventInOneTrigger = true;
        
        int nhitPerModule[4];
        int nhitChipPerModule[4];
        int nhitPerChip[4][26];
        int hitChip[4][26];
        int hitChan[4][26];
        int hitAdc[4][26];
        int hitBco[4][26];
        int hitBco_full[4][26];
        int nclsnChip_Ret[4];
        int Nhits_Ret[4][26];
        double chan_Ret[4][26],
            chip_Ret[4][26];
        int adc_Ret[10][10];
        int ncls_Ret[4][26];
        int COSMIC[4][26]; 
        for(int i=0;i<4;i++){
          nhitPerModule[i] = 0;
          nhitChipPerModule[i] = 0;
          for(int j=0;j<26;j++){
            ncls_Ret[i][j] = 0;
            nhitPerChip[i][j] = 0;
            Nhits_Ret[i][j] = 0;
            hitChip[i][j] = 0;
            hitChan[i][j] = 0;
            hitAdc[i][j] = 0;
            hitBco[i][j] = 0;
            hitBco_full[i][j] = 0;
            chan_Ret[i][j] = 0;
            chip_Ret[i][j] = 0;
            COSMIC[i][j] = 0;
          }
        }
        for(int i=0;i<10;i++){
          for(int j=0;j<10;j++){
            adc_Ret[i][j] = 0;
          }
        }
        m_nhit->Fill(nhit);
        int PreChip = 0;
        for(int i=0;i<nhit;i++)
        {
          if(m_chipArray[i]<14)
          {
            hitChip[m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_chipArray[i];
            hitChan[m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_chanArray[i];
            hitAdc [m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_adcArray[i];
            hitBco [m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_bcoArray[i];
            hitBco_full [m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_bco_fullArray[i];
            nhitPerChip[m_moduleArray[i]][m_chipArray[i]-1]++;
            nhitPerModule[m_moduleArray[i]]++;
          }
          if(m_chipArray[i]>=14)
          {
            hitChip[m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_chipArray[i]-13.; 
            hitChan[m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = 255. - m_chanArray[i]; 
            hitAdc [m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_adcArray[i];
            hitBco [m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_bcoArray[i];
            hitBco_full [m_moduleArray[i]][nhitPerModule[m_moduleArray[i]]] = m_bco_fullArray[i];
            nhitPerChip[m_moduleArray[i]][m_chipArray[i]-13-1]++;
            nhitPerModule[m_moduleArray[i]]++;
          }
        }
        
        bool COSMIC3Ladder=true;
        double clustered_adc[3] = {0.,0.,0.};  
        if(nhitPerModule[m_moduleArray[0]]>0)cout<<"------ clustered data **chip_id(1-13) chan_id(0-255) ------"<<endl;
        for(int i=0;i<3;i++)
        {
          if(nhitPerModule[i]>0){
            process_hits(nhitPerModule[i], hitChip[i], hitChan[i], hitAdc[i], Nhits_Ret[i], chan_Ret[i], chip_Ret[i], ncls_Ret[i], adc_Ret, COSMIC[i]);
            
            for(int ii=0;ii<ncls_Ret[i][0];ii++)
            {
              //if(COSMIC[i][0]==1)
              {
                cout<<"Ladder No."<<i
                <<" chip:"<<chip_Ret[i][ii]
                <<" chan:"<<chan_Ret[i][ii]
                <<" nhit:"<<Nhits_Ret[i][ii]<<endl;
                for(int j=0;j<Nhits_Ret[i][ii];j++)
                {
                  cout<<" adc :"<<adc_Ret[ii][j]<<endl;
                  clustered_adc[i] = clustered_adc[i] + adc_mV(adc_Ret[ii][j]);
                }
              }
            }
            if(nhitPerModule[i]==0&&COSMIC[i][0]==0)COSMIC3Ladder = false;
          }
          clustered_adc[i] = mV_adc2(clustered_adc[i]);
        }
            

        double ef_LadderAxis[3] = {0.,1.,2.};
        double ef_ChipAxis[3][5];
        double ef_ChipAxisError[3] = {1./sqrt(12.),1./sqrt(12.),1./sqrt(12.)};
        double ef_ChanAxis[3][5];
        double ef_ChanAxisError[3] = {1./sqrt(12.),1./sqrt(12.),1./sqrt(12.)};
        double expect_ChipAxis[3];
        double expect_ChanAxis[3];
        
        // You can change the offset value form the mean of fittion function (gaus) for regidual distribution. 
        double residual_chan_13[3] = {7.5652730,-3.6684312,7.3548750}; 


        if(m_boolMC)
        {
          residual_chan_13[0] = 0.;
          residual_chan_13[1] = 0.;
          residual_chan_13[2] = 0.;
        }

        double chip_for2dhist[3];
        double chan_for2dhist[3];
        if(nhitPerModule[m_moduleArray[0]]>0)cout<<"------ clustered and residual configed data **chip_id(1-13) chan_id(0-255) ------"<<endl;
        for(int I=0;I<3;I++){
          chip_for2dhist[I] = chip_Ret[I][0]; 
          chan_for2dhist[I] = chan_Ret[I][0]; 
          
          for(int II=0;II<ncls_Ret[I][0];II++)
          {
            ef_ChipAxis[I][II] = chip_Ret[I][II];
            ef_ChanAxis[I][II] = chan_Ret[I][II];
            cout<<"Ladder No."<<I
                <<" chip:"<<chip_Ret[I][II]
                <<" chan:"<<chan_Ret[I][II]
                <<" nhit:"<<Nhits_Ret[I][II]<<endl;
            for(int j=0;j<Nhits_Ret[I][II];j++)
            {
              if(j==Nhits_Ret[I][II]-1)cout<<" clustered adc :"<<clustered_adc[I]<<endl;
            }
          }
        }


        
        // Select chip_id to calculate the efficiency
        // For the efficiency of limited chips
        // *****************************
        int CHOOSE_CHIP1 = 7;
        if(m_boolMC)CHOOSE_CHIP1 = 6;
        int CHOOSE_CHIP2 = 8;
        int CHOOSE_CHIP3 = 9;
        // *****************************
       
        double XAxis[2];
        double YAxis[2];
        bool   boolPrintEff = false;
        bool   SELECT_CHIP_ADC;
        bool   SELECT_CHIP1,SELECT_CHIP2,SELECT_CHIP3;
        bool   SELECT_CHIP;
        //if(nhit>0&&COSMIC3Ladder)
        if(COSMIC[0][0]==1&&COSMIC[1][0]==1&&COSMIC[2][0]==1) // This is for 1 cluster in a ladder to calculate the efficiency. If you want to allow multiple clusters, comment out this line. 
        {
          if(ncls_Ret[0][0]==1)
          {

            // *********** efficiency for ladder 2 *****************

            SELECT_CHIP1 = chip_for2dhist[0]==chip_for2dhist[1]&&chip_for2dhist[1]==CHOOSE_CHIP1;
            SELECT_CHIP2 = chip_for2dhist[0]==chip_for2dhist[1]&&chip_for2dhist[1]==CHOOSE_CHIP2;
            SELECT_CHIP3 = chip_for2dhist[0]==chip_for2dhist[1]&&chip_for2dhist[1]==CHOOSE_CHIP3;
            
            // ***** select one bool ************************
            SELECT_CHIP = chip_for2dhist[0]==chip_for2dhist[1];                         // for the efficiency of all chips (one ladder)  
            //SELECT_CHIP = SELECT_CHIP1==true||SELECT_CHIP2==true||SELECT_CHIP3==true; // for the efficiency of limited chips 
            // ******************************************************
            
            // ***** select adc valule ******************************
            SELECT_CHIP_ADC = clustered_adc[0]>2&&clustered_adc[1]>2; // adc is from 0 to 7
            // ******************************************************

            if(SELECT_CHIP)
            if(ncls_Ret[1][0]==1)
            if(SELECT_CHIP_ADC)
            {
              m_hcoll_chip[2]->Fill(chip_for2dhist[0],chip_for2dhist[1]);
              m_hcoll_chan[2]->Fill(chan_for2dhist[0],chan_for2dhist[1]);

              if(abs(chan_for2dhist[0]-chan_for2dhist[1])<=5)
              {

                XAxis[0] = ef_LadderAxis[0];
                XAxis[1] = ef_LadderAxis[1];
                YAxis[0] = ef_ChipAxis[0][0];
                YAxis[1] = ef_ChipAxis[1][0];
                expect_ChipAxis[2] = YAxis[0]; // YAxis[0]==YAxis[1], chip_for2dhist[0]==chip_for2dhist[1], ef_ChipAxis[0][0]==ef_ChipAxis[1][0] 

                YAxis[0] = ef_ChanAxis[0][0];
                YAxis[1] = ef_ChanAxis[1][0];
                expect_ChanAxis[2] = ef_ChanAxis[1][0]*2.-ef_ChanAxis[0][0];

                double ef_ChanAxis_new;
                double ef_ChipAxis_new;
                
                double chandiff = 225.;
                for(int ii=0; ii<ncls_Ret[2][0]; ii++)
                {
                  if(chandiff>expect_ChanAxis[2]-(chan_Ret[2][ii] + residual_chan_13[2]))
                  {
                    chandiff = expect_ChanAxis[2]-(chan_Ret[2][ii] + residual_chan_13[2]);
                    ef_ChanAxis_new = chan_Ret[2][ii] + residual_chan_13[2];
                    ef_ChipAxis_new = chip_Ret[2][ii];
                  }
                }
                m_residual_chip[2]->Fill(expect_ChipAxis[2]-ef_ChipAxis_new);
                m_residual_chan[2]->Fill(expect_ChanAxis[2]-ef_ChanAxis_new);
                
                cout<<"[ladder2] hit    chip:"<<ef_ChipAxis_new   <<" chan:"<<ef_ChanAxis_new<<endl;
                cout<<"          expect chip:"<<expect_ChipAxis[2]<<" chan:"<<expect_ChanAxis[2]<<endl;

                // ********* decide width for strips from residual distridution ********************
                // You can change the offset value form the sigma of fittion function (gaus) for regidual distribution. ex)sigma*N
                double cutwindow;
                //if(m_boolMC)  cutwindow = 7.70906e+00*3.; // choose event of 1 cluster hit in 2 ladders
                if(m_boolMC)  cutwindow = 1.13262e+00*3.;   // choose event of 1 cluster hit in all ladders
                //if(!m_boolMC) cutwindow = 5.;
                if(!m_boolMC) cutwindow = 1.06619e+00*3.;
                //if(!m_boolMC) cutwindow = 9.95920e-01*3.;
                //if(!m_boolMC) cutwindow = 1.11467e+00*3.;
                if(  expect_ChanAxis[2] -cutwindow <= ef_ChanAxis_new
                    &&expect_ChanAxis[2] +cutwindow >= ef_ChanAxis_new)
                // *****************************
                {
                  // ******** choose chip_id +- 1 (ex. 4 -> 3, 4, 5, 16, 17, 18)*********************
                  if(expect_ChipAxis[2]<=ef_ChipAxis_new+1. && expect_ChipAxis[2]>=ef_ChipAxis_new-1.) 
                  // *****************************
                  {
                    m_ef_GOODhit[2]++;
                    cout<<"+ 1hit for efficiency of ladder"<<2<<endl;
                    boolPrintEff = true;
                  }
                }
                m_ef_Allhit[2]++;
                cout<<"m_ef_Allhit++"<<endl;
              }
            }


            
            // *********** efficiency for ladder 1 *****************

            SELECT_CHIP1 = chip_for2dhist[0]==chip_for2dhist[2]&&chip_for2dhist[0]==CHOOSE_CHIP1;
            SELECT_CHIP2 = chip_for2dhist[0]==chip_for2dhist[2]&&chip_for2dhist[0]==CHOOSE_CHIP2;
            SELECT_CHIP3 = chip_for2dhist[0]==chip_for2dhist[2]&&chip_for2dhist[0]==CHOOSE_CHIP3;
            
            // ***** select one bool ************************
            //SELECT_CHIP = SELECT_CHIP1==true||SELECT_CHIP2==true||SELECT_CHIP3==true;  // for the efficiency of limited chips 
            SELECT_CHIP = chip_for2dhist[0]==chip_for2dhist[2];                          // for the efficiency of all chips (one ladder)
            // ******************************************************
            
            // ***** select adc valule ******************************
            SELECT_CHIP_ADC = clustered_adc[0]>2&&clustered_adc[2]>2;  // adc is from 0 to 7
            // ******************************************************
            if(SELECT_CHIP)
            if(ncls_Ret[2][0]==1)
            if(SELECT_CHIP_ADC)
            {
              m_hcoll_chip[1]->Fill(chip_for2dhist[0],chip_for2dhist[2]);
              m_hcoll_chan[1]->Fill(chan_for2dhist[0],chan_for2dhist[2]);
              
              if(abs(chan_for2dhist[0]-chan_for2dhist[2])<=5)
              {
              
                XAxis[0] = ef_LadderAxis[0];
                XAxis[1] = ef_LadderAxis[2];
                YAxis[0] = ef_ChipAxis[0][0];
                YAxis[1] = ef_ChipAxis[2][0];
                expect_ChipAxis[1] = YAxis[0]; // YAxis[0]==YAxis[2], chip_for2dhist[0]==chip_for2dhist[2], ef_ChipAxis[0][0]==ef_ChipAxis[2][0] 

                YAxis[0] = ef_ChanAxis[0][0];
                YAxis[1] = ef_ChanAxis[2][0];
                expect_ChanAxis[1] = (ef_ChanAxis[2][0]+ef_ChanAxis[0][0])/2.;

                double ef_ChanAxis_new, ef_ChipAxis_new;
                double chandiff = 225.;
                for(int ii=0; ii<ncls_Ret[1][0]; ii++)
                {
                  if(chandiff>expect_ChanAxis[1]-(chan_Ret[1][ii] + residual_chan_13[1]))
                  {
                    chandiff = expect_ChanAxis[1]-(chan_Ret[1][ii] + residual_chan_13[1]);
                    ef_ChanAxis_new = chan_Ret[1][ii] + residual_chan_13[1];
                    ef_ChipAxis_new = chip_Ret[1][ii];
                  }
                }
                  
                m_residual_chip[1]->Fill(expect_ChipAxis[1]-ef_ChipAxis_new);
                m_residual_chan[1]->Fill(expect_ChanAxis[1]-ef_ChanAxis_new);
                
                cout<<"[ladder1] hit    chip:"<<ef_ChipAxis_new   <<" chan:"<<ef_ChanAxis_new<<endl;
                cout<<"          expect chip:"<<expect_ChipAxis[1]<<" chan:"<<expect_ChanAxis[1]<<endl;
                
                // ********* decide width for strips from residual distridution ********************
                // You can change the offset value form the sigma of fittion function (gaus) for regidual distribution. ex)sigma*N
                double cutwindow;
                if(m_boolMC)  cutwindow = 6.34370e-01*3.; // choose event of 1 cluster hit in all ladders
                if(!m_boolMC) cutwindow = 5.99862e-01*3.;
                //if(!m_boolMC) cutwindow = 5.95493e-01*3.;
                //if(!m_boolMC) cutwindow = 6.17027e-01*3.;
                //if(!m_boolMC) cutwindow = 5.;
                if(  expect_ChanAxis[1] -cutwindow <= ef_ChanAxis_new
                    &&expect_ChanAxis[1] +cutwindow >= ef_ChanAxis_new)
                // ******************************************************
                {
                  // ******** choose chip_id +- 1 (ex. 4 -> 3, 4, 5, 16, 17, 18)*********************
                  if(expect_ChipAxis[1]<=ef_ChipAxis_new+1.&&expect_ChipAxis[1]>=ef_ChipAxis_new-1.)
                  // ******************************************************
                  {
                    m_ef_GOODhit[1]++;
                    cout<<"+ 1hit for efficiency of ladder"<<1<<endl;
                    boolPrintEff = true;
                  }
                }
                m_ef_Allhit[1]++;
                cout<<"m_ef_Allhit++"<<endl;
              }
            }
          }
          if(ncls_Ret[1][0]==1)
          {
            

            // *********** efficiency for ladder 0 *****************
            
            SELECT_CHIP1 = chip_for2dhist[2]==chip_for2dhist[1]&&chip_for2dhist[1]==CHOOSE_CHIP1;
            SELECT_CHIP2 = chip_for2dhist[2]==chip_for2dhist[1]&&chip_for2dhist[1]==CHOOSE_CHIP2;
            SELECT_CHIP3 = chip_for2dhist[2]==chip_for2dhist[1]&&chip_for2dhist[1]==CHOOSE_CHIP3;
            
            // ***** select one bool ************************
            //SELECT_CHIP = SELECT_CHIP1==true||SELECT_CHIP2==true||SELECT_CHIP3==true;  // for the efficiency of limited chips 
            SELECT_CHIP = chip_for2dhist[2]==chip_for2dhist[1];                          // for the efficiency of all chips (one ladder)
            // ******************************************************
            
            // ***** select adc valule ******************************
            SELECT_CHIP_ADC = clustered_adc[2]>2&&clustered_adc[1]>2;  // adc is from 0 to 7
            // ******************************************************
            if(SELECT_CHIP)
            if(ncls_Ret[2][0]==1)
            if(SELECT_CHIP_ADC)
            {
              m_hcoll_chip[0]->Fill(chip_for2dhist[1],chip_for2dhist[2]);
              m_hcoll_chan[0]->Fill(chan_for2dhist[1],chan_for2dhist[2]);
              
              if(abs(chan_for2dhist[1]-chan_for2dhist[1])<=5)
              {
              
                XAxis[0] = ef_LadderAxis[1];
                XAxis[1] = ef_LadderAxis[2];
                YAxis[0] = ef_ChipAxis[1][0];
                YAxis[1] = ef_ChipAxis[2][0];
                expect_ChipAxis[0] = YAxis[0]; // YAxis[2]==YAxis[1], chip_for2dhist[2]==chip_for2dhist[1], ef_ChipAxis[2][0]==ef_ChipAxis[1][0] 

                YAxis[0] = ef_ChanAxis[1][0];
                YAxis[1] = ef_ChanAxis[2][0];
                expect_ChanAxis[0] = (2.*ef_ChanAxis[1][0]-ef_ChanAxis[2][0]);
                
                double ef_ChanAxis_new, ef_ChipAxis_new;
                double chandiff = 225.;
                for(int ii=0; ii<ncls_Ret[0][0]; ii++)
                {
                  if(chandiff>expect_ChanAxis[0]-(chan_Ret[0][ii] + residual_chan_13[0]))
                  {
                    chandiff = expect_ChanAxis[0]-(chan_Ret[0][ii] + residual_chan_13[0]);
                    ef_ChanAxis_new = chan_Ret[0][ii] + residual_chan_13[0];
                    ef_ChipAxis_new = chip_Ret[0][ii];
                  }
                }
                
                m_residual_chip[0]->Fill(expect_ChipAxis[0]-ef_ChipAxis_new);
                m_residual_chan[0]->Fill(expect_ChanAxis[0]-ef_ChanAxis_new);
                
                cout<<"[ladder0] hit    chip:"<<ef_ChipAxis_new<<" chan:"<<ef_ChanAxis_new<<endl;
                cout<<"          expect chip:"<<expect_ChipAxis[0]<<" chan:"<<expect_ChanAxis[0]<<endl;
                

                // ********* decide width for strips from residual distridution ********************
                // You can change the offset value form the sigma of fittion function (gaus) for regidual distribution. ex)sigma*N
                double cutwindow; 
                if(m_boolMC)  cutwindow = 1.14710e+00*3.; // choose event of 1 cluster hit in all ladder
                if(!m_boolMC) cutwindow = 1.09515e+00*3.; // run023
                //if(!m_boolMC) cutwindow = 9.99403e-01*3.; // run089
                if(  expect_ChanAxis[0] -cutwindow <= ef_ChanAxis_new
                    &&expect_ChanAxis[0] +cutwindow >= ef_ChanAxis_new)
                // ******************************************************
                {
                  // ******** choose chip_id +- 1 (ex. 4 -> 3, 4, 5, 16, 17, 18)*********************
                  if(expect_ChipAxis[0]<=ef_ChipAxis_new+1.&&expect_ChipAxis[0]>=ef_ChipAxis_new-1.)
                  // ******************************************************
                  {
                    m_ef_GOODhit[0]++;
                    cout<<"+ 1hit for efficiency of ladder"<<0<<endl;
                    boolPrintEff = true;
                  }
                }
                m_ef_Allhit[0]++;
                cout<<"m_ef_Allhit++"<<endl;
              }
            }
          }
          for(int i=0;i<3;i++)
          {
            if(boolPrintEff)
            {
              cout<<"**********************************"<<endl;
              cout<<"m_ef_GOODhit["<<i<<"]="<<m_ef_GOODhit[i]<<endl;
              cout<<"m_ef_Allhit["<<i<<"]="<<m_ef_Allhit[i]<<endl;
              cout<<"efficiency = "<<100.*(double)m_ef_GOODhit[i]/(double)m_ef_Allhit[i]<<"%"<<endl;
              cout<<"**********************************"<<endl;
            }
          }
        }
        // Copy previous data
        precosmichit  =  cosmichit;
        prenoisehit   =  noisehit;
        prenhit       = nhit;
        cosmichit     = 0;
        noisehit      = 0;
        processhit    = 0;
        nhit          = 0;
        for(int jhit=0; jhit<prenhit; jhit++){
          m_prechipArray[jhit]    = m_chipArray[jhit]    ;
          m_prechanArray[jhit]    = m_chanArray[jhit]    ;
          m_preadcArray[jhit]     = m_adcArray[jhit]     ;
          m_preeventArray[jhit]   = m_eventArray[jhit]   ;
          m_premoduleArray[jhit]   = m_moduleArray[jhit]   ;
          m_prebcoArray[jhit]     = m_bcoArray[jhit]     ;
          m_prebco_fullArray[jhit]= m_bco_fullArray[jhit];
        }
        

        //return;
        if(m_NextEventInOneTrigger==1)return;
      }
      
      // ******** Cut situation **********    
      // If you want to cut noisy channel and chip, add `if( -condition you wan to cut- )continue;
      /*
      if(module->at(ins)==6&&chan_id->at(ins)==1&&chip_id->at(ins)==14)continue;
      if(module->at(ins)==6&&chan_id->at(ins)==1&&chip_id->at(ins)==16)continue;
      if(module->at(ins)==5&&chan_id->at(ins)==255-136&&chip_id->at(ins)==19)continue;
       if(module->at(ins)==6&&chan_id->at(ins)==0&&chip_id->at(ins)==14)continue;
      */ 
      if(ampl->at(ins)!=0)continue;
      if(adc->at(ins)<0||adc->at(ins)>7)continue;
      if(chip_id->at(ins)<1||chip_id->at(ins)>26)continue;
      if(chan_id->at(ins)<0||chan_id->at(ins)>127)continue;
      //if(!INTT_event)continue;
      // *********************************


      cout<<"Instance "<<ins<<" | bco:"<<bco->at(ins)<<" bco_full:"<<bco_full->at(ins)<<" chip: "<<chip_id->at(ins)<<" channnel:"<<chan_id->at(ins)<<" module:"<<module->at(ins)<<" adc:"<<adc->at(ins)<<(adc->at(ins)>0?"hit":" ")<<endl;;
      if(ins>10)return;
      m_module[m_NHIT] = module->at(ins);
      m_chan[m_NHIT]   = chan_id->at(ins);
      m_chip[m_NHIT]   = chip_id->at(ins);
      m_adc[m_NHIT]    = adc->at(ins);
      
      m_chipArray[nhit]    = chip_id->at(ins);
      m_chanArray[nhit]    = chan_id->at(ins);
      m_adcArray[nhit]     = adc->at(ins);
      m_eventArray[nhit]   = event->at(ins);
      m_moduleArray[nhit]  = module->at(ins);
      m_bcoArray[nhit]     = bco->at(ins);
      m_bco_fullArray[nhit]= bco_full->at(ins);
      
       
       // for data
      if(!m_boolMC)
      {
      //if(module->at(ins)==1)      m_module[m_NHIT] = 0;
       //else if(module->at(ins)==6) m_module[m_NHIT] = 1;
       //else if(module->at(ins)==5) m_module[m_NHIT] = 2;
       //else                        m_module[m_NHIT] = -1;
       //if(module->at(ins)==1)      m_moduleArray[nhit] = 0;
       //else if(module->at(ins)==6) m_moduleArray[nhit] = 1;
       //else if(module->at(ins)==5) m_moduleArray[nhit] = 2;
       //else                        m_moduleArray[nhit] = -1;
      if(module->at(ins)==m_LADDERID[0])      m_module[m_NHIT] = 0;
       else if(module->at(ins)==m_LADDERID[1]) m_module[m_NHIT] = 1;
       else if(module->at(ins)==m_LADDERID[2]) m_module[m_NHIT] = 2;
       else                                    m_module[m_NHIT] = -1;
       if(module->at(ins)==m_LADDERID[0])      m_moduleArray[nhit] = 0;
       else if(module->at(ins)==m_LADDERID[1]) m_moduleArray[nhit] = 1;
       else if(module->at(ins)==m_LADDERID[2]) m_moduleArray[nhit] = 2;
       else                                    m_moduleArray[nhit] = -1;
       }
      // for MC
      if(m_boolMC)
      {  
      //if(module->at(ins)==5)      m_module[m_NHIT] = 0;
       //else if(module->at(ins)==8) m_module[m_NHIT] = 1;
       //else if(module->at(ins)==2) m_module[m_NHIT] = 2;
       //else                        m_module[m_NHIT] = -1;
       //if(module->at(ins)==5)      m_moduleArray[nhit] = 0;
       //else if(module->at(ins)==8) m_moduleArray[nhit] = 1;
       //else if(module->at(ins)==2) m_moduleArray[nhit] = 2;
       //else                        m_moduleArray[nhit] = -1;
      if(module->at(ins)==m_LADDERID[0])      m_module[m_NHIT] = 0;
       else if(module->at(ins)==m_LADDERID[1]) m_module[m_NHIT] = 1;
       else if(module->at(ins)==m_LADDERID[2]) m_module[m_NHIT] = 2;
       else                                    m_module[m_NHIT] = -1;
       if(module->at(ins)==m_LADDERID[0])      m_moduleArray[nhit] = 0;
       else if(module->at(ins)==m_LADDERID[1]) m_moduleArray[nhit] = 1;
       else if(module->at(ins)==m_LADDERID[2]) m_moduleArray[nhit] = 2;
       else                                    m_moduleArray[nhit] = -1;
      }

      m_hitmap      ->Fill(m_module[m_NHIT], m_chan[m_NHIT], m_chip[m_NHIT]);
      m_hitLaddermap->Fill(m_module[m_NHIT], m_chan[m_NHIT], m_adc[m_NHIT]+1);
      
      for(int j=0;j<3;j++){
        if(m_module[m_NHIT]==j)m_Nhitmodule[j]=1;
      }

       if(m_module[m_NHIT]>=0) {
         if(chip_id->at(ins)>=14)m_hitmap_perLadder[(int)m_module[m_NHIT]] -> Fill(m_chip[m_NHIT]-13, 255 - chan_id->at(ins), m_adc[m_NHIT]+1);
         else                    m_hitmap_perLadder[(int)m_module[m_NHIT]] -> Fill(m_chip[m_NHIT]   , m_chan[m_NHIT], m_adc[m_NHIT]+1);
       }
      m_prebco     = bco->at(ins);
      m_prebcofull = bco_full->at(ins);
      m_NHIT++;
      nhit++;
    }
  }
}

void readtree::initHist()
{
  c1 = new TCanvas("c1","hitmap",900,500);
  c1->Divide(2,1);
  c1->cd(2)->Divide(1,4);
  string stmp1;
  for(int i=0;i<4;i++){
    if(i == 0) stmp1 = Form("channel vs. chip MODULE1");
    if(i == 1) stmp1 = Form("channel vs. chip MODULE8");
    if(i == 2) stmp1 = Form("channel vs. chip MODULE6");
    if(i == 3) stmp1 = Form("No ladder");
    m_hitmap_perLadder[i] = new TH2F(Form("hitmap[%d]",i),stmp1.c_str(), 13, 0, 13, 255, 0, 255);
    m_hitmap_perLadder[i]->GetXaxis()->SetTitle("chip_id");
    m_hitmap_perLadder[i]->GetYaxis()->SetTitle("channel_id");
    m_hitmap_perLadder[i]->SetMaximum(8);
  }

  m_hitmap = new TH2F("hitmap", "hitmap", 5, -1, 4, 285, -30, 255);
  m_hitmap->GetXaxis()->SetTitle("ladder position");
  m_hitmap->GetYaxis()->SetTitle("channel_id");
  m_hitmap->SetMaximum(26);
  
  m_hitLaddermap = new TH2F("hitLaddermap", "hitLaddermap", 5, -1, 4, 285, -30, 255);
  m_hitLaddermap->GetXaxis()->SetTitle("ladder position");
  m_hitLaddermap->GetYaxis()->SetTitle("channel_id");
  m_hitLaddermap->SetMaximum(8);
  m_hitLaddermap->SetMarkerSize(2.5);
  
  m_noise = new TH2F("hitnoise", "hitnoise", 26, 0, 26, 285, -30, 255);
  m_noise->GetXaxis()->SetTitle("chip_id");
  m_noise->GetYaxis()->SetTitle("chan_id");
  m_noise->SetMaximum(26);

  m_nhit = new TH1F("m_nhit","number of hits per one trigger event", 100, 0., 100.);
  m_nhit->GetXaxis()->SetTitle("n hits");
  for(int i=0;i<4;i++)
  {
    m_residual_chip[i] = new TH1F(Form("m_residual_chip[%d]",i),Form("m_residual_chip[%d]",i), 26, -13., 13.);
    m_residual_chip[i]->GetXaxis()->SetTitle("number of chip");
    m_residual_chan[i] = new TH1F(Form("m_residual_chan[%d]",i),Form("m_residual_chan[%d]",i), 256, -128., 128.);
    m_residual_chan[i]->GetXaxis()->SetTitle("number of channel");
    
    int tmp1,tmp2; 
    if(i==0){
      tmp1 = 1;
      tmp2 = 2;
    }
    if(i==1){
      tmp1 = 0;
      tmp2 = 2;
    }
    if(i==2){
      tmp1 = 0;
      tmp2 = 1;
    }
    m_hcoll_chip[i] = new TH2D(Form("m_hcoll_chip[%d]",i),Form("m_hcoll_chip[%d]",i), 13, 0., 13.,13,0.,13.);
    m_hcoll_chip[i]->GetXaxis()->SetTitle(Form("Ladder%d",tmp1));
    m_hcoll_chip[i]->GetYaxis()->SetTitle(Form("Ladder%d",tmp2));
    m_hcoll_chan[i] = new TH2D(Form("m_hcoll_chan[%d]",i),Form("m_hcoll_chan[%d]",i), 255, 0., 255.,255,0.,255.);
    m_hcoll_chan[i]->GetXaxis()->SetTitle(Form("Ladder%d",tmp1));
    m_hcoll_chan[i]->GetYaxis()->SetTitle(Form("Ladder%d",tmp2));

  }
}

void readtree::next_continue()
{
  //TFile* fout = new TFile(FILLAD_OUT.c_str(),"recreate"); //Open outout file
  //TFile* fout = new TFile("OUT_BeamData_20211208-1648_0.root","recreate"); //Open outout file
  for(int i=0;i<(fChain->GetEntriesFast());i++)
  {
    //if(1000%i==0)cout<<"Entry:"<<i<<endl;
    next();
    //c1->Write(Form("bcofull%d",m_prebcofull));
  }
  c2 = new TCanvas("c2","residualmap",900,500);
  c2->Divide(3,4);
  c2->cd();
  for(int i=0;i<3;i++)
  {
    c2->cd(i+1);
    m_residual_chip[i]->Draw();
    c2->cd(i+4);
    m_residual_chan[i]->Draw();
    TF1* f_gaus = new TF1("f_gaus","gaus",-127,127);
    m_residual_chan[i]->Fit("f_gaus","","",-10,10);
    //cout<<"f_gaus->GetParameter(2)"<<f_gaus->GetParameter(2)<<endl;
    //cout<<"f_gaus->GetMean()"<<f_gaus->GetMean()<<endl;


    cout<<i<<" m_residual_chan "<<m_residual_chan[i]->GetXaxis()->GetBinCenter(m_residual_chan[i]->GetMaximumBin())<<endl;;
    c2->cd(i+7);
    m_hcoll_chip[i]->Draw("colz");
    c2->cd(i+10);
    m_hcoll_chan[i]->Draw("colz");
  
  }
  //c2->cd(i+7);
  //m_nhit->Draw();
  //fout->Close();
}
void readtree::next()
{
  cout<<" ********************************next*********************************"<<endl;
  m_NextEventInOneTrigger = false;
  for(int i=0;i<3;i++){m_Nhitmodule[i] = 0;}
  Loop(m_ent);
  m_ent++;
  
  c1->cd(1);
  gStyle->SetPalette(kVisibleSpectrum);
  if(m_NEXTPERBCO)m_hitLaddermap->Draw("colz");

  double CHIP[100], CHAN[100], MODULE[100], ADC[100];
  int HIT = 0;

  for(int i=0;i<m_NHIT;i++){
    MODULE[i] = m_module[i];
    CHIP[i] = m_chip[i];
    CHAN[i] = m_chan[i];
    ADC[i] = m_adc[i];
    HIT++;
  }
  if(HIT==0)return;
  double err[HIT];
  for(int I=0;I<HIT;I++)err[I] = 1./(78./sqrt(12.));
  TGraphErrors* m_hitLaddermap_plot = new TGraphErrors(HIT, m_module, m_chan,0,err);
  m_hitLaddermap_plot->GetXaxis()->SetTitle("ladder position");
  m_hitLaddermap_plot->GetYaxis()->SetTitle("channel_id");
  m_hitLaddermap_plot->SetMarkerStyle(4);
  
  if(m_NEXTPERBCO)
  {
    m_hitLaddermap_plot->Draw("P");
    if(HIT>1)m_hitLaddermap_plot->Fit("pol1","Q","",0.,3.);
    c1->Update();
    TPaveStats *st1 = (TPaveStats*)m_hitLaddermap->FindObject("stats");
    st1->SetX1NDC(0.7);
    st1->SetX2NDC(0.9);
    st1->SetY1NDC(0.7);
    st1->SetY2NDC(0.9);
    c1->Modified();
    TLine* line = new TLine(-1, 0,4,0);
    line->SetLineStyle(2);
    t1->SetTextSize(0.03);
    t1->DrawTextNDC(0.6, 0.26, Form("bco_full=%d",m_prebcofull));
    t1->DrawTextNDC(0.6, 0.23, "Z Axis:adc");
    t1->DrawTextNDC(0.6, 0.2, "module=-1 is error");
    for(int i=0;i<4;i++)
    {
      c1->cd(2)->cd(i+1);
      m_hitmap_perLadder[i]->Draw("colz");
      c1->Update();
      TPaveStats *st3 = (TPaveStats*)m_hitmap_perLadder[i]->FindObject("stats");
      st3->SetX1NDC(0.7);
      st3->SetX2NDC(0.9);
      st3->SetY1NDC(0.7);
      st3->SetY2NDC(0.9);
      c1->Modified();
      t1->SetTextSize(0.1);
      t1->DrawTextNDC(0.13, 0.33, Form("bco_full=%d",m_prebcofull));
      t1->DrawTextNDC(0.13, 0.23, "Z Axis:adc");
      string stmp1;
      if(i == 0) stmp1 = Form("module %d",m_LADDERID[0]);
      if(i == 1) stmp1 = Form("module %d",m_LADDERID[1]);
      if(i == 2) stmp1 = Form("module %d",m_LADDERID[2]);
      if(i == 3) stmp1 = Form("No ladder");
      t1->DrawTextNDC(0.13, 0.13, stmp1.c_str());
    }
  }
  //c1->Write(Form("bcofull%d",m_prebcofull));
  //c2->Write();

}
