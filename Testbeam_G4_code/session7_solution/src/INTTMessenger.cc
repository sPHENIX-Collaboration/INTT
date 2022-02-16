/// \file INTTMessenger.cc
/// \brief Implementation of the INTTMessenger class

#include "INTTMessenger.hh"

INTTMessenger::INTTMessenger()
  : G4GenericMessenger( 0 ),
    is_plate_( false ),
    trigger_type_( 0 ),
    rotation_status_( 0 ),
    is_beam_smearing_( false ),
    darkbox_offset_x_( 0 ),
    darkbox_offset_y_( 0 ),
    output_name_( "" ),
    fDirectory_(0)
{
}

INTTMessenger::INTTMessenger( void* obj, const G4String& dir, const G4String& doc )
  : G4GenericMessenger( obj, dir, doc ),
    is_plate_( false ),
    trigger_type_( 0 ),
    rotation_status_( 0 ),
    is_beam_smearing_( false ),
    darkbox_offset_x_( 0 ),
    darkbox_offset_y_( 0 ),
    output_name_( "" ),
    fDirectory_(0)
{
  
  fDirectory_ = new G4UIdirectory("/INTT/");
  fDirectory_->SetGuidance("Commands for INTT");

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
  // Seeting of the dark box offset in x direction
  darkbox_offset_x_command_ = new G4UIcmdWithADouble( "/INTT/geom/setDarkboxOffsetX", this );
  darkbox_offset_x_command_->SetGuidance( "Set the horixontal offset of the dark box position" );
  darkbox_offset_x_command_->SetParameterName( "darkbox_offset_x", true ); // (name, is_omittable)
  darkbox_offset_x_command_->SetDefaultValue( 0.0 );

  //////////////////////////////////////////////////////////////////////////////
  // Seeting of the beam offset in x direction
  darkbox_offset_y_command_ = new G4UIcmdWithADouble( "/INTT/geom/setDarkboxOffsetY", this );
  darkbox_offset_y_command_->SetGuidance( "Set the horixontal offset of the darkbox position" );
  darkbox_offset_y_command_->SetParameterName( "darkbox_offset_y", true ); // (name, is_omittable)
  darkbox_offset_y_command_->SetDefaultValue( 0.0 );

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
  // Parameters for something else                                            //
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Switch for the trigger scintillators
  debug_level_command_ = new G4UIcmdWithAnInteger( "/INTT/misc/debugLevel", this );
  debug_level_command_->SetGuidance( "Debug level. 0: not debug, 1: , 2: only this messenger" );
  debug_level_command_->SetParameterName( "DebugLevel", true ); // (name, is_omittable)
  debug_level_command_->SetDefaultValue( 0 );
  debug_level_command_->SetRange( "0<=DebugLevel && DebugLevel<=2" );

  // command to put additional tag to the name of the output file
  output_command_ = new G4UIcmdWithAString( "/INTT/misc/setOutputTag", this );
  output_command_->SetGuidance( "Set the name of the output file. " );
  output_command_->SetParameterName( "output_tag", true ); // (name, is_omittable)
  output_command_->SetDefaultValue( "none" );

}

