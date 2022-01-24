#pragma once

#include <iostream>
#include <iomanip>
#include <utility>

#include "TObject.h"
#include "TVector3.h"

using namespace std;
class StepMC : public TObject
{
private :
  int id_; // id of this step in an event
  int track_id_; // id of track that this step belongs to
  int event_id_; // event ID that this track belongs to
  int volume_copy_no_; // copy number of the volume, which contains the pre-step point
  
  double energy_; // the energy this track has at this step
  double energy_deposit_; // total energy deposit made at this step
  double time_; // time difference b/w pre- and post- step points
  double delta_time_; // time difference b/w pre- and post- step points
  double delta_energy_; // energy difference  b/w pre- and post- step points
  double step_length_; // total length of this track
  string volume_name_; // name of the volume which contains the pre-step point
  string volume_material_; // material name of the volume which contains the pre-step point
  string interaction_; // name of the interaction happen at the pre-step point
  
  vector < double > position_; // [0, 1, 2] = x, y, z
  vector < double > momentum_;
  vector < double > position_pre_; // [0, 1, 2] = x, y, z
  vector < double > momentum_pre_;
  vector < double > delta_position_;
  vector < double > delta_momentum_;
  
  /*
  //TVector3* delta_momentum_; // momentum difference b/w pre- and post- step points

  TVector3* delta_position_; // position difference b/w pre- and post- step points
  TVector3* momentum_; // 3d momentum in lab frame at each vertex
  

  TVector3*  position_; // points where this track interact with other material
*/
  
  // G4SteppingControl 	GetControlFlag () const
  //   const std::vector< const G4Track * > * 	GetSecondaryInCurrentStep () const
  //   const G4TrackVector * 	GetSecondary () const
  //   G4TrackVector * 	GetfSecondary ()


public:
  StepMC() :
    id_( -1 ),
    track_id_( -1 ),
    event_id_( -1 ),
    volume_copy_no_( -1 ),
    energy_( -1 ),
    energy_deposit_( -1.0 ),
    time_( -1 ),
    delta_time_( -1.0 ),
    step_length_( -1.0 ),
    volume_name_( "" ),
    volume_material_( "" ),
    interaction_( "" ),
    position_( 3 ),
    momentum_( 3 ),
    position_pre_( 3 ),
    momentum_pre_( 3 ),
    delta_position_( 3 ),
    delta_momentum_( 3 )
    // delta_energy_( -1.0 ),
  {};
  
  StepMC( int id );

  ~StepMC(){};

  
  
  bool operator<(const StepMC &another) const
  {
    return GetID() < another.GetID();
  };

  bool operator>(const StepMC &another) const
  {
    return GetID() > another.GetID();
  };


  static int compare(const StepMC& l, const StepMC& r)
  {
    int d = ( (l.GetID() > r.GetID() ) - (l.GetID() < r.GetID())   );
    if(d) return d;

    return d;
  };
  
  static int compare(const StepMC* l, const StepMC* r) 
  {
    if( r->GetID() == l->GetID() )
      return 0;
    
    int d = (r != NULL) - (l != NULL);
    
    if(d)
      {
	return d;
      }

    return compare(*l, *r);
  };
  
  static bool Less(const StepMC* l, const StepMC* r)
  {
    return compare(l,r) < 0;
  }
  static bool Greater(const StepMC* l, const StepMC* r)
  {
    return compare(l,r) > 0;
  }
  
  int GetEventID() const { return event_id_;};

  int GetID() const { return id_;};
  vector < double > GetDeltaMomentum() const { return delta_momentum_;};
  vector < double > GetDeltaPosition() const { return delta_position_;};
  double GetEnergyDeposit() const { return energy_deposit_;};
  string  GetInteraction() const { return interaction_;};
  vector < double > GetMomentum() const { return momentum_;};
  vector < double > GetMomentumPre() const { return momentum_pre_;};
  vector < double > GetPosition() const { return position_;};
  vector < double > GetPositionPre() const { return position_pre_;};
  double GetStepLength() const { return step_length_;};
  int GetTrackID() const { return track_id_;};
  int    GetVolumeCopyNo() const { return volume_copy_no_;};
  string GetVolumeName() const { return volume_name_;}; // volume name for the pre-step point
  string GetVolumeMaterial() const { return volume_material_;}; // material name of the volume for the pre-step point
  
  void SetDeltaMomentum( double x, double y, double z );
  void SetDeltaMomentum( vector < double >& mom ){ delta_momentum_ = mom; };
  void SetDeltaPosition( double x, double y, double z );
  void SetDeltaPosition( vector < double >& pos ){ delta_position_ = pos; };
  void SetDeltaTime( double delta ){ delta_time_ = delta;};
  void SetEnergyDeposit( double depo ){ energy_deposit_ = depo;};
  
  void SetEventID( int id ){ event_id_ = id;};
  void SetID( int id ){ id_ = id;};
  void SetInteraction( string name ){ interaction_ = name;};
  void SetMomentum( double x, double y, double z );
  void SetMomentumPre( double x, double y, double z );
  void SetMomentum( vector < double >& mom ){ momentum_ = mom; };
  void SetPosition( double x, double y, double z );
  void SetPositionPre( double x, double y, double z );
  void SetPosition( vector < double >& pos ){ position_ = pos; };
  void SetTime( double timing ){ time_ = timing;};
  void SetTrackID( int id ){ track_id_ = id;};
  void SetStepLength( double length ){ step_length_ = length;};
  void SetVolumeCopyNo( int num ){ volume_copy_no_ = num; };
  void SetVolumeName( string name ){ volume_name_ = name ;}; // volume name for the pre-step point
  void SetVolumeMaterial( string material ){ volume_material_ = material ;}; // volume material for the pre-step point

  void Set3Values( vector < double >& values, double x, double y, double z );
  
  void Print( int level = 0 );
  ClassDef( StepMC, 1 );
};
