/// \file EDEventAction.cc
/// \brief Implementation of the EDEventAction class

#include "EDEventAction.hh"

EDEventAction::EDEventAction()
 : G4UserEventAction(),
   fVerbose(true),
   tracks_(),
   steps_()
{}

EDEventAction::~EDEventAction()
{}

void EDEventAction::BeginOfEventAction(const G4Event* event)
{

  //std::cerr << "void EDEventAction::BeginOfEventAction(const G4Event* event)";
  // if ( false ) 
  // {
  //   //G4cout << ">>> Start event: " << event->GetEventID() << G4endl;
  // }
  //  std::cerr << "----------> ends" << std::endl;
}

void EDEventAction::EndOfEventAction(const G4Event* event)
{

  // link step information to a track

  for( auto& track : tracks_ )
    {
      for( auto& step : steps_ )
	{
	  track->AddStepMC( step ); // only steps belongs to this track are stored
	}
    }
 
  // for( auto& track : tracks_ )
  //   track->Print( 3 );

  
  auto run_manager = G4RunManager::GetRunManager();
  auto run_action = (EDRunAction*) run_manager->GetUserRunAction();
  run_action->SetEventID( event->GetEventID() );
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  /////////////////////////////////////////////////////////////////////
  // Processes for the trigger scintillators                         //
  /////////////////////////////////////////////////////////////////////
  G4int eID = 0;
  //  const G4Event* evt = G4MTRunManager::GetRunManager()->GetCurrentEvent();
  if( event )
    eID = event->GetEventID();


  // G4int collId;

  // // HGCAL EE + FH
  // auto sdManager = G4SDManager::GetSDMpointer();
  // collId = sdManager->GetCollectionID( "INTT_sciLV1" );
  // // = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");

  // auto hc = hce->GetHC(collId);

  // G4int n_hit = fHitsCollection->entries();

  // for (G4int i = 0; i < n_hit; i++) 
  //   {
  //     G4double fEdep = (*fHitsCollection)[i]->GetEnergyDeposit();
  //     if (fEdep>0.)
  //     {
  //       // analysisManager->FillNtupleIColumn(3, 0, eID);
  //       // analysisManager->FillNtupleIColumn(3, 1, i);
  //       // analysisManager->FillNtupleDColumn(3, 2, fEdep);  
  //       // analysisManager->AddNtupleRow(3);
  //       // //if ( eID % 10000 == 0)
  // 	// //if( verbose == 2 )
  // 	// if( false )
  //       //   {
  //       //     G4cout<<" SCI, eID : "<<eID<<" Sci trigger ID : "<<i<<" edep : "<<fEdep<<G4endl;
  //       //   }
  //     }

  //   }

  
  // auto tree = run_action -> GetTree();

  // run_action->SetTrackMCs( tracks_ );
  // run_action->SetStepMCs( steps_ );

  // tree->Fill();
  // G4cout << "\r\c" << event->GetEventID() << "\t" << "EDEventAction::EndOfEventAction, "
  //  	 << tracks_.size() << " TrackMCs are filled";
  // 	 << endl;

  
  // delete all information for this run
  tracks_.erase( tracks_.begin(), tracks_.end() );
  steps_.erase( steps_.begin(), steps_.end() );
  
  //  std::cerr << "void EDEventAction::EndOfEventAction(const G4Event* event)";
  // if ( false ) 
  // {
  //   //G4cout << ">>> End event: " << event->GetEventID() << G4endl;
  // }  
  // G4int eID = event->GetEventID();
  // if ( eID % 20000 == 0)
  //   G4cout << "Event Process: " << eID << G4endl;

  //  std::cerr << "  ----> ends" << std::endl;
  //std::cerr << std::endl;
}    

TrackMC* EDEventAction::GetTrackMC( int id )
{
  for( auto& track : tracks_ )
    if( track->GetID() == id )
      return track;

  G4cerr << " TrackMC* EDEventAction::GetTrackMC( int id )" << G4endl;
  G4cerr << " The track ID " << id << " is not found in EDEventAction" << G4endl;
  return nullptr;
}
