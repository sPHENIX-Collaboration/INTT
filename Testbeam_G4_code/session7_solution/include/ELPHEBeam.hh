#pragma once

#include <iostream>
#include <string>
#include <cstdlib>

#include "TGraph.h"
#include "TFile.h"

#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class ELPHEBeam
{
public:
  ELPHEBeam();
  ELPHEBeam( std::string beam_line, std::string production_target, int position_restriction, double momentum );
  ~ELPHEBeam();
  double GetMomentumSpread();
  double GetMomentum(); // for -30 beam line

  void Print( int level );
private:
  std::string datafile_;
  std::string beam_line_; // -23(default) or -30
  std::string production_target_; // Au_20um(defau;t), W_200um, Cu_8mm
  
  //
  // 0: no, (default)
  // 1: weak, | x_ps | < 4.5 mm
  // 2: moderate, | x_ps | < 3.0 mm
  // 3:strong, | x_ps | < 1.5 mm
  int position_restriction_;
  
  // factor to get beam momentum for -30 degree beam line,
  // According to [7]T. Ishikawa, 光子ビームで生成される電子・陽電子, GeV-γ 解析ノート HD No. 388E (2015):
  //  - 1.32 * m_{-30 degree} = m_{-23 degree}
  //  - 25 * sigma_{-30 degree} = sigma_{-23 degree}
  // Because we'll use beam with higher momentum, the -23 degree beam line shoud be the default choice.
  double momentum_;
  double momentum_factor_;
  double spread_factor_;
  TFile* data_;
  TGraph* graph_;
  void ReadDatabase();
  



};
