/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"

SteppingAction::SteppingAction(
                      const EDDetectorConstruction* detectorConstruction)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction)
{
}

SteppingAction::~SteppingAction()
{ 
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  /*
  // how can I get it when given by use?
  auto ui_manager = G4UImanager::GetUIpointer();
  verbose = ui_manager->GetCurrentIntValue( "/event/verbose", 1, true );

  auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
  EDEventAction* event_action = (EDEventAction*)G4RunManager::GetRunManager()->GetUserEventAction();

  // I'm not sure which point should be used, pre or post
  auto pre_step_point			= step->GetPreStepPoint(); // G4StepPoint*
  auto post_step_point			= step->GetPostStepPoint(); // G4StepPoint*

  StepMC* step_mc = new StepMC( event_action->GetStoredStepMCNum() );
  // Get the track that this step belongs to
  auto track				= step->GetTrack(); // G4Track
  step_mc->SetTrackID( track->GetTrackID() );

  // Collect energy and track length step by step
  // get volume of the c_urrent step
  auto volume = pre_step_point->GetTouchableHandle()->GetVolume(); // G4PhysicalVolume
  //  G4cout << step_mc->GetID() << " " << volume->GetName() << " " << volume->GetCopyNo() << G4endl;
  step_mc->SetVolumeName( volume->GetName() );
  step_mc->SetVolumeCopyNo( volume->GetCopyNo() );
 step_mc->SetVolumeMaterial( pre_step_point->GetMaterial()->GetName() );
  
  // energy deposit  
  step_mc->SetStepLength( step->GetStepLength() );
  step_mc->SetEnergyDeposit( step->GetTotalEnergyDeposit() );

  if( pre_step_point->GetProcessDefinedStep() != nullptr )
    step_mc->SetInteraction( pre_step_point->GetProcessDefinedStep()->GetProcessName() );
  
  auto position_pre = pre_step_point->GetPosition();
  auto momentum_pre = pre_step_point->GetMomentum();
  auto position_post = post_step_point->GetPosition();
  auto momentum_post = post_step_point->GetMomentum();

  step_mc->SetMomentum( momentum_post.x(), momentum_post.y(), momentum_post.z() );
  step_mc->SetPosition( position_post.x(), position_post.y(), position_post.z() );
  
  step_mc->SetMomentumPre( momentum_pre.x(), momentum_pre.y(), momentum_pre.z() );
  step_mc->SetPositionPre( position_pre.x(), position_pre.y(), position_pre.z() );
  
  auto step_delta_momentum		= momentum_post - momentum_pre;
  step_mc->SetDeltaMomentum( step_delta_momentum.x(), step_delta_momentum.y(), step_delta_momentum.z() );

  auto step_delta_position		= position_post - position_pre;
  step_mc->SetDeltaPosition( step_delta_position.x(), step_delta_position.y(), step_delta_position.z() );

  step_mc->SetDeltaTime( step->GetDeltaTime() );
  step_mc->SetTime( pre_step_point->GetGlobalTime() ); // it may be local time or proper time. I'm not sure

  auto step_secondary_in_current_step	= step->GetSecondaryInCurrentStep(); // vector
  auto step_secondary			= step->GetSecondary(); // G4TrackVector -> vector < G4Track* >*
  //  auto step_std::vector< G4ThreeVector > * 	step->GetPointerToVectorOfAuxiliaryPoints()  // ?

  // G4cout << "SteppingAction, Step#" << step_mc->GetID() << " "
  // 	 << "p = ("
  // 	 << setw(10) << setprecision(4) << vertex_momentum.x() << ", "
  // 	 << setw(10) << setprecision(4) << vertex_momentum.y() << ", "
  // 	 << setw(10) << setprecision(4) << vertex_momentum.z() << ") "
  // 	 << G4endl;
  
  // TVector3* vertex_position_root = new TVector3( vertex_position.x(), vertex_position.y(), vertex_position.z() );
  // TVector3* vertex_momentum_root = new TVector3( vertex_momentum.x(), vertex_momentum.y(), vertex_momentum.z() );
  //track_stored->AddVertexMomentum( vertex_position_root, vertex_momentum_root );


  /*
  G4cout << "SteppingAction: " << step_energy_deposit << " MeV ";
  if( step_secondary->size() != 0 )
    {
      auto vertex_position = (*step_secondary)[0]->GetVertexPosition(); // CLHEP::Hep3Vector
      G4cout << " seconday pos ("
	     << std::setw(7) << std::setprecision(4) << vertex_position.x() << ", "
	     << std::setw(7) << std::setprecision(4) << vertex_position.y() << ", "
	     << std::setw(7) << std::setprecision(4) << vertex_position.z() << ")";

    }
  
  G4cout << G4endl;
  * /
  
  //track->Print();
  
  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  //  step_mc->Print( 2 );
  event_action->AddStepMC( step_mc );
  */
  
}
