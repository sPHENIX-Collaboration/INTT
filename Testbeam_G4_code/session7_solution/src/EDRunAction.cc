/// \file EDRunAction.cc
/// \brief Implementation of the EDRunAction class

#include "EDRunAction.hh"
#include "EDAnalysis.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

G4double EDRunAction::beam_energy;
ELPHEBeam* EDRunAction::beam;

EDRunAction::EDRunAction( INTTMessenger* INTT_mess, EDPrimaryGeneratorAction* pga, EDEventAction* event ) : //, OutputManager* output ) : 
  G4UserRunAction(),
  INTT_mess_( INTT_mess ),
  event_( event ),
    event_id_( -1 ) // ,
  //vec(),
  // track_(),
  // tracks_(),
  // steps_(),
  //  output_( output )
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

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ntuple id =1 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ntuple id = 2
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  analysisManager->CreateNtuple("event_particle", "event_particle");
  analysisManager->CreateNtupleIColumn("PID_order");
  analysisManager->CreateNtupleIColumn("PID");
  analysisManager->CreateNtupleDColumn("PID_energy");
  analysisManager->CreateNtupleDColumn("post_process_id");
  analysisManager->CreateNtupleDColumn("particle_X");
  analysisManager->CreateNtupleDColumn("particle_Y");
  analysisManager->CreateNtupleDColumn("particle_Z");
  analysisManager->CreateNtupleDColumn("theta_in");
  analysisManager->CreateNtupleDColumn("theta_out");
  analysisManager->CreateNtupleDColumn("dtheta");
  analysisManager->CreateNtupleDColumn("phi_in");
  analysisManager->CreateNtupleDColumn("phi_out");  
  analysisManager->CreateNtupleIColumn("volume_type");
  analysisManager->CreateNtupleIColumn("Event_ID");
  analysisManager->FinishNtuple();

  // G4PhysicsListHelper::DumpOrdingParameterTable  :
  //         TypeName      ProcessType        SubType         AtRest      AlongStep        PostStep     Duplicable
  //   Transportation              1             91             -1              0              0  false
  //      CoupleTrans              1             92             -1              0              0  false
  //      CoulombScat              2              1             -1             -1           1000  false
  //       Ionisation              2              2             -1              2              2  false
  //            Brems              2              3             -1             -1              3  false
  //  PairProdCharged              2              4             -1             -1              4  false
  //            Annih              2              5              5             -1              5  false
  //      AnnihToMuMu              2              6             -1             -1              6  false
  //       AnnihToHad              2              7             -1             -1              7  false
  //     NuclearStopp              2              8             -1              8             -1  false
  //    ElectronSuper              2              9             -1              1              1  false
  //              Msc              2             10             -1              1             -1  false
  //         Rayleigh              2             11             -1             -1           1000  false
  //    PhotoElectric              2             12             -1             -1           1000  false
  //          Compton              2             13             -1             -1           1000  false
  //             Conv              2             14             -1             -1           1000  false
  //       ConvToMuMu              2             15             -1             -1           1000  false
  //       GammaSuper              2             16             -1             -1           1000  false
  //    PositronSuper              2             17              1              1              1  false
  //         Cerenkov              2             21             -1             -1           1000  false
  //    Scintillation              2             22           9999             -1           9999  false
  //         SynchRad              2             23             -1             -1           1000  false
  //         TransRad              2             24             -1             -1           1000  false
  //      SurfaceRefl              2             25             -1             -1           1000  false
  //         OpAbsorb              3             31             -1             -1           1000  false
  //       OpBoundary              3             32             -1             -1           1000  false
  //       OpRayleigh              3             33             -1             -1           1000  false
  //            OpWLS              3             34             -1             -1           1000  false
  //          OpMieHG              3             35             -1             -1           1000  false
  //           OpWLS2              3             36             -1             -1           1000  false
  //       DNAElastic              2             51             -1             -1           1000  false
  //         DNAExcit              2             52             -1             -1           1000  false
  //    DNAIonisation              2             53             -1             -1           1000  false
  //      DNAVibExcit              2             54             -1             -1           1000  false
  //    DNAAttachment              2             55             -1             -1           1000  false
  //     DNAChargeDec              2             56             -1             -1           1000  false
  //     DNAChargeInc              2             57             -1             -1           1000  false
  //      DNAElecSolv              2             58             -1             -1           1000  false
  //    DNAMolecDecay              6             59           1000             -1             -1  false
  //      ITTransport              1             60             -1              0              0  false
  //    DNABrownTrans              1             61             -1              0              0  false
  //    DNADoubleIoni              2             62             -1             -1           1000  false
  //     DNADoubleCap              2             63             -1             -1           1000  false
  //  DNAIoniTransfer              2             64             -1             -1           1000  false
  //       HadElastic              4            111             -1             -1           1000  false
  //     HadInelastic              4            121             -1             -1           1000  false
  //       HadCapture              4            131             -1             -1           1000  false
  //    MuAtomCapture              4            132             -1             -1           1000  false
  //       HadFission              4            141             -1             -1           1000  false
  //        HadAtRest              4            151           1000             -1             -1  false
  //           HadCEX              4            161             -1             -1           1000  false
  //            Decay              6            201           1000             -1           1000  false
  //       DecayWSpin              6            202           1000             -1           1000  false
  //      DecayPiSpin              6            203           1000             -1           1000  false
  //       DecayRadio              6            210           1000             -1           1000  false
  //     DecayUnKnown              6            211             -1             -1           1000  false
  //      DecayMuAtom              6            221           1000             -1           1000  false
  //         DecayExt              6            231           1000             -1           1000  false
  //      StepLimiter              7            401             -1             -1           1000  false
  //      UsrSepcCuts              7            402             -1             -1           1000  false
  //    NeutronKiller              7            403             -1             -1           1000  false
  //    ParallelWorld             10            491           9900              1           9900  true
	    
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ntuple id = 3
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  analysisManager->CreateNtuple("sci_trigger", "sci_trigger");
  analysisManager->CreateNtupleIColumn("Event_ID");
  analysisManager->CreateNtupleIColumn("sci_ID");
  analysisManager->CreateNtupleDColumn("sci_edep"); // colume id = 0
  analysisManager->FinishNtuple();    

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ntuple id = 4
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  analysisManager->CreateNtuple("tree", "tree");
  analysisManager->CreateNtupleIColumn( "adc"		); // column ID: 0
  analysisManager->CreateNtupleIColumn( "ampl"		); // column ID: 1
  analysisManager->CreateNtupleIColumn( "chip_id"	); // column ID: 2
  analysisManager->CreateNtupleIColumn( "fpga_id"	); // column ID: 3
  analysisManager->CreateNtupleIColumn( "module"	); // column ID: 4
  analysisManager->CreateNtupleIColumn( "chan_id"	); // column ID: 5
  analysisManager->CreateNtupleIColumn( "fem_id"	); // column ID: 6
  analysisManager->CreateNtupleIColumn( "bco"		); // column ID: 7
  analysisManager->CreateNtupleIColumn( "bco_full"	); // column ID: 8
  analysisManager->CreateNtupleIColumn( "event"		); // column ID: 9

  // MC truth
  analysisManager->CreateNtupleIColumn( "event_id_MC"	); // column ID: 10

  // hit information
  analysisManager->CreateNtupleDColumn( "edep_MC"	); // column ID: 11
  analysisManager->CreateNtupleIColumn( "dac_MC"	); // column ID: 12

  // track information
  
  analysisManager->CreateNtupleIColumn( "track_id_MC"	); // column ID: 13
  analysisManager->CreateNtupleIColumn( "track_pid_MC"	); // column ID: 14
  analysisManager->CreateNtupleDColumn( "track_energy_in_MC"	); // column ID: 15
  analysisManager->CreateNtupleDColumn( "track_energy_out_MC"	); // column ID: 16
  //  analysisManager->CreateNtupleDColumn( "track_energy_MC"	); // column ID: 14

    
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ntuple id = 5
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  analysisManager->CreateNtuple("tree_camac", "tree_camac");
  analysisManager->CreateNtupleIColumn( "camac_adc", event_->GetContainerCamacAdc() );
  analysisManager->CreateNtupleIColumn( "camac_tdc", event_->GetContainerCamacTdc() );
  analysisManager->CreateNtupleDColumn( "camac_edep_MC", event_->GetContainerCamacEdepMC() );
  analysisManager->CreateNtupleDColumn( "camac_time_MC", event_->GetContainerCamacTimeMC() );
  analysisManager->CreateNtupleDColumn( "camac_theta_MC", event_->GetContainerCamacThetaMC() );
  analysisManager->CreateNtupleDColumn( "camac_phi_MC", event_->GetContainerCamacPhiMC() );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // ntuple id = 6
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  analysisManager->CreateNtuple("tree_both", "tree_both");

  // CAMAC information (trigger sci.)
  analysisManager->CreateNtupleIColumn( "camac_adc", event_->GetContainerCamacAdc() ); 
  analysisManager->CreateNtupleIColumn( "camac_tdc", event_->GetContainerCamacTdc() );
  analysisManager->CreateNtupleDColumn( "camac_edep_MC", event_->GetContainerCamacEdepMC() ); 
  analysisManager->CreateNtupleDColumn( "camac_time_MC", event_->GetContainerCamacTimeMC() ); 
  analysisManager->CreateNtupleDColumn( "camac_theta_MC", event_->GetContainerCamacThetaMC() );
  analysisManager->CreateNtupleDColumn( "camac_phi_MC", event_->GetContainerCamacPhiMC() );

  // INTT hit
  analysisManager->CreateNtupleIColumn( "adc", event_->GetContainerAdc() ); 
  analysisManager->CreateNtupleIColumn( "ampl", event_->GetContainerAmpl() ); 
  analysisManager->CreateNtupleIColumn( "chip_id", event_->GetContainerChipId() ); 
  analysisManager->CreateNtupleIColumn( "fpga_id", event_->GetContainerFpgaId() ); 
  analysisManager->CreateNtupleIColumn( "module", event_->GetContainerModule() ); 
  analysisManager->CreateNtupleIColumn( "chan_id", event_->GetContainerChanId() ); 
  analysisManager->CreateNtupleIColumn( "fem_id", event_->GetContainerFemId() ); 
  analysisManager->CreateNtupleIColumn( "bco", event_->GetContainerBco() ); 
  analysisManager->CreateNtupleIColumn( "bco_full", event_->GetContainerBcoFull() ); 
  analysisManager->CreateNtupleIColumn( "event", event_->GetContainerEvent() ); 

  // INTT hit (MC truth)
  
  analysisManager->CreateNtupleIColumn( "event_id_MC", event_->GetContainerEventMC() );
  analysisManager->CreateNtupleDColumn( "edep_MC", event_->GetContainerEdepMC() );
  analysisManager->CreateNtupleIColumn( "dac_MC", event_->GetContainerDacsMC() );

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

  //  output_->Book();

  
  if( this->is_first )
    {
      beam_energy =  UImanager->GetCurrentDoubleValue( "/gun/energy" ) * GeV; // GeV is used whatever I give? changed to MeV
      is_first = false;
    }

  /*
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
  */
  
  // Open an output file
  G4String fileName = INTT_mess_->GetOutputPath();
  analysisManager->OpenFile(fileName);  

  this->beam = new ELPHEBeam( INTT_mess_ );
  
  if( 1<= INTT_mess_->GetDebugLevel() && INTT_mess_->IsSmearing())
    beam->Print();  
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

// void EDRunAction::ClearEventBuffer( string mode )
// {
//   if( mode == "TrackMC" || mode == "all" )
//     tracks_.erase( tracks_.begin(), tracks_.end() );

//   if( mode == "StepMC" || mode == "all" )
//     steps_.erase( steps_.begin(), steps_.end() );

// }

// void EDRunAction::SetTrackMCs( vector < TrackMC* >& tracks )
// {
//   this->ClearEventBuffer( "TrackMC" );
//   for( auto& track : tracks )
//     tracks_.push_back( track );

// }

// void EDRunAction::SetStepMCs( vector < StepMC* >& steps )
// {
//   this->ClearEventBuffer( "StepMC" );
//   for( auto& step : steps )
//     steps_.push_back( step );

// }
