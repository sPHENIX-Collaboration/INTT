// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// std libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

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

#include <trackbase/InttDefs.h>
#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSetContainerv1.h>
//#include <trackbase/TrkrHitSetContainerv2.h>
#include <trackbase/TrkrHitSetv1.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitv1.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrDefs.h>

#include <ffaobjects/FlagSavev1.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitv2.h>
#include <ffarawobjects/InttRawHitContainer.h>
#include <ffarawobjects/InttRawHitContainerv2.h>

#include <intt/InttMapping.h>
#include <intt/InttDacMap.h>
#include "InttQaCommon.h"

class PHCompositeNode;

class TrkrHitInfo
{
private:
  InttNameSpace::Online_s hit_;
  uint16_t bco_fphx_;
  int event_counter_;
  int dac_;

public:
  TrkrHitInfo( InttNameSpace::Online_s hit, uint16_t bco_fphx, int event_counter, int dac );

  InttNameSpace::Online_s GetHit(){ return hit_;};
  uint16_t GetFPHXBco(){ return bco_fphx_;};
  int GetEventCounter(){ return event_counter_; };
  int GetDac(){ return dac_;};

  ////////////////////////////////////////////
  // struct Online_s                        //
  //   int lyr, int ldr,  int arm           //
  //   int chp, int chn                     //
  ////////////////////////////////////////////

  ////////////////////////////////////////////
  // struct RawData_s:                      //
  int GetFelixServer	(){ return InttNameSpace::ToRawData(hit_).felix_server; };
  int GetFelixChannel	(){ return InttNameSpace::ToRawData(hit_).felix_channel; };
  int GetChip		(){ return InttNameSpace::ToRawData(hit_).chip; };
  int GetChannel	(){ return InttNameSpace::ToRawData(hit_).channel; };
      
  ////////////////////////////////////////////
  // struct Offline_s                       //
  int Getlayer		(){ return InttNameSpace::ToOffline(hit_).layer; };
  int Getladder_phi	(){ return InttNameSpace::ToOffline(hit_).ladder_phi; };
  int Getladder_z	(){ return InttNameSpace::ToOffline(hit_).ladder_z; };
  int Getstrip_x	(){ return InttNameSpace::ToOffline(hit_).strip_x; };
  int Getstrip_y	(){ return InttNameSpace::ToOffline(hit_).strip_y; };

  void Print();
};

class InttFineDelayScanTrkrHit : public SubsysReco
{
protected:

  ///////////////////////////////////////////
  // general variables
  ///////////////////////////////////////////
  int run_num_ = 0;
  bool is_official_ = true;
  int year_ = 2024;  
  int pid_ref_ = 0;
  bool intt_header_found_ = false;
  int last_event_counter_ = 0;
  
  ///////////////////////////////////////////
  // variables for the output
  ///////////////////////////////////////////
  std::string output_dir_ = "/sphenix/tg/tg01/commissioning/INTT/QA/trkr_hit/";
  std::string output_basename_ = "InttFineDelayScanTrkrHit_run";
  std::string output_root_ = "";
  std::string output_pdf_ = "";
  std::string output_txt_ = "";
  TFile* tf_output_;

  ///////////////////////////////////////////
  // objects to be output
  ///////////////////////////////////////////

  // mother 3D hist
  TH3D* hist_fee_chip_chan_[ InttQa::kFelix_num ]; // ch vs chip vs ladder vs felix

  TH3D* hist_fee_chip_bco_[ InttQa::kFelix_num ]; // BCO vs chip vs ladder vs FELIX
  TH3D* hist_fee_chip_bco_diff_all_; // BCO diff vs chip vs ladder vs felix, sum of all FELIX
  TH3D* hist_fee_chip_bco_diff_[ InttQa::kFelix_num ]; // BCO diff vs chip vs ladder vs FELIX

  TH3D* hist_fee_chip_sidelobe_ratio_even_[ InttQa::kFelix_num ]; // BCOdiff_{odd} / (BCOdiff_{odd} + BCOdiff_{even} )

  TH3D* hist_fee_chip_bco_diff_raw_[ InttQa::kFelix_num ]; // BCO diff (raw) vs chip vs ladder vs FELIX, BCO diff (raw) means the difference but +128 for negavie values is not done.
  TH3D* hist_fee_chip_sidelobe_ratio_raw_even_[ InttQa::kFelix_num ]; // BCOdiff_{odd} / (BCOdiff_{odd} + BCOdiff_{even} ) : BCO diff (raw)

