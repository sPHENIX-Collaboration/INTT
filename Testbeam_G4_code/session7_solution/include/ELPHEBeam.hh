#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>

#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"
#include "INTTMessenger.hh"

/*!
  @class BeamParameter
  @brief A class to store beam parameters
*/
class BeamParameter
{
public:

  //! default constructor
  BeamParameter(){};

  //! use this constructor
  BeamParameter( std::string line ) :
    energy_( -999 ),
    momentum_direction_{0},
    position_{0}
  {
    std::stringstream ss(line);

    ss >> energy_
       >> momentum_direction_[0] >> momentum_direction_[1] >> momentum_direction_[2]
       >> position_[0] >> position_[1];

    // multiply MeV
    energy_ *= CLHEP::MeV;
  }

  //! returns position vector of this beam
  G4ThreeVector GetPosition()
  {
    return  G4ThreeVector( position_[0] * CLHEP::mm,
			   position_[1] * CLHEP::mm,
			   position_[2] * CLHEP::mm );
  };

  //! returns a vector of a momentum direction
  G4ThreeVector GetMomentumDirection()
  {
    return G4ThreeVector( momentum_direction_[0],
			  momentum_direction_[1],
			  momentum_direction_[2]);
  };

  //! returns energy of this beam
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

/*!

 */
class ELPHEBeam
{
public:
  ELPHEBeam();
  ELPHEBeam( INTTMessenger* mess );
  ~ELPHEBeam();
  
  void GenerateNextBeam();

  G4double GetBeamEnergy(){ return (*index_)->GetEnergy();};
  G4ThreeVector GetBeamPosition(){ return (*index_)->GetPosition();};
  G4ThreeVector GetBeamMomentumDirection(){ return (*index_)->GetMomentumDirection();};

  void Print();
  
private:
  INTTMessenger*  INTT_mess_;
  std::string datafile_;
  G4bool is_first_;

  std::ifstream ifs_;
  std::string beamfile_path_;
  std::vector < G4double > beam_data_;
  std::vector < BeamParameter* > beam_parameter_;
  std::vector < BeamParameter* >::iterator index_;

  void Init();
  void ReadBeamFile();

  int line_length_;
  void PrintLine( std::string line, std::string header = " |", std::string footer = "|" );
};
