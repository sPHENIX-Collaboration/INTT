/// \file EDEventAction.cc
/// \brief Implementation of the EDEventAction class

#include "EDEventAction.hh"

EDEventAction::EDEventAction( INTTMessenger* INTT_mess )
 : G4UserEventAction(),
   fVerbose(true),
   INTT_mess_( INTT_mess )
   // tracks_(),
   // steps_()
{}

EDEventAction::~EDEventAction()
{}

void EDEventAction::BeginOfEventAction(const G4Event* event)
{

  // BCO and BCO_full are determined at the beginning of an event
  bco_full_ = CLHEP::RandFlat::shootInt( long(0), long(65535) );
  unsigned int lowest_7_bits = 255;  
  bco_=  bco_full_ & lowest_7_bits; // take the lowest 7 bits from the BCO full

  dac_values_ = INTT_mess_->GetDacValues();  
}

EDChamberHitsCollection*
EDEventAction::GetHitsCollection(G4int ID,
				 const G4Event* event) const
{
  auto hitsCollection
    = static_cast<EDChamberHitsCollection*>(
					    event->GetHCofThisEvent()->GetHC(ID) );

  if ( ! hitsCollection )
    {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << ID;
      G4Exception("EDEventAction::GetHitsCollection()",
		  "MyCode0003", FatalException, msg);
    }
  
  return hitsCollection;
}

void EDEventAction::GetHitsCollectionIDs()
{

  // skip everything if IDs were already got
  //  if ( fEcalAbsHCID != -1 )
  if( sensor_IDs_.size() != 0 )
    return;

  //////////////////////////////////////////////////////////////////////////////
  // silicon strip                                                            //
  //////////////////////////////////////////////////////////////////////////////  
  for( int i=0; i<2; i++ ) // loop over Chamber1(0, type-A) and Chamber2(1, type-B)
    {
      std::string sensor_type = "Chamber" + std::to_string( i+1 );
      
      for( int j=0; j<2; j++ ) // loop over odd(0) and even(1)
	{

	  std::string sensor_column = "";
	  if( j == 0 )
	    sensor_column = "odd";
	  else
	    sensor_column = "even";
	  
	  std::stringstream strip_HC;
	  strip_HC << sensor_type << "HitsCollection_" << sensor_column ;// 
	  G4int id = G4SDManager::GetSDMpointer()->GetCollectionID( strip_HC.str().c_str() );
	  sensor_IDs_.push_back( id );

	  //G4cout << strip_HC.str() << "\t" << id << endl;
	  
	} // end of the loop over sensor columns
    } // end of the loop over sensor types
    
}


