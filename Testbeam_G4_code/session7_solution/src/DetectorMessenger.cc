/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class

#include "DetectorMessenger.hh"

//DetectorMessenger::DetectorMessenger( EDDetectorConstruction* detectorConstruction )
DetectorMessenger::DetectorMessenger( void* obj, const G4String& dir, const G4String& doc, EDDetectorConstruction* detectorConstruction )
  : G4GenericMessenger( obj, dir, doc ),
  fDetectorConstruction_(detectorConstruction)
    //  fDirectory_(0)
  //   fSetMethodCmd(0)
{
  
  // define command directory using generic messenger class
  // fMessenger = new G4GenericMessenger(this, "/HGCalTestbeam/setup/",
  //                                     "Configuration specifications");

  
  // fDirectory_ = new G4UIdirectory("/placement/");
  // fDirectory_->SetGuidance("Transform example detector control");

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

  fSetMethodCmd_
    = new G4UIcmdWithAString("/placement/setMethod",this);
  fSetMethodCmd_->SetGuidance("Select method for definition of transformations.");
  fSetMethodCmd_->SetParameterName("Method", false);
  fSetMethodCmd_->SetCandidates(
    "WithDirectMatrix WithInverseMatrix WithAxialRotations WithEulerAngles WithReflections");

    // fMessenger
    // = new G4GenericMessenger(this, "/INTT/geom/", "Commands for the geometry in this application");

  //////////////////////////////////////////////////////////////////////////////
  // Switch to construct the geometry
  // Usage: /INTT/geom/construct
  constructGeometry_ = new G4UIcmdWithoutParameter( "/INTT/geom/construct", this );

  //////////////////////////////////////////////////////////////////////////////
  // Switch to construct the additional plate
  // Usage: /INTT/geom/constructPlate
  // Note: Material, thickness, and distance for the plate need to be set in advance
  //  constructPlate_ = fMessenger->DeclareMethod( "constructPlate", &EDDetectorConstruction::ConstructPlate, "" );
  //  constructPlate_ = new G4GenericMessenger( "/INTT/geom/constructPlate", this );
  constructPlate_ = new G4UIcmdWithoutParameter( "/INTT/geom/constructPlate", this );

  //////////////////////////////////////////////////////////////////////////////
  // Switch to the vertically rotated setup or not
  setRotation_ = new G4UIcmdWithAString( "/INTT/geom/setRotation", this );
  setRotation_->SetGuidance( "Set the vertically rotated setup or not" );
  setRotation_->SetParameterName( "rotation", true ); // (name, is_omittable)
  setRotation_->SetDefaultValue( "none" );

  // # selection of the trigger scintillator configuration (but how?)
  // # Usage: /INTT/geom/???

}

DetectorMessenger::~DetectorMessenger()
{
  //  delete fDirectory_;
  delete fSetMethodCmd_;
  delete constructPlate_;
  delete setRotation_;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, 
                                                     G4String newValue)
{

  // if( command == fSetMethodCmd_ )
  //   {
  //     /*
  //     DetectorConstruction::EMethod method;
  //     if ( newValue == "WithDirectMatrix" )         
  // 	method = DetectorConstruction::kWithDirectMatrix;
  //     else if ( newValue == "WithInverseMatrix" )         
  // 	method = DetectorConstruction::kWithInverseMatrix;
  //     else if ( newValue == "WithAxialRotations" )     
  // 	method = DetectorConstruction::kWithAxialRotations;
  //     else if ( newValue == "WithEulerAngles" ) 
  // 	method = DetectorConstruction::kWithEulerAngles;
  //     else if ( newValue == "WithReflections" )    
  // 	method = DetectorConstruction::kWithReflections;
  //     else return;  

  //     fDetectorConstruction->SetMethod(method);
  //     */
  //   }
  // else if( command == constructGeometry_ )
  //   {
  //     fDetectorConstruction_->DoConstruction();
      
  //   }
  // else if( command == constructPlate_ )
  //   {
  //     fDetectorConstruction_->SetPlateConstruction( true );
      
  //   }
  
  
}
