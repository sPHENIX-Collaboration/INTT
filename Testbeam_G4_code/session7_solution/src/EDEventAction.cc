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
/// \file EDEventAction.cc
/// \brief Implementation of the EDEventAction class

#include "EDEventAction.hh"

#include "G4Event.hh"
#include "G4ios.hh"

EDEventAction::EDEventAction()
 : G4UserEventAction(),
   fVerbose(true)
{}

EDEventAction::~EDEventAction()
{}

void EDEventAction::BeginOfEventAction(const G4Event* event)
{

  //std::cerr << "void EDEventAction::BeginOfEventAction(const G4Event* event)";
  // if ( false ) 
  // {
  //   //G4cout << ">>> Start event: " << event->GetEventID() << G4endl;
  // }
  //  std::cerr << "----------> ends" << std::endl;
}

void EDEventAction::EndOfEventAction(const G4Event* event)
{
  //  std::cerr << "void EDEventAction::EndOfEventAction(const G4Event* event)";
  // if ( false ) 
  // {
  //   //G4cout << ">>> End event: " << event->GetEventID() << G4endl;
  // }  
  // G4int eID = event->GetEventID();
  // if ( eID % 20000 == 0)
  //   G4cout << "Event Process: " << eID << G4endl;

  //  std::cerr << "  ----> ends" << std::endl;
  //std::cerr << std::endl;
}    

