//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file EDPrimaryGeneratorAction.cc
/// \brief Implementation of the EDPrimaryGeneratorAction class

#include "EDPrimaryGeneratorAction.hh"

EDPrimaryGeneratorAction::EDPrimaryGeneratorAction( INTTMessenger* INTT_mess )
  : G4VUserPrimaryGeneratorAction(),
    fParticleGun(0),
    fRandom(true)
{   
  G4int nofParticles = 1;
  fParticleGun  = new G4ParticleGun(nofParticles);

  INTT_mess_ = INTT_mess;

  // Define particle properties
  G4String particleName = "e+";
  
  // Default particle kinematics
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle( particleName );
  fParticleGun->SetParticleDefinition( particle );
  
}

EDPrimaryGeneratorAction::~EDPrimaryGeneratorAction()
{}


void EDPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{

  //this function is called at the begining of ecah event
  G4double energy = 0.0;
  G4ThreeVector momentum_vec( 0, 0, 0);
  G4ThreeVector position( 0, 0, 0);
  
  // if no beam smearing required, just generate the beam here
  if( INTT_mess_->IsSmearing() == false )
    {
      auto UImanager = G4UImanager::GetUIpointer();
      energy =  UImanager->GetCurrentDoubleValue( "/gun/energy" ) * GeV; // GeV is used whatever I give? changed to MeV
      momentum_vec = G4ThreeVector(0, 0, 1);
      position = G4ThreeVector(0, 0, -1.0 * m); // set z position to 1 m upstream of the setup
      
    }
  else
    {

      EDRunAction::beam->GenerateNextBeam();

      if( INTT_mess_->GetDebugLevel() == 1 )
	EDRunAction::beam->Print();
      
      energy = EDRunAction::beam->GetBeamEnergy();
      momentum_vec = EDRunAction::beam->GetBeamMomentumDirection();
      position = EDRunAction::beam->GetBeamPosition();
      position[2] = -CLHEP::m;
      
    }

  fParticleGun->SetParticleEnergy( energy );
  fParticleGun->SetParticleMomentumDirection( momentum_vec );
  fParticleGun->SetParticlePosition( position );

  G4int fNtupleId = 1;
  G4int eID = event->GetEventID();

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleDColumn(fNtupleId, 0, position.x() );
  analysisManager->FillNtupleDColumn(fNtupleId, 1, position.y() );
  analysisManager->FillNtupleDColumn(fNtupleId, 2, position.z() );
  analysisManager->FillNtupleDColumn(fNtupleId, 3, momentum_vec.theta() / M_PI * 180.0 );
  analysisManager->FillNtupleDColumn(fNtupleId, 4, momentum_vec.phi() / M_PI * 180.0 );
  analysisManager->FillNtupleIColumn(fNtupleId, 5, eID);

  auto beam_particle = fParticleGun->GetParticleDefinition();
  G4double momentum_abs = sqrt( pow( energy, 2 ) -  pow( beam_particle->GetPDGMass(), 2) );
  analysisManager->FillNtupleDColumn(fNtupleId, 6, momentum_abs * momentum_vec.x() );
  analysisManager->FillNtupleDColumn(fNtupleId, 7, momentum_abs * momentum_vec.y() );
  analysisManager->FillNtupleDColumn(fNtupleId, 8, momentum_abs * momentum_vec.z() );
  analysisManager->FillNtupleDColumn(fNtupleId, 9, energy );

  analysisManager->AddNtupleRow(fNtupleId);
  
  fParticleGun->GeneratePrimaryVertex(event);
  return;
}
