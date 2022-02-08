/// \file INTTMessenger.cc
/// \brief Implementation of the INTTMessenger class

#include "INTTMessenger.hh"

INTTMessenger::INTTMessenger()
  : G4GenericMessenger( 0 ),
    is_plate_( false ),
    trigger_type_( 0 ),
    rotation_status_( 0 ),
    is_beam_smearing_( false ),
    beam_line_( "-23" ),
    target_( "W_200um" ),
    position_restriction_( 0 ),
    fDirectory_(0)
{
}

INTTMessenger::INTTMessenger( void* obj, const G4String& dir, const G4String& doc )
  : G4GenericMessenger( obj, dir, doc ),
    is_plate_( false ),
    trigger_type_( 0 ),
    rotation_status_( 0 ),
    is_beam_smearing_( false ),
    beam_line_( "-23" ),
    target_( "W_200um" ),
    position_restriction_( 0 ),
    fDirectory_(0)
{
  
  fDirectory_ = new G4UIdirectory("/INTT/");
  fDirectory_->SetGuidance("Commands for INTT");

  /*
  // configuration command
  auto &configCmd = fMessenger->DeclareMethod(
      "configuration", &DetectorConstruction::SelectConfiguration,
      "Select the configuration (0 for HGCal test beam, 1 for same HGCal"
      " with beamline (upstream material), or 2 for simple test setup)");
  configCmd.SetParameterName("index", true);
  configCmd.SetDefaultValue("0");
  
  auto &configCmd = this->DeclareMethod(
					"/INTT/geom/construct2", &EDDetectorConstruction::DoConstruction,
					"Select the configuration (0 for HGCal test beam, 1 for same HGCal"
					" with beamline (upstream material), or 2 for simple test setup)");
  configCmd.SetParameterName("index", true);
  configCmd.SetDefaultValue("0");
  */

  //////////////////////////////////////////////////////////////////////////////
  // Parameters for the geometry                                              //
  //////////////////////////////////////////////////////////////////////////////
  // fSetMethodCmd_
  //   = new G4UIcmdWithAString("/placement/setMethod",this);
  // fSetMethodCmd_->SetGuidance("Select method for definition of transformations.");
  // fSetMethodCmd_->SetParameterName("Method", false);
  // fSetMethodCmd_->SetCandidates(
  //   "WithDirectMatrix WithInverseMatrix WithAxialRotations WithEulerAngles WithReflections");

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the trigger scintillators
  trigger_type_command_ = new G4UIcmdWithAnInteger( "/INTT/geom/triggerType", this );
  trigger_type_command_->SetGuidance( "Setup of the trigger scintillator. 0: sci_D sci_B INTTs sci_C, 1: sci_D sci_A INTTs sci_B sci_C" );
  trigger_type_command_->SetParameterName( "TriggerType", true ); // (name, is_omittable)
  trigger_type_command_->SetDefaultValue( 0 );
  trigger_type_command_->SetRange( "0<=TriggerType && TriggerType<=1" );
  
  //////////////////////////////////////////////////////////////////////////////
  // Switch to construct the additional plate
  // Usage: /INTT/geom/constructPlate
  // Note: Material, thickness, and distance for the plate need to be set in advance
  //  plate_command_ = fMessenger->DeclareMethod( "constructPlate", &EDDetectorConstruction::ConstructPlate, "" );
  //  plate_command_ = new G4GenericMessenger( "/INTT/geom/constructPlate", this );
  plate_command_ = new G4UIcmdWithABool( "/INTT/geom/isPlate", this );
  plate_command_->SetParameterName( "isPlate", false, false );
  plate_command_->SetGuidance( "Switch to construct the additional lead plate(true) or not(false)" );
  
  //////////////////////////////////////////////////////////////////////////////
  // Switch to the vertically rotated setup or not
  rotation_command_ = new G4UIcmdWithAString( "/INTT/geom/setRotation", this );
  rotation_command_->SetGuidance( "Set the vertically/horixontally rotated setup or not" );
  rotation_command_->SetParameterName( "rotation", true ); // (name, is_omittable)
  rotation_command_->SetDefaultValue( "none" );
  rotation_command_->SetCandidates( "none vertical horizontal" );

  //////////////////////////////////////////////////////////////////////////////
  // Parameters for the beam                                                  //
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Switch to use mono-energy beam(false) or more realistic beam(true)
  beam_smearing_command_ = new G4UIcmdWithABool( "/INTT/beam/beamSmearing", this );
  beam_smearing_command_->SetGuidance( "Switch to the realistic beam(true) or mono-energy beam at x=0 & y=0(false)." );
  beam_smearing_command_->SetParameterName("beamSmearing", false, false );

  //////////////////////////////////////////////////////////////////////////////
  // Beam file selection
  beam_file_command_ = new G4UIcmdWithAString( "/INTT/beam/beamFile", this );
  beam_file_command_->SetGuidance( "Selection of the beam file to be used." );
  beam_file_command_->SetParameterName( "BeanFile", true ); // (name, is_omittable)
  //beam_file_command_->SetDefaultValue( "beam_parameter/beam_W_400A.dat" );
  //beam_line_command_->SetCandidates( "beam_parameter/beam_W_400A.dat beam_parameter/beam_W_400A_original.dat beam_parameter/beam_W_400A_mini.dat" );

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the beam line
  beam_line_command_ = new G4UIcmdWithAString( "/INTT/beam/beamLine", this );
  beam_line_command_->SetGuidance( "Selection of the beam line. -23 and -30 are available." );
  beam_line_command_->SetParameterName( "BeamLine", true ); // (name, is_omittable)
  beam_line_command_->SetDefaultValue( "-23" );
  beam_line_command_->SetCandidates( "-23 -30" );

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the beam line
  target_command_ = new G4UIcmdWithAString( "/INTT/beam/target", this );
  target_command_->SetGuidance( "Selection of the production target: Au_20um, W_200um are available. (Cu_8mm not ready)" );
  target_command_->SetParameterName( "Target", true ); // (name, is_omittable)
  target_command_->SetDefaultValue( "W_200um" );
  target_command_->SetCandidates( "Au_20um W_200um" );

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the x_p limitation (restriction of horizontal position of the beam, see the ELPH report)
  position_restriction_command_ = new G4UIcmdWithAnInteger( "/INTT/beam/positionRestriction", this );
  position_restriction_command_->SetGuidance( "Selection of position restrictioin of the beam. 0(no), 1(weak), 2(moderate), and 3(strong) are available." );
  position_restriction_command_->SetParameterName( "PositionRestriction", true ); // (name, is_omittable)
  position_restriction_command_->SetDefaultValue( 0 );
  position_restriction_command_->SetRange( "0<=PositionRestriction && PositionRestriction<=3" );

  //////////////////////////////////////////////////////////////////////////////
  // Parameters for something else                                            //
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Switch for the trigger scintillators
  debug_level_command_ = new G4UIcmdWithAnInteger( "/INTT/misc/debugLevel", this );
  debug_level_command_->SetGuidance( "Debug level. 0: not debug, 1: , 2: only this messenger" );
  debug_level_command_->SetParameterName( "DebugLevel", true ); // (name, is_omittable)
  debug_level_command_->SetDefaultValue( 0 );
  debug_level_command_->SetRange( "0<=DebugLevel && DebugLevel<=2" );
  //aCmd->SetRange("x>0. && y>z && z<(x+y)");
}

