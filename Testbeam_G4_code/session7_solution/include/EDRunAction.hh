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
/// \file EDRunAction.hh
/// \brief Definition of the EDRunAction class

#ifndef EDRunAction_h
#define EDRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4GenericMessenger.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"
#include "EDAnalysis.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "EDPrimaryGeneratorAction.hh"

class G4Run;
class EDPrimaryGeneratorAction;

/// Run action class
class EDRunAction : public G4UserRunAction
{
public:
  EDRunAction( EDPrimaryGeneratorAction* pga );
  
  virtual ~EDRunAction();

  G4bool is_first = true;
  static G4double beam_energy;//  = 800 * MeV;
  static G4bool is_beam_smearing;
  virtual void BeginOfRunAction(const G4Run* );
  virtual void   EndOfRunAction(const G4Run* );

private:
  
  G4String tag = "";
  G4String output_dir = "";

  bool isFirst = true; // it should be true in the initial state

  G4GenericMessenger* fMessenger;
  
  void DefineCommands();
};

#endif
