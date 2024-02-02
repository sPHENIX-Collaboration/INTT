#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include <TFile.h>
#include <TTree.h>
// #include <TH1D.h>
// #include <TH2D.h>
// #include <TH3D.h>
// #include <TCut.h>
#include <TStyle.h>

#include "BaseClass.hh"
//#include "LadderMap.hh"

using namespace std;

class HistMaker : public BaseClass
{
private:

  // variables to contain values from TTree
  int adc_;
  int ampl_;
  int chip_id_;
  int fpga_id_;
  int module_;
  int chan_id_;
  int fem_id_;
  int bco_;
  int bco_full_;
  int event_;

  // ROOT objects
  // TFile* f1_;
  TTree* tr1_;

  // TH2D* hist_adc_ch_[14][26];
  // TH1D* hist_adc_[14][26];
  // TH1D* hist_ch_[14][26];
  void Init() override;
  
public:

  // constructors
  HistMaker(){};
  HistMaker( string filename_arg );

  // destructor
  ~HistMaker(){};

  int DoAll();
  
  int Process();
  void SaveHists();
  
};

#ifndef CLING
#include "HistMaker.cc"
#endif
