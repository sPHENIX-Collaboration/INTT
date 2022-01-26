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
/// \file EDDetectorConstruction.hh
/// \brief Definition of the EDDetectorConstruction class

#ifndef EDDetectorConstruction_h
#define EDDetectorConstruction_h 1

#include <cstdlib>
#include <cmath>

#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "G4RunManager.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4TwoVector.hh"
#include "G4GenericTrap.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ProductionCuts.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIparameter.hh"
#include "G4Tokenizer.hh"
#include "G4UIcommand.hh"
#include "G4Tokenizer.hh"
#include "public_variable.hh"

#include "EDChamberSD.hh"
#include "EDEmCalorimeterSD.hh"
#include "INTTMessenger.hh"

//extern G4double theoffset;
class G4VPhysicalVolume;
class INTTMessenger;

/// Detector construction class to define materials and geometry.
class EDDetectorConstruction : public G4VUserDetectorConstruction
{
  
  //void movement (G4double theoffset) {theoffset=35.;}
  //extern G4double theoffset =35;
  
public:
  EDDetectorConstruction( INTTMessenger* INTT_mess );
  //static double theoffset =35;
  virtual ~EDDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();
  //   void Settheoffset( G4double theoffset) { fmovement = theoffset; }
  //   G4double Gettheoffset() const {return fmovement; }

  // the stupid units
  static const G4double inch; // = 25.4; // 1 inch = 25.4 mm
  static const G4double ft; // = 304.8; // 1 ft = 304.8 mm  

  //void SetPlateConstruction( G4bool arg ){ bl_plate_ = arg;};
  //void DoConstruction( int i=0 );
  
private:
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  const G4double kSilicon_strip_width; // along y-axis
  const G4double kSilicon_strip_thickness; // along z-axis
  const G4double kSilicon_length_type_a; // along x-axis
  const G4double kSilicon_length_type_b; // along x-axis
  G4double silicon_module_gap[3];
  const G4double kFPHX_width; // along y-axis
  const G4double kFPHX_thickness; // along xz-axis
  const G4double kFPHX_length; // along x-axis
  const G4double kSilver_epoxy_glue_silicon_thickness;
  const G4double kSilver_epoxy_glue_FPHX_thickness;
  const G4double kINTT_CFRP_thickness;
  
  // G4double fmovement;
  G4double world_size[3]; // x, y, z
  G4double experimental_size[3]; // x, y, z
  G4double experimental_offset[3];  // x, y, z
  G4double INTT_testbeam_BOX_size[3];  // x, y, z
  G4double darkbox_offset[3];  // x, y, z
  const G4double kDarkbox_wall_thickness_body;
  const G4double kDarkbox_wall_thickness_side;
  const G4double kDarkbox_stage_width; // width of the movable stage
  
  INTTMessenger* INTT_mess_;

  G4Material* DefaultMaterial;
  G4Material* Silicon;
  G4Material* Kapton;
  G4Material* Copper;
  G4Material* Epoxy;
  G4Material* foam;
  G4Material* Water;
  G4Material* CFRP;
  G4Material* BBHodo_Scinti;
  G4Material* SilverEpoxyGlue;
  G4Material* DarkBox;
  
  G4VPhysicalVolume* INTT_testbeam_BOXPV;

  G4LogicalVolume* worldLog;
  G4LogicalVolume* experimental_areaLV; //  the dark box and the trigger scintillators are in this volume. it may be rotated
  G4LogicalVolume* INTT_testbeam_BOXLV; // the dark box is in this volume
  
  G4VisAttributes* color_invisible;
  G4VisAttributes* color_silicon_active;
  G4VisAttributes* color_silicon_inactive;
  G4VisAttributes* color_silicon_not_used;
  G4VisAttributes* color_glue;
  G4VisAttributes* color_FPHX;
  G4VisAttributes* color_HDI_copper;
  G4VisAttributes* color_HDI_kapton;
  G4VisAttributes* color_CFRP_carbon;
  G4VisAttributes* color_CFRP_water;
  G4VisAttributes* color_CFRP_foam;
  G4VisAttributes* color_scintillator;
  G4VisAttributes* color_darkbox_wall;
  G4VisAttributes* color_plate;

  // parameters for the additional plate
  G4String plate_material;
  G4double plate_thickness;
  G4double plate_distance; // distance from the upstream trigger scintillator

  void DefineVisAttributes();
  void DefineMaterials();

  void ConstructDarkBox();
  void ConstructLadders();
  void ConstructTriggers();
  void ConstructPlate();
};

#endif
