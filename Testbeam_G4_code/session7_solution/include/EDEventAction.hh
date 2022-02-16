/// \file EDEventAction.hh
/// \brief Definition of the EDEventAction class

#ifndef EDEventAction_h
#define EDEventAction_h 1

#include <vector>

#include "G4UserEventAction.hh"
#include "G4RunManager.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4ios.hh"

#include "TTree.h"

#include "EDRunAction.hh"
#include "StepMC.hh"
#include "TrackMC.hh"

/// Event action class

class EDEventAction : public G4UserEventAction
{
public:
  EDEventAction();
  virtual ~EDEventAction();
  
  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);
  
  void AddTrackMC( TrackMC* track ){ tracks_.push_back( track );};
  void AddStepMC( StepMC* step ){ steps_.push_back( step );};

  TrackMC* GetTrackMC( int id );
  G4int GetStoredStepMCNum(){ return steps_.size();};
  G4int GetStoredTrackMCNum(){ return tracks_.size();};

  std::vector < G4int > camac_adc_;
  std::vector < G4int > camac_tdc_;
  
private:
  G4bool fVerbose;
  
  vector < TrackMC* > tracks_;
  vector < StepMC* > steps_;
  //vector < G4double > eer
  
};

#endif    
