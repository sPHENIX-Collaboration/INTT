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
// #include <G4MTRandGauss.hh>
#include <RandGauss.h>
//#include <Random.h>
//#include "RandGauss.h"
//#include "TRandom.h"
//#include "TMath.h"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EDPrimaryGeneratorAction::EDPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fMessenger(0),
  fParticleGun(0),
  fRandom(true)
{   
  G4int nofParticles = 1;
  fParticleGun  = new G4ParticleGun(nofParticles);

  // Define particle properties
  G4String particleName = "mu-";
  // G4String particleName = "geantino";
  //G4ThreeVector position(0*mm, 0*mm, -20167.*mm);   
  //G4ThreeVector momentum(0, 0, 120.*GeV);
  
  // Default particle kinematics
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
  fParticleGun->SetParticleDefinition(particle);
  //fParticleGun->SetParticleMomentum(momentum);
  //fParticleGun->SetParticlePosition(position);
  
  
  // Generic messenger
  // Define /B4/event commands using generic messenger class
  fMessenger 
    = new G4GenericMessenger(this, "/ED/primary/", "Primary generator control");

  // Define /B4/event/setPrintModulo command
  fMessenger
    ->DeclareProperty("setRandom", 
                      fRandom, 
                      "Activate/Inactivate random option");
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EDPrimaryGeneratorAction::~EDPrimaryGeneratorAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EDPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //this function is called at the begining of ecah event
  //

  G4int eID=0;
  eID=event->GetEventID();

  //G4cout<<"1111111111 "<<eID<<G4endl;

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4int fNtupleId=1;
  if ( 1==1 ) {

    // randomized direction
    G4double dtheta = 2.*deg;
    G4double dphi = 360*deg;
    G4double pi=3.1415926535897;
    // G4double theta = G4UniformRand()*dtheta;
    // G4double phi = G4UniformRand()*dphi;
    //G4double theta = (1.62- THETAANGLE *0.00025)*deg;
    G4double thetagaus=CLHEP::RandGauss::shoot(0,0.002);
    //G4double phigaus=G4MTRandGauss::shoot(90,0.002);
    G4double phigaus=G4UniformRand()*360;
    G4double X_position=CLHEP::RandGauss::shoot(0,0.0559)*mm;
    G4double Y_position=CLHEP::RandGauss::shoot(0,0.1182)*mm;
    G4double energy_spectrum=CLHEP::RandGauss::shoot(120,2.4);


    G4double theta =  thetagaus*deg;
    G4double phi =  phigaus*deg;
    //G4double theta = (126*0.01)*deg;
    //G4double phi = 90.*deg;
    
    //=========================================the original one 2021/8/12===========================================

    // fParticleGun->SetParticleMomentumDirection(
    //   G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
    // //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,-20167.*mm));
    // fParticleGun->SetParticlePosition(G4ThreeVector(X_position,Y_position,-20167.*mm));
    // fParticleGun->SetParticleEnergy(energy_spectrum*GeV);

    //==========================================the origianl one 2021/8/12==========================================


    // G4double X_position_new = 25.1;
    // G4double Y_position_new = 3;
    // G4double Z_position_new = -200;

    // G4double aaa [2]={3,7};
    // G4double bbb [2]={200,0};
    // G4double ccc [2]={200,100};

    // fParticleGun->SetParticleMomentumDirection(
    //   G4ThreeVector(0, aaa[eID],ccc[eID]));
    // //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,-20167.*mm));
    // fParticleGun->SetParticlePosition(G4ThreeVector(0*mm,0*mm,-1*bbb[eID]*mm));
    // fParticleGun->SetParticleEnergy(120.*GeV);

    G4double X_position_new [6]= {104,104,-104,-104,-107.1,25.1};
    G4double Y_position_new [6]= {9,-9,9,-9,-9.961,0.055};
    G4double Z_position_new [6]= {-200,-200,-200,-200,-200,-200.};    


    fParticleGun->SetParticleMomentumDirection(
      G4ThreeVector(0,0,1));
    //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,-20167.*mm));
    fParticleGun->SetParticlePosition(G4ThreeVector(X_position_new[eID]*mm,Y_position_new[eID]*mm,Z_position_new[eID]*mm));
    fParticleGun->SetParticleEnergy(1*GeV);






    //=================for testing==========================================  
    // fParticleGun->SetParticleMomentumDirection(
    //   G4ThreeVector(0,0,1));
    // //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,-20167.*mm));
    // fParticleGun->SetParticlePosition(G4ThreeVector(0*mm,7.*mm,-300.*mm));
    // fParticleGun->SetParticleEnergy(20*GeV);

    //=================for testing==========================================  




    //G4cout<<" llllllllllll1 "<<fParticleGun->GetParticleMomentumDirection()<<G4endl;
    //G4cout<<thetagaus<<" "<<X_position<<" "<<Y_position<<" "<<"???????"<<G4endl;


    analysisManager->FillNtupleDColumn(fNtupleId, 0, X_position_new[eID]);
    analysisManager->FillNtupleDColumn(fNtupleId, 1, Y_position_new[eID]);
    analysisManager->FillNtupleDColumn(fNtupleId, 2, Z_position_new[eID]);
    analysisManager->FillNtupleDColumn(fNtupleId, 3, theta/pi*180.);
    //analysisManager->FillNtupleDColumn(fNtupleId, 3, thetagaus);
    analysisManager->FillNtupleDColumn(fNtupleId, 4, phi/pi*180.);
    analysisManager->FillNtupleIColumn(fNtupleId, 5, eID);
    analysisManager->FillNtupleDColumn(fNtupleId, 6, energy_spectrum);
    analysisManager->AddNtupleRow(fNtupleId);
    if ( eID % 10000 == 0)
      {
        G4cout<<"!!!Event Process: "<<eID<<G4endl;
        G4cout<<"!!!particle gun direction "<<fParticleGun->GetParticleMomentumDirection()<<G4endl;
        G4cout<<"!!!theta : "<<theta/pi*180.<<" phi : "<<phi/pi*180.<<G4endl;
        G4cout<<"!!!particle gun position "<<fParticleGun->GetParticlePosition()<<G4endl;
        G4cout<<"!!!particle gun energy : "<<fParticleGun->GetParticleEnergy()<<G4endl;     
      }

  }
  else {
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    //G4cout<<" llllllllllll2 "<<fParticleGun->GetParticleMomentumDirection()<<G4endl;
  }  
  //G4cout<<" particle gun direction "<<fParticleGun->GetParticleMomentumDirection()<<G4endl;
  fParticleGun->GeneratePrimaryVertex(event);
  if ( eID % 10000 == 0)
      {
        G4cout<<"!!!direciton cross check "<<fParticleGun->GetParticleMomentumDirection()<<G4endl;
      }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......










