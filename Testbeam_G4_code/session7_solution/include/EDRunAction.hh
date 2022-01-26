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

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TBRIK.h"

#include "EDPrimaryGeneratorAction.hh"
#include "EDEventAction.hh"
#include "ELPHEBeam.hh"
#include "OutputManager.hh"

// original classes for scoreing and analysis
#include "StepMC.hh"
#include "TrackMC.hh"
#include "Eventa.hh"
#include "INTTMessenger.hh"

class G4Run;
class EDPrimaryGeneratorAction;
class EDEventAction;
class INTTMessenger;

/// Run action class
class EDRunAction : public G4UserRunAction
{
public:
  EDRunAction( INTTMessenger* INTT_mess, EDPrimaryGeneratorAction* pga, EDEventAction* event, OutputManager* otput );
  
  virtual ~EDRunAction();

  G4bool is_first = true;
  static G4double beam_energy;//  = 800 * MeV;
  static ELPHEBeam* beam;
  EDEventAction* event_;
  
  virtual void BeginOfRunAction(const G4Run* );
  virtual void   EndOfRunAction(const G4Run* );

  //inline TFile* GetOutputTFile() const { return tf_output_;};
  inline std::shared_ptr < TFile > GetOutputTFile() const { return tf_output_;};
  //  inline TTree* GetTree() const { return tree_;};
  inline std::shared_ptr < TTree > GetTree() const { return tree_;};

  //void SetEvent( Event* ev ){ ev_ = ev; };
  void AddTrackMC( TrackMC* track ){ tracks_.push_back( track ); };
  //  void AddBox( TBox* box ){ boxes_.push_back( box );};
  void AddBrik( TBRIK* brik ){ briks_.push_back( brik );};
  
  void SetEventID( int id ){ event_id_ = id;};
  //void SetTrackMCs( vector < TrackMC* >& tracks ) const { tracks_ = tracks; };
  void SetTrackMCs( vector < TrackMC* >& tracks );
  void SetStepMCs( vector < StepMC* >& steps );
  
private:

  G4int event_id_;
  G4String tag = "";
  G4String output_dir = "";

  bool isFirst = true; // it should be true in the initial state

  //TFile* tf_output_;
  std::shared_ptr <TFile> tf_output_;
  //  TTree* tree_;
  std::shared_ptr <TTree> tree_;
  TLorentzVector vec;
  Eventa* ev_;
  TrackMC* track_;
  vector < TrackMC* > tracks_;
  vector < StepMC* > steps_;
  vector < TBRIK* > briks_;
  
  INTTMessenger* INTT_mess_;
  OutputManager* output_;

  void ClearEventBuffer( string mode );

};

#endif
