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
// $Id: EDActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file EDActionInitialization.cc
/// \brief Implementation of the EDActionInitialization class

#include "EDActionInitialization.hh"
#include "EDPrimaryGeneratorAction.hh"
#include "EDEventAction.hh"
#include "EDRunAction.hh"

EDActionInitialization::EDActionInitialization()
 : G4VUserActionInitialization()
{}

EDActionInitialization::~EDActionInitialization()
{}

void EDActionInitialization::BuildForMaster() const
{
  SetUserAction(new EDRunAction( new EDPrimaryGeneratorAction() ) );
}

void EDActionInitialization::Build() const
{
  auto pga = new EDPrimaryGeneratorAction();  
  SetUserAction( pga );
  SetUserAction(new EDRunAction( pga ) );
  SetUserAction(new EDEventAction);
}  
/*
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  39   │ void B4cActionInitialization::BuildForMaster() const
  40   │ {
  41   │   SetUserAction( new B4RunAction( new B4PrimaryGeneratorAction() ) );
  42   │ }
  43   │
  44   │ void B4cActionInitialization::Build() const
  45   │ {
  46   │   auto pga = new B4PrimaryGeneratorAction();
  47   │   SetUserAction( pga );
  48   │   //  SetUserAction(new B4PrimaryGeneratorAction);
  49   │
  50   │   SetUserAction( new B4RunAction( pga ) );
  51   │   SetUserAction( new B4cEventAction );
  52   │   SetUserAction( new TrackingAction( pga ) );
  53   │ }
*/
