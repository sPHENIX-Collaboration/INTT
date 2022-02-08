//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file EDRunAction.cc
/// \brief Implementation of the EDRunAction class

#include "EDRunAction.hh"
#include "EDAnalysis.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

G4double EDRunAction::beam_energy;
ELPHEBeam* EDRunAction::beam;

EDRunAction::EDRunAction( INTTMessenger* INTT_mess, EDPrimaryGeneratorAction* pga, EDEventAction* event, OutputManager* output ) : 
  G4UserRunAction(),
  INTT_mess_( INTT_mess ),
  event_( event ),
  event_id_( -1 ),
  vec(),
  track_(),
  tracks_(),
  steps_(),
  output_( output )
{

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(0);
  analysisManager->SetNtupleMerging(true);
  G4cout << "Using " << analysisManager->GetType() 
         << " analysis manager" << G4endl;

  //============================NEW one, only store the ID====================================================        
  analysisManager->CreateNtuple("Chamber1", "Chamber 1 hits");
  analysisManager->CreateNtupleIColumn("Event_ID");
  analysisManager->CreateNtupleIColumn("UpandDown");   // column id = 0 
  analysisManager->CreateNtupleIColumn("Xpos");    // column id = 1 
  analysisManager->CreateNtupleIColumn("Ypos");    // column id = 2
  analysisManager->CreateNtupleIColumn("Zpos");    // column id = 3 
  analysisManager->CreateNtupleIColumn("silicon_type");    // column id = 3   
  analysisManager->CreateNtupleDColumn("Edep");    // column id = 3 
  analysisManager->FinishNtuple();
  //============================NEW one, only store the ID====================================================

  //ntuple id =1 
  analysisManager->CreateNtuple("Beam_angle", "Beam_angle");
  analysisManager->CreateNtupleDColumn("Beam_X"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Y"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Z"); // colume id = 0 
  analysisManager->CreateNtupleDColumn("Beam_Theta"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Phi"); // colume id = 1
  analysisManager->CreateNtupleIColumn("Event_ID"); // colume id = 1
  analysisManager->CreateNtupleDColumn("Beam_Px"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Py"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Pz"); // colume id = 0 
  analysisManager->CreateNtupleDColumn("Beam_energy"); // in MeV
  analysisManager->FinishNtuple();

  // ntuple id = 2
  analysisManager->CreateNtuple("event_particle", "event_particle");
  analysisManager->CreateNtupleIColumn("PID_order"); // colume id = 0
  analysisManager->CreateNtupleIColumn("PID"); // colume id = 0
  analysisManager->CreateNtupleDColumn("PID_energy");
  analysisManager->CreateNtupleDColumn("particle_X"); // colume id = 0
  analysisManager->CreateNtupleDColumn("particle_Y");
  analysisManager->CreateNtupleDColumn("particle_Z");
  analysisManager->CreateNtupleIColumn("volume_type");
  analysisManager->CreateNtupleIColumn("Event_ID"); // colume id = 1
  analysisManager->FinishNtuple();

  // ntuple id = 3
  analysisManager->CreateNtuple("sci_trigger", "sci_trigger");
  analysisManager->CreateNtupleIColumn("Event_ID");
  analysisManager->CreateNtupleIColumn("sci_ID");
  analysisManager->CreateNtupleDColumn("sci_edep"); // colume id = 0
  analysisManager->FinishNtuple();    

  // ntuple id = 4
  analysisManager->CreateNtuple("tree_camac", "tree_camac");
  analysisManager->CreateNtupleIColumn( "camac_adc", event_->camac_adc_ ); // column Id = 0
  analysisManager->CreateNtupleIColumn( "camac_tdc", event_->camac_tdc_ ); // column Id = 1

  
  
  /*
  if( event_ )
    {
      //tf_output_ = new TFile( "test.root", "RECREATE" );
      //tf_output_ = std::shared_ptr < TFile >( "test.root", "RECREATE" );
      tf_output_ = std::make_shared< TFile >( "test.root", "RECREATE" );
      
      //tree_ = new TTree( "tree", "tree" );
      tree_ = std::make_shared< TTree >( "tree", "tree" );
      tree_->Branch( "event", &event_id_, "event/I" );
      //analysisManager->CreateNtuple( "TrackMC", "TrackMC" );
      tree_->Branch( "vec", &vec );
      //      tree_->Branch( "event", &event );      
      //tree_->Branch( "track", &track_ );
      tree_->Branch( "tracks", &tracks_ );
      tree_->Branch( "steps", &steps_ );

    }
  */
  
}

EDRunAction::~EDRunAction()
{
  delete G4AnalysisManager::Instance();  
}


void EDRunAction::BeginOfRunAction(const G4Run* kRun )
{

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();  
  auto UImanager = G4UImanager::GetUIpointer();

  output_->Book();
  
  if( this->is_first )
    {
      beam_energy =  UImanager->GetCurrentDoubleValue( "/gun/energy" ) * GeV; // GeV is used whatever I give? changed to MeV
      is_first = false;
    }

  // define output name
  G4String particle = UImanager->GetCurrentStringValue( "/gun/particle" );
  std::stringstream ss;
  ss << beam_energy / GeV;
  G4String energy   = ss.str() + "GeV";
  //  G4String event_num = UImanager->GetCurrentStringValue( "/gun/beamOn") + "events";

  // if energy is less than 1, it should be in MeV
  if( beam_energy < 1 ){
    std::stringstream ss2;
    ss2 << beam_energy << "MeV";
    energy =  ss.str();
  }

  // Open an output file
  G4String fileName = "ED";
  analysisManager->OpenFile(fileName);  

  auto beam_line = INTT_mess_->GetBeamLine();
  auto target = INTT_mess_->GetTarget();
  auto position_restriction = INTT_mess_->GetPositionRestriction();
  this->beam = new ELPHEBeam( INTT_mess_, beam_line, target, position_restriction, this->beam_energy );

  if( 1<= INTT_mess_->GetDebugLevel() && INTT_mess_->IsSmearing())
    beam->Print( 0 );  
}

void EDRunAction::EndOfRunAction(const G4Run* kRun )
{


  // save histograms 
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

  //output_->Save();
  //tf_output_->WriteTObject( tree_, tree_->GetName() );

  //  tf_output_->Close();

}

void EDRunAction::ClearEventBuffer( string mode )
{
  if( mode == "TrackMC" || mode == "all" )
    tracks_.erase( tracks_.begin(), tracks_.end() );

  if( mode == "StepMC" || mode == "all" )
    steps_.erase( steps_.begin(), steps_.end() );

}

void EDRunAction::SetTrackMCs( vector < TrackMC* >& tracks )
{
  this->ClearEventBuffer( "TrackMC" );
  for( auto& track : tracks )
    tracks_.push_back( track );

}

void EDRunAction::SetStepMCs( vector < StepMC* >& steps )
{
  this->ClearEventBuffer( "StepMC" );
  for( auto& step : steps )
    steps_.push_back( step );

}
