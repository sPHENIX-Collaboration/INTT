#include "StepMC.hh"

ClassImp( StepMC );

StepMC::StepMC( int id ) :
  id_( id ),
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
{}


void StepMC::SetDeltaMomentum( double x, double y, double z )
{
  Set3Values( delta_momentum_, x, y, z );
}

void StepMC::SetDeltaPosition( double x, double y, double z )
{
  Set3Values( delta_position_, x, y, z );
}

void StepMC::SetMomentum( double x, double y, double z )
{
  Set3Values( momentum_, x, y, z );
}

void StepMC::SetMomentumPre( double x, double y, double z )
{
  Set3Values( momentum_pre_, x, y, z );
}

void StepMC::SetPosition( double x, double y, double z )
{
  Set3Values( position_, x, y, z );
}

void StepMC::SetPositionPre( double x, double y, double z )
{
  Set3Values( position_pre_, x, y, z );
}

void StepMC::Set3Values( vector < double >& values, double x, double y, double z )
{
  values[0] = x;
  values[1] = y;
  values[2] = z;
}




void StepMC::Print( int level )
{

  int header_width = 20;
  cout << "+" << string(38, '-') << "+" << endl;
  cout << "| " << setw(header_width) << "Event ID: " << event_id_ << endl;
  cout << "| " << setw(header_width) << "Step ID: " << id_ << endl;
  cout << "| " << setw(header_width) << "Track ID: " << track_id_ << endl;
  cout << "| " << setw(header_width) << "Track Length: " << step_length_ << " cm" << endl;
  cout << "| " << setw(header_width) << "Volume: " << volume_name_ << " #" << volume_copy_no_
       << ", " << volume_material_ << ", " << interaction_ << endl;
  
  cout << "| " << setw(header_width) << "Energy: " << energy_ << " MeV/c^2" << endl;
  cout << "| " << setw(header_width) << "Energy deposit: " << energy_deposit_ << " MeV/c^2" << endl;
  
  cout << "| " << setw(header_width) << "Time: " << time_ << " ns" << endl;
  cout << "| " << setw(header_width) << "Delta time: " << delta_time_ << " ns" << endl;

  if( level > 0 )
    {
      cout << "| " << string( 5, ' ' )
	   << "+ " << "p = ("
	   << momentum_[0] << ", "
	   << momentum_[1] << ", "
	   << momentum_[2] << ") MeV/c "
	   << "at " << " pos = ("
	   << position_[0] << ", "
	   << position_[1] << ", "
	   << position_[2] << ") cm"
	   << endl;
      

      // more information
      if( level > 1 )
	{
	  cout << "| " << string( 5, ' ' )
	       << "+ " << "dp = ("
	       << delta_momentum_[0] << ", "
	       << delta_momentum_[1] << ", "
	       << delta_momentum_[2] << ") MeV/c "
	       << "at " << " dpos = ("
	       << delta_position_[0] << ", "
	       << delta_position_[1] << ", "
	       << delta_position_[2] << ") cm"
	       << endl;
	  
	}
	  
    }
  
  
  cout << "+" << string(38, '-') << "+" << endl;
}
