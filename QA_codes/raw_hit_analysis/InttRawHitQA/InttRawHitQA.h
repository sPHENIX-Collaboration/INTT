// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTRAWHITQA_H
#define INTTRAWHITQA_H

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

#include <trackbase/InttEventInfo.h>
#include <trackbase/InttEventInfov1.h>

#include <ffaobjects/FlagSavev1.h>
#include <ffarawobjects/InttRawHit.h>
#include <ffarawobjects/InttRawHitContainer.h>

class PHCompositeNode;

class InttRawHitQA : public SubsysReco
{
private:

  // general variables
  int run_num_ = 0;
  static const int kFelix_num_ = 8; // the number of our FELIX server
  static const int kFee_num_ = 14;  // the number of half-ladders in a single FELIX server
  static const int kChip_num_ = 26; // the number of chip in a half-ladder
  static const int kChan_num_ = 128; // the number of channel in a single chip
  static const int kFirst_pid_ = 3001; // the first pid (packet ID), which means intt0
  
  // variables for the output
  std::string output_dir_ = "/sphenix/tg/tg01/commissioning/INTT/QA/raw_hit/";
  std::string output_basename_ = "InttRawHitQA_run";
  std::string output_root_ = "";
  std::string output_pdf_ = "";
  std::string output_txt_ = "";
  TFile* tf_output_;
  
  // objects to be output

  // mother 3D hist
  TH3I* hist_fee_chip_chan_[ kFelix_num_ ];

  // 2D hists
  //TH2I* hist_hitmap_[ kFelix_num_ ][ kFee_num_ ];
  TProfile2D* hist_hitmap_[ kFelix_num_ ][ kFee_num_ ];
  
  // a simple 1D hists
  TH1I* hist_nhit_; // the number of INTTRAWHIT
  TH1I* hist_pid_; // the number of hits for each FELIX server

  // TH1I* hist_fee_;
  // TH1I* hist_chip_;
  // TH1I* hist_chan_;
  TH1I* hist_adc_;
  TH1I* hist_bco_; // FPHX BCO
  TH1D* hist_bco_full_; // BCO full
  
  // nodes
  InttEventInfo*          node_intteventheader_map_;
  InttRawHitContainer*    node_inttrawhit_map_;

  // functions
  void DrawHists(); //! Draw hits and save them into a PDF file
  void InitPaths();
  void ProcessHists(); //! Some processes for hits, like making 1D and 2D hists from 3D hists, are done
  
  int GetNodes(PHCompositeNode *topNode);


  // misc
  template < typename TH >
  TPaletteAxis* DrawPaletteAxis( TH* hist,
				 double xmin, double ymin,
				 double xmax, double ymax,
				 double label_size = 0.04 )
    
  {
    
    gPad->Update();
    TPaletteAxis *pal = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
    pal->GetAxis()->SetLabelSize( label_size );
    pal->GetAxis()->CenterTitle();
    
    pal->SetX1NDC( xmin );
    pal->SetX2NDC( xmax );
    
    pal->SetY1NDC( ymin );
    pal->SetY2NDC( ymax );
    pal->Draw();
    
    return pal;
  }
  
  template < typename TH >
  //void DrawStats( TH* hist, double xmin, double ymin, double xmax, double ymax )
  void DrawStats( TH* hist, double xmin, double ymin, double xmax, double ymax, int font = 4)
  {
    
    gPad->Update();
    TPaveStats *st = (TPaveStats*)hist->FindObject("stats");
    if( st == nullptr )
      return;
    
    st->SetTextColorAlpha( hist->GetLineColor(), 1.0 );
    st->SetLineColorAlpha( hist->GetLineColor(), 1.0 );
    st->SetFillStyle( 1001 );
    st->SetFillColor( 0 );
    
    st->SetX1NDC( xmin );
    st->SetX2NDC( xmax );
    st->SetY1NDC( ymin );
    st->SetY2NDC( ymax );
    
    st->Draw("same");
  }
  
  std::vector < std::string > printed_contents_;
  void PrintLine( std::string head, std::string contents, std::string separator = ":" );
  void DumpPrintBuffer();

public:

  InttRawHitQA(const std::string &name = "InttRawHitQA");

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

  void SetOutputDir( std::string dir );
};

#endif // INTTRAWHITQA_H
