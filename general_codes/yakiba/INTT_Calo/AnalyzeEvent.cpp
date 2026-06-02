#include <TFile.h>
#include <TH1F.h>
#include "INTT_Calo_Ana.h"
#include "AnalyzeEvent.h"
#include "INTT_Calo_trkReco.h"

void AnalyzeInit(void) {

  hzgvtx = new TH1F("hzgvtx","global Zvtx",100,-50,50);
  hINTTdphi = new TH1F("hINTTdphi","INTT1phi - INTT0phi",100,-0.1,0.1);
}

void AnalyzeEnd(void) {
  hzgvtx->Draw();
  hINTTdphi->Draw();

  TFile *hfile = new TFile("INTT_Calo_Ana.root","RECREATE");
  hzgvtx->Write();
  hINTTdphi->Write();
  hfile->Close();
}

void AnalyzeEvent(void) {
  hzgvtx->Fill(zgvtx);

  LoadTrackerData();
  FindInttPair();
  FindEmcIntt();
  FindInttMvtx();
  FindCaloInttMvtx();
}

