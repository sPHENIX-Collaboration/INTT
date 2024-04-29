// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INTTRAWHITQA_H
#define INTTRAWHITQA_H

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

class PHCompositeNode;

class InttRawHitQA : public SubsysReco
{
private:

  // general variables
  int run_num_ = 0;
  int year_ = 2024;
  static const int kFelix_num_ = 8; // the number of our FELIX server
  static const int kFee_num_ = 14;  // the number of half-ladders in a single FELIX server
  static const int kChip_num_ = 26; // the number of chip in a half-ladder
  static const int kChan_num_ = 128; // the number of channel in a single chip
  static const int kFirst_pid_ = 3001; // the first pid (packet ID), which means intt0

  int pid_ref_ = 0;
  
  // variables for the output
  std::string output_dir_ = "/sphenix/tg/tg01/commissioning/INTT/QA/raw_hit/";
  std::string output_basename_ = "InttRawHitQA_run";
  std::string output_root_ = "";
  std::string output_pdf_ = "";
  std::string output_txt_ = "";
  TFile* tf_output_;

  int colors[10] = {
    kBlack, 
    kRed, 
    kBlue, 
    kGreen+2, 
    kMagenta+1, 
    kYellow+1, 
    kCyan+1, 
    kOrange+1, 
    kBlue+9, 
    kGray + 2
  };

  // objects to be output

  // mother 3D hist
  TH3I* hist_fee_chip_chan_[ kFelix_num_ ]; // ch vs chip vs ladder vs felix
  TH3I* hist_fee_bco_full_event_counter_[ kFelix_num_ ]; // event counter vs bco full vs ladder vs felix
  TH3I* hist_fee_bco_full_event_counter_diff_[ kFelix_num_ ]; // difference of event counter vs difference of bco full vs ladder vs felix, difference means ( val - Min( val(felix=Min(felix) ) ) )
  
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

  // felix vs event counter
  TH1D* hist_event_counter_[ kFelix_num_];
  TH1D* hist_event_counter_diff_[ kFelix_num_];

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

  template < class TH >
  void HistConfig( TH* hist, int index=0 )
  {
    hist->SetLineColor( this->GetColor(index) );
    hist->SetFillColorAlpha( hist->GetLineColor(), 0.1 );
  }
  
  template < class TH >
  void HistsConfig( int hist_num, TH* hists )
  {

    std::vector < int > bin_x_with_entry;
    std::vector < int > bin_y_contents;
    for( int i=0; i<hist_num; i++ )
      {

	for( int j=1; j<hists[i]->GetNbinsX()+1; j++ )
	  {
	    if( hists[i]->GetBinContent( j ) != 0 )
	      {
		bin_x_with_entry.push_back( j );
		bin_y_contents.push_back( hists[i]->GetBinContent( j ) );
	      }
	  }
      }

    int min_non_zero_x = *std::min_element( bin_x_with_entry.begin(), bin_x_with_entry.end() );
    if( min_non_zero_x > 1 )
      min_non_zero_x--;
    
    int max_non_zero_x = *std::max_element( bin_x_with_entry.begin(), bin_x_with_entry.end() );
    if( max_non_zero_x < hists[0]->GetNbinsX() )
      max_non_zero_x++;
    
    hists[0]->GetXaxis()->SetRange( min_non_zero_x, max_non_zero_x );

    int min_y = *std::min_element( bin_y_contents.begin(), bin_y_contents.end() );
    int max_y = *std::max_element( bin_y_contents.begin(), bin_y_contents.end() );
    if( gPad->GetLogy() == 0 ) // linear
      max_y *= 1.2;
    else
      max_y *= 2;
      
    hists[0]->GetYaxis()->SetRangeUser( min_y, max_y );
    for( int i=0; i<hist_num; i++ )
      {
	this->HistConfig( hists[i], i );
      }
    
    std::cout << "X range: " << min_non_zero_x << "\t" << max_non_zero_x << std::endl;
    std::cout << "Y range: " << min_y << "\t" << max_y << std::endl;
  }
  
  int GetColor( int num );  
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

  void SetOutputDir( std::string dir = "");
  void SetYear( int year ){ year_ = year;};
  
};

#endif // INTTRAWHITQA_H
