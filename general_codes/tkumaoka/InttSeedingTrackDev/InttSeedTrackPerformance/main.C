

#include "src/InttSeedTrackPerformance.h"
#include "src/InttSeedTrackPerformance.cxx"

#include "src/InttSeedTracking.h"
#include "src/InttSeedTracking.cxx"
#include "src/SPHTracKuma.h"

#include <typeinfo>

int main(int argc, char* argv[]){
  TChain *tc = new TChain("tree");

  std::string fDir = "./";
  // std::string fInputName = fDir + "singleE1GeVMg14.root";
  // std::string fInputName = fDir + "input/singleE500to1500MeVEta0.root"; // 05, 1, 2, 5, 8, 10
  std::string fInputName = fDir + "input/singleE0to10MeVEtaWide.root"; // 05, 1, 2, 5, 8, 10
  // std::string fInputName = fDir + "input/singleE05GeV.root"; // 05, 1, 2, 5, 8, 10

  tc->Add(fInputName.c_str()); 
  // std::string fOutputName = fDir+"trackingWInttCal1GeVMg14";
  // std::string fOutputName = fDir+"trackingWInttCal1GeVModifEcal";
  // std::string fOutputName = fDir+"/output/singleE05GeVTracking/trackingWInttCalEtaFull"; // 05, 1, 2, 5, 10
  // std::string fOutputName = fDir+"/output/trackingWInttCalClu10GeV_v2"; // 05, 1, 2, 5, 10
  std::string fOutputName = fDir+"/output/trackingWInttCalClu0to10MeVEtaWide_v2"; // 05, 1, 2, 5, 10
  // std::string fOutputName = fDir+"/t";

  TTree *tt = (TTree*)tc;

  Int_t runNum = atoi(argv[1]);
  InttSeedTrackPerformance *h = new InttSeedTrackPerformance(tt, fInputName, fOutputName, runNum);

  h->Loop(runNum);

  return 1;
}



