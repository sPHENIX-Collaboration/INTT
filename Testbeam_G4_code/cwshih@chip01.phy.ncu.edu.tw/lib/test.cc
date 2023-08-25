#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "Event.hh"
#include "TrackMC.hh"

using namespace std;

TrackMC* GenerateTrackMC( int id, bool is_primary )  
{
  TrackMC* track = new TrackMC();
  track->SetID( id );
  
  if( is_primary )
    {
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;
      cout << "alivgupoarit,vpwerthnwiurt,ocmtrmar," << endl;

      track->SetName( "e+" );
      track->SetQ( 1 );
      track->SetPrimary( true );
      
      for( int j=0; j<2; j++ )
	{
	  TVector3* v = new TVector3( j, j, j );
	  TVector3* p = new TVector3( 3, 4, 600 / (j+1) );
	  track->AddVertex( v, p );
	}      
    }
  else
    {
      track->SetName( "e-" );
      track->SetQ( -1 );
    }


  return track;
}

Event* GenerateEvent( int event_id )
{
  auto event = new Event( event_id );

  for( int i=0; i<3; i++ )
    {
      auto track = GenerateTrackMC( i, i==0 );	  
      track->SetEventID( event_id );
      track->Print();
      event->AddTrackMC( track );
      //tracks.push_back( track );
      //track->Print(1);
    }

  
  return event;
}

int test()
{

  TFile* tf = new TFile( "test.root", "RECREATE" );
  TTree* tr = new TTree( "tr", "tr" );

  int event_id = 0;
  tr->Branch( "event_id", &event_id, "event_id/I" );
  
  //  vector < TrackMC* > tracks;
  //tr->Branch( "tracks", &tracks );
  Event* event = new Event();
  tr->Branch( "event", &event );
  
  for( int i=0; i<2; i++ )
    {
      event_id = i;
      event = GenerateEvent( i );
      event->Print( 1 );
      tr->Fill();
      
    }
  
  //  tr->Print();
  tf->WriteTObject( tr, tr->GetName() );
  tf->Close();

  return 0;
}

/*
int main( int argc, char* argv[] )
{
  return test();
}
*/