INTTMessenger::~INTTMessenger()
{
  delete fDirectory_;
  delete plate_command_;
  delete rotation_command_;
  delete beam_smearing_command_;
  delete beam_file_command_;
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
  else if( command == darkbox_offset_x_command_ )
    {
      darkbox_offset_x_ = darkbox_offset_x_command_->GetNewDoubleValue( val ) * CLHEP::mm;
    }
  else if( command == darkbox_offset_y_command_ )
    {
      darkbox_offset_y_ = darkbox_offset_y_command_->GetNewDoubleValue( val ) * CLHEP::mm;
    }
  else if( command == beam_smearing_command_ )
    {
      is_beam_smearing_ = beam_smearing_command_->GetNewBoolValue( val );
    }
  else if( command == beam_file_command_ )
    {
      beam_file_ = val;
    }
  else if( command == debug_level_command_ )
    {
      debug_level_ = debug_level_command_->GetNewIntValue( val );
    }
  else if( command == output_command_ )
    {
      output_tag_ = val;
    }
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

  G4cout << "--- Misc ---" << G4endl;
  G4cout << "  G4int  debug_level_: " << debug_level_ << endl;

  G4cout << std::string( 50, '-' ) << G4endl;


  auto physics_list_helper = G4PhysicsListHelper::GetPhysicsListHelper();
  //physics_list_helper->DumpOrdingParameterTable();
  
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


/*!
  @fn std::string INTTMessenger::GetOutputPath()
  @brief A name of the output file is returned.
  @details The format of the output file name is :
  [beam particle]_[beam energy + unit / beamfile]_[Pb plate or not]_[rotation setup]_[debug or not]_[user's tag]_[numbering].root
  Here:
  [beam particle] : the name is taken from the macro command /gun/particle. For example, e+
  _[beam energy + unit / beamfile]: If /INTT/beam/beamSmearing true is used, "_beamfile" is used. Or [beam_energy] + unit is applied. The value is taken from the macro command /gun/energy. It works only for MeV and GeV.
  _[Pb plate or not] : It depends on /INTT/geom/isPlate. If true, "_PbPlate" is added. Otherwise nothing added.
  _[rotation setup] : It depends on /INTT/geom/setRotation. In the case of "none", nothing added. "vertical" and "horizontal" cases add "_vrot" and "_hrot", respectively.
  _[debug or not] : It depends on /INTT/misc/debugLevel. If 0, nothing added. Oterwise, "_debug" is added.
  _[user's tag] : It depends on /INTT/misc/setOutputTag. If "none" is given, nothing added. Otherwise, "_" and the value given to the macro command is added.
  _[numbering]: ID starts from 0.

  For example: e+_beamfile_hrot_test_1.root
 */
std::string INTTMessenger::GetOutputPath()
{
  if( output_name_ != "" )
    return output_name_;
  
  std::string rtn = "";

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();  
  auto UImanager = G4UImanager::GetUIpointer();

  // define output name
  std::string particle = UImanager->GetCurrentStringValue( "/gun/particle" );
  rtn += particle;
  
  if( this->IsSmearing() )
    {
      rtn += "_beam-para";
    }
  else
    {

      G4double beam_energy = UImanager->GetCurrentDoubleValue( "/gun/energy" ); // GeV is used whatever I give? changed to MeV

      std::string energy_unit = UImanager->GetCurrentStringValue( "/gun/energy", 2 ); // it's always GeV somewhow

      // if energy is less than 1, it should be in MeV
      if( beam_energy < 1 )
	{
	  beam_energy *= GeV * MeV;
	  energy_unit = "MeV";
	}
      
      rtn += "_" + to_string( int(beam_energy) ) + energy_unit;
    }

  // std::string event_num = UImanager->GetCurrentStringValue( "/gun/beamOn") + "events";
  // rtn += event_num;

  if( this->IsPlate() )
    rtn += "_PbPlate";
  
  if( this->IsVerticalRotation() )
    rtn += "_vrot";
  else if( this->IsHorizontalRotation() )
    rtn += "_hrot";

  if( debug_level_ > 0 )
    rtn += "_debug";
  
  if( output_tag_ != "none" && output_tag_ != "" )
    rtn += "_" + output_tag_;

  // put numbering
  std::string temp = rtn + "_0";

  if( this->DoesExist( temp + ".root" ) )
    {
      int counter = 1;
      std::string name_candidate = temp.substr(0, temp.size()-1)  + to_string( counter ) + ".root";

      while( !this->DoesExist( name_candidate ) )
	{
	  //G4cout << " ---> " << this->DoesExist( name_candidate ) << G4endl;
	  counter++;
	  name_candidate = temp.substr(0, temp.size()-1)  + to_string( counter ) + ".root";
	  //G4cout << counter << "\t" << name_candidate ;
	}

      output_name_ = name_candidate;
      G4cout << "Output file: " << output_name_ << G4endl;
      return output_name_;
    }
  
  temp += ".root";
  output_name_ = temp;
  //G4cout << "Output file: " << output_name_ << G4endl;
  return temp;
}

G4bool INTTMessenger::DoesExist( std::string name )
{

  std::ifstream ifs( name.c_str(), std::ios::in );

  G4bool rtn = ifs.fail();
  ifs.close();

  return rtn;
}
