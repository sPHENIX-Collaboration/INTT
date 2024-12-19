//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Nov 23 01:L:33 2024 by ROOT version 6.26/06
// from TTree tree/sPHENIX info.
// found on file: ana439_Electron_2GeV_00150.root
//////////////////////////////////////////////////////////

#ifndef InttSeedTracking_h
#define InttSeedTracking_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <TMath.h>
#include "Fit/Fitter.h"
#include <Math/Functor.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>

#include "TArc.h"



#include "SPHTracKuma.h"


// Header file for the classes stored in the TTree if any.
#include <vector>

class InttSeedTracking {
public :
   InttSeedTracking(std::vector<tracKuma>& tracks,\
      std::vector<hitStruct > vFMvtxHits,\
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
      std::vector<hitStruct > vEmcalHits,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);
   InttSeedTracking(){};
   virtual ~InttSeedTracking();

   void HitMatching(std::vector<tracKuma>& tracks, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits,\
   std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
   std::vector<hitStruct > vEmcalHits,\
   std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);

   Double_t AddHCalE(Double_t emcalPhi, Double_t emcalE,\
      std::vector<hitStruct > vIHCalHits, std::vector<hitStruct > vOHcalHits);

   void AddMvtxHits(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);


   void TrackPropertiesEstimation(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);
   Double_t TrackPtEstimation(tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
   std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);


   void RoughEstiSagittaCenter3Point(Double_t& sagittaR, \
      Double_t& sagittaCenterX, Double_t& sagittaCenterY, Double_t HitsXY[3][2]);

   Double_t AccuratePtEstimation(Double_t sagittaR, Double_t centerX, Double_t centerY, tracKuma& trk, std::vector<hitStruct > vFMvtxHits,\
      std::vector<hitStruct > vSMvtxHits, std::vector<hitStruct > vTMvtxHits);

   Int_t TempINTTIOMatching(Double_t oINTTPhi, Double_t refPhi, std::vector<hitStruct > vIInttHits); //????
   Double_t TempCalcdPhidR(Int_t iInttID, Int_t oInttID, Double_t refPhi,\
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits); //????
   Int_t TempInttCalMatch(Int_t iInttID, Int_t oInttID, Double_t refPhi,\
      std::vector<hitStruct > vIInttHits, std::vector<hitStruct > vOInttHits,\
      std::vector<hitStruct > vEmcalHits); //????

   void SagittaRByCircleFit(Double_t& cernterX, Double_t& centerY, Double_t& sagittaR,
      std::vector<Double_t > r, std::vector<Double_t > phi , Double_t oInttPhi, Double_t emcalPhi);

   bool MatchingMvtxHits(Int_t& mvtxMatchId, Double_t& mvtxClusX, Double_t& mvtxClusY,\
      Double_t sagittaR, Double_t centerX, Double_t centerY, std::vector<hitStruct > vMvtxHits);


   Double_t EstimateRecoTheta(tracKuma trk, Int_t type);
   void EstiVertex(Double_t& vX, Double_t& vY, Double_t sagittaR, Double_t centerX, Double_t centerY);
   
   bool ReturnHitsRPhiVect(std::vector<Double_t >& hitR, std::vector<Double_t >& hitPhi,\
   std::vector<Int_t > subDetSet, tracKuma trk);


   // == s == Simple math equations  ##############################################

   inline Double_t CalcSagittaPt(Double_t sagittaR){
      Double_t pT = 0.3*1.4*sagittaR*0.01; //0.3 * 1.4 T *(R [cm -> m])
      return pT;
   }

   inline Double_t dPhiOInttEmcalEsti(tracKuma trk){
      Double_t xIIntt = trk.getHitR(4)*std::cos(trk.getHitPhi(4));
      Double_t yIIntt = trk.getHitR(4)*std::sin(trk.getHitPhi(4));
      Double_t xOIntt = trk.getHitR(5)*std::cos(trk.getHitPhi(5));
      Double_t yOIntt = trk.getHitR(5)*std::sin(trk.getHitPhi(5));
      Double_t xEmcal = trk.getHitR(6)*std::cos(trk.getHitPhi(6));
      Double_t yEmcal = trk.getHitR(6)*std::sin(trk.getHitPhi(6));

      Double_t phiIInttOIntt = std::atan((yOIntt-yIIntt)/(xOIntt-xIIntt));
      Double_t phiOInttEmcal = std::atan((yEmcal-yOIntt)/(xEmcal-xOIntt));

      Double_t dPhiOInttEmcal = phiOInttEmcal - phiIInttOIntt;
      
      return dPhiOInttEmcal;
   }

   inline Double_t FitFunctionPt(Double_t dPhi){
      Double_t pT = -0.00192479 -0.259884/dPhi  -0.0853628/(dPhi*dPhi);
      return pT;
   }

