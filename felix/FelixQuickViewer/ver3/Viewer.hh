#pragma once

#include <iostream>
#include <iomanip>
#include <algorithm>

#include <TTree.h>
#include <TPad.h>
#include <TH1D.h>
#include <TH2D.h>
#include <iostream>
#include <TCut.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TAttText.h>
#include <string>
#include <TH3D.h>
#include <TFile.h>
#include <TStyle.h>

#include "BaseClass.hh"
#include "LadderMap.hh"

using namespace std;

class Viewer : public BaseClass
{
private:
  void Init() override;
  //void InitLadderMap();

  template < class... T >
  void PrintLine( string header, string separator, string footer, T... contents )
  {
    stringstream ss;
    ss << header << separator;

    if( initializer_list<string>{contents... }.size() > 0  )
      ss << " " ;
    
    for( auto& it : initializer_list<string>{contents... } )
      ss << it << " " << separator;

    print_buffer_.push_back( ss.str() );
  }

  // constant variables
  const string map_dir = "/sphenix/tg/tg01/commissioning/INTT/map_ladder/";
  
  // input
  string filename_; // The path to the data file
  string ladder_map_path_;
  LadderMap* ladder_map_;
  
  // output
  string output_basename_;
  string root_suffix_ = "_hist.root"; // suffix of the output ROOT file which contains histgram objects
  //string figure_suffix_ = ".pdf";
  string figure_suffix_ = ".png";

  // variables for misc
  int width_ = 100;

  vector < string > print_buffer_;
  
  // ROOT objects
  //TFile* f1_;
  //  TTree* tr1_;

  TH2D* hist_adc_ch_[kLadder_num_][kChip_num_];
  TH1D* hist_adc_[kLadder_num_][kChip_num_];
  TH1D* hist_ch_[kLadder_num_][kChip_num_];

  TCanvas* c_ch_;
  TCanvas* c_adc_;
  TCanvas* c_adc_ch_;
  TCanvas* c_pedestal_;

  unsigned int GetMaxBinContent( TH1D* hists[kLadder_num_][kChip_num_], int rank );
  unsigned int GetMaxBinContentRatio( TH1D* hists[kLadder_num_][kChip_num_], double remove_top ); // Bins at top x% are ignored
  
public:

  // constructors
  Viewer(){};
  Viewer( string filename_arg);

  // destructor
  ~Viewer(){};

  int DoAll();
  int Draw();
  int Draw_AdcChannel();
  int Draw_Channel();
  int Draw_Pedestal();
  int Draw_Calibration();
  
  TCanvas* GetCanvasCh(){ return (TCanvas*)(c_ch_->Clone()); }
  
  void Print();
  int Process();
  void SaveHists();
  
  void SetStyle();

};

#ifndef CLING
#include "Viewer.cc"
#endif
