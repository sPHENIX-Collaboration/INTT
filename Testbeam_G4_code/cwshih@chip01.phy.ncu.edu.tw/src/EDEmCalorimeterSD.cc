/// \file EDEmCalorimeterSD.cc
/// \brief Implementation of the EDEmCalorimeterSD class
//

#include "EDEmCalorimeterSD.hh"

EDEmCalorimeterSD::EDEmCalorimeterSD(const G4String& name, 
                                     const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name),
   fHitsCollection(0)
{
  collectionName.insert(hitsCollectionName);
}

EDEmCalorimeterSD::~EDEmCalorimeterSD()
{}

void EDEmCalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  // store the verbose level for debugging
  auto ui_manager = G4UImanager::GetUIpointer();
  verbose = ui_manager->GetCurrentIntValue( "/run/verbose", 1, true );

  fHitsCollection 
    = new EDEmCalorimeterHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 

  for (G4int i=0; i<3; ++i) {
    EDEmCalorimeterHit* newHit = new EDEmCalorimeterHit();
    newHit->SetLayerNumber(i);
    fHitsCollection->insert(newHit);
  } 
}

G4bool EDEmCalorimeterSD::ProcessHits(G4Step* step,
                                      G4TouchableHistory* /*history*/)
{
  
  G4int eID_1 = 0;
  const G4Event* evt_1 = G4MTRunManager::GetRunManager()->GetCurrentEvent();
  if (evt_1)
    eID_1 = evt_1->GetEventID();

  const G4VTouchable* touchable
    = step->GetPreStepPoint()->GetTouchable();
  G4VPhysicalVolume* motherPhysical = touchable->GetVolume(0); // mother

  G4double edep = step->GetTotalEnergyDeposit();

  // G4String motherPhysical_name_test = motherPhysical->GetName(); // not used anymore 
  
  G4int copyNo = motherPhysical->GetCopyNo();
  // G4int copyNo =  touchable->GetVolume(0)-> GetCopyNo();   // test

  EDEmCalorimeterHit* HitThisUnit = (*fHitsCollection)[copyNo];
  HitThisUnit->AddEdep(edep);

  // energy deposit
  //G4double edep = step->GetTotalEnergyDeposit();
  //if ( edep==0. ) return false;     
  
  /*const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();
  G4VPhysicalVolume* layerPV = touchable->GetVolume();
  G4int layerNumber = layerPV->GetCopyNo();*/
  
  // Get hit accounting data for this layer
  //EDEmCalorimeterHit* hit = (*fHitsCollection)[layerNumber];
  /*if ( ! hit ) {
    G4cerr << "Cannot access hit " << layerNumber << G4endl;
    exit(1);
  }*/         
  
  // Update hit accounting data for all layers either for charged or
  // for neutral particles
  /*if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0 ) {
    EDEmCalorimeterHit* hitCharged = (*fHitsCollection)[10];
    // Add values
    hitCharged->AddEdep(edep);
  }
  else {
    EDEmCalorimeterHit* hitNeutral = (*fHitsCollection)[11];
    // Add values
    hitNeutral->AddEdep(edep);
  }
  
  // Update hit depending if track is primary or not:
  if ( step->GetTrack()->GetParentID() == 0 ) {
    EDEmCalorimeterHit* hitPrimary = (*fHitsCollection)[12];
    // Add values
    hitPrimary->AddEdep(edep);
  }
  else {
    EDEmCalorimeterHit* hitOthers = (*fHitsCollection)[13];
    // Add values
    hitOthers->AddEdep(edep);
  }*/

  // Add values
  //hit->AddEdep(edep);

  //if( verbose == 2 )
  if( false )
    {
      // G4cout<<"!!!! SCI pre_TESTnameTEST : "<<motherPhysical_name_test<<G4endl; // not used anymore 
      G4cout<<"!!!! SCI pre_getcopynumberTEST : "<<copyNo<<" edep : "<<edep<<G4endl;
    }
  
  return true;
}

void EDEmCalorimeterSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  //  std::cerr << "void EDEmCalorimeterSD::EndOfEvent(G4HCofThisEvent* /*hce*/)";
  G4int eID=0;
  const G4Event* evt=G4MTRunManager::GetRunManager()->GetCurrentEvent();
  if (evt) eID=evt->GetEventID();

  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  G4int n_hit = fHitsCollection->entries();

  for (G4int i = 0; i < n_hit; i++) 
    {
      G4double fEdep = (*fHitsCollection)[i]->GetEnergyDeposit();
      if (fEdep>0.)
      {
        analysisManager->FillNtupleIColumn(3, 0, eID);
        analysisManager->FillNtupleIColumn(3, 1, i);
        analysisManager->FillNtupleDColumn(3, 2, fEdep);  
        analysisManager->AddNtupleRow(3);
        //if ( eID % 10000 == 0)
	//if( verbose == 2 )
	if( false )
          {
            G4cout<<" SCI, eID : "<<eID<<" Sci trigger ID : "<<i<<" edep : "<<fEdep<<G4endl;
          }
      }


    }

  //  std::cerr << "  ----> ends" << std::endl;
  // Suppress heavy print:
  // G4cout << "\n-------->" <<  fHitsCollection->GetName() 
  //        << ": in this event: " << G4endl;

  // G4int nofHits = fHitsCollection->entries();
  // for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  
  // Loop over hits
  // and fill each non-zero value in appropriate histogram
  //G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  //G4int nofHits = fHitsCollection->entries();
  /*for ( G4int i=0; i<nofHits; i++ )  {
    EDEmCalorimeterHit* hit = (EDEmCalorimeterHit*)(*fHitsCollection)[i];
    G4double edep = hit->GetEdep();
    if ( edep > 0. ) analysisManager->FillH1(i, edep/MeV);
  }*/
}
