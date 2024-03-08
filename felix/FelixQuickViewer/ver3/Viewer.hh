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
  
  // output
  string output_basename_;
  string root_suffix_ = "_hist.root"; // suffix of the output ROOT file which contains histgram objects
  //string figure_suffix_ = ".pdf";
  string figure_suffix_ = ".png";

  // for drawings
  double margin_left	= 0.1;
  double margin_right	= 0.01;
  double margin_top	= 0.075;
  double margin_bottom	= 0.05;
  double pad_width = 0.1;
  double pad_height = 0.1;
  int color_ = kBlack;
  
  // variables for misc
  int width_ = 100;

  vector < string > print_buffer_;
  
  TCanvas* c_ch_;
  TCanvas* c_adc_;
  TCanvas* c_adc_ch_;

  unsigned int GetMaxBinContent( TH1D* hists[kLadder_num_][kChip_num_], int rank, int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_);
  unsigned int GetMaxBinContentRatio( TH1D* hists[kLadder_num_][kChip_num_], double remove_top, int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_); // Bins at top x% are ignored
  string GetOutputPath( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_, string keyword="default" );

  void DivideCanvas( TCanvas* c, int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ );
  
  void WriteLabelFelix( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ );
  void WriteLabelRoc( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ );
  void WriteLabelLadder( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ );
  void WriteLabelChip( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_ );
  void WriteLabelXaxis( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_, string label_low="0", string label_high="127" );
  void WriteLabelYmax( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_, int y_max=0 );

  void CanvasPreparation( TCanvas* c,
			 int x_min, int x_max, int y_min, int y_max,
			 int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_,
			 string hist_name = "");
  
public:

  // constructors
  Viewer(){};
  Viewer( string filename_arg, int year = 9997 );

  // destructor
  ~Viewer(){};

  int DoAll();
  int Draw();
  int Draw_AdcChannel();
  int Draw_Channel();
  int Draw_HitDist( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_);
  int Draw_AmplAdc( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_);
  int Draw_ChAmpl( int ladder_min=0, int ladder_max=kLadder_num_, int chip_min=0, int chip_max=kChip_num_);
  
  TCanvas* GetCanvasCh(){ return (TCanvas*)(c_ch_->Clone()); }
  
  void Print() override;
  int Process();
  void SaveHists();
  
  void SetStyle() override;

};

#ifndef CLING
#include "Viewer.cc"
#endif
