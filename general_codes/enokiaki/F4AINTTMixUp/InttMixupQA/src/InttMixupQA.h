#ifndef INTTMIXUPQA_H__
#define INTTMIXUPQA_H__

// std headers
#include <vector>
#include <filesystem>
#include <array>
#include <iostream>
#include <iomanip> // setw, setfill

#include <iostream>
#include <vector>
#include <string>

// ROOT headers
#include <TObject.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <TLine.h>
#include <TLegend.h>

// Fun4All headers
#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>


class PHCompositeNode;

class InttMixupQA : public SubsysReco {


 public:
  InttMixupQA(const std::string &name = "InttMixupQA", const int run_num=0);

  virtual ~InttMixupQA();

  int Init(PHCompositeNode *);
  
  int InitRun(PHCompositeNode *);
  
  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int EndRun(PHCompositeNode *);

  int End(PHCompositeNode *);

  int SetOutputDir( std::string dir );
  
  
 private:

  // general variables
  int run_num_ = 0;
  static const int kFelix_num_ = 8; // the number of our FELIX server
  static const int kFee_num_ = 14;  // the number of half-ladders in a single FELIX server
  static const int kChip_num_ = 26; // the number of chip in a half-ladder
  static const int kChan_num_ = 128; // the number of channel in a single chip
  static const int kFirst_pid_ = 3001; // the first pid (packet ID), which means intt0

  // variables for the output
  std::string output_dir_ = "./";
  std::string output_basename_ = "InttMixupEventQA_run";
  std::string output_root_ = "tmp.root";
  std::string output_pdf_ = "tmp.pdf";
  TFile* tf_output_;

  int ievent_ = 0;
  
  TH1F *h_allmulti_[kFelix_num_];
  TH1F *h_allclone_[kFelix_num_];

  void DrawHists();

  void FindMixupEvent();
};
#endif
