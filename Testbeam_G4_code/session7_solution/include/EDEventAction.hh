/// \file EDEventAction.hh
/// \brief Definition of the EDEventAction class

#ifndef EDEventAction_h
#define EDEventAction_h 1

#include <vector>
#include <numeric>

#include "G4UserEventAction.hh"
#include "G4RunManager.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4ios.hh"
#include "CLHEP/Random/RandFlat.h"

//#include "TTree.h"

#include "EDRunAction.hh"
#include "EDChamberHit.hh"
#include "EDEmCalorimeterHit.hh"
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

  std::vector < G4int >& GetContainerCamacAdc(){ return camac_adcs_;};
  std::vector < G4int >& GetContainerCamacTdc(){ return camac_tdcs_;};
  std::vector < G4double >& GetContainerCamacEdepMC(){ return camac_edeps_MC_;};
  std::vector < G4double >& GetContainerCamacTimeMC(){ return camac_timing_MC_;};
  std::vector < G4double >& GetContainerCamacThetaMC(){ return camac_theta_MC_;};
  std::vector < G4double >& GetContainerCamacPhiMC(){ return camac_phi_MC_;};

  std::vector < G4int >& GetContainerAdc(){ return adcs_;};
  std::vector < G4int >& GetContainerAmpl(){ return ampls_;};
  std::vector < G4int >& GetContainerChipId(){ return chip_ids_;};
  std::vector < G4int >& GetContainerFpgaId(){ return fpga_ids_;};
  std::vector < G4int >& GetContainerModule(){ return modules_;};
  std::vector < G4int >& GetContainerChanId(){ return chan_ids_;};
  std::vector < G4int >& GetContainerFemId(){ return fem_ids_;};
  std::vector < G4int >& GetContainerBco(){ return bcos_;};
  std::vector < G4int >& GetContainerBcoFull(){ return bco_fulls_;};
  std::vector < G4int >& GetContainerEvent(){ return events_;};
  std::vector < G4int >& GetContainerEventMC(){ return event_ids_MC_;};
  std::vector < G4double >& GetContainerEdepMC(){ return edeps_MC_;};
  std::vector < G4int >& GetContainerDacsMC(){ return dacs_MC_;};

private:
  G4bool fVerbose;
  G4int bco_;
  G4int bco_full_;
  
  std::vector < G4int > sensor_IDs_;
  std::vector < G4int > trigger_IDs_;
  std::vector < pair < string, G4int > > trigger_name_IDs_;
  std::vector < G4int > dac_values_;

  // vector variables for output
  // CAMAC information (trigger sci)
  std::vector < G4int > camac_adcs_;
  std::vector < G4int > camac_tdcs_;
  std::vector < G4double > camac_edeps_MC_; // MC truth
  std::vector < G4double > camac_timing_MC_; // MC truth
  std::vector < G4double > camac_theta_MC_; // beam track angle theta, MC truth
  std::vector < G4double > camac_phi_MC_; // beam track angle phi, MC truth

  // INTT hit information (same as the test bench)
  std::vector < G4int > adcs_;
  std::vector < G4int > ampls_;
  std::vector < G4int > chip_ids_;
  std::vector < G4int > fpga_ids_;
  std::vector < G4int > modules_;
  std::vector < G4int > chan_ids_;
  std::vector < G4int > fem_ids_;
  std::vector < G4int > bcos_;
  std::vector < G4int > bco_fulls_;
  std::vector < G4int > events_;

  // INTT hit information (MC truth)
  std::vector < G4int > event_ids_MC_;
  std::vector < G4double > edeps_MC_;
  std::vector < G4int > dacs_MC_;
  
  INTTMessenger* INTT_mess_;
  EDChamberHitsCollection* fHitsCollection_;  

  void FillINTTEvent( const G4Event* event );
  void FillTriggerEvent( const G4Event* event );
  // vector < TrackMC* > tracks_;
  // vector < StepMC* > steps_;
  //vector < G4double > eer

  void CleanContainer();
  EDChamberHitsCollection* GetHitsCollection(G4int ID,
					     const G4Event* event) const;
  EDEmCalorimeterHitsCollection* GetTriggerHitsCollection(G4int ID,
					     const G4Event* event) const;
  
  void GetHitsCollectionIDs();
  std::string GetTriggerName( G4int id );
  
  G4int Z2Module( G4int z );
  G4int Edep2Dac( G4double energy );
  G4int Dac2Adc( G4int dac );
  G4int GetFemId( G4int module );
  void XY2ChipChan( G4int up_or_down, G4int silicon_type, G4int x, G4int y, G4int& chip_id, G4int& chan_id );
  
};

#endif    
