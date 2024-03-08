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
  TTree* tr1_;

  void Init() override;
  
public:

  // constructors
  HistMaker(){};
  HistMaker( string filename_arg, int year = 9998 );

  // destructor
  ~HistMaker(){};

  int DoAll();
  
  int Process();
  void SaveHists();
  
};

#ifndef CLING
#include "HistMaker.cc"
#endif
