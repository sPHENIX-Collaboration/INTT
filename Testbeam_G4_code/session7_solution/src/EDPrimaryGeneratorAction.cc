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
  //std::cerr << " beam";
  auto UImanager = G4UImanager::GetUIpointer();

  //this function is called at the begining of ecah event

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4double energy = 0.0;
  G4double momentum = EDRunAction::beam->GetMomentum();
  G4double theta =  0.0 * deg;
  G4double phi =  0.0 * deg;
  G4ThreeVector momentum_vec( 0, 0, 0);
  G4ThreeVector position( 0, 0, 0);

  // //auto energy_test = EDRunAction::beam->GetNextBeamTemp() * CLHEP::MeV;
  // G4double energy_test = 0.0;
  // EDRunAction::beam->GetNextBeamTemp();// * CLHEP::MeV;
  // momentum = energy_test;
  // //G4cout << "Get next beam temp: " << energy_test << G4endl;
  // momentum_vec.setZ( energy_test );
  
  // if no beam smearing required, just generate the beam here
  if( INTT_mess_->IsSmearing() == false )
    {
      //      std::cerr << "Primary generator action, mono-energy mode, " << momentum << " "

      auto UImanager = G4UImanager::GetUIpointer();
      energy =  UImanager->GetCurrentDoubleValue( "/gun/energy" ) * GeV; // GeV is used whatever I give? changed to MeV
      //      energy = CLHEP::GeV;
      momentum_vec = G4ThreeVector(0, 0, 1);
      position = G4ThreeVector(0, 0, -1.0 * m);
    }
  else
    {

      EDRunAction::beam->GenerateNextBeamTemp();

      if( INTT_mess_->GetDebugLevel() == 1 )
	EDRunAction::beam->Print();
      
      //position = G4ThreeVector( EDRunAction::beam->GetThisX(), EDRunAction::beam->GetThisY(), -1.0 * m ) ;
      //momentum = EDRunAction::beam->GetThisMomentum();

      energy = EDRunAction::beam->GetBeamEnergy();
      momentum_vec = EDRunAction::beam->GetBeamMomentumDirection();
      position = EDRunAction::beam->GetBeamPosition();
      position[2] = -CLHEP::m;
      
    }

  // //  fParticleGun->SetParticleMomentum( G4ThreeVector(0, 0, CLHEP::GeV) );
  // fParticleGun->SetParticlePosition( G4ThreeVector(0, 0, -CLHEP::m ) );

  // //  G4cout << momentum_vec.z() << G4endl;
  // //  G4cout << position.z() << G4endl;
  // //fParticleGun->SetParticleMomentum( momentum_vec );
  // // fParticleGun->SetParticlePosition( position );

  // //  fParticleGun->SetParticleMomentum( momentum );
  // fParticleGun->SetParticleMomentumDirection( G4ThreeVector(0, 0, 1) );
  // //  fParticleGun->SetParticleEnergy( momentum );
  // fParticleGun->SetParticleEnergy( energy_test );


  fParticleGun->SetParticleEnergy( energy );
  fParticleGun->SetParticleMomentumDirection( momentum_vec );
  fParticleGun->SetParticlePosition( position );

  G4int fNtupleId = 1;
  G4int eID = 0;
  eID = event->GetEventID();
  analysisManager->FillNtupleDColumn(fNtupleId, 0, position.x() );
  analysisManager->FillNtupleDColumn(fNtupleId, 1, position.y() );
  analysisManager->FillNtupleDColumn(fNtupleId, 2, position.z() );
  analysisManager->FillNtupleDColumn(fNtupleId, 3, momentum_vec.theta() / M_PI * 180.0 );
  //analysisManager->FillNtupleDColumn(fNtupleId, 3, thetagaus);
  analysisManager->FillNtupleDColumn(fNtupleId, 4, momentum_vec.phi() / M_PI * 180.0 );
  analysisManager->FillNtupleIColumn(fNtupleId, 5, eID);
  //analysisManager->FillNtupleDColumn(fNtupleId, 6, momentum);

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
