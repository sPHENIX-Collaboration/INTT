#pragma once

#include <iostream>
#include <iomanip>
#include <utility>

#include "TObject.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TGraph2D.h"

#include "StepMC.hh"

using namespace std;
class TrackMC : public TObject
{
private :
  int id_; // id of this track in an event
  int event_id_; // event ID that this track belongs to
  string particle_name_; // name of particle: e-, e+, etc.
  int q_; // electric charge
  bool is_primary_; // 

  double total_energy_;
  double kinetic_energy_;
  double track_length_; // total length of this track
  double track_time_; // track time (at the end of step?)
  
  vector < pair < TVector3* , TVector3* > > vertex_momentum_;
  
  //vector < TVector3* > vertices_; // points where this track interact with other material
  vector < double > momentum_;
  vector < double > position_;

  vector < StepMC* > steps_;

public:
  TrackMC() :
    id_( -1 ),
    event_id_( -1 ),
    particle_name_( "none" ),
    q_( -999 ),
    is_primary_( false ),
    total_energy_( -1 ),
    kinetic_energy_( -1 ),
    track_length_( -1 ),
    track_time_( -1 ),
    momentum_( 3 ),
    position_( 3 ),
    steps_( 0 )
  {};
  
  TrackMC( int id, string name, int q );

  ~TrackMC(){};

  void AddStepMC( StepMC* step );

  //  void CalcSteps();
  int GetEventID(){ return event_id_;};
  TGraph2D* GetGraph();
  TGraph* GetGraphOn( string xaxis, string yaxis, double cut_min = -9999, double cut_max = 9999 );
  TGraph* GetGraphOnXY(double cut_min = -9999, double cut_max = 9999 ){ return GetGraphOn( "x", "y", cut_min, cut_max );};
  TGraph* GetGraphOnZX(double cut_min = -9999, double cut_max = 9999 ){ return GetGraphOn( "z", "x", cut_min, cut_max );};
  TGraph* GetGraphOnZY(double cut_min = -9999, double cut_max = 9999 ){ return GetGraphOn( "z", "y", cut_min, cut_max );};
  
  int GetID(){ return id_;};
  bool GetPrimary(){ return is_primary_;};
  double GetKineticEnergy(){ return kinetic_energy_;};
  int GetQ(){ return q_;};
  string GetName(){ return particle_name_;};
  vector < double > GetMomentum(){ return momentum_;};
  double GetTrackLength(){ return track_length_;};
  double GetTrackTime(){ return track_time_;};
  double GetTotalEnergy(){ return total_energy_;};
  vector < StepMC* > GetStepMCs(){ return steps_;};

  bool IsPrimary(){ return this->GetPrimary();};

  void SetEventID( int id ){ event_id_ = id;};
  void SetID( int id ){ id_ = id;};
  void SetKineticEnergy( double e ){ kinetic_energy_ = e;};
  void SetName( string name ){ particle_name_ = name;};
  void SetMomentum( vector < double >& p ){ momentum_ = p;};
  void SetMomentum( double x, double y, double z );
  void SetPosition( vector < double >& x ){ position_ = x;};
  void SetPosition( double x, double y, double z );
  void SetPrimary( bool flag ){ is_primary_ = flag;};
  void SetQ( int q ){ q_ = q;};
  void SetTotalEnergy( double e ){ total_energy_ = e;};
  void SetTrackLength( double length ){ track_length_ = length;};
  void SetTrackTime( double timing ){ track_time_ = timing;};
  
  void Print( int level = 0 );
  ClassDef( TrackMC, 1 );
};
