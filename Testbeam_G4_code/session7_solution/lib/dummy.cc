#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TRandom3.h"
#include "StepMC.hh"
#include "TrackMC.hh"
//#include "Eventa.hh"

using namespace std;
int main()
{

  auto tf = new TFile( "test.root", "RECREATE" );
  auto tr = new TTree( "tree", "tree" );
  
  int num = 1;
  tr->Branch( "event", &num, "event/I" );

  vector < double > values;
  tr->Branch( "values", &values );
  values.push_back( 10 );
  

  vector < TrackMC* > tracks;
  tr->Branch( "tracks", &tracks );
  //tracks.push_back( new TrackMC() );
  
  vector < StepMC* > steps;
  tr->Branch( "steps", &steps );
  //steps.push_back( new StepMC() );

  auto track = new TrackMC(); //  = new TrackMC();
  vector < double > vals;
  vals.push_back( 123 );
  vals.push_back( 234 );
  vals.push_back( 345 );

  TRandom3 random_id( 0 );
  for( int i=0; i<5; i++ )
    {

      StepMC* step =  new StepMC( random_id.Uniform(0, 100 )  );
      step->SetEventID( 12 );
      step->SetTrackID( 1 );
      step->SetStepLength( i * i );
      step->SetDeltaPosition( vals );
      step->SetPosition( vals );
      step->SetDeltaMomentum( vals );
      step->SetMomentum( vals );
      //step->Print( 2 );
      
      steps.push_back( step );      
      track->AddStepMC( step );      
    }

  for( int i=0; i<steps.size(); i++ )
    cout << steps[i]->GetID() << " ";
  cout << endl;

  cout << "Less" << endl;
  sort( steps.begin(), steps.end(), StepMC::Less );
  for( int i=0; i<steps.size(); i++ )
    cout << steps[i]->GetID() << " " ;
  cout << endl;
  
  cout << "Greater" << endl;
  sort( steps.begin(), steps.end(), StepMC::Greater );
  for( int i=0; i<steps.size(); i++ )
    cout << steps[i]->GetID() << " " ;
  cout << endl;
  
  tracks.push_back( track );
  // tr->Branch( "track", "TrackMC", &track );
  // tr->Branch( "step", "stepMC", &step );
  
  //track.AddStepMC( &step );
  // track->SetStepMC( step );

  
  //  auto ev = new Eventa();
  //track->Print( 2 );
  //track.Print( 2 );
  
  //auto steps = track->GetStepMCs();
  
  //ev->AddTrackMC( track );
  //  ev->Print( 1 );

  //tr->Branch( "event", &ev );
  
  tr->Fill();
  //tr->Fill();
  // tr->Fill();

  //tr->Print();
  tf->WriteTObject( tr, tr->GetName() );

  delete tr;
  delete tf;
  
  return 0;
}
