#include "InttCluster.h"

using namespace std;

////////////////////////////////////////////////////////
// Constructor                                        //
////////////////////////////////////////////////////////
InttCluster::InttCluster() :
  position_( 0.0, 0.0, 0.0 )
{
  this->Init();
}

InttCluster::InttCluster( double x, double y, double z ) :
  position_( x, y, z )
{
  this->Init();
}

////////////////////////////////////////////////////////
// Init                                               //
////////////////////////////////////////////////////////
void InttCluster::Init()
{
  position_vertex_ = TVector3( -9999.9, -9999.9, -9999.9 );
  position_relative_ = TVector3( -9999.9, -9999.9, -9999.9 );
}

////////////////////////////////////////////////////////
// Private functions                                  //
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Public functions                                   //
////////////////////////////////////////////////////////
void InttCluster::Calc()
{
  // parameters independent from vertex position
  phi_ = position_.Phi();
  theta_ = position_.Theta();
  eta_ = position_.PseudoRapidity();
  
  // parameters depends on vertex position
  position_relative_ = position_ - position_vertex_;
  phi_relative_ = position_relative_.Phi();
  theta_relative_ = position_relative_.Theta();
  eta_relative_ = position_relative_.PseudoRapidity();
  
}

void InttCluster::SetPosition( double x, double y, double z )
{
  position_ = TVector3( x, y, z );
}

void InttCluster::SetVertexPosition( double x, double y, double z )
{
  position_vertex_ = TVector3( x, y, z );
}

void InttCluster::Print()
{
  cout 
    << setw(2) << layer_ << " "
    << "( "
    << setw(10) << setprecision(8) <<  position_.X() << " " 
    << setw(10) << setprecision(8) <<  position_.Y() << " " 
    << setw(10) << setprecision(8) <<  position_.Z() << " " 
    << " )"
    << setw(5) <<  adc_ << " "
    << setw(4) << size_phi_  << " "
    << setw(2) << size_z_ << " "
    << setw(4) << setprecision(3) <<  theta_relative_  << " "
    << setw(4) << setprecision(3) <<  phi_relative_  << " "
    << setw(4) << setprecision(3) <<  eta_relative_
    << endl;
  
}