   inline void Set3PointsXY(Double_t (&HitXY)[3][2], tracKuma trk, Int_t type){
      Double_t iInttR = trk.getHitR(4);
      Double_t iInttPhi = trk.getHitPhi(4);

      Double_t oInttR = trk.getHitR(5);
      Double_t oInttPhi = trk.getHitPhi(5);
      
      Double_t emcalR = trk.getHitR(6);
      Double_t emcalPhi = trk.getHitPhi(6);

      if(type==0){
         HitXY[0][0] = 0.;
         HitXY[0][1] = 0.;
         HitXY[1][0] = iInttR*cos(iInttPhi);
         HitXY[1][1] = iInttR*sin(iInttPhi);
         HitXY[2][0] = oInttR*cos(oInttPhi);
         HitXY[2][1] = oInttR*sin(oInttPhi);

      }else if(type==1){
         HitXY[0][0] = iInttR*cos(iInttPhi);
         HitXY[0][1] = iInttR*sin(iInttPhi);
         HitXY[1][0] = oInttR*cos(oInttPhi);
         HitXY[1][1] = oInttR*sin(oInttPhi);
         HitXY[2][0] = m_ECalR*cos(emcalPhi);
         HitXY[2][1] = m_ECalR*sin(emcalPhi);
      }else if(type==2){
         HitXY[0][0] = 0.;
         HitXY[0][1] = 0.;
         HitXY[1][0] = oInttR*cos(oInttPhi);
         HitXY[1][1] = oInttR*sin(oInttPhi);
         HitXY[2][0] = m_ECalR*cos(emcalPhi);
         HitXY[2][1] = m_ECalR*sin(emcalPhi);
      }

   }

   inline void CalcPerpBis(Double_t& slope, Double_t& sector, 
      Double_t HitXY1[2], Double_t HitXY2[2]){

      Double_t tempSlope = (HitXY2[1] - HitXY1[1])/(HitXY2[0] - HitXY1[0]);
      Double_t tempSector = HitXY1[1] - tempSlope*HitXY1[0];

      Double_t centerX = (HitXY2[0] + HitXY1[0])/2;
      Double_t centerY = (HitXY2[1] + HitXY1[1])/2;

      slope = -1/tempSlope;
      sector = centerY - slope*centerX;

      return;
   }

   inline void CalcLineEq(Double_t& slope, Double_t& section,\
      Double_t x1, Double_t y1, Double_t x2, Double_t y2){
      slope = (y2 - y1)/(x2 - x1);
      section = y1 - slope*x1;
   }

   inline void CalcCrossPoint(Double_t& crossX, Double_t& crossY,\
      Double_t origSlope, Double_t origSection, Double_t nextPointX, Double_t nextPointY){
      Double_t invSlope = - (1/origSlope);      
      Double_t invSection = nextPointY - invSlope*nextPointX;
      crossX = - (invSection - origSection)/(invSlope - origSlope);
      crossY = origSlope*crossX + origSection;
   }

   inline Double_t CalcSection(Double_t x, Double_t y, Double_t slope){
      Double_t section = y - slope*x;
      return section;
   }

   inline void CrossLineCircle(Double_t& xv, Double_t& yv,\
      Double_t xc, Double_t yc, Double_t R){
      // line (0,0) -> (xc, yc): y = slope*x
      // circle: (y-yc)^2 = R^2 - (x-xc)^2
      // x^2 + 2bx + c = 0
      // -> x = -b pm sqrt(b^2 - c)
      Double_t slope = yc/xc;

      Double_t b = (xc - slope*yc)/(slope*slope + 1);
      Double_t c = (yc*yc - R*R + xc*xc)/(slope*slope + 1);

      Double_t xv1 = -b + std::sqrt(b*b - c);
      Double_t xv2 = -b - std::sqrt(b*b - c);
      Double_t yv1 = slope*xv1;
      Double_t yv2 = slope*xv2;

      if((xv1*xv1+yv1*yv1) < (xv2*xv2+yv2*yv2)) xv = xv1;
      else xv = xv2;

      yv = slope*xv;

      return;
   }
   // == e == Simple math equations  ##############################################

private:
   Double_t m_ECalR = 93.5; //surface
   // const Double_t m_ECalR = 106.; // center 106
   Double_t m_iHCalR = 127.502;
   Double_t m_oHCalR = 225.87;

   Double_t m_InttMatchPhiMin = -0.05;
   Double_t m_InttMatchPhiMax = 0.05;

   Double_t m_EcalEThre = 0.1; // GeV
   Double_t m_iHcalEThre = 5; // GeV
   Double_t m_oHcalEThre = 5; // GeV

   Double_t m_EcalSearchPhiMin = 0.;
   Double_t m_EcalSearchPhiMax = 0.;

   std::vector<hitStruct > m_fMvtxHits;
   std::vector<hitStruct > m_sMvtxHits;
   std::vector<hitStruct > m_tMvtxHits;

   std::vector<hitStruct > m_iInttHits;
   std::vector<hitStruct > m_oInttHits;

   std::vector<hitStruct > m_emcalHits;
   std::vector<hitStruct > m_iHCalHits;
   std::vector<hitStruct > m_oHCalHits;

};



#endif








