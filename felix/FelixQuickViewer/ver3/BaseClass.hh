#pragma once

#include <iostream>
#include <iomanip>

#include <string>
#include <TROOT.h>
#include <TFile.h>
#include <TStyle.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>

#include "LadderMap.hh"

using namespace std;

class BaseClass
{
protected:
  /////////////////////////////////////
  // protected member variables     //
  /////////////////////////////////////

  // constant variables
  static const int kChip_num_ = 26;
  static const int kLadder_num_ = 14; //
  //const string map_dir = "/direct/sphenix+tg+tg01/commissioning/INTT/map_ladder/";
  const string map_dir = "/Users/genki/map_ladder/";

  // input
  string filename_; // The path to the data file
  string run_type_ = "calib";
  string ladder_map_path_;
  LadderMap* ladder_map_;
  
  // output
  string output_basename_;
  string root_suffix_ = "_hist.root"; // suffix of the output ROOT file which contains histgram objects
  string figure_suffix_ = ".png";

  // variables for misc
  int width_ = 100;

  TFile* f1_;

  vector < string > print_buffer_;
  TH2D* hist_adc_ch_[14][26];
  TH1D* hist_adc_[14][26];
  TH1D* hist_ch_[14][26];

  /////////////////////////////////////
  // protected member functions      //
  /////////////////////////////////////
  virtual void Init();
  void InitLadderMap();
  
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
  };
  
public:

  // constructors
  BaseClass(){};
  //BaseClass( string filename_arg ){ filename_ = filename_arg;};
  BaseClass( string filename_arg );

  // destructor
  ~BaseClass()
  {
    f1_->Close();
  };
  
  /////////////////////////////////////
  // Get methods                     //
  /////////////////////////////////////
  // input parameters
  string GetData() const { return filename_;};
  string GetLadderMapPath() const { return ladder_map_path_;};
  
  // output parameters
  string GetOutput() const { return output_basename_ + root_suffix_; }; // ROOT file
  string GetOutputBasename() const { return output_basename_;};

  void SetRunType( string arg ){ run_type_ = arg;};
  
  //virtual bool ReadHistograms();
  virtual void Print();
  virtual void SetStyle();  
    
};

#ifndef CLING
#include "BaseClass.cc"
#endif
