/// \file SteppingAction.hh
/// \brief Definition of the SteppingAction class

#ifndef SteppingAction_h
#define SteppingAction_h 1

// STD headers
#include <utility>

// Geant4 headers
#include "G4UserSteppingAction.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VProcess.hh"

// ROOT headers
#include "TVector3.h"

// headers for this application
//#include "RunData.hh"
#include "EDDetectorConstruction.hh"
#include "EDEventAction.hh"

#include "StepMC.hh"

class B4DetectorConstruction;

/// Stepping action class.
///
/// In UserSteppingAction() there are collected the energy deposit and track 
/// lengths of charged particles in Absober and Gap layers and
/// updated in RunData object.

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(const EDDetectorConstruction* detectorConstruction);
  virtual ~SteppingAction();

  virtual void UserSteppingAction(const G4Step* step);
    
private:
  const EDDetectorConstruction* fDetConstruction;
  int verbose = 0;
};

#endif
