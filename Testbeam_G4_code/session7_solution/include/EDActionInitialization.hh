/// \file EDActionInitialization.hh
/// \brief Definition of the EDActionInitialization class

#ifndef EDActionInitialization_h
#define EDActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "EDPrimaryGeneratorAction.hh"
#include "EDEventAction.hh"
#include "EDRunAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
//#include "OutputManager.hh"
#include "INTTMessenger.hh"
/// Action initialization class.

class EDActionInitialization : public G4VUserActionInitialization
{
public:
  EDActionInitialization( INTTMessenger* INTT_mess, EDDetectorConstruction* fDetConstruction_arg );
  virtual ~EDActionInitialization();
  
  virtual void Build() const;
  virtual void BuildForMaster() const;
  const EDDetectorConstruction* fDetConstruction;
  INTTMessenger* INTT_mess_;
};

#endif
