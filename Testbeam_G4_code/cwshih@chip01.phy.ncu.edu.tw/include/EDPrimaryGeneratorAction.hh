/// \file EDPrimaryGeneratorAction.hh
/// \brief Definition of the EDPrimaryGeneratorAction class

#ifndef EDPrimaryGeneratorAction_h
#define EDPrimaryGeneratorAction_h 1

#include <cmath> // for M_PI

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4MTRunManager.hh"
#include "G4VSensitiveDetector.hh"
#include "Randomize.hh"

#include "EDAnalysis.hh"
#include "EDRunAction.hh"
#include "EDRunAction.hh"
#include "ELPHEBeam.hh"
#include "INTTMessenger.hh"

class G4Event;
class G4ParticleGun;
class G4GenericMessenger;
class INTTMessenger;

/// The primary generator action class.

class EDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    EDPrimaryGeneratorAction( INTTMessenger* INTT_mess );    
    virtual ~EDPrimaryGeneratorAction();

    // method from the base class
  virtual void GeneratePrimaries(G4Event*);         
  G4ParticleGun*  GetParticleGun() const { return fParticleGun; };
  private:
    INTTMessenger*  INTT_mess_;
    G4ParticleGun*  fParticleGun;  
    G4bool          fRandom;
  
  void DefineCommands();
};


#endif
