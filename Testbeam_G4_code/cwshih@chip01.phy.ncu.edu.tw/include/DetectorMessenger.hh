/// \file DetectorMessenger.hh
/// \brief Definition of the DetectorMessenger class

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4GenericMessenger.hh"
//#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
  
#include "globals.hh"

#include "EDDetectorConstruction.hh"

class EDDetectorConstruction;
//class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADouble;

/// Messenger class that defines commands for DetectorConstruction.
///
/// It implements commands:
/// - /transform/setMethod method

//class DetectorMessenger: public G4UImessenger
class DetectorMessenger: public G4GenericMessenger
{
public:
  DetectorMessenger( void* obj, const G4String& dir, const G4String& doc, EDDetectorConstruction* detectorConstruction);

  ~DetectorMessenger();
    
  virtual void SetNewValue(G4UIcommand* command, G4String newValue);
    
private:
  EDDetectorConstruction*      fDetectorConstruction_;
  //G4UIdirectory*             fDirectory_;
  G4UIcmdWithAString*        fSetMethodCmd_;

  G4UIcmdWithoutParameter* constructGeometry_;
  G4UIcmdWithoutParameter* constructGeometry2_;
  G4UIcmdWithoutParameter* constructPlate_;
  G4UIcmdWithAString* setRotation_;
  
};

#endif

