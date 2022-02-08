#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>

#include "TGraph.h"
#include "TFile.h"
#include "TH2D.h"
#include "TF2.h"
#include "TRandom3.h"

#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
#include "INTTMessenger.hh"

class BeamParameter
{
public:
  BeamParameter(){};
  BeamParameter( std::string line ) :
    energy_( -999 ),
    momentum_direction_{0},
    position_{0}
  {
    std::stringstream ss(line);

    ss >> energy_
       >> momentum_direction_[0] >> momentum_direction_[1] >> momentum_direction_[2]
       >> position_[0] >> position_[1];

    energy_ *= CLHEP::MeV;
  }

  G4ThreeVector GetPosition()
  {
    return  G4ThreeVector( position_[0] * CLHEP::mm,
			   position_[1] * CLHEP::mm,
			   position_[2] * CLHEP::mm );
  };
  
  G4ThreeVector GetMomentumDirection()
  {
    return G4ThreeVector( momentum_direction_[0],
			  momentum_direction_[1],
			  momentum_direction_[2]);
  };

  G4double GetEnergy(){ return energy_;};

  void Print()
  {
    G4cout << energy_ << "\t"
	   << "("
	   << momentum_direction_[0] << ", " << momentum_direction_[1] << ", " << momentum_direction_[2] << "), "
	   << "("
	   << position_[0] << ", " << position_[1] << ", " << position_[2] << ")"
	   << G4endl;

  }
  
private: 
  G4double energy_;
  G4double momentum_direction_[3]; // x, y, z
  G4double position_[3]; // x, y, z
  
};

class ELPHEBeam
{
public:
  ELPHEBeam();
  ELPHEBeam( INTTMessenger* mess, std::string beam_line, std::string production_target, int position_restriction, double momentum );
  ~ELPHEBeam();

  void GenerateNextBeam();
  
  double GetMomentumSpread();
  double GetMomentum(); // for -30 beam line

  //G4double GetThisMomentum(){ return momentum_;};
  G4double GetThisMomentum(){ return 1 * CLHEP::GeV;};
  G4double GetThisX(){ return pos_x_;};
  G4double GetThisY(){ return pos_y_;};

  //G4ThreeVector GetThisMomentumVector() const { momentum_;}; //  it crushes the process, why?
  G4ThreeVector GetThisMomentumVector(){ return G4ThreeVector( momentum_.x(),momentum_.y(),momentum_.z() );};
  G4ThreeVector GetThisPositionVector(){ return G4ThreeVector( position_.x(),position_.y(),position_.z() );};

  void GenerateNextBeamTemp();

  G4double GetBeamEnergy(){ return (*index_)->GetEnergy();};
  G4ThreeVector GetBeamPosition(){ return (*index_)->GetPosition();};
  G4ThreeVector GetBeamMomentumDirection(){ return (*index_)->GetMomentumDirection();};

  void Print( int level = 0 );
  
private:
  INTTMessenger*  INTT_mess_;
  std::string datafile_;
  std::string beam_line_; // -23(default) or -30
  std::string production_target_; // Au_20um(defau;t), W_200um, Cu_8mm

  //TRandom3* random_gen_;
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
  G4double given_momentum_;
  //  G4double momentum_;
  G4double momentum_factor_;
  G4double spread_factor_;
  G4double pos_x_;
  G4double pos_y_;

  G4ThreeVector position_;
  G4ThreeVector momentum_;
  
  std::vector < BeamParameter* > beam_parameter_;

  std::ifstream ifs_;
  std::string beamfile_path_;
  std::vector < G4double > beam_data_;
  //std::vector < G4double >::iterator index_;
  std::vector < BeamParameter* >::iterator index_;
  
  void Init();
  //void ReadDatabase();
  void ReadBeamFile();
  
  G4double GetRandomX( TH2D* hist, G4double y );



};