  TH3D* hist_fee_chip_sidelobe_ratio_bco_even_[ InttQa::kFelix_num ]; // BCOdiff_{odd} / (BCOdiff_{odd} + BCOdiff_{even} )

  // sidelobe ratio
  
  // a simple 1D hists
  TH1I* hist_bco_; // FPHX BCO
  TH1D* hist_bco_full_; // BCO full
  TH1D* hist_sidelobe_ratio_bco_even_; // BCOdiff_{odd} / (BCOdiff_{odd} + BCOdiff_{even} ), Sum of all FELIXes
  TH1D* hist_sidelobe_ratio_even_; // BCOdiff_{odd} / (BCOdiff_{odd} + BCOdiff_{even} ), Sum of all FELIXes

  TH1D* hist_sidelobe_ratio_raw_even_; // BCOdiff_{odd} / (BCOdiff_{odd} + BCOdiff_{even} ), Sum of all FELIXes

  ///////////////////////////////////////////
  // nodes
  ///////////////////////////////////////////
  InttEventInfo*          node_intteventheader_map_;
  InttRawHitContainer*    node_inttrawhit_map_;
  TrkrHitSetContainer*    node_trkrhitset_map_;

  ///////////////////////////////////////////
  // functions
  ///////////////////////////////////////////

  void DrawRatioToPad( TCanvas* c,                                    // +-----------_----+---------+  |
                       int pad_num,                                   // |    Ratio | |   |----|----|  |
                       std::string title_ratio,                       // |   ______/  |   |----|----|  |
                       std::vector < TH1D* > hists_ratio,             // +----------------+---------+  |
                       double ymax = -1 );                
  
  void DrawBcoDiffRatioPair( TCanvas* c,                              // +----------------+---------+  |
                             std::string title_bco_diff,              // |   _____| BCO   |----|----|  |
                             std::vector < TH3D* > hists_bco_diff,    // |  |     | diff  |----|----|  |
                             std::string title_ratio,                 // +----------------+---------+  |
                             std::vector < TH1D* > hists_ratio,       // +-----------_----+---------+  |
                             bool inherit_ymax=false );               // |    Ratio | |   |----|----|  |
                                                                      // |   ______/  |   |----|----|  |
                                                                      // +----------------+---------+  |  

  void DrawRatioPair( TCanvas* c,                                     // +-----------_----+---------+  |
                      std::string title_ratio_upper,                  // |    Ratio | |   |----|----|  |
                      std::vector < TH1D* > hists_ratio_upper,        // |   ______/  |   |----|----|  |
                      std::string title_ratio_lower,                  // +----------------+---------+  |
                      std::vector < TH1D* > hists_ratio_lower,        // +-----------_----+---------+  |
                      bool inherit_ymax );                            // |    Ratio | |   |----|----|  |
                                                                      // |   ______/  |   |----|----|  |
                                                                      // +----------------+---------+  |
  
  void DrawHists(); //! Draw hits and save them into a PDF file
  void InitPaths();
  void ProcessHists(); //! Some processes for hits, like making 1D and 2D hists from 3D hists, are done
  
  int GetNodes(PHCompositeNode *topNode);
  std::vector < InttRawHit* > GetHits();

  std::vector < InttRawHit* > GetRawHits();
  std::vector < InttRawHit* > GetRawHitsWithoutClone();
  std::vector < std::pair < InttNameSpace::Online_s,  unsigned int > > GetTrkrHits( TrkrHitSetContainer::ConstRange hitsets );
  std::vector < std::pair < uint16_t, int > > GetBcoEventCounter();

  std::vector < TrkrHitInfo* > GetHitInfo();
  
  bool IsSame( InttRawHit* hit1, InttRawHit* hit2 );
  
  ///////////////////////////////////////////
  // misc
  ///////////////////////////////////////////

  //int process_event_clone_hit(  PHCompositeNode* topNode );
  
  std::vector < std::string > printed_contents_;
  void PrintLine( std::string head, std::string contents, std::string separator = ":" );
  void DumpPrintBuffer();

public:

  InttFineDelayScanTrkrHit(const std::string &name = "InttFineDelayScanTrkrHit", bool is_official=true );

  ~InttFineDelayScanTrkrHit() override;

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
