#include "InttEvent.h"

using namespace std;

////////////////////////////////////////////////////////
// Constructor                                        //
////////////////////////////////////////////////////////
InttEvent::InttEvent() :
  position_vertex_( 0, 0, 0 )
{
  this->Init();

}

////////////////////////////////////////////////////////
// Init                                               //
////////////////////////////////////////////////////////
void InttEvent::Init()
{
}

////////////////////////////////////////////////////////
// Private functions                                  //
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Public functions                                   //
////////////////////////////////////////////////////////
void InttEvent::Calc()
{
  cluster_num_inner_ = cluster_num_outer_ = 0; // init in case of multiple executions of this function
  
  for( auto& cluster : clusters_ )
    {
      cluster->SetVertex( position_vertex_ );
      cluster->Calc();

      cluster->GetLayer() < 2 ? cluster_num_inner_++ : cluster_num_outer_++;
    }

  cluster_num_ = clusters_.size();
}

void InttEvent::Reset()
{

  // int
  run_ = event_num_ = cluster_num_ 
    = cluster_num_inner_  = cluster_num_outer_
    = -1;

  // bool
  is_vertex_good_ = false;

  // double 
  z_chi2_ndf_ = -1;  
  
  position_vertex_ = TVector3( 0, 0, 0 );
  
  clusters_.erase( clusters_.begin(), clusters_.end() );
}

void InttEvent::SetVertex( double x, double y, double z )
{
  position_vertex_ = TVector3( x, y, z );
}

void InttEvent::SetVertexZ( double z )
{
  position_vertex_.SetZ( z );
}

void InttEvent::Print()
{
  if( event_num_ % 100 == 0 )
    {
      cout 
	<< setw(  8 ) << "Run num" << " "
	<< setw( 12 ) << "Event ID" << " "
	<< setw(  4 ) << "#cls" << " "
	<< setw(  4 ) << "#inr" << " "
	<< setw(  4 ) << "#otr" << " " 
	<< "vtx" << " "
	<< "( "
	<< setw(  8 ) << setprecision(2) << "x"<< ", "
	<< setw(  8 ) << setprecision(2) << "y" << ", "
	<< setw(  8 ) << setprecision(2) << "z" << ") "  
	<< endl;
    }

    cout 
    << setw(  8 ) << run_ << " "
    << setw( 12 ) << event_num_ << " "
    << setw(  4 ) << cluster_num_ << " "
    << setw(  4 ) << cluster_num_inner_ << " "
    << setw(  4 ) << cluster_num_outer_ << " " 
    << "( "
    << setw(  8 ) << setprecision(2) << position_vertex_.X() << ", "
    << setw(  8 ) << setprecision(2) << position_vertex_.Y() << ", "
    << setw(  8 ) << setprecision(2) << position_vertex_.Z() << ") "  
    << endl;

  if( verbosity_ > 0 )
    {
      for( auto& cluster : clusters_ )
	{
	  cluster->Print();
	  if( verbosity_ == 1 )
	    break;
	  
	}
      
    }
}