INTTMessenger::~INTTMessenger()
{
  delete fDirectory_;
  delete plate_command_;
  delete rotation_command_;
  delete beam_smearing_command_;
  delete beam_file_command_;
  delete beam_line_command_;
  delete trigger_type_command_;
  delete position_restriction_command_;
  delete debug_level_command_;

}

void INTTMessenger::SetNewValue(G4UIcommand* command, G4String val)
{

  if( command == trigger_type_command_ )
    {
      trigger_type_ = trigger_type_command_->GetNewIntValue( val );
    }
  else if( command == plate_command_ )
    {
      is_plate_ = plate_command_->GetNewBoolValue( val );
    }
  else if( command == rotation_command_ )
    {
      
      if( val == "none" )
	rotation_status_ = 0;
      else if( val == "vertical" )
	rotation_status_ = 1;
      else if( val == "horizontal" )
	rotation_status_ = 2;
      
    }
  else if( command == beam_smearing_command_ )
    {
      is_beam_smearing_ = beam_smearing_command_->GetNewBoolValue( val );
    }
  else if( command == beam_file_command_ )
    {
      beam_file_ = val;
      G4cout << beam_file_ << G4endl;
    }
  else if( command == beam_line_command_ )
    {
      beam_line_ = val;
    }
  else if( command == target_command_ )
    {
      target_ = val;
    }
  if( command == position_restriction_command_ )
    {
      position_restriction_ = position_restriction_command_->GetNewIntValue( val );
    }
  else if( command == debug_level_command_ )
    {
      debug_level_ = debug_level_command_->GetNewIntValue( val );
    }
  // if( command == fSetMethodCmd_ )
  //   {
  //     /*
  //     DetectorConstruction::EMethod method;
  //     if ( val == "WithDirectMatrix" )         
  // 	method = DetectorConstruction::kWithDirectMatrix;
  //     else if ( val == "WithInverseMatrix" )         
  // 	method = DetectorConstruction::kWithInverseMatrix;
  //     else if ( val == "WithAxialRotations" )     
  // 	method = DetectorConstruction::kWithAxialRotations;
  //     else if ( val == "WithEulerAngles" ) 
  // 	method = DetectorConstruction::kWithEulerAngles;
  //     else if ( val == "WithReflections" )    
  // 	method = DetectorConstruction::kWithReflections;
  //     else return;  

  //     fDetectorConstruction->SetMethod(method);
  //     */
  //   }
  
  
}

void INTTMessenger::Print()
{
  G4cout << std::string( 50, '-' ) << G4endl;
  G4cout << "INTTMessenger:Print()" << G4endl;

  G4cout << "--- Geometry ---" << G4endl;
  G4cout << "  G4int  trigger_type_: " << trigger_type_ << endl;
  G4cout << "  G4bool is_plate_: " << is_plate_ << endl;
  G4cout << "  G4int  rotation_status_: " << rotation_status_ << endl;

  G4cout << "--- Beam ---" << G4endl;
  G4cout << "  G4bool is_beam_smearing: " <<   is_beam_smearing_ << endl;
  G4cout << "  G4String beam_line_: " << beam_line_ << endl;
  G4cout << "  G4String target_: " << target_ << endl;
  G4cout << "  G4int  position_restriction_: " << position_restriction_ << endl;

  G4cout << "--- Misc ---" << G4endl;
  G4cout << "  G4int  debug_level_: " << debug_level_ << endl;

 G4cout << std::string( 50, '-' ) << G4endl;

}

G4bool INTTMessenger::IsVerticalRotation()
{
  if( rotation_status_ == 1 )
    return true;

  return false;
}


G4bool INTTMessenger::IsHorizontalRotation()
{
  if( rotation_status_ == 2 )
    return true;

  return false;
}
