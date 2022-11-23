/// \file EDEventAction.cc
/// \brief Implementation of the EDEventAction class

#include "EDEventAction.hh"

EDEventAction::EDEventAction( INTTMessenger* INTT_mess )
 : G4UserEventAction(),
   fVerbose(true),
   INTT_mess_( INTT_mess )
{}

EDEventAction::~EDEventAction()
{}

void EDEventAction::BeginOfEventAction(const G4Event* event)
{

  CleanContainer(); // for init of camac vectors
  
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


EDEmCalorimeterHitsCollection*
EDEventAction::GetTriggerHitsCollection(G4int ID,
				 const G4Event* event) const
{
  auto hitsCollection
    = static_cast<EDEmCalorimeterHitsCollection*>(
						  event->GetHCofThisEvent()->GetHC(ID) );

  if ( ! hitsCollection )
    {
      G4ExceptionDescription msg;
      msg << "Cannot access EmCalorimeterhitsCollection ID " << ID;
      G4Exception("EDEventAction::GetTriggerHitsCollection()",
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

	} // end of the loop over sensor columns
    } // end of the loop over sensor types

  //////////////////////////////////////////////////////////////////////////////
  // trigger scintillator                                                     //
  //////////////////////////////////////////////////////////////////////////////

  vector < string > trigger_hits_collection_names;
  trigger_hits_collection_names.push_back( "EmCalorimeterHitsCollection" );
  trigger_hits_collection_names.push_back( "EmCalorimeterHitsCollection_mini" );

  if( INTT_mess_->GetTriggerType() == 1 ) // only for the full setup
    trigger_hits_collection_names.push_back( "EmCalorimeterHitsCollection1" );
  
  for( auto& hc_name : trigger_hits_collection_names )
    {
      G4int id = G4SDManager::GetSDMpointer()->GetCollectionID( hc_name.c_str() );
      trigger_IDs_.push_back( id );

      trigger_name_IDs_.push_back( pair < string, G4int >( hc_name, id ) );
      
    } // end of the loop over all triigers

}

std::string EDEventAction::GetTriggerName( G4int id )
{
  for( auto& entry : trigger_name_IDs_ )
    {
      if( entry.second == id )
	return entry.first;
    }

  return "trigger_name_not_found";
}

void EDEventAction::EndOfEventAction(const G4Event* event)
{
  
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
  // Processes for the trigger scintillator hits                     //
  /////////////////////////////////////////////////////////////////////
  this->FillTriggerEvent( event );
  const int camac_tree_id = 4;

  bool trigger_flag = true;
  for( int i=0; i<camac_edeps_MC_.size()-1; i++ ){ // the last element in camac_edeps_MC_ is the mini sci., i.e. not trigger
    
    // typically, the energy deposit should be larger than 0.5 MeV. The case of the thin sci. not implemented yet
    if( camac_edeps_MC_[i] < 0.5 * MeV ){
      trigger_flag = false;
      break;
    }
  }

  // if the triggers are not fired, nothing saved
  if( trigger_flag == false ){
    this->CleanContainer();
    return;
  }

  analysisManager->AddNtupleRow( camac_tree_id );

  /////////////////////////////////////////////////////////////////////
  // Processes for the INTT hits                                     //
  /////////////////////////////////////////////////////////////////////
    this->FillINTTEvent( event );

  const int INTT_tree_both_id = 6;
  analysisManager->AddNtupleRow( INTT_tree_both_id );

  
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
}    

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

void EDEventAction::FillTriggerEvent( const G4Event* event )
{

  for( int i=0; i<trigger_IDs_.size(); i++ ) // loop over all hits colection of the silicon sensors
    {
      
      auto hc = GetTriggerHitsCollection( trigger_IDs_[i], event );      
      //hc->PrintAllHits();
      
      for( int j=0; j<hc->entries(); j++ ) // loop over all hits in this hits collection
	{
	  auto hit = (*hc)[j]; // it's EDChamberHit class
	  G4double energy = hit->GetEnergyDeposit();
	  
	  if( energy <= 0.0 ) // hits with finit energy deposit should pass this if statement
	    continue;
	  
	  G4int event_id = event->GetEventID();
	  G4int x = hit->Getxposition();
	  G4int y = hit->Getyposition();
	  G4int z = hit->Getzposition();

	  G4int trigger_id_in_tree = 0;
	  if( this->GetTriggerName( trigger_IDs_[i]).find( "mini" ) != std::string::npos )
	    {
	      trigger_id_in_tree= camac_edeps_MC_.size()-1; // mini sci., in the mostdownstream, assigned to the last element
	      
	    }
	  else
	    {
	      if( this->GetTriggerName(trigger_IDs_[i]).find( "HitsCollection1" ) != std::string::npos ) // for the thin sci., which was only in the full setup
		trigger_id_in_tree = 0; // thin sci., in the mostupstream, assigned to the first element
	      else // for the normal trigger sci.
		{
		  
		  if( INTT_mess_->GetTriggerType() == 0 ) // the normal setup (mini - sci_B - INTT - sci_C)
		    trigger_id_in_tree = 0;
		  else // the full setup (mini - sci_A - INTT - sci_B - sci_C )
		    trigger_id_in_tree = 1;
		  
		  trigger_id_in_tree += hit->GetLayerNumber(); // layer0 is upstream layer1

		} // end of if( this->GetTriggerName(trigger_IDs_[i]).find( "HitsCollection1" ) != std::string::npos )
	     	      
	    } // end of if( this->GetTriggerName( trigger_IDs_[i]).find( "mini" ) != std::string::npos )

	  //hit->Print();	  

	  camac_edeps_MC_[trigger_id_in_tree] = energy;
	  camac_timing_MC_[trigger_id_in_tree] = hit->GetTiming();
	  camac_theta_MC_[trigger_id_in_tree] = hit->GetTrackAngleTheta();
	  camac_phi_MC_[trigger_id_in_tree] = hit->GetTrackAnglePhi();
	  
	} // end of for( int j=0; j<hc->entries(); j++ )

    } // end of for( int i=0; i<trigger_IDs_.size(); i++ )
  
  
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

	  // hit position in the lab frame
	  G4double x_lab = hit->GetPosition().x();
	  G4double y_lab = hit->GetPosition().y();
	  G4double z_lab = hit->GetPosition().z();

	  G4double x = hit->Getxposition();
	  G4double y = hit->Getyposition();
	  G4double z = hit->Getzposition();

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
	  int column_id = 0;
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, adc ); // adc, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, 0 ); // ampl, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, chip_id ); // chip_id
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, 0 ); // fpga_id, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, module ); // module, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, chan_id ); // chan_id
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, fem_id ); // fem_id, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, bco_ ); // bco, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, bco_full_ ); // bco_full, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, 0 ); // event, no need to do, just use Entry$

	  
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, event_id ); // event ID (MC)
	  analysisManager->FillNtupleDColumn(INTT_tree_id, column_id++, energy ); // energy deposit, MC truth, done
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, dac ); // DAC value converted from energy deposit, MC truth, done
	  analysisManager->FillNtupleDColumn(INTT_tree_id, column_id++, x_lab ); // x coordinate of this hit in the lab frame (mm)
	  analysisManager->FillNtupleDColumn(INTT_tree_id, column_id++, y_lab ); // y coordinate of this hit in the lab frame (mm)
	  analysisManager->FillNtupleDColumn(INTT_tree_id, column_id++, z_lab ); // z coordinate of this hit in the lab frame (mm)

	  auto track = hit->GetTrack();
	  auto particle = track->GetParticleDefinition();	  
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, track->GetTrackID() ); // track ID, MC truth
	  analysisManager->FillNtupleIColumn(INTT_tree_id, column_id++, particle->GetPDGEncoding() ); // track's partile ID, MC truth, done
	  analysisManager->FillNtupleDColumn(INTT_tree_id, column_id++, track->GetVertexKineticEnergy() ); //
	  analysisManager->FillNtupleDColumn(INTT_tree_id, column_id++, track->GetKineticEnergy() ); //
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
  camac_edeps_MC_.erase( camac_edeps_MC_.begin(), camac_edeps_MC_.end() );
  camac_timing_MC_.erase( camac_timing_MC_.begin(), camac_timing_MC_.end() );
  camac_theta_MC_.erase( camac_theta_MC_.begin(), camac_theta_MC_.end() );
  camac_phi_MC_.erase  ( camac_phi_MC_.begin()  , camac_phi_MC_.end() );

  int trigger_num = 3;
  if( INTT_mess_->GetTriggerType() == 1 ) // only for the full setup
    trigger_num = 4;

  camac_edeps_MC_.resize( trigger_num );
  camac_timing_MC_.resize( trigger_num );
  camac_theta_MC_.resize( trigger_num );
  camac_phi_MC_.resize( trigger_num );

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
