// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// std libraries
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

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

// Fun4All libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

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
#include <ffarawobjects/InttRawHitContainer.h>

#include <intt/InttMapping.h>

#include "InttQaCommon.h"
#include "TH2INTT.h"

class PHCompositeNode;

class InttTrkrHitQA : public SubsysReco
{
private:

  // general variables
  int run_num_ = 0;
  
  // variables for the output
  std::string output_dir_ = "/sphenix/tg/tg01/commissioning/INTT/QA/trkr_hit/";
  std::string output_basename_ = "InttTrkrHitQA_run";
  std::string output_root_ = "";
  std::string output_pdf_ = "";
  std::string output_txt_ = "";
  TFile* tf_output_;
  
  // objects to be output

  // mother 3D hist
  TH3I* hist_fee_chip_chan_[ InttQa::kFelix_num ];

  // 2D hists
  //TH2I* hist_hitmap_[ InttQa::kFelix_num ][ InttQa::kFee_num ];
  TProfile2D* hist_hitmap_[ InttQa::kFelix_num ][ InttQa::kFee_num ];
  
  // a simple 1D hists
  TH1I* hist_nhit_; // the number of INTTTRKRHIT
  TH1I* hist_pid_; // the number of hits for each FELIX server

  // TH1I* hist_fee_;
  // TH1I* hist_chip_;
  // TH1I* hist_chan_;
  TH1I* hist_adc_;
  TH1I* hist_bco_; // FPHX BCO
  TH1D* hist_bco_full_; // BCO full
  
  // nodes
  InttEventInfo*          node_intteventheader_map_;
  //  InttRawHitContainer*    node_inttrawhit_map_;
  // TRKR_HITSET (IO,TrkrHitSetContainerv1)
  TrkrHitSetContainer* node_trkrhitset_map_;
  
  // functions
  void GetHits( TrkrHitSetContainer::ConstRange hitsets ); //! Draw hits and save them into a PDF file
  void DrawHists();
  void InitPaths();
  void ProcessHists(); //! Some processes for hits, like making 1D and 2D hists from 3D hists, are done
  
  int GetNodes(PHCompositeNode *topNode);
  
  std::vector < std::string > printed_contents_;
  void PrintLine( std::string head, std::string contents, std::string separator = ":" );
  void DumpPrintBuffer();

public:

  InttTrkrHitQA(const std::string &name = "InttTrkrHitQA");

  ~InttTrkrHitQA() override;

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

  void SetOutputDir( std::string dir );
};
