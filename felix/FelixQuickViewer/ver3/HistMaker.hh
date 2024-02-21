#pragma once

#include "BaseClass.hh"

#include <iostream>
#include <iomanip>

#include <TTree.h>
#include <TPad.h>
#include <TH1F.h>
#include <TH2F.h>
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

#include "LadderMap.hh"

using namespace std;

class HistMaker : public BaseClass
{
private:
  void Init() override;

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
  TFile* f1_;
  TTree* tr1_;

  TH2D* hist_adc_ch_[14][26];
  TH1D* hist_adc_[14][26];
  TH1D* hist_ch_[14][26];
  
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

#ifdef __CINT__
#include "HistMaker.cc"
#endif
