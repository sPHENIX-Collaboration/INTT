#include "Eventa.hh"

ClassImp( Eventa );

Eventa::Eventa() :
  eventa_id_( -1 )
{};

Eventa::Eventa( int id ) :
  eventa_id_( id )
{}

Eventa::~Eventa()
{
  

}

void Eventa::AddTrackMC( TrackMC* track )
{
  tracks_MC_.push_back( track );
}

TrackMC* Eventa::GetTrackMC( int id )
{
  assert( id <= tracks_MC_.size() );
  return tracks_MC_[ id ];
}

void Eventa::PrintLine( string header, string contents, int total_num, char padding_word )
{
  cout << "|" << padding_word
       << header << padding_word
       << contents << padding_word
       << string( total_num - header.size() - contents.size() - 3, padding_word )
       << "|"
       << endl;
  
}

void Eventa::Print( int level )
{
  int total_width = 50;
  int width = 10;
  this->PrintLine( "", "", total_width, '-' );
  this->PrintLine( "Eventa", to_string(eventa_id_), total_width );
  this->PrintLine( "  #TrackMC:", to_string( GetNTrackMC() ), total_width );
  if( level == 1 )
    {
      for( auto track : tracks_MC_ )
	track->Print( level );

    }
  
  this->PrintLine( "  #HitMC:", to_string( GetNHitMC() ), total_width );
  this->PrintLine( "", "", total_width, '-' );



}
