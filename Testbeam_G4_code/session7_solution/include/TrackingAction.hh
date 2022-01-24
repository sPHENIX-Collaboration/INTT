/// \file electromagnetic/TestEm1/include/TrackingAction.hh
/// \brief Definition of the TrackingAction class

#ifndef TrackingAction_h
#define TrackingAction_h 1

#include <iostream>
#include <iomanip>

#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4UserTrackingAction.hh"
#include  "G4UImanager.hh"

//#include "B4Analysis.hh"
#include "EDEventAction.hh"
#include "EDPrimaryGeneratorAction.hh"
//#include "Run.hh"
//#include "HistoManager.hh"

#include "TrackMC.hh"

class EDPrimaryGeneratorAction;

class TrackingAction : public G4UserTrackingAction {

public:  
  //    TrackingAction(PrimaryGeneratorAction*);
  TrackingAction( EDPrimaryGeneratorAction* );
  ~TrackingAction() {};
  
  virtual void  PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);

  void Print( const G4Track* );
  
  
private:
  EDPrimaryGeneratorAction* fPrimary;
  int verbose = 0;

  //void Initialize();
};

#endif
