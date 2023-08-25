/// \file EDEmCalorimeterSD.hh
/// \brief Definition of the EDEmCalorimeterSD class
//

#ifndef EDEmCalorimeterSD_h
#define EDEmCalorimeterSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "EDEmCalorimeterHit.hh"
#include "EDAnalysis.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class EDEmCalorimeterSD : public G4VSensitiveDetector
{
  public:
    EDEmCalorimeterSD(const G4String& name, 
                      const G4String& hitsCollectionName);
    virtual ~EDEmCalorimeterSD();

    virtual void   Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hce);

  private:
    EDEmCalorimeterHitsCollection* fHitsCollection;
  G4int verbose = 0;
};




#endif

