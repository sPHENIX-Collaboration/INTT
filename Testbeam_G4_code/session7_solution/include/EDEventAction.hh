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
#include "CLHEP/Random/RandFlat.h"

#include "TTree.h"

#include "EDRunAction.hh"
#include "EDChamberHit.hh"
// #include "StepMC.hh"
// #include "TrackMC.hh"
#include "INTTMessenger.hh"

/// Event action class
class INTTMessenger;

class EDEventAction : public G4UserEventAction
{
public:
  EDEventAction( INTTMessenger* INTT_mess );
  virtual ~EDEventAction();
  
  virtual void  BeginOfEventAction(const G4Event* event);
  virtual void    EndOfEventAction(const G4Event* event);
  
  // void AddTrackMC( TrackMC* track ){ tracks_.push_back( track );};
  // void AddStepMC( StepMC* step ){ steps_.push_back( step );};

  // TrackMC* GetTrackMC( int id );
  // G4int GetStoredStepMCNum(){ return steps_.size();};
  // G4int GetStoredTrackMCNum(){ return tracks_.size();};

  std::vector < G4int > camac_adc_;
  std::vector < G4int > camac_tdc_;

  
private:
  G4bool fVerbose;
  G4int bco_;
  G4int bco_full_;
  
  std::vector < G4int > sensor_IDs_;
  std::vector < G4int > dac_values_;

  INTTMessenger* INTT_mess_;
  EDChamberHitsCollection* fHitsCollection_;  

  void FillINTTEvent( const G4Event* event );
  // vector < TrackMC* > tracks_;
  // vector < StepMC* > steps_;
  //vector < G4double > eer

  
  EDChamberHitsCollection* GetHitsCollection(G4int ID,
					     const G4Event* event) const;
  void GetHitsCollectionIDs();

  G4int Z2Module( G4int z );
  G4int Edep2Dac( G4double energy );
  G4int Dac2Adc( G4int dac );
  G4int GetFemId( G4int module );
  void XY2ChipChan( G4int up_or_down, G4int silicon_type, G4int x, G4int y, G4int& chip_id, G4int& chan_id );
  
};

#endif    
