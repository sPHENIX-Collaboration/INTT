// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// std libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>

// ROOT libraries
#include "TFile.h"
#include "TH3I.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TPaveStats.h"
#include "TPaletteAxis.h"
#include "TLegend.h"
#include "TGraph.h"

// Fun4All libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>

#include <ffaobjects/FlagSavev1.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitv2.h>
#include <ffarawobjects/InttRawHitContainer.h>
#include <ffarawobjects/InttRawHitContainerv2.h>

#include "InttQaCommon.h"
#include "TH2INTT.h"

class PHCompositeNode;

class InttRawHitQA : public SubsysReco
{
protected:

  ///////////////////////////////////////////
  // general variables
  ///////////////////////////////////////////
  static const int kFelix_num_ = 8; // the number of our FELIX server
  static const int kFee_num_ = 14;  // the number of half-ladders in a single FELIX server
  static const int kChip_num_ = 26; // the number of chip in a half-ladder
  static const int kChan_num_ = 128; // the number of channel in a single chip
  static const int kFirst_pid_ = 3001; // the first pid (packet ID), which means intt0

  int run_num_			= 0;
  int year_			= 2024;  
  int pid_ref_			= 0;

  int previous_event_counter_	= -1;
  int last_event_counter_	= 0;
  int event_counter_by_myself_  = 0; // because the event counter is not reliable, I count it by myself for histogram normalization
  
  bool is_first_event_		= true;
  bool is_official_		= true;
  bool intt_header_found_	= false;

  
  ///////////////////////////////////////////
  // variables for the output
  ///////////////////////////////////////////
  std::string output_dir_ = "/sphenix/tg/tg01/commissioning/INTT/QA/raw_hit/";
  std::string output_basename_ = "InttRawHitQA_run";
  std::string output_root_ = "";
  std::string output_pdf_ = "";
  std::string output_txt_ = "";
  TFile* tf_output_;

  ///////////////////////////////////////////
  // objects to be output
  ///////////////////////////////////////////

  // mother 3D hist
  TH3D* hist_fee_chip_chan_[ kFelix_num_ ]; // ch vs chip vs ladder vs felix
  //TH3D* hist_fee_chip_chan_woclonehit_[ kFelix_num_ ]; // ch vs chip vs ladder vs felix ; without clonehit
  TH3D* hist_fee_bco_full_event_counter_[ kFelix_num_ ]; // event counter vs bco full vs ladder vs felix
  TH3D* hist_fee_bco_full_event_counter_diff_[ kFelix_num_ ]; // difference of event counter vs difference of bco full vs ladder vs felix, difference means ( val - Min( val(felix=Min(felix) ) ) )

  // 2D hists
  //TH2I* hist_hitmap_[ kFelix_num_ ][ kFee_num_ ];
  TProfile2D* hist_hitmap_[ kFelix_num_ ][ kFee_num_ ];
  
  // a simple 1D hists
  TH1D* hist_nhit_; // the number of INTTRAWHIT
  TH1D* hist_pid_; // the number of hits for each FELIX server
  TH1D* hist_nhit_south_; // the number of INTTRAWHIT
  TH1D* hist_nhit_north_; // the number of INTTRAWHIT

  // TH1D* hist_fee_;
  // TH1D* hist_chip_;
  // TH1D* hist_chan_;
  TH1D* hist_adc_;
  TH1D* hist_bco_; // FPHX BCO
  TH1D* hist_bco_full_; // BCO full

  // felix vs event counter
  TH1D* hist_event_counter_[ kFelix_num_];
  TH1D* hist_event_counter_diff_[ kFelix_num_];

  // TGraph* g_event_hit_num_;
  // TGraph* g_event_hit_num_strange_peak_;
  
  // bool to check the clone hit 
  //bool IsCloneHit_[ kFelix_num_ ][ kFee_num_ ][ kChip_num_ ][ kChan_num_ ]{};

  ///////////////////////////////////////////
  // nodes
  ///////////////////////////////////////////
  InttEventInfo*          node_intteventheader_map_;
  InttRawHitContainer*    node_inttrawhit_map_;

  ///////////////////////////////////////////
  // functions
  ///////////////////////////////////////////
  void DrawHists(); //! Draw hits and save them into a PDF file
  void InitPaths();
  void ProcessHists(); //! Some processes for hits, like making 1D and 2D hists from 3D hists, are done
  void WriteResults2Txt();
  
  int GetNodes(PHCompositeNode *topNode);
  virtual std::vector < InttRawHit* > GetHits();
  
  ///////////////////////////////////////////
  // misc
  ///////////////////////////////////////////
  //int process_event_clone_hit(  PHCompositeNode* topNode );  
  std::vector < std::string > printed_contents_;
  void PrintLine( std::string head, std::string contents, std::string separator = ":" );
  void DumpPrintBuffer();

public:

  InttRawHitQA(const std::string &name = "InttRawHitQA", bool is_official=true );

  ~InttRawHitQA() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  void SetOutputDir( std::string dir = "");
  void SetYear( int year ){ year_ = year;};
  
};
