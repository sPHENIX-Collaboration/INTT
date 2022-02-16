/// \file INTTMessenger.hh
/// \brief Definition of the INTTMessenger class

#ifndef INTTMessenger_h
#define INTTMessenger_h 1

#include <iostream>

#include "G4GenericMessenger.hh"
//#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
//#include "G4AnalysisManager.hh"
#include "G4UImanager.hh"
#include "G4PhysicsListHelper.hh"
#include "globals.hh"

#include "EDDetectorConstruction.hh"

class EDDetectorConstruction;
//class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADouble;

/// Messenger class for INTT configurations

//class INTTMessenger: public G4UImessenger
class INTTMessenger: public G4GenericMessenger
{
public:
  INTTMessenger();
  INTTMessenger( void* obj, const G4String& dir, const G4String& doc );

  ~INTTMessenger();
    
  virtual void SetNewValue(G4UIcommand* command, G4String newValue);

  void Print();

  // for geometry
  G4bool GetTriggerType(){ return trigger_type_;};
  G4bool IsPlate(){ return is_plate_;};
  G4bool IsVerticalRotation();
  G4bool IsHorizontalRotation();

  // for beam
  G4bool   IsSmearing(){ return is_beam_smearing_;};
  G4String GetBeamFile(){ return beam_file_;};
  G4double GetDarkboxOffsetX(){ return darkbox_offset_x_; };
  G4double GetDarkboxOffsetY(){ return darkbox_offset_y_; };

  // for something else
  G4int GetDebugLevel(){ return debug_level_;};
  std::string GetOutputPath();
  
private:
  G4UIdirectory*             fDirectory_;

  // for the geometry
  G4UIcmdWithAnInteger* trigger_type_command_;
  G4int trigger_type_;
  
  G4UIcmdWithABool* plate_command_;
  G4bool is_plate_;

  G4UIcmdWithAString* rotation_command_;
  G4int rotation_status_; // 0: none, 1: vertical, 2: horizontal
  
  G4UIcmdWithADouble* darkbox_offset_x_command_;
  G4double darkbox_offset_x_;

  G4UIcmdWithADouble* darkbox_offset_y_command_;
  G4double darkbox_offset_y_;

  // for the beam
  G4UIcmdWithABool* beam_smearing_command_;
  G4bool is_beam_smearing_;

  G4UIcmdWithAString* beam_file_command_;
  G4String beam_file_;

  // for misc
  G4UIcmdWithAnInteger* debug_level_command_;
  G4int debug_level_; // 0: not debug, 1: 

  G4UIcmdWithAString* output_command_;
  std::string output_name_;
  std::string output_tag_;

  G4bool DoesExist( std::string name );
};

#endif

