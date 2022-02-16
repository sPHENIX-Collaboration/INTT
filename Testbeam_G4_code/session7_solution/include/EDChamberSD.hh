/// \file EDChamberSD.hh
/// \brief Definition of the EDChamberSD class
//

#ifndef EDChamberSD_h
#define EDChamberSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4MTRunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4AutoLock.hh"
#include "G4UImanager.hh"

#include "CalHit.hh"  
#include "EDChamberHit.hh"
#include "EDAnalysis.hh"

#include "EDDetectorConstruction.hh"
#include "public_variable.hh"
//#include "Encoder64Cal.hh"
//#include "VSensitiveDetector.hh"


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class EDChamberSD : public G4VSensitiveDetector
{
  public:
    EDChamberSD(const G4String& name,
                const G4String& hitsCollectionName,
                G4int ntupleID);
    virtual ~EDChamberSD();

    virtual void   Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hce);

  private:
    EDChamberHitsCollection* fHitsCollection;  
    G4int fNtupleId;
    //G4int funitofside;
  G4int verbose = 0; // 2 for debugging, it can be 0, 1, or 2
};

#endif
