#pragma once

//#include <iostream>
#include <cdbobjects/CDBTTree.h>
#include "constant_values.hh"
#include "functions.hh"

R__LOAD_LIBRARY(libcdbobjects.so)

// It supposed to be used as a part of ROOT macros
class InttClassifier
{
private:

  /////////////////////////////////////////////////
  // constant values: see constant_values.hh
  
  /////////////////////////////////////////////////
  // general variables
  int run_num_		= 0;
  int year_		= 2024; // used in the directory selection for both input and output
  double sig_cut_	= 3.0;
  int in_sig_		= 0; 
  bool Writecsv_	= false;
  bool is_official_	= true;
  bool isbeam_		= true; // true is beam, false is cosmics
  bool did_init_	= false;
  string fitting_mode_	= "gaus"; // "gaus" and "double_gaus" are available
  bool process_done_	= false; // a flag to know whether ProcessBeam was done or not


  /////////////////////////////////////////////////
  // File I/O
  bool force_suffix_       = false;
  string suffix_           = "_official"; // It's attached to the input/output file. To disable it, do SetFileSuffix("" );
  string output_tag_       = ""; // It's attached b/w basename and the suffix (not file suffix above, the real suffix like .root)
  string hitmap_dir_       = "./";
  std::string hitmap_file_ = ""; // path to the hitmap file

  // for CDB
  std::string output_cdb_dir_  = kIntt_cdb_dir; // path to the output dir of CDB files
  std::string output_cdb_file_ = ""; // path th the file

  // for QA
  std::string output_qa_root_dir_  = kIntt_hotmap_dir; // path to the QA ROOT dir
  std::string output_qa_root_file_ = ""; // path to the QA ROOT file
  std::string output_qa_pdf_dir_   = kIntt_hotmap_dir; // path to the QA pdf dir
  std::string output_qa_pdf_file_  = ""; // path to the QA pdf file
  std::string output_qa_txt_file_  = "";
  
  // TFile I/O
  TFile* tf_hitmap_; // input
  TTree* tt_runinfo_; // input
  
  // TTree I Branch
  TFile* tf_qa_; // output
  int runnumber_;
  uint32_t total_event_;
  uint32_t merged_event_ = 0;

  /////////////////////////////////////////////////
  // QA hists/outputs
  int bin_num_ = 500;
  double xmin_ = 0;
  double xmax_ = 1e-3;
  
  TH1D* h1_hist_MPV_; // #hit distribution for each channel. 0 count is excluded.  
  TH1D* h1_hist_gaus_; // #hit distribution for each channel. 0 count is excluded.
  TH1D* h1_hist_gaus_felix_[8]; // #hit distribution for each channel. 0 count is excluded.
  TH1D* h1_hist_chip_;  
  TH1D* h1_hist_MPV_ladder_;
  int NumberOfFiredChan_[8][14] = { 0 };
  TFitResultPtr DoGaussianFit(TH1D* hist);
  
  TTree* tree_output_;

  // variables for the tree_output_
  double ch_entry_;
  double mean_gaus_;
  double sigma_gaus_;
  int felix_;
  int module_;
  int chip_id_;
  int chan_;
  int type_;
  int flag_;

  /////////////////////////////////////////////////
  //
  TH1D *h1_hist_fit_A_[8][14]; // filled but not used, why?
  TH1D *h1_hist_fit_B_[8][14]; // filled but not used, why?

  TH2D *h2_AllMap_[8][14]; // Original HitMap
  TH2D *h2_ColdMap_[8][14];// 2D histogram for coldmap
  TH2D *h2_HalfMap_[8][14];// 2D histogram for halfmap (half entry chips)
  TH2D *h2_DeadMap_[8][14];// 2D histogram for deadmap
  TH2D *h2_HotMap_[8][14]; // 2D histogram for hotmap

  /////////////////////////////////////////////////
  // Cut variables
  int hot_ch_num_		= 0;
  int hot_ch_num_felix_[8] = {0};
  double hot_ch_cut_		= 0;
  double hot_ladder_cut_	= 0.0;
  double hot_ch_cut_gaus_[8]	= {0.}; 

  /////////////////////////////////////////////////
  // Something else
  struct Half_Chip
  {
    int _felix_id_;
    int _module_id_;
    int _chip_id_;
  };
  
