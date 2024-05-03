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

class Plotter : public BaseClass 
{
private:
  void Init() override;

  // ROOT objects
  //TFile* f1_;
  TTree* tr1_;

  TH2D* hist_adc_ch_[14][26];
  TH1D* hist_adc_[14][26];
  TH1D* hist_ch_[14][26];
  
public:

  // constructors
  Plotter(){};
  //  Plotter( string filename_arg );

  // destructor
  ~Plotter(){};

  int DoAll();
  int Draw();
  int Draw_AdcChannel();
  int Draw_Channel();

  void Print() override;
  int Process();

};

#ifdef __CINT__
#include "Plotter.cc"
#endif