void EDEventAction::EndOfEventAction(const G4Event* event)
{

  // G4cout << G4endl;
  
  // G4cout << "End of event" << G4endl;
  
  // link step information to a track

  // for( auto& track : tracks_ )
  //   {
  //     for( auto& step : steps_ )
  // 	{
  // 	  track->AddStepMC( step ); // only steps belongs to this track are stored
  // 	}
  //   }
 
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

  this->GetHitsCollectionIDs();
    
  /////////////////////////////////////////////////////////////////////
  // Processes for the INTT hits                                     //
  /////////////////////////////////////////////////////////////////////
  this->FillINTTEvent( event );

  const int INTT_tree_both_id = 6;
  analysisManager->AddNtupleRow( INTT_tree_both_id );
  this->CleanContainer();
  
  // auto trajectories = event->GetTrajectoryContainer();
  // for( int i=0; i<trajectories->size(); i++ )
  //   {
  //     //G4VTrajectory* trajectory = (trajectories[i]->GetVector());
  //     //      G4VTrajectory* trajectory = (trajectories->GetVector()[i]);
  //     G4cout << std::setw(5)
  // 	     << i << " "
  // 	     << std::setw(5) << trajectory->GetTrackID() << " "
  // 	     << std::setw(5) << trajectory->GetParentID() << " "
  // 	     << std::setw(8) << trajectory->GetParticleName() << " "
  // 	     << "( "
  // 	     << std::setw(5) << trajectory->GetInitialMomentum().x() << ", "
  // 	     << std::setw(5) << trajectory->GetInitialMomentum().y() << ", "
  // 	     << std::setw(5) << trajectory->GetInitialMomentum().z() << ") "
  // 	     << G4endl;
  //   }
  
      
  
  
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
  // tracks_.erase( tracks_.begin(), tracks_.end() );
  // steps_.erase( steps_.begin(), steps_.end() );
  
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

// TrackMC* EDEventAction::GetTrackMC( int id )
// {
//   for( auto& track : tracks_ )
//     if( track->GetID() == id )
//       return track;

//   G4cerr << " TrackMC* EDEventAction::GetTrackMC( int id )" << G4endl;
//   G4cerr << " The track ID " << id << " is not found in EDEventAction" << G4endl;
//   return nullptr;
// }

G4int EDEventAction::Z2Module( G4int z )
{

  if( z == 0 ) // most upstream ladder, which was not operated in the test beam experiment
    return -1;
  else if( z == 1 ) // upstream ladder
    return 1;
  else if( z == 2 ) // central ladder
    return 6;
  else if( z == 3 ) // downstream ladder
    return 5;

  return -1; // dummy
}

G4int EDEventAction::Edep2Dac( G4double energy )
{

  G4double gain = 100.0;
  G4double offset = 200.0;  
  G4double dac = 0.65 * (energy * 100 * 1.6 * gain / 3.6 + offset - 210. )/4.;

  return dac;  
}

G4int EDEventAction::Dac2Adc( G4int dac )
{

  if( dac < dac_values_[0] ) // for the case of dac < ADC0
    return -1;
  else if( dac_values_[ dac_values_.size() -1 ] <= dac ) // for the case of ADC7 < dac
    return dac_values_.size() - 1; 
  
  // loop over all DAC values to find the corresponding ADC value
  for( int i=0; i<dac_values_.size()-1; i++ )
    {
      if( dac >= dac_values_[i] && dac < dac_values_[i+1] ) // for the case of DAC[i] <= dac < DAC[i+1]
	return i-1;
    }
  
  return -999; // dummy
}

G4int EDEventAction::GetFemId( G4int module )
{
  if( module == 1 )
    return 1;
  else if( module == 5 || module == 6 )
    return 8;
  
  return 0; // dummy, it happen sometimes for noise hits
}

void EDEventAction::XY2ChipChan( G4int up_or_down, G4int silicon_type, G4int x, G4int y, G4int& chip_id, G4int& chan_id )
{

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Conversion for the chip ID                                                                             //
  // chip ID starts from the outer side (eta -1.1 or 1.1) type-B side,                                      //
  // while it starts from central side of a silicon sensor (chip col 5 for type-B, chip col 13 for type-A)  //
  // and goes to the outer side (opposite direction wrt the real definition).                               //
  // So                                                                                                     //
  //   - x should be multiplied with -1 to reverse the direction.                                           //
  //   - offset of 5 is needed in any cases. It means the number of chip column in type-B                   //
  //   - additional offset of 8 is needed for type-A                                                        //
  //   - additional offset of 13 is needed for the down side                                                //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( up_or_down == 0 ) // for down-side
    chip_id = 26 - x;  
  else // for upper-side
    chip_id = 13 - x;
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Conversion for the channel ID                                                                          //
  // In the case of chips in the down side, just use y.                                                     //
  // In the case of the upper side,                                                                         //
  //   - direction of channel ID need to be reversed                                                        //
  //   - offset of 127 is needed                                                                            //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  chan_id = y;  
  if( up_or_down == 1 ) // in the case of the upper side, channel ID should be modified
    chan_id = 127 - y;
    
}


void EDEventAction::FillINTTEvent( const G4Event* event )
{

  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  for( int i=0; i<sensor_IDs_.size(); i++ ) // loop over all hits colection of the silicon sensors
    {

      auto hc = GetHitsCollection( sensor_IDs_[i], event );
      //hc->PrintAllHits();
            
      for( int j=0; j<hc->entries(); j++ ) // loop over all hits in this hits collection
	{
	  auto hit = (*hc)[j]; // it's EDChamberHit class
	  G4double energy = hit->GetEnergyDeposit();

	  if( energy <= 0.0 ) // hits with finit energy deposit should pass this if statement
	    continue;
	  
	  //hit->Print();	  

	  G4int event_id = event->GetEventID();
	  G4int x = hit->Getxposition();
	  G4int y = hit->Getyposition();
	  G4int z = hit->Getzposition();

	  G4int up_or_down = hit->Getupordown();
	  G4int silicon_type = hit->Getsilicon_type(); // 0: type-A, 1: type-B	  
	  G4int chip_id = -1, chan_id = -1;
	  this->XY2ChipChan( up_or_down, silicon_type, x, y, chip_id, chan_id );
	  
	  G4int module = this->Z2Module( z );
	  G4int fem_id = this->GetFemId( module );
	  G4int dac = this->Edep2Dac( energy );
	  G4int adc = this->Dac2Adc( dac );	  

	  if( adc < 0 ) // skip this hit if the energy deposit is not 0 but too small
	    continue;
	  
	  const int INTT_tree_id = 4;	  
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 0, adc ); // adc, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 1, 0 ); // ampl, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 2, chip_id ); // chip_id
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 3, 0 ); // fpga_id, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 4, module ); // module, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 5, chan_id ); // chan_id
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 6, fem_id ); // fem_id, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 7, bco_ ); // bco, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 8, bco_full_ ); // bco_full, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 9, 0 ); // event, no need to do, just use Entry$

	  
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 10, event_id ); // event ID (MC)
	  analysisManager->FillNtupleDColumn(INTT_tree_id, 11, energy ); // energy deposit, MC truth, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 12, dac ); // DAC value converted from energy deposit, MC truth, done

	  auto track = hit->GetTrack();
	  auto particle = track->GetParticleDefinition();	  
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 13, track->GetTrackID() ); // track ID, MC truth
	  analysisManager->FillNtupleIColumn(INTT_tree_id, 14, particle->GetPDGEncoding() ); // track's partile ID, MC truth, done
	  analysisManager->FillNtupleDColumn(INTT_tree_id, 15, track->GetVertexKineticEnergy() ); //
	  analysisManager->FillNtupleDColumn(INTT_tree_id, 16, track->GetKineticEnergy() ); //
	  //analysisManager->FillNtupleDColumn(INTT_tree_id, 14, track->GetKineticEnergy() ); //

	  analysisManager->AddNtupleRow( INTT_tree_id );

	  adcs_.push_back( adc );
	  ampls_.push_back( 0 );
	  chip_ids_.push_back( chip_id );
	  fpga_ids_.push_back( 0 );
	  modules_.push_back( module );
	  chan_ids_.push_back( chan_id );
	  fem_ids_.push_back( fem_id );
	  bcos_.push_back( bco_ );
	  bco_fulls_.push_back( bco_full_ );
	  events_.push_back( 0 );

	  event_ids_MC_.push_back( event_id );
	  edeps_MC_.push_back( energy );
	  dacs_MC_.push_back( dac );
	  //hit->PrintEvent();
	  
	  /*
	  G4cout << setw(6) << j << " "
		 << "mod" << module << " "
	    //<< fem_id << " "
		 << hit->GetEnergyDeposit() << "\t"
		 << "DAC" << dac << "\t"
		 << "ADC" << adc << "\t"
		 << "BCO" << bco_ << "\t"
		 << "BCO full" << bco_full_ << "\t"
		 << "(" << x << ", " << y << ", " << z << ")\t"
		 << "chip" << chip_id << "\t"
		 << "chan" << chan_id
	    //		 << 
		 << G4endl;
	  */
	  
	} // end of the loop over all hits in this hits collection
    } // endl of the loop over all hits collection
  


}


void EDEventAction::CleanContainer()
{
  camac_adcs_.erase( camac_adcs_.begin(), camac_adcs_.end() );
  camac_tdcs_.erase( camac_tdcs_.begin(), camac_tdcs_.end() );

  adcs_.erase( adcs_.begin(), adcs_.end() );
  ampls_.erase( ampls_.begin(), ampls_.end() );
  chip_ids_.erase( chip_ids_.begin(), chip_ids_.end() );
  fpga_ids_.erase( fpga_ids_.begin(), fpga_ids_.end() );
  modules_.erase( modules_.begin(), modules_.end() );
  chan_ids_.erase( chan_ids_.begin(), chan_ids_.end() );
  fem_ids_.erase( fem_ids_.begin(), fem_ids_.end() );
  bcos_.erase( bcos_.begin(), bcos_.end() );
  bco_fulls_.erase( bco_fulls_.begin(), bco_fulls_.end() );
  events_.erase( events_.begin(), events_.end() );

  event_ids_MC_.erase( event_ids_MC_.begin(), event_ids_MC_.end() );
  edeps_MC_.erase( edeps_MC_.begin(), edeps_MC_.end() );
  dacs_MC_.erase( dacs_MC_.begin(), dacs_MC_.end() );
}