  std::vector<Half_Chip> half_Chips_ = // Chip number 0~25
  {
    // Felix 0
    {0, 7, 14},
    // Felix 2
    // Felix 3
    {3, 13, 20},
    {3, 13, 22},
    {3, 13, 24},
    // Felix 7
    {7, 0, 0},
    {7, 0, 1},
    {7, 0, 2},
    {7, 0, 3},
    {7, 0, 4},
    {7, 0, 6},
    {7, 0, 8},
    {7, 0, 10},
    {7, 0, 12},
    {7, 0, 13},
    {7, 0, 14},
    {7, 0, 15},
    {7, 0, 16},
    {7, 0, 17},
    {7, 0, 19},
    {7, 0, 21},
    {7, 0, 23},
    {7, 0, 25},
    {7, 1, 0},
    {7, 1, 1},
  };
  
  /////////////////////////////////////////////////
  // Init functions
  void Init();
  void InitPaths();
  void InitHists();
  void InitQaTree();

  /////////////////////////////////////////////////
  // misc
  void DrawHists();
  void DrawHist( TCanvas* c, TH1D* hist, double threshold, int hot_ch_num, int total_ch_num );
  void DrawFittedFunction( TH1D* hist, double threshold, int hot_ch_num, int total_ch_num );
  TLine* DrawThreshold( double threshold, bool log_y=false );

  template < typename TH >
  void DrawStats( TH* hist, double xmin, double ymin, double xmax, double ymax, int font = 4)
  {
    
    gPad->Update();
    TPaveStats *st = (TPaveStats*)hist->FindObject("stats");
    if( st == nullptr )
      return;

    st->SetTextColorAlpha( hist->GetLineColor(), 1.0 );
    st->SetLineColorAlpha( hist->GetLineColor(), 1.0 );
    st->SetFillStyle( 1001 );
    //    st->SetFillColor( 0 );
    
    st->SetX1NDC( xmin );
    st->SetX2NDC( xmax );
    st->SetY1NDC( ymin );
    st->SetY2NDC( ymax );
    st->Draw("same");
  }
  
  template <class TH>
  int GetMaxBinWithEntry(TH *hist)
  {
    for (int i = hist->GetNbinsX(); i > 0; i--)
    {
      if (hist->GetBinContent(i) > 0)
      {
        return i;
        break;
      }
    }
    return 1;
  }

  template < class TH >
  void HistConfig( TH* hist )
  {
    //hist->SetLineColor( kAzure + 1 );
    hist->SetLineColor( kBlack );
    hist->SetLineWidth( 2 );
    hist->SetFillColorAlpha( hist->GetLineColor(), 0.05 );
    hist->GetXaxis()->SetLabelSize( 0.025 );
    hist->GetXaxis()->CenterTitle();
    hist->GetYaxis()->SetLabelSize( 0.04 );
    hist->GetYaxis()->CenterTitle();
    
    int max_bin = GetMaxBinWithEntry( hist );
    hist->GetXaxis()->SetRange( 1, max_bin );
  }

  
  
  /////////////////////////////////////////////////
  // Things for print
  vector < std::string > printed_contents_;
  void PrintLine( std::string head, string contents, string separator = ":" );
  void DumpPrintBuffer();

public:
  /////////////////////////////////////////////////
  // Constructor
  InttClassifier( int run_num , double sig_cut );

  /////////////////////////////////////////////////
  // Analysis functions
  void ProcessCosmics();
  void ProcessBeam();

  /////////////////////////////////////////////////
  // Get/Set function to change something inside this class

  // paths
  void SetHitmapDir		( string path )		{ hitmap_dir_ = path; };
  void SetCdcOupoutDir		( std::string dir )	{ output_cdb_dir_ = dir; }; //output_cdb_dir_
  void SetQaRootOutputDir	( std::string dir )	{ output_qa_root_dir_ = dir ;}; // output_qa_root_path_
  void SetQaPdfOutputDir	( std::string dir )	{ output_qa_pdf_dir_ = dir ;};; // output_qa_pdf_path_ 

  // for input/output
  void SetOutputTag	( string tag )	{ output_tag_ = tag ;}; //! tag is attached to name of the output files.
  void SetYear		( int year )	{ year_ = year;}; //! year of this data is set. This is used for the output path.

  // flags
  void SetBeamFlag	( bool flag )		{ isbeam_ = flag; };
  string GetFileSuffix	(); //! You can get the suffix for the input/output files
  void SetFittingMode	( string mode ); //! "gaus" and "double gaus" are available
  void SetFileSuffix	( string suffix )	{ suffix_ = suffix; force_suffix_ = true; }; //! Set the file suffix and force to use it.
  
  void SetOfficialDstFlag( bool flag ){ is_official_ = flag; }; // 
  
  /////////////////////////////////////////////////
  // Print function to show the status
  void Print();
  
  /////////////////////////////////////////////////
  // Output function
  void WriteResults();
};


// Trick to write like the standard C++ but run as a part of ROOT macro
#ifndef InttClassifier_cc
#include "InttClassifier.cc"
#endif
