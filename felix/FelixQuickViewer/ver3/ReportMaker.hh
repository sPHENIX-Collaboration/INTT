#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TFile.h>
#include <TStyle.h>

#include "BaseClass.hh"

using namespace std;

class ReportMaker : public BaseClass
{
private:
  void Init() override;

  // output
  string output_basename_;
  string root_suffix_ = "_hist.root"; // suffix of the output ROOT file which contains histgram objects
  string figure_suffix_ = ".png";

public:

  // constructors
  ReportMaker(){};
  ReportMaker( string filename_arg );

  // destructor
  ~ReportMaker(){};

  int DoAll();

  //void Print();
  int Process();
  void SaveHists();
  
};

#ifdef CLING
#include "ReportMaker.cc"
#endif
